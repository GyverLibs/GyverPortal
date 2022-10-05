#pragma once
#include "utils.h"
#include "objects.h"
#include "log.h"

#ifdef ESP8266
#include <ESP8266WebServer.h>
extern ESP8266WebServer* _gp_s;
#else
#include <WebServer.h>
extern WebServer* _gp_s;
#endif

extern int _gp_bufsize;
extern String* _gp_uri;
extern String* _gp_page;
static uint8_t _gp_nav_pos = 0;
static uint8_t _gp_nav_id = 0;

struct Builder {
    int reloadTimeout = 150;
    
    // задержка перезагрузки страницы для SELECT и BUTTON при включенном rel
    void setReloadTimeout(int tout) {
        reloadTimeout = tout;
    }
    
    // ======================= БИЛДЕР =======================
    void BUILD_BEGIN(int width = 350) {
        PAGE_BEGIN();
        JS_TOP();
        PAGE_BLOCK_BEGIN(width);
    }
    void BUILD_BEGIN_FILE(int width = 350) {
        PAGE_BEGIN();
        JS_TOP_FILE();
        PAGE_BLOCK_BEGIN(width);
    }
    void BUILD_END() {
        PAGE_BLOCK_END();
        JS_BOTTOM();
        PAGE_END();
    }
    
    // ===================== ОТПРАВКА RAW =====================
    void SEND(const String& s) {
        *_gp_page += s;
        send();
    }
    void SEND_P(PGM_P s) {
        send(true);
        _gp_s->sendContent_P(s);
    }
    
    void send(bool force = 0) {
        if ((int)_gp_page->length() > (force ? 0 : _gp_bufsize)) {
            _gp_s->sendContent(*_gp_page);
            *_gp_page = "";
        }
    }

    // ======================= СТРАНИЦА =======================
    void PAGE_BEGIN() {
        _gp_nav_id = 0;
        SEND(F("<!DOCTYPE HTML><html><head>\n"
        "<meta charset='utf-8'>\n"
        "<meta name='viewport' content='width=device-width, initial-scale=1'>\n"
        "</head><body>\n"));
    }
    void JS_TOP_FILE() {
        SEND(F("<script src='/gp_data/scripts.js'></script>\n"));
    }
    void JS_TOP() {
        SEND_P(PSTR("<script>\n"
        "function GP_hint(id,txt){document.getElementById(id).title=txt;}\n"
        "function GP_send(req){var xhttp=new XMLHttpRequest();xhttp.open('POST',req,true);xhttp.send();}\n"
        "function GP_clickUD(arg,dir){if(arg.name)GP_send('/GP_click?'+arg.name+'=1&_dir='+dir);}\n"
        "function GP_click(arg,r=0){if(!arg.name)return;var v;if(arg.type=='number'){\n"
        "if(arg.hasAttribute('min')&&Number(arg.value)<=Number(arg.min))arg.value=arg.min;\n"
        "if(arg.hasAttribute('max')&&Number(arg.value)>=Number(arg.max))arg.value=arg.max;}\n"
        "if(arg.type=='checkbox')v=arg.checked?'1':'0';else v=arg.value;\n"
        "if(v.charAt(0)=='#')v=v.substring(1);GP_send('/GP_click?'+arg.name+'='+v);\n"
        "if(r!=0)setTimeout(function(){location.reload();},r);}\n"
        "function GP_clickid(btn,tar){GP_send('/GP_click?'+btn+'='+document.getElementById(tar).value);}\n"
        "function GP_change(arg){arg.style.backgroundSize=(arg.value-arg.min)*100/(arg.max-arg.min)+'% 100%';\n"
        "document.getElementById(arg.id+'_val').value=arg.value}\n"
        "function GP_wheel(arg){var e=window.event;arg.value-=Math.sign(e.deltaY||e.detail||e.wheelDelta)*Number(arg.step);}\n"
        "function saveFile(id){document.getElementById(id).click();}\n"
        "function GP_subm(id){document.getElementById(id).submit();event.preventDefault();}\n"
        "function openTab(tab,btn,blk){var x=document.getElementsByClassName(blk);\n"
        "for(var i=0;i<x.length;i++)x[i].style.display='none';\n"
        "document.getElementById(tab).style.display='block';\n"
        "x=document.getElementsByClassName(btn.className);\n"
        "for(var i=0;i<x.length;i++)x[i].style.background='';\n"
        "btn.style.background='#2a2d35';}\n"
        "function GP_spin(id,stp,dec){var num=document.getElementById(id);num.value=(Number(num.value)+stp).toFixed(dec);var e=new Event('change');num.dispatchEvent(e);}\n"
        "function GP_update(ids){\n"
        "var xhttp=new XMLHttpRequest();xhttp.open('GET','/GP_update?'+ids+'=',true);xhttp.send();\n"
        "xhttp.onreadystatechange=function(){if(this.readyState==4&&this.status==200){\n"
        "var resp=this.responseText.split(',');ids=ids.split(',');if(ids.length!=resp.length)return;\n"
        "for(let i=0;i<ids.length;i++){\n"
        "var item=document.getElementById(ids[i]);if(!item||!resp[i])continue;\n"
        "if(item.type=='hidden'&&item.value=='_reload'){if(resp[i]=='1')location.reload();}\n"
        "else if(item.type=='checkbox'||item.type=='radio')item.checked=Number(resp[i]);\n"
        "else if(item.type=='select-one')document.querySelector('#'+ids[i]).value=resp[i];\n"
        "else if(item.type==undefined)item.innerHTML=resp[i];\n"
        "else item.value=resp[i];\n"
        "if(item.type=='range')GP_change(item);}}};}\n"
        "</script>\n"));
        
        /*"function GP_update(ids){ids.split(',').forEach(function(id){\n"
        "var xhttp=new XMLHttpRequest();\n"
        "xhttp.onreadystatechange=function(){\n"
        "if(this.readyState==4&&this.status==200){\n"
        "var resp=this.responseText;\n"
        "var item=document.getElementById(id);if(item==null)return;\n"
        "if(item.type=='hidden'&&item.value=='_reload'){if(resp=='1')location.reload();}\n"
        "else if(item.type=='checkbox'||item.type=='radio')item.checked=Number(resp);\n"
        "else if(item.type=='select-one')document.querySelector('#'+id).value=resp;\n"
        "else if(item.type==undefined)item.innerHTML=resp;\n"
        "else item.value=resp;\n"
        "if(item.type=='range')GP_change(item);}};\n"
        "xhttp.open('GET','/GP_update?'+id+'=',true);xhttp.send();});}\n"*/
    }
    void JS_BOTTOM() {
        SEND(F("<script>document.querySelectorAll('input[type=range]').forEach(x=>{GP_change(x)})</script>\n"));
    }
    
    void THEME(PGM_P style) {
        SEND_P(style);
    }
    void THEME_FILE(const String& style) {
        *_gp_page += F("<link rel='stylesheet' href='/gp_data/");
        *_gp_page += style;
        *_gp_page += F(".css'>\n");
        send();
    }
    
    void PAGE_BLOCK_BEGIN(int width = 350) {
        *_gp_page += F("<div align='center' style='margin:auto;max-width:");
        *_gp_page += width;
        *_gp_page += F("px;'>\n");
        send();
    }
    void PAGE_BLOCK_END() {
        SEND(F("</div>\n"));
    }
    void PAGE_END() {
        SEND(F("</body></html>"));
    }
    
    void SPOILER_BEGIN(const String& text, PGM_P st = GP_GREEN) {
        *_gp_page += F("<details><summary align='left' style='");
        if (st != GP_GREEN) {
            *_gp_page += F("background-color:");
            *_gp_page += FPSTR(st);
            *_gp_page += ';';
        }
        *_gp_page += F("'>");
        *_gp_page += text;
        *_gp_page += F("</summary><div align='center' style='");
        if (st != GP_GREEN) {
            *_gp_page += F("border-color:");
            *_gp_page += FPSTR(st);
            *_gp_page += ';';
        }
        *_gp_page += F("'>\n");
        send();
    }
    void SPOILER_END() {
        SEND(F("</div></details>\n"));
    }
    
    void HINT(const String& name, const String& txt) {
        *_gp_page += F("<script>GP_hint('");
        *_gp_page += name;
        *_gp_page += F("','");
        *_gp_page += txt;
        *_gp_page += F("')</script>");
        send();
    }
    
    // ======================= UPDATE =======================
    void UPDATE(const String& list, int prd = 1000) {
        *_gp_page += F("<script>setInterval(function(){GP_update('");
        *_gp_page += list;
        *_gp_page += "')},";
        *_gp_page += prd;
        *_gp_page += F(");</script>\n");
        send();
    }
    void AJAX_UPDATE(const String& list, int prd = 1000) {
        UPDATE(list, prd);
    }
    
    void JQ_SUPPORT_FILE() {
        SEND(F("<script src='/gp_data/jquery.js'></script>\n"));
    }
    void JQ_SUPPORT() {
        SEND(F("<script src='https://code.jquery.com/jquery-3.6.1.min.js'></script>\n"));
    }
    void JQ_UPDATE_BEGIN(int prd = 1000, int del = 100) {
        *_gp_page += F("<script>var globalupd=1;\n"
        "$(function(ev){$('#jqupd').click(function (ev){globalupd=0;setTimeout(function(){$('#jqupd').load('# #jqupd');},");
        *_gp_page += del;
        *_gp_page += F(");});});\n setInterval(function(){if(globalupd){$('#jqupd').load('# #jqupd');}else{globalupd=1;}},");
        *_gp_page += prd;
        *_gp_page += F(");</script>\n<div id='jqupd' style='width:100%;'>");
        send();
    }
    void JQ_UPDATE_END() {
        SEND(F("</div>\n"));
    }
    
    // 
    void RELOAD(const String& name) {
        HIDDEN(name, F("_reload"));
    }
    
    
    // ====================== ПОПАПЫ =======================
    void ALERT(const String& name, int prd = 1000) {
        *_gp_page += F("<script>setInterval(function(){var xhttp=new XMLHttpRequest();xhttp.onreadystatechange=function(){\n"
        "if(this.readyState==4&&this.status==200&&this.responseText.length)alert(this.responseText);};\n"
        "xhttp.open('GET','/GP_update?'+'");
        *_gp_page += name;
        *_gp_page += F("=',true);xhttp.send();},");
        *_gp_page += prd;
        *_gp_page += F(");</script>\n");
        send();
    }
    void ALERT(const String& name, const String& text, int prd = 1000) {
        *_gp_page += F("<script>setInterval(function(){var xhttp=new XMLHttpRequest();xhttp.onreadystatechange=function(){\n"
        "if(this.readyState==4&&this.status==200&&this.responseText.length)alert('");
        *_gp_page += text;
        *_gp_page += F("');};\n xhttp.open('GET','/GP_update?'+'");
        *_gp_page += name;
        *_gp_page += F("=',true);xhttp.send();},");
        *_gp_page += prd;
        *_gp_page += F(");</script>\n");
        send();
    }
    
    void PROMPT(const String& name, int prd = 1000) {
        *_gp_page += F("<script>setInterval(function(){var xhttp=new XMLHttpRequest();xhttp.onreadystatechange=function(){\n"
        "if(this.readyState==4&&this.status==200&&this.responseText.length){let res=prompt(this.responseText,'');GP_send('/GP_click?");
        *_gp_page += name;
        *_gp_page += F("='+res);}};\n xhttp.open('GET','/GP_update?'+'");
        *_gp_page += name;
        *_gp_page += F("=',true);xhttp.send();},");
        *_gp_page += prd;
        *_gp_page += F(");</script>\n");
        send();
    }
    void PROMPT(const String& name, const String& text, int prd = 1000) {
        *_gp_page += F("<script>setInterval(function(){var xhttp=new XMLHttpRequest();xhttp.onreadystatechange=function(){\n"
        "if(this.readyState==4&&this.status==200&&this.responseText.length){let res=prompt('");
        *_gp_page += text;
        *_gp_page += F("','');GP_send('/GP_click?");
        *_gp_page += name;
        *_gp_page += F("='+res);}};\n xhttp.open('GET','/GP_update?'+'");
        *_gp_page += name;
        *_gp_page += F("=',true);xhttp.send();},");
        *_gp_page += prd;
        *_gp_page += F(");</script>\n");
        send();
    }
    
    void CONFIRM(const String& name, int prd = 1000) {
        *_gp_page += F("<script>setInterval(function(){var xhttp=new XMLHttpRequest();xhttp.onreadystatechange=function(){\n"
        "if(this.readyState==4&&this.status==200&&this.responseText.length){let res=confirm(this.responseText);GP_send('/GP_click?");
        *_gp_page += name;
        *_gp_page += F("='+(res?'1':'0'));}};\n xhttp.open('GET','/GP_update?'+'");
        *_gp_page += name;
        *_gp_page += F("=',true);xhttp.send();},");
        *_gp_page += prd;
        *_gp_page += F(");</script>\n");
        send();
    }
    void CONFIRM(const String& name, const String& text, int prd = 1000) {
        *_gp_page += F("<script>setInterval(function(){var xhttp=new XMLHttpRequest();xhttp.onreadystatechange=function(){\n"
        "if(this.readyState==4&&this.status==200&&this.responseText.length){let res=confirm('");
        *_gp_page += text;
        *_gp_page += F("');GP_send('/GP_click?");
        *_gp_page += name;
        *_gp_page += F("='+(res?'1':'0'));}};\n xhttp.open('GET','/GP_update?'+'");
        *_gp_page += name;
        *_gp_page += F("=',true);xhttp.send();},");
        *_gp_page += prd;
        *_gp_page += F(");</script>\n");
        send();
    }
    
    // ======================= ФОРМА =======================
    void FORM_BEGIN(const String& action) {
        *_gp_page += F("<form action='");
        *_gp_page += action;
        *_gp_page += F("' method='POST'>\n<input type='hidden' name='GP_form' value=''>\n");
        send();
    }
    void FORM_END() {
        SEND(F("</form>\n"));
    }
    void SUBMIT(const String& text, PGM_P st = GP_GREEN) {
        *_gp_page += F("<input type='submit' value='");
        *_gp_page += text;
        if (st != GP_GREEN) {
            *_gp_page += F("' style='background:");
            *_gp_page += FPSTR(st);
        }
        *_gp_page += F("'>\n");
        send();
    }
    void FORM_SUBMIT(const String& name, const String& text, PGM_P st = GP_GREEN) {
        FORM_BEGIN(name);
        SUBMIT(text, st);
        FORM_END();
    }
    void HIDDEN(const String& name, const String& text) {
        *_gp_page += F("<input type='hidden' name='");
        *_gp_page += name;
        *_gp_page += F("' value='");
        *_gp_page += text;
        *_gp_page += F("' id='");
        *_gp_page += name;
        *_gp_page += F("'>\n");
        send();
    }
    void FORM_SUBMIT(const String& name, const String& text, const String& namehidden, const String& valuehidden, PGM_P st = GP_GREEN) {
        FORM_BEGIN(name);
        HIDDEN(namehidden, valuehidden);
        SUBMIT(text, st);
        FORM_END();
    }

    // ======================= ОФОРМЛЕНИЕ =======================
    void GRID_BEGIN() {
        SEND(F("<div class='grid' id='grid'>\n"));
    }
    void GRID_END() {
        BLOCK_END();
    }
    void GRID_RESPONSIVE(int width) {
        *_gp_page += F("<style type='text/css'>@media screen and (max-width:");
        *_gp_page += width;
        *_gp_page += F("px){\n.grid{display:block;}\n#grid .block{margin:20px 10px;width:unset;}}</style>\n");
        send();
    }
    
    void BLOCK_BEGIN(const String& width = "") {
        *_gp_page += F("<div class='block' id='blockBack'");
        if (width.length()) {
            *_gp_page += F(" style='max-width:");
            *_gp_page += width;
            *_gp_page += "'";
        }
        *_gp_page += F(">\n");
        send();
    }
    void BLOCK_TAB_BEGIN(const String& label, const String& width = "", PGM_P st = GP_GREEN) {
        *_gp_page += F("<div class='block blockTab' id='blockBack'");
        if (width.length()) {
            *_gp_page += F(" style='max-width:");
            *_gp_page += width;
            *_gp_page += "'";
        }
        *_gp_page += F("><div class='blockHeader'");
        if (st != GP_GREEN) {
            *_gp_page += F(" style='background:");
            *_gp_page += FPSTR(st);
            *_gp_page += "'";
        }
        *_gp_page += F(">");
        *_gp_page += label;
        *_gp_page += F("</div>\n");
        send();
    }
    void BLOCK_THIN_BEGIN(const String& width = "") {
        *_gp_page += F("<div class='block thinBlock'");
        if (width.length()) {
            *_gp_page += F(" style='max-width:");
            *_gp_page += width;
            *_gp_page += "'";
        }
        *_gp_page += F(">\n");
        send();
    }
    void BLOCK_THIN_TAB_BEGIN(const String& label, const String& width = "") {
        *_gp_page += F("<div class='block blockTab thinBlock'");
        if (width.length()) {
            *_gp_page += F(" style='max-width:");
            *_gp_page += width;
            *_gp_page += "'";
        }
        *_gp_page += F("><div class='blockHeader thinTab'><span class='thinText'>");
        *_gp_page += label;
        *_gp_page += F("</span></div>\n");
        send();
    }
    void BLOCK_END() {
        SEND(F("</div>\n"));
    }
    
    void BOX_BEGIN(GPalign al = GP_EDGES, const String& w = "100%") {
        *_gp_page += F("<div style='width:");
        *_gp_page += w;
        *_gp_page += F(";justify-content:");
        *_gp_page += FPSTR(GPgetAlign(al));
        *_gp_page += F("' class='inliner'>\n");
        send();
    }
    void BOX_END() {
        SEND(F("</div>\n"));
    }
    
    void BREAK() {
        SEND(F("<br>\n"));
    }
    void HR() {
        SEND(F("<hr>\n"));
    }
    
    // ======================= ТЕКСТ =======================
    void TAG_RAW(const String& tag, const String& val, const String& name) {
        *_gp_page += F("<");
        *_gp_page += tag;
        if (name.length()) {
            *_gp_page += F(" id='");
            *_gp_page += name;
            *_gp_page += "'";
        }
        *_gp_page += F(">");
        *_gp_page += val;
        *_gp_page += F("</");
        *_gp_page += tag;
        *_gp_page += F(">\n");
        send();
    }
    
    void TITLE(const String& val, const String& name="") {
        TAG_RAW(F("h2"), val, name);
    }
    void LABEL(const String& val, const String& name="") {
        TAG_RAW(F("label"), val, name);
    }
    void SPAN(const String& val, const String& align = "", const String& name = "") {
        if (align.length()) {
            *_gp_page += F("<div style='text-align:");
            *_gp_page += align;
            *_gp_page += F("'>");
        } else *_gp_page += F("<div>");
        TAG_RAW(F("span"), val, name);
        *_gp_page += F("</div>");
        send();
    }
    void LABEL_BLOCK(const String& val, const String& name = "", PGM_P st = GP_GREEN) {
        *_gp_page += F("<label class='display' ");
        if (st != GP_GREEN) {
            *_gp_page += F("style='background:");
            *_gp_page += FPSTR(st);
            *_gp_page += F(";' ");
        }
        if (name.length()) {
            *_gp_page += F("id='");
            *_gp_page += name;
            *_gp_page += "'";
        }
        *_gp_page += F(">");
        *_gp_page += val;
        *_gp_page += F("</label>\n");
        send();
    }
    
    // ======================= ЛЕДЫ =======================
    void LED(const String& name, bool state = 0) {
        *_gp_page += F("<input class='ledn' type='radio' disabled ");
        if (state) *_gp_page += F("checked ");
        *_gp_page += F("name='");
        *_gp_page += name;
        *_gp_page += F("' id='");
        *_gp_page += name;
        *_gp_page += F("'>\n");
        send();
    }
    void LED_RED(const String& name, bool state = 0) {
        *_gp_page += F("<input class='led red' type='radio' disabled ");
        if (state) *_gp_page += F("checked ");
        *_gp_page += F("name='");
        *_gp_page += name;
        *_gp_page += F("' id='");
        *_gp_page += name;
        *_gp_page += F("'>\n");
        send();
    }
    void LED_GREEN(const String& name, bool state = 0) {
        *_gp_page += F("<input class='led green' type='radio' disabled ");
        if (state) *_gp_page += F("checked ");
        *_gp_page += F("name='");
        *_gp_page += name;
        *_gp_page += F("' id='");
        *_gp_page += name;
        *_gp_page += F("'>\n");
        send();
    }
    
    // ======================= ИКОНКИ =======================
    void ICON_SUPPORT() {
        SEND(F("<link rel='stylesheet' href='https://cdnjs.cloudflare.com/ajax/libs/font-awesome/4.7.0/css/font-awesome.min.css'>\n"));
    }
    
    String ICON(const String& name, int size = 0) {
        String s(F("<i class='fa fa-"));
        s += name;
        if (size) {
            s += F("' style='font-size:");
            s += size;
            s += F("px;");
        }
        s += F("'></i>");
        return s;
    }
    String ICON_FILE(const String& uri, int size) {
        String s(F("<i style='background-repeat:no-repeat;background-image:url("));
        s += uri;
        s += F(");width:");
        s += size;
        s += F("px;height:");
        s += size;
        s += F("px;'></i>");
        return s;
    }
    
    // ======================= НАВИГАЦИЯ =======================
    void NAV_TABS_LINKS(const String& urls, const String& names, PGM_P st = GP_GREEN) {
        *_gp_page += F("<div class='navtab'><ul ");
        if (st != GP_GREEN) {
            *_gp_page += F("style='background:");
            *_gp_page += FPSTR(st);
            *_gp_page += "'";
        }
        *_gp_page += F(">\n");
        GP_parser n;
        GP_parser u;
        while (n.parse(names)) {
            u.parse(urls);
            *_gp_page += F("<li ");
            if (_gp_uri->equals(u.str)) *_gp_page += F("style='background:#2a2d35' ");
            *_gp_page += F("onclick='location.href=\"");
            *_gp_page += u.str;
            *_gp_page += F("\";'>");
            *_gp_page += n.str;
            *_gp_page += F("</li>\n");
        }
        *_gp_page += F("</ul></div>\n");
        send();
    }
    
    void NAV_TABS_M(const String& name, const String& list, PGM_P st = GP_GREEN) {
        _gp_nav_pos = 0;
        *_gp_page += F("<div class='navtab'><ul ");
        if (st != GP_GREEN) {
            *_gp_page += F("style='background:");
            *_gp_page += FPSTR(st);
            *_gp_page += "' ";
        }
        *_gp_page += F(">\n");
        GP_parser p;
        while (p.parse(list)) {
            *_gp_page += F("<li ");
            if (!p.count) *_gp_page += F("style='background:#2a2d35' ");
            *_gp_page += F("' class='");
            *_gp_page += name;
            *_gp_page += F("' onclick='openTab(\"");
            *_gp_page += name;
            *_gp_page += '/';
            *_gp_page += p.count;
            *_gp_page += F("\",this,\"block_");
            *_gp_page += name;
            *_gp_page += F("\");GP_send(\"/GP_click?");
            *_gp_page += name;
            *_gp_page += '/';
            *_gp_page += p.count;
            *_gp_page += F("=\");'>");
            *_gp_page += p.str;
            *_gp_page += F("</li>\n");
        }
        *_gp_page += F("</ul></div>\n");
        send();
    }
    
    void NAV_BLOCK_BEGIN(const String& name, int pos) {
        *_gp_page += F("<div class='navblock block_");
        *_gp_page += name;
        *_gp_page += F("' id='");
        *_gp_page += name;
        *_gp_page += '/';
        *_gp_page += pos;
        *_gp_page += "' ";
        if (!pos) *_gp_page += F("style='display:block'");
        *_gp_page += F(">\n");
        send();
    }
    
    void NAV_TABS(const String& list, PGM_P st = GP_GREEN) {
        _gp_nav_id++;
        _gp_nav_pos = 0;
        *_gp_page += F("<div class='navtab'><ul ");
        if (st != GP_GREEN) {
            *_gp_page += F("style='background:");
            *_gp_page += FPSTR(st);
            *_gp_page += "' ";
        }
        *_gp_page += F(">\n");
        GP_parser p;
        while (p.parse(list)) {
            *_gp_page += F("<li ");
            if (!p.count) *_gp_page += F("style='background:#2a2d35' ");
            *_gp_page += F("class='nt-");
            *_gp_page += _gp_nav_id;
            *_gp_page += F("' onclick='openTab(\"ntab-");
            *_gp_page += _gp_nav_id;
            *_gp_page += '/';
            *_gp_page += p.count;
            *_gp_page += F("\",this,\"nb-");
            *_gp_page += _gp_nav_id;
            *_gp_page += F("\")'>");
            *_gp_page += p.str;
            *_gp_page += F("</li>\n");
        }
        *_gp_page += F("</ul></div>\n");
        send();
    }
    
    void NAV_BLOCK_BEGIN() {
        *_gp_page += F("<div class='navblock nb-");
        *_gp_page += _gp_nav_id;
        *_gp_page += F("' id='ntab-");
        *_gp_page += _gp_nav_id;
        *_gp_page += '/';
        *_gp_page += _gp_nav_pos;
        *_gp_page += "' ";
        if (!_gp_nav_pos) *_gp_page += F("style='display:block'");
        *_gp_page += F(">\n");
        send();
        _gp_nav_pos++;
    }
    
    void NAV_BLOCK_END() {
        SEND(F("</div>\n"));
    }
    
    // ======================= ФАЙЛЫ =======================
    void FILE_UPLOAD_RAW(const String& name, const String& text = "", const String& accept = "", const String& options = "", const String& action = "/GP_upload") {
        *_gp_page += F("<form action='");
        *_gp_page += action;
        *_gp_page += F("' method='POST' enctype='multipart/form-data' id='");
        *_gp_page += name;
        *_gp_page += F("_form' style='display:flex;justify-content:center;'>\n"
        "<div id='ubtn' onclick='saveFile(\"");
        *_gp_page += name;
        *_gp_page += F("_inp\")'>");
        *_gp_page += text;
        *_gp_page += F("</div>\n"
        "<div id='ubtnclr'><input ");
        *_gp_page += options;
        *_gp_page += F("name='");
        *_gp_page += name;
        if (accept.length()) {
            *_gp_page += F("' accept='");
            *_gp_page += accept;
        }
        *_gp_page += F("' id='");
        *_gp_page += name;
        *_gp_page += F("_inp' type='file' onchange='GP_subm(\"");
        *_gp_page += name;
        *_gp_page += F("_form\")'/></div>\n"
        "</form>\n");
        send();
    }
    
    void FILE_UPLOAD(const String& name, const String& text = "", const String& accept = "") {
        FILE_UPLOAD_RAW(name, "📄 " + text, accept, F("multiple "));
    }
    
    void FOLDER_UPLOAD(const String& name, const String& text = "") {
        FILE_UPLOAD_RAW(name, "📁 " + text, "", F("multiple webkitdirectory allowdirs "));
    }
	
	void OTA_FIRMWARE(const String& text = "OTA firmware") {
        FILE_UPLOAD_RAW(F("firmware"), "🧱 " + text, F(".bin,.bin.gz"), "", F("/GP_OTAupload"));
    }
	
	void OTA_FILESYSTEM(const String& text = "OTA filesystem") {
        FILE_UPLOAD_RAW(F("filesystem"), "💽 " + text, F(".bin,.bin.gz"), "", F("/GP_OTAupload"));
    }
    
    void IMAGE(const String& uri, const String& w = "") {
        *_gp_page += F("<img src='");
        *_gp_page += uri;
        *_gp_page += F("' style='width:");
        *_gp_page += w;
        *_gp_page += F("'>\n");
        send();
    }
    void VIDEO(const String& uri, const String& w = "") {
        *_gp_page += F("<video src='");
        *_gp_page += uri;
        *_gp_page += F("' style='width:");
        *_gp_page += w;
        *_gp_page += F("' controls>Browser doesn't support video.</video>\n");
        send();
    }
    void EMBED(const String& uri, const String& w = "100%", const String& h = "") {
        *_gp_page += F("<embed src='");
        *_gp_page += uri;
        *_gp_page += F("' style='width:90%;border-radius:5px;background:white;width:");
        *_gp_page += w;
        if (h.length()) {
            *_gp_page += F(";height:");
            *_gp_page += h;
        }
        *_gp_page += F(";'>\n");
        send();
    }
    
    // ======================= КНОПКА =======================
    void BUTTON_RAW(const String& name, const String& value, const String& tar, PGM_P st, const String& width = "", const String& cls = "", bool dis = 0, bool rel = 0) {
        *_gp_page += F("<input type='button' ");
        if (cls.length()) {
            *_gp_page += F("class='");
            *_gp_page += cls;
            *_gp_page += "' ";
        }
        *_gp_page += F("style='");
        if (st != GP_GREEN) {
            *_gp_page += F("background:");
            *_gp_page += FPSTR(st);
            *_gp_page += ';';
        }
        if (width.length()) {
            *_gp_page += F("width:");
            *_gp_page += width;
            *_gp_page += ';';
        }
        *_gp_page += F("' name='");
        *_gp_page += name;
        *_gp_page += F("' value='");
        *_gp_page += value;
        *_gp_page += F("' onmousedown='GP_clickUD(this,0)' onmouseup='GP_clickUD(this,1)' ");
        if (tar.length()) {
            *_gp_page += F("onclick=\"GP_clickid('");
            *_gp_page += name;
            *_gp_page += F("','");
            *_gp_page += tar;
            *_gp_page += F("')\"");
        } else {
            *_gp_page += F("onclick='GP_click(this,");
            *_gp_page += rel ? reloadTimeout : 0;
            *_gp_page += F(")'");
        }
        if (dis) *_gp_page += F(" disabled");
        *_gp_page += F(">\n");
        send();
    }
    
    void BUTTON(const String& name, const String& value, const String& tar="", PGM_P st = GP_GREEN, const String& width = "", bool dis = 0, bool rel = 0) {
        BUTTON_RAW(name, value, tar, st, width, "", dis, rel);
    }
    void BUTTON_MINI(const String& name, const String& value, const String& tar="", PGM_P st = GP_GREEN, const String& width = "", bool dis = 0, bool rel = 0) {
        BUTTON_RAW(name, value, tar, st, width, F("miniButton"), dis, rel);
    }
    
    // ======================= КНОПКА-ССЫЛКА =======================
    void BUTTON_LINK_RAW(const String& url, const String& value, PGM_P st = GP_GREEN, const String& width = "", const String& cls = "", const String& name = "") {
        *_gp_page += F("<input type='button' ");
        if (name.length()) {
            *_gp_page += F("name='");
            *_gp_page += name;
            *_gp_page += "' "; 
        }
        if (cls.length()) {
            *_gp_page += F("class='");
            *_gp_page += cls;
            *_gp_page += "' ";
        }
        *_gp_page += F("style='");
        if (st != GP_GREEN) {
            *_gp_page += F("background:");
            *_gp_page += FPSTR(st);
            *_gp_page += ';';
        }
        if (width.length()) {
            *_gp_page += F("width:");
            *_gp_page += width;
            *_gp_page += ';';
        }
        *_gp_page += F("' value='");
        *_gp_page += value;
        
        if (name.length()){
            *_gp_page += F("' onclick='GP_click(this);setTimeout(function(){location.href=\"");
            *_gp_page += url;
            *_gp_page += F("\";},");
            *_gp_page += reloadTimeout;
            *_gp_page += F(");'>\n");
        } else {
            *_gp_page += F("' onclick='location.href=\"");
            *_gp_page += url;
            *_gp_page += F("\";'>\n");
        }
        send();
    }
    void BUTTON_LINK(const String& url, const String& value, PGM_P st = GP_GREEN, const String& width = "", const String& name = "") {
        BUTTON_LINK_RAW(url, value, st, width, "", name);
    }
    void BUTTON_MINI_LINK(const String& url, const String& value, PGM_P st = GP_GREEN, const String& width = "", const String& name = "") {
        BUTTON_LINK_RAW(url, value, st, width, F("miniButton"), name);
    }
    
    // ==================== КНОПКА-СКАЧКА ====================
    void BUTTON_DOWNLOAD_RAW(const String& url, const String& value, PGM_P st = GP_GREEN, const String& width = "", const String& cls = "") {
        *_gp_page += F("<a style='text-decoration:none;' href='");
        *_gp_page += url;
        *_gp_page += F("' download><input type='button' value='");
        *_gp_page += value;
        *_gp_page += "' ";
        if (cls.length()) {
            *_gp_page += F("class='");
            *_gp_page += cls;
            *_gp_page += "' ";
        }
        *_gp_page += F("style='");
        if (st != GP_GREEN) {
            *_gp_page += F("background:");
            *_gp_page += FPSTR(st);
            *_gp_page += ';';
        }
        if (width.length()) {
            *_gp_page += F("width:");
            *_gp_page += width;
            *_gp_page += ';';
        }
        *_gp_page += F("'>");
        *_gp_page += F("</a>\n");
        send();
    }
    void BUTTON_DOWNLOAD(const String& url, const String& value, PGM_P st = GP_GREEN, const String& width = "") {
        BUTTON_DOWNLOAD_RAW(url, value, st, width);
    }
    void BUTTON_MINI_DOWNLOAD(const String& url, const String& value, PGM_P st = GP_GREEN, const String& width = "") {
        BUTTON_DOWNLOAD_RAW(url, value, st, width, F("miniButton"));
    }
    
    // ========================= ВВОД ========================
    void NUMBER_RAW(const String& name, const String& place, const String& value, const String& minv, const String& maxv, const String& width, bool dis) {
        *_gp_page += F("<input type='number' step='any' name='");
        *_gp_page += name;
        *_gp_page += F("' id='");
        *_gp_page += name;
        if (value.length()) {
            *_gp_page += F("' value='");
            *_gp_page += value;
        }
        if (width.length()) {
            *_gp_page += F("' style='width:");
            *_gp_page += width;
        }
        if (minv.length()) {
            *_gp_page += F("' min='");
            *_gp_page += minv;
        }
        if (maxv.length()) {
            *_gp_page += F("' max='");
            *_gp_page += maxv;
        }
        *_gp_page += F("' placeholder='");
        *_gp_page += place;
        *_gp_page += F("' onchange='GP_click(this)'");
        if (dis) *_gp_page += F(" disabled");
        *_gp_page += F(">\n");
        send();
    }
    void NUMBER(const String& name, const String& place, int value = INT32_MAX, const String& width = "", bool dis = false) {
        NUMBER_RAW(name, place, (value == INT32_MAX ? String("") : String(value)), "", "", width, dis);
    }
    void NUMBER_F(const String& name, const String& place, float value = NAN, uint8_t dec = 2, const String& width = "", bool dis = false) {
        NUMBER_RAW(name, place, (isnan(value) ? String("") : String(value, (uint16_t)dec)), "", "", width, dis);
    }
    
    void TEXT(const String& name, const String& place = "", const String& value = "", const String& width = "", bool dis = false) {
        *_gp_page += F("<input type='text' name='");
        *_gp_page += name;
        *_gp_page += F("' id='");
        *_gp_page += name;
        *_gp_page += F("' value='");
        *_gp_page += value;
        if (width.length()) {
            *_gp_page += F("' style='width:");
            *_gp_page += width;
        }
        *_gp_page += F("' placeholder='");
        *_gp_page += place;
        *_gp_page += F("' onchange='GP_click(this)'");
        if (dis) *_gp_page += F(" disabled");
        *_gp_page += F(">\n");
        send();
    }
    void PASS(const String& name, const String& place = "", const String& value = "", const String& width = "") {
        *_gp_page += F("<input type='password' name='");
        *_gp_page += name;
        *_gp_page += F("' id='");
        *_gp_page += name;
        *_gp_page += F("' value='");
        *_gp_page += value;
        if (width.length()) {
            *_gp_page += F("' style='width:");
            *_gp_page += width;
        }
        *_gp_page += F("' placeholder='");
        *_gp_page += place;
        *_gp_page += F("' onchange='GP_click(this)'>\n");
        send();
    }
    
    void AREA(const String& name, int rows = 1, const String& value = "", bool dis = false) {
        *_gp_page += F("<textarea onchange='GP_click(this)' style='height:auto' name='");
        *_gp_page += name;
        *_gp_page += F("' id='");
        *_gp_page += name;
        *_gp_page += F("' rows='");
        *_gp_page += rows;
        *_gp_page += "'";
        if (dis) *_gp_page += F(" disabled");
        *_gp_page += ">";
        if (value.length()) *_gp_page += value;
        *_gp_page += F("</textarea>\n");
        send();
    }
    
    void AREA_LOG(int rows = 5, int prd = 1000) {
        *_gp_page += F("<textarea style='height:auto' id='GP_log' rows='");
        *_gp_page += rows;
        *_gp_page += F("' disabled></textarea>\n");
        *_gp_page += F("<script>setInterval(function(){var xhttp=new XMLHttpRequest();\n"
        "xhttp.onreadystatechange=function(){\n"
        "if(this.readyState==4&&this.status==200){\n"
        "var elm=document.getElementById('GP_log');\n"
        "elm.innerHTML+=this.responseText;elm.scrollTop=elm.scrollHeight;}};\n"
        "xhttp.open('GET','/GP_log',true);xhttp.send();},");
        *_gp_page += prd;
        *_gp_page += F(");</script>\n");
        send();
    }
    
    void AREA_LOG(GPlog& log, int rows = 5, int prd = 1000) {
        log.clear();
        *_gp_page += F("<textarea style='height:auto' id='");
        *_gp_page += log.name;
        *_gp_page += F("' rows='");
        *_gp_page += rows;
        *_gp_page += F("' disabled></textarea>\n");
        *_gp_page += F("<script>setInterval(function(){var xhttp=new XMLHttpRequest();\n"
        "xhttp.onreadystatechange=function(){\n"
        "if(this.readyState==4&&this.status==200){\n"
        "var elm=document.getElementById('");
        *_gp_page += log.name;
        *_gp_page += F("');\n"
        "elm.innerHTML+=this.responseText;elm.scrollTop=elm.scrollHeight;}};\n"
        "xhttp.open('GET','/GP_update?");
        *_gp_page += log.name;
        *_gp_page += F("=',true);xhttp.send();},");
        *_gp_page += prd;
        *_gp_page += F(");</script>\n");
        send();
    }
    
    // ======================= НАСТРОЙКА =======================
    void CHECK(const String& name, bool state = 0, bool dis = false) {
        *_gp_page += F("<input type='checkbox' name='");
        *_gp_page += name;
        *_gp_page += F("' id='");
        *_gp_page += name;
        *_gp_page += "' ";
        if (state) *_gp_page += F("checked ");
        if (dis) *_gp_page += F("disabled ");
        *_gp_page += F("onclick='GP_click(this)'>\n");
        *_gp_page += F("<input type='hidden' value='0' name='");
        *_gp_page += name;
        *_gp_page += "'>";
        send();
    }
    void SWITCH(const String& name, bool state = 0, bool dis = false) {
        *_gp_page += F("<label class='switch'><input type='checkbox' name='");
        *_gp_page += name;
        *_gp_page += F("' id='");
        *_gp_page += name;
        *_gp_page += "' ";
        if (state) *_gp_page += F("checked ");
        if (dis) *_gp_page += F("disabled ");
        *_gp_page += F("onclick='GP_click(this)'>\n");
        *_gp_page += F("<span class='slider'></span></label>");
        *_gp_page += F("<input type='hidden' value='0' name='");
        *_gp_page += name;
        *_gp_page += "'>";
        send();
    }
    
    void DATE(const String& name, bool dis = false) {
        GPdate d;
        DATE(name, d, dis);
    }
    void DATE(const String& name, GPdate d, bool dis = false) {
        *_gp_page += F("<input step='any' type='date' name='");
        *_gp_page += name;
        *_gp_page += F("' id='");
        *_gp_page += name;
        if (d.year) {
            *_gp_page += F("' value='");
            *_gp_page += d.encode();
        }
        *_gp_page += "' ";
        if (dis) *_gp_page += F("disabled ");
        *_gp_page += F("onchange='GP_click(this)'>\n");
        send();
    }
    
    void TIME(const String& name, bool dis = false) {
        *_gp_page += F("<input step='any' type='time' name='");
        *_gp_page += name;
        *_gp_page += F("' id='");
        *_gp_page += name;
        *_gp_page += "' ";
        if (dis) *_gp_page += F("disabled ");
        *_gp_page += F("onchange='GP_click(this)'>\n");
        send();
    }
    void TIME(const String& name, GPtime t, bool dis = false) {
        *_gp_page += F("<input step='any' type='time' name='");
        *_gp_page += name;
        *_gp_page += F("' id='");
        *_gp_page += name;
        *_gp_page += F("' value='");
        *_gp_page += t.encode();
        *_gp_page += "' ";
        if (dis) *_gp_page += F("disabled ");
        *_gp_page += F("onchange='GP_click(this)'>\n");
        send();
    }
    
    void SLIDER(const String& name, float value = 0, float min = 0, float max = 100, float step = 1, uint8_t dec = 0, PGM_P st = GP_GREEN, bool dis = false) {
        *_gp_page += F("<input type='range' name='");
        *_gp_page += name;
        *_gp_page += F("' id='");
        *_gp_page += name;
        *_gp_page += F("' value='");
        *_gp_page += value;
        *_gp_page += F("' min='");
        *_gp_page += min;
        *_gp_page += F("' max='");
        *_gp_page += max;
        *_gp_page += F("' step='");
        *_gp_page += step;
        *_gp_page += F("' style='background-image:linear-gradient(");
        *_gp_page += FPSTR(st);
        *_gp_page += ',';
        *_gp_page += FPSTR(st);
        *_gp_page += F(");' onload='GP_change(this)' onchange='GP_click(this)' oninput='GP_change(this)' onmousewheel='GP_wheel(this);GP_change(this);GP_click(this)' ");
        if (dis) *_gp_page += F("disabled");
        *_gp_page += F(">\n");
        *_gp_page += F("<output id='");
        *_gp_page += name;
        *_gp_page += F("_val' ");
        if (st != GP_GREEN) {
            *_gp_page += F("style='background:");
            *_gp_page += FPSTR(st);
            *_gp_page += F(";'");
        }
        *_gp_page += F(">");
        if (!dec) *_gp_page += (int)round(value);
        else *_gp_page += String(value, (uint16_t)dec);
        *_gp_page += F("</output>\n");
        send();
    }
    
    void SPIN_BTN(const String& name, float step, bool dir, PGM_P st, uint8_t dec) {
        *_gp_page += F("<input type='button' class='spinBtn' onclick='GP_spin(\"");
        *_gp_page += name;
        *_gp_page += F("\",");
        *_gp_page += dir ? step : -step;
        *_gp_page += ',';
        *_gp_page += dec;
        *_gp_page += F(")' ");
        *_gp_page += F("value='");
        *_gp_page += dir ? '+' : '-';
        *_gp_page += "' ";
        if (st != GP_GREEN) {
            *_gp_page += F(" style='background:");
            *_gp_page += FPSTR(st);
            *_gp_page += F(";'");
        }
        *_gp_page += F(">\n");
    }
    void SPINNER(const String& name, float value = 0, float min = NAN, float max = NAN, float step = 1, uint8_t dec = 0, PGM_P st = GP_GREEN, const String& width = "") {
        *_gp_page += F("<div id='spinner' class='spinner'>\n");
        SPIN_BTN(name, step, false, st, dec);
        *_gp_page += F("<input type='number' name='");
        *_gp_page += name;
        *_gp_page += F("' id='");
        *_gp_page += name;
        *_gp_page += F("' step='");
        *_gp_page += String(step, (uint16_t)dec);
        if (width.length()) {
            *_gp_page += F("' style='width:");
            *_gp_page += width;
        }
        *_gp_page += F("' onchange='GP_click(this)' onmousewheel='GP_wheel(this);GP_click(this)' value='");
        if (!dec) *_gp_page += (int)round(value);
        else *_gp_page += String(value, (uint16_t)dec);
        if (!isnan(min)) {
            *_gp_page += F("' min='");
            *_gp_page += min;
        }
        if (!isnan(max)) {
            *_gp_page += F("' max='");
            *_gp_page += max;
        }
        *_gp_page += F("'>\n");
        SPIN_BTN(name, step, true, st, dec);
        *_gp_page += F("</div>\n");
        send();
    }
    
    void COLOR(const String& name, uint32_t value = 0, bool dis = false) {
        *_gp_page += F("<input type='color' name='");
        *_gp_page += name;
        *_gp_page += F("' id='");
        *_gp_page += name;
        *_gp_page += F("' value='");
        GPcolor col(value);
        *_gp_page += col.encode();
        *_gp_page += "' ";
        if (dis) *_gp_page += F("disabled ");
        *_gp_page += F("onchange='GP_click(this)'>\n");
        send();
    }
    void COLOR(const String& name, GPcolor col, bool dis = false) {
        *_gp_page += F("<input type='color' name='");
        *_gp_page += name;
        *_gp_page += F("' id='");
        *_gp_page += name;
        *_gp_page += F("' value='");
        *_gp_page += col.encode();
        *_gp_page += "' ";
        if (dis) *_gp_page += F("disabled ");
        *_gp_page += F("onchange='GP_click(this)'>\n");
        send();
    }
    
    void SELECT(const String& name, const String& list, int sel = 0, bool nums = 0, bool dis = false, bool rel = 0) {
        if (sel < 0) sel = 0;
        *_gp_page += F("<select name='");
        *_gp_page += name;
        *_gp_page += F("' id='");
        *_gp_page += name;
        *_gp_page += "' ";
        if (dis) *_gp_page += F("disabled ");
        *_gp_page += F("onchange='GP_click(this,");
		*_gp_page += rel ? reloadTimeout : 0;
		*_gp_page += F(")'>\n");
        
        GP_parser p;
        int idx = 0; 
        while (p.parse(list)) {
            *_gp_page += F("<option value='");
            *_gp_page += idx;
            *_gp_page += "'";
            if (p.count == sel) *_gp_page += F(" selected");
            *_gp_page += F(">");
            if (nums) {
                *_gp_page += idx;
                *_gp_page += ". ";
            }
            *_gp_page += p.str;
            *_gp_page += F("</option>\n");
            idx++;
        }
        *_gp_page += F("</select>\n");
        send();
    }
    void SELECT(const String& name, String* list, int sel = 0, bool nums = 0, bool dis = false, bool rel = 0) {
        if (sel < 0) sel = 0;
        *_gp_page += F("<select name='");
        *_gp_page += name;
        *_gp_page += F("' id='");
        *_gp_page += name;
        *_gp_page += "' ";
        if (dis) *_gp_page += F("disabled ");
        *_gp_page += F("onchange='GP_click(this,");
		*_gp_page += rel ? reloadTimeout : 0;
		*_gp_page += F(")'>\n");
        int idx = 0; 
        while (list[idx].length()) {
            *_gp_page += F("<option value='");
            *_gp_page += idx;
            *_gp_page += "'";
            if (idx == sel) *_gp_page += F(" selected");
            *_gp_page += F(">");
            if (nums) {
                *_gp_page += idx;
                *_gp_page += ". ";
            }
            *_gp_page += list[idx];
            *_gp_page += F("</option>\n");
            idx++;
        }
        *_gp_page += F("</select>\n");
        send();
    }
    void SELECT(const String& name, char** list, int sel = 0, bool nums = 0, bool dis = false, bool rel = 0) {
        if (sel < 0) sel = 0;
        *_gp_page += F("<select name='");
        *_gp_page += name;
        *_gp_page += F("' id='");
        *_gp_page += name;
        *_gp_page += "' ";
        if (dis) *_gp_page += F("disabled ");
        *_gp_page += F("onchange='GP_click(this,");
		*_gp_page += rel ? reloadTimeout : 0;
		*_gp_page += F(")'>\n");
        int idx = 0; 
        while (list[idx]!=nullptr) {
            *_gp_page += F("<option value='");
            *_gp_page += idx;
            *_gp_page += "'";
            if (idx == sel) *_gp_page += F(" selected");
            *_gp_page += F(">");
            if (nums) {
                *_gp_page += idx;
                *_gp_page += ". ";
            }
            *_gp_page += list[idx];
            *_gp_page += F("</option>\n");
            idx++;
        }
        *_gp_page += F("</select>\n");
        send();
    }
    
    // ======================= ГРАФИКИ =======================
    template <int ax, int am>
    void PLOT(const String& id, const char** labels, int16_t vals[ax][am], int dec = 0, int height = 400) {
        *_gp_page += F("<script type='text/javascript' src='https://www.gstatic.com/charts/loader.js'></script>\n"
        "<script type='text/javascript'>\n"
        "google.charts.load('current', {'packages':['corechart']});\n"
        "google.charts.setOnLoadCallback(drawChart);\n"
        "function drawChart() {\n"
        "var data = google.visualization.arrayToDataTable([\n");
        send();
        
        *_gp_page += '[';
        for (int i = 0; i < ax+1; i++) {
            *_gp_page += '\'';
            if (i) *_gp_page += labels[i-1];
            *_gp_page += "',";
        }
        *_gp_page += "],\n";
        for (int j = 0; j < am; j++) {
            *_gp_page += '[';
            for (int i = 0; i < ax+1; i++) {
                if (!i) *_gp_page += '\'';
                if (!i) *_gp_page += j;
                else {
                    if (dec) *_gp_page += (float)vals[i-1][j] / dec;
                    else *_gp_page += vals[i-1][j];
                }
                if (!i) *_gp_page += '\'';
                *_gp_page += ',';
            }
            *_gp_page += F("],\n");
        }
        send();
        
        *_gp_page += F("]);var options = {pointSize:5,curveType:'function','chartArea':{'width':'90%','height':'90%'},\n"
        "backgroundColor:'none',hAxis:{title:''},vAxis:{title:''},\n"
        "legend: {position:'bottom'}};\n"
        "var chart = new google.visualization.LineChart(document.getElementById('");
        *_gp_page += id;
        *_gp_page += F("'));\n");
        *_gp_page += F("chart.draw(data, options);}\n"
        "</script><div id='");
        *_gp_page += id;
        *_gp_page += F("' class='chartBlock' style='height:");
        *_gp_page += height;
        *_gp_page += F("px'></div>\n");
        send();
    }
    
    template <int ax, int am>
    void PLOT_DARK(const String& id, const char** labels, int16_t vals[ax][am], int dec = 0, int height = 400) {
        *_gp_page += F("<script type='text/javascript' src='https://www.gstatic.com/charts/loader.js'></script>\n"
        "<script type='text/javascript'>\n"
        "google.charts.load('current', {'packages':['corechart']});\n"
        "google.charts.setOnLoadCallback(drawChart);\n"
        "function drawChart() {\n"
        "var data = google.visualization.arrayToDataTable([\n");
        send();
        
        *_gp_page += '[';
        for (int i = 0; i < ax+1; i++) {
            *_gp_page += '\'';
            if (i) *_gp_page += labels[i-1];
            *_gp_page += "',";
        }
        *_gp_page += "],\n";
        for (int j = 0; j < am; j++) {
            *_gp_page += '[';
            for (int i = 0; i < ax+1; i++) {
                if (!i) *_gp_page += '\'';
                if (!i) *_gp_page += j;
                else {
                    if (dec) *_gp_page += (float)vals[i-1][j] / dec;
                    else *_gp_page += vals[i-1][j];
                }
                if (!i) *_gp_page += '\'';
                *_gp_page += ',';
            }
            *_gp_page += F("],\n");
        }
        send();
        
        *_gp_page += F("]);var options = {pointSize:5,curveType:'function','chartArea':{'width':'90%','height':'90%'},\n"
        "backgroundColor:'none',hAxis:{title:'',titleTextStyle:{color:'#ddd'},textStyle:{color:'#bbb'}},\n"
        "vAxis:{title:'',titleTextStyle:{color:'#ddd'},gridlines:{color:'#777'},textStyle:{color:'#bbb'}},\n"
        "legend: {position:'bottom',textStyle:{color:'#eee'}}};\n"
        "var chart = new google.visualization.LineChart(document.getElementById('");
        *_gp_page += id;
        *_gp_page += F("'));\n");
        *_gp_page += F("chart.draw(data, options);}\n");
        *_gp_page += F("</script><div id='");
        *_gp_page += id;
        *_gp_page += F("' class='chartBlock' style='height:");
        *_gp_page += height;
        *_gp_page += F("px'></div>\n");
        send();
    }
    
    void AJAX_PLOT(const String& name, int axes, int xamount = 20, int prd = 1000, int height = 400, bool local = 0) {
        *_gp_page += F("<script src='");
        
        if (local) *_gp_page += F("/gp_data/AJAX_PLOT.js");
        else *_gp_page += F("https://code.highcharts.com/highcharts.js");
        
        *_gp_page += F("'></script>\n<div id='");
        *_gp_page += name;
        *_gp_page += F("' class='chartBlock' style='height:");
        *_gp_page += height;
        *_gp_page += F("px'></div><script>\n var ");
        *_gp_page += name;
        *_gp_page += F("=new Highcharts.Chart({\n"
        "chart:{borderRadius:10,renderTo:'");
        *_gp_page += name;
        *_gp_page += F("',style:{fontFamily:'sans-serif'}},\n"
        "title:{text:''},"
        "series:[");
        for (int i = 0; i < axes; i++) {
            *_gp_page += F("{data:[]}");
            if (i != axes - 1) *_gp_page += ',';
        }
        *_gp_page += F("],\n"
        "xAxis:{type:'datetime',dateTimeLabelFormats:{second:'%H:%M:%S'}},\n"
        "yAxis:{title:{enabled:false}},"
        "credits:{enabled:false}});\n"
        "setInterval(function(){var xhttp=new XMLHttpRequest();var ch=");
        *_gp_page += name;
        *_gp_page += F("\n"
        "xhttp.onreadystatechange=function(){if(this.readyState==4&&this.status==200){\n"
        "var x=new Date().getTime()-new Date().getTimezoneOffset()*60000;"
        "var arr=this.responseText.split(',');"
        "var move=(ch.series[0].data.length>");
        *_gp_page += xamount;
        *_gp_page += F(");\n"
        "for(let i=0;i<arr.length;i++)ch.series[i].addPoint([x,Number(arr[i])],true,move,true);\n"
        "}};xhttp.open('GET','/GP_update?");
        *_gp_page += name;
        *_gp_page += F("=',true);xhttp.send();},\n");
        *_gp_page += prd;
        *_gp_page += F(");</script>\n");
        send();
    }
    
    void AJAX_PLOT_DARK(const String& name, int axes, int xamount = 20, int prd = 1000, int height = 400, bool local = 0) {
        if (local) *_gp_page += F("<script src='/gp_data/AJAX_PLOT.js'></script>\n"
        "<script src='/gp_data/AJAX_PLOT_DARK.js'></script>\n");
        else *_gp_page += F("<script src='https://code.highcharts.com/highcharts.js'></script>\n"
        "<script src='https://code.highcharts.com/themes/dark-unica.js'></script>\n");
        
        *_gp_page += F("<div id='");
        *_gp_page += name;
        *_gp_page += F("' class='chartBlock' style='height:");
        *_gp_page += height;
        *_gp_page += F("px'></div><script>\n var ");
        *_gp_page += name;
        *_gp_page += F("=new Highcharts.Chart({\n"
        "chart:{borderRadius:10,renderTo:'");
        *_gp_page += name;
        *_gp_page += F("',style:{fontFamily:'sans-serif'}},\n"
        "title:{text:''},"
        "series:[");
        for (int i = 0; i < axes; i++) {
            *_gp_page += F("{data:[]}");
            if (i != axes - 1) *_gp_page += ',';
        }
        *_gp_page += F("],\n"
        "xAxis:{type:'datetime',dateTimeLabelFormats:{second:'%H:%M:%S'}},\n"
        "yAxis:{title:{enabled:false}},"
        "credits:{enabled:false}});\n"
        "setInterval(function(){var xhttp=new XMLHttpRequest();var ch=");
        *_gp_page += name;
        *_gp_page += F("\n"
        "xhttp.onreadystatechange=function(){if(this.readyState==4&&this.status==200){\n"
        "var x=new Date().getTime()-new Date().getTimezoneOffset()*60000;"
        "var arr=this.responseText.split(',');"
        "var move=(ch.series[0].data.length>");
        *_gp_page += xamount;
        *_gp_page += F(");\n"
        "for(let i=0;i<arr.length;i++)ch.series[i].addPoint([x,Number(arr[i])],true,move,true);\n"
        "}};xhttp.open('GET','/GP_update?");
        *_gp_page += name;
        *_gp_page += F("=',true);xhttp.send();},\n");
        *_gp_page += prd;
        *_gp_page += F(");</script>\n");
        send();
    }
    
    template <int ax, int am>
    void PLOT_STOCK(const String& id, const char** labels, uint32_t* times, int16_t vals[ax][am], int dec = 0, int height = 400, bool local = 0) {
        *_gp_page += F("<script src='");
        
        if (local) *_gp_page += F("/gp_data/PLOT_STOCK.js");
        else *_gp_page += F("https://code.highcharts.com/stock/highstock.js");
        
        *_gp_page += F("'></script>\n <div class='chartBlock' style='height:");
        *_gp_page += height;
        *_gp_page += F("px' id='");
        *_gp_page += id;
        *_gp_page += F("'></div>");
        *_gp_page += F("<script>Highcharts.stockChart('");
        *_gp_page += id;
        *_gp_page += F("',{chart:{},\n"
        "rangeSelector:{buttons:[\n"
        "{count:1,type:'minute',text:'1M'},\n"
        "{count:1,type:'hour',text:'1H'},\n"
        "{count:1,type:'day',text:'1D'},\n"
        "{type:'all',text:'All'}],\n"
        "inputEnabled:false,selected:0},\n"
        "time:{useUTC:false},\n"
        "credits:{enabled:false},series:[\n");
        send();
        for (int axs = 0; axs < ax; axs++) {
            *_gp_page += F("{name:'");
            *_gp_page += labels[axs];
            *_gp_page += F("',data:[\n");
            for (int ams = 0; ams < am; ams++) {
                *_gp_page += '[';
                *_gp_page += times[ams];
                *_gp_page += F("000");
                *_gp_page += ',';
                if (dec) *_gp_page += (float)vals[axs][ams] / dec;
                else *_gp_page += vals[axs][ams];
                *_gp_page += "],\n";
                send();
            }
            *_gp_page += "]},\n";
        }
        *_gp_page += F("]});</script>\n");
        send();
    }
    
    template <int ax, int am>
    void PLOT_STOCK_DARK(const String& id, const char** labels, uint32_t* times, int16_t vals[ax][am], int dec = 0, int height = 400, bool local = 0) {
        if (local) *_gp_page += F("<script src='/gp_data/PLOT_STOCK.js'></script>\n"
        "<script src='/gp_data/PLOT_STOCK_DARK.js'></script>\n");
        else *_gp_page += F("<script src='https://code.highcharts.com/stock/highstock.js'></script>\n"
        "<script src='https://code.highcharts.com/themes/dark-unica.js'></script>\n");
        
        *_gp_page += F("<div class='chartBlock' style='height:");
        *_gp_page += height;
        *_gp_page += F("px' id='");
        *_gp_page += id;
        *_gp_page += F("'></div>");
        *_gp_page += F("<script>Highcharts.stockChart('");
        *_gp_page += id;
        *_gp_page += F("',{chart:{},\n"
        "rangeSelector:{buttons:[\n"
        "{count:1,type:'minute',text:'1M'},\n"
        "{count:1,type:'hour',text:'1H'},\n"
        "{count:1,type:'day',text:'1D'},\n"
        "{type:'all',text:'All'}],\n"
        "inputEnabled:false,selected:0},\n"
        "time:{useUTC:false},\n"
        "credits:{enabled:false},series:[\n");
        send();
        for (int axs = 0; axs < ax; axs++) {
            *_gp_page += F("{name:'");
            *_gp_page += labels[axs];
            *_gp_page += F("',data:[\n");
            for (int ams = 0; ams < am; ams++) {
                *_gp_page += '[';
                *_gp_page += times[ams];
                *_gp_page += F("000");
                *_gp_page += ',';
                if (dec) *_gp_page += (float)vals[axs][ams] / dec;
                else *_gp_page += vals[axs][ams];
                *_gp_page += "],\n";
                send();
            }
            *_gp_page += "]},\n";
        }
        *_gp_page += F("]});</script>\n");
        send();
    }
    
    
    // ================== ОТПРАВКА ОБЪЕКТОВ ===================
    void TITLE(GP_TITLE& title) {
        TITLE(title.text, title.name);
    }
    void LABEL(GP_LABEL& label) {
        LABEL(label.text, label.name);
    }
    void LABEL_BLOCK(GP_LABEL_BLOCK& label) {
        LABEL_BLOCK(label.text, label.name, label.style);
    }
    
    void LED(GP_LED& led) {
        LED(led.name, led.state);
    }
    void LED_RED(GP_LED_RED& led) {
        LED_RED(led.name, led.state);
    }
    void LED_GREEN(GP_LED_GREEN& led) {
        LED_GREEN(led.name, led.state);
    }
    
    void BUTTON(GP_BUTTON& btn) {
        BUTTON(btn.name, btn.text, btn.target, btn.style, btn.width);
    }
    void BUTTON_MINI(GP_BUTTON_MINI& btn) {
        BUTTON_MINI(btn.name, btn.text, btn.target, btn.style, btn.width);
    }
    
    void NUMBER(GP_NUMBER& num) {
        NUMBER_RAW(num.name, num.placeholder, (num.value == INT32_MAX ? String("") : String(num.value)), num.min, num.max, num.width, num.disabled);
    }
    void NUMBER_F(GP_NUMBER_F& num) {
        NUMBER_F(num.name, num.placeholder, num.value, num.decimals, num.width, num.disabled);
        NUMBER_RAW(num.name, num.placeholder, (isnan(num.value) ? String("") : String(num.value, (uint16_t)num.decimals)), num.min, num.max, num.width, num.disabled);
    }
    
    void TEXT(GP_TEXT& num) {
        TEXT(num.name, num.placeholder, num.text, num.width, num.disabled);
    }
    void PASS(GP_PASS& num) {
        PASS(num.name, num.placeholder, num.text, num.width);
    }
    
    void AREA(GP_AREA& ar) {
        AREA(ar.name, ar.rows, ar.text, ar.disabled);
    }
    
    void CHECK(GP_CHECK& ch) {
        CHECK(ch.name, ch.state, ch.disabled);
    }
    void SWITCH(GP_SWITCH& sw) {
        SWITCH(sw.name, sw.state, sw.disabled);
    }
    
    void DATE(GP_DATE& d) {
        DATE(d.name, d.date, d.disabled);
    }
    void TIME(GP_TIME& t) {
        TIME(t.name, t.time, t.disabled);
    }
    void COLOR(GP_COLOR& c) {
        COLOR(c.name, c.color, c.disabled);
    }
    
    void SPINNER(GP_SPINNER& s) {
        SPINNER(s.name, s.value, s.min, s.max, s.step, s.decimals, s.style, s.width);
    }
    void SLIDER(GP_SLIDER& s) {
        SLIDER(s.name, s.value, s.min, s.max, s.step, s.decimals, s.style, s.disabled);
    }
    
    void SELECT(GP_SELECT& s) {
        SELECT(s.name, s.list, s.selected, s.disabled);
    }
};