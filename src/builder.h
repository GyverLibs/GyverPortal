#pragma once
#include "utils.h"
#include "objects.h"
#include "log.h"
#include <FS.h>
#include "version.h"
#include "buildMacro.h"

#ifdef ESP8266
#include <ESP8266WebServer.h>
extern ESP8266WebServer* _gp_s;
#else
#include <WebServer.h>
extern WebServer* _gp_s;
#endif

extern int _gp_bufsize;
extern String* _gp_uri;
extern String* _GPP;
extern uint32_t _gp_unix_tmr;
extern uint32_t _gp_local_unix;

struct Builder {
    int reloadTimeout = 150;
    uint8_t _gp_nav_pos = 0;
    uint8_t _gp_nav_id = 0;
    
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
        *_GPP += s;
        send();
    }
    void SEND_P(PGM_P s) {
        send(true);
        _gp_s->sendContent_P(s);
    }
    
    void send(bool force = 0) {
        if ((int)_GPP->length() > (force ? 0 : _gp_bufsize)) {
            _gp_s->sendContent(*_GPP);
            *_GPP = "";
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
        updateTime();
    }
    void JS_TOP() {
        SEND_P(PSTR("<script>\n"
        "var _clkRelList='',_redirTout=150,_touch=0;\n"
        "function GP_redirect(url){setTimeout(function(){location.href=url;},_redirTout);}\n"
        "function GP_hint(id,txt){document.getElementById(id).title=txt;}\n"
        "function GP_send(req){var xhttp=new XMLHttpRequest();xhttp.open('POST',req,true);xhttp.send();}\n"
        "function GP_clickUD(arg,dir){if(arg.name)GP_send('/GP_click?'+arg.name+'=&_dir='+dir);}\n"
        "function GP_click(arg,r=0){if(!arg.name)return;var v;if(arg.type=='number'){\n"
        "if(arg.hasAttribute('min')&&Number(arg.value)<=Number(arg.min))arg.value=arg.min;\n"
        "if(arg.hasAttribute('max')&&Number(arg.value)>=Number(arg.max))arg.value=arg.max;}\n"
        "if(arg.type=='checkbox')v=arg.checked?'1':'0';else if(arg.type=='button')v='';else v=arg.value;\n"
        "if(v.charAt(0)=='#')v=v.substring(1);GP_send('/GP_click?'+arg.name+'='+encodeURIComponent(v));\n"
        "if(r!=0||_clkRelList.split(',').includes(arg.name))setTimeout(function(){location.reload();},(r?r:_redirTout));}\n"
        "function GP_clickid(btn,tar){GP_send('/GP_click?'+btn+'='+encodeURIComponent(document.getElementById(tar).value));}\n"
        "function GP_change(arg){arg.style.backgroundSize=(arg.value-arg.min)*100/(arg.max-arg.min)+'% 100%';\n"
        "document.getElementById(arg.id+'_val').value=arg.value}\n"
        "function GP_wheel(arg){var e=window.event;arg.value-=Math.sign(e.deltaY||e.detail||e.wheelDelta)*Number(arg.step);}\n"
        "function saveFile(id){document.getElementById(id).click();}\n"
        "function GP_submId(id){document.getElementById(id).submit();event.preventDefault();}\n"
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
        "function GP_sendForm(id){\n"
        "var elms=document.getElementById(id).elements;var qs='';\n"
        "for(var i=0,elm;elm=elms[i++];){if(elm.name){\n"
        "var v=elm.value;if(v.charAt(0)=='#')v=v.substring(1);\n"
        "if(elm.type=='checkbox')v=elm.checked?1:0;\n"
        "qs+=elm.name+'='+encodeURIComponent(v)+'&';}\n"
        "}GP_send(id+'?'+qs.slice(0,-1));}\n"
        "function GP_eye(arg){var p=arg.previousElementSibling;\n"
        "p.type=p.type=='text'?'password':'text';\n"
        "arg.style.color=p.type=='text'?'#bbb':'#13161a';}\n"
        "</script>\n"));
        updateTime();
    }
    void JS_BOTTOM() {
        SEND(F("<script>document.querySelectorAll('input[type=range]').forEach(x=>{GP_change(x)})</script>\n"));
    }
    
    // время
    uint32_t _timeUpdPrd = 10*60*1000ul;
    void setTimeUpdatePeriod(uint32_t t) {
        _timeUpdPrd = t;
    }
    void updateTime() {
        if (!_gp_unix_tmr || millis() - _gp_unix_tmr >= _timeUpdPrd) {
            SEND(F("<script>GP_send('/GP_time?unix='+Math.round(new Date().getTime()/1000)+'&gmt='+(-new Date().getTimezoneOffset()));</script>\n"));
        }
    }
    
    void THEME(PGM_P style) {
        SEND_P(style);
    }
    void THEME_FILE(const String& style) {
        *_GPP += F("<link rel='stylesheet' href='/gp_data/");
        *_GPP += style;
        *_GPP += F(".css'>\n");
        send();
    }
    
    void PAGE_BLOCK_BEGIN(int width = 350) {
        *_GPP += F("<div align='center' style='margin:auto;max-width:");
        *_GPP += width;
        *_GPP += F("px;'>\n");
        send();
    }
    void PAGE_BLOCK_END() {
        SEND(F("</div>\n"));
    }
    void PAGE_END() {
        SEND(F("</body></html>"));
    }
    
    void SPOILER_BEGIN(const String& text, PGM_P st = GP_GREEN) {
        *_GPP += F("<details><summary align='left' style='");
        if (st != GP_GREEN) {
            *_GPP += F("background-color:");
            *_GPP += FPSTR(st);
            *_GPP += ';';
        }
        *_GPP += F("'>");
        *_GPP += text;
        *_GPP += F("</summary><div align='center' style='");
        if (st != GP_GREEN) {
            *_GPP += F("border-color:");
            *_GPP += FPSTR(st);
            *_GPP += ';';
        }
        *_GPP += F("'>\n");
        send();
    }
    void SPOILER_END() {
        SEND(F("</div></details>\n"));
    }
    
    void HINT(const String& name, const String& txt) {
        *_GPP += F("<script>GP_hint('");
        *_GPP += name;
        *_GPP += F("','");
        *_GPP += txt;
        *_GPP += F("')</script>\n");
        send();
    }
    
    // ======================= UPDATE =======================
    void UPDATE(const String& list, int prd = 1000) {
        *_GPP += F("<script>setInterval(function(){GP_update('");
        *_GPP += list;
        *_GPP += F("'.replace(' ',''))},");
        *_GPP += prd;
        *_GPP += F(");</script>\n");
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
        *_GPP += F("<script>var globalupd=1;\n"
        "$(function(ev){$('#jqupd').click(function (ev){globalupd=0;setTimeout(function(){$('#jqupd').load('# #jqupd');},");
        *_GPP += del;
        *_GPP += F(");});});\n setInterval(function(){if(globalupd){$('#jqupd').load('# #jqupd');}else{globalupd=1;}},");
        *_GPP += prd;
        *_GPP += F(");</script>\n<div id='jqupd' style='width:100%;'>\n");
        send();
    }
    void JQ_UPDATE_END() {
        SEND(F("</div>\n"));
    }
    
    // перезагрузка. Имя нужно также указать в списке update
    void RELOAD(const String& name) {
        HIDDEN(name, F("_reload"));
    }
    
    // список имён компонентов, изменение (клик) которых приведёт к перезагрузке страницы
    void RELOAD_CLICK(const String& list) {
        *_GPP += F("<script>_clkRelList='");
        *_GPP += list;
        *_GPP += F("';_redirTout=");
        *_GPP += reloadTimeout;
        *_GPP += F(";</script>\n");
        send();
    }
    
    // ====================== ТАБЛИЦЫ ======================
    GPalign* _als = nullptr;
    int _alsCount = 0;
    
    void TABLE_BEGIN(const String& tdw = "", GPalign* als = nullptr, const String& w = "100%") {
        _als = als;
        *_GPP += F("<table width='");
        *_GPP += w;
        *_GPP += F("'>\n");
        send();
        
        if (tdw.length()) {
            TR();
            GP_parser p(tdw);
            while (p.parse()) {
                if (p.str.length()) {
                    *_GPP += F("<td width='");
                    *_GPP += p.str;
                    *_GPP += F("'>\n");
                }
            }
        }
    }
    void TR(GPalign al = GP_CENTER) {
        _alsCount = 0;
        *_GPP += F("<tr align='");
        *_GPP += FPSTR(GPgetAlign(al));
        *_GPP += F("'>\n");
    }
    void TD(GPalign al = GP_CENTER, uint8_t cs = 1, uint8_t rs = 1) {
        *_GPP += F("<td");
        if (al != GP_CENTER || _als) {
            *_GPP += F(" align=");
            if (al == GP_CENTER && _als && _als[_alsCount] >= 0 && _als[_alsCount] <= 3) *_GPP += FPSTR(GPgetAlign(_als[_alsCount++]));
            else *_GPP += FPSTR(GPgetAlign(al));
        }
        if (cs > 1) {
            *_GPP += F(" colspan=");
            *_GPP += cs;
        }
        if (rs > 1) {
            *_GPP += F(" rowspan=");
            *_GPP += rs;
        }
        *_GPP += ">\n";
        send();
    }
    void TABLE_END() {
        _als = nullptr;
        SEND(F("</table>\n"));
    }
    
    
    // ====================== ПОПАПЫ =======================
    void ALERT(const String& name, int prd = 1000) {
        *_GPP += F("<script>setInterval(function(){var xhttp=new XMLHttpRequest();xhttp.onreadystatechange=function(){\n"
        "if(this.readyState==4&&this.status==200&&this.responseText.length)alert(this.responseText);};\n"
        "xhttp.open('GET','/GP_update?'+'");
        *_GPP += name;
        *_GPP += F("=',true);xhttp.send();},");
        *_GPP += prd;
        *_GPP += F(");</script>\n");
        send();
    }
    void ALERT(const String& name, const String& text, int prd = 1000) {
        *_GPP += F("<script>setInterval(function(){var xhttp=new XMLHttpRequest();xhttp.onreadystatechange=function(){\n"
        "if(this.readyState==4&&this.status==200&&this.responseText.length)alert('");
        *_GPP += text;
        *_GPP += F("');};\n xhttp.open('GET','/GP_update?'+'");
        *_GPP += name;
        *_GPP += F("=',true);xhttp.send();},");
        *_GPP += prd;
        *_GPP += F(");</script>\n");
        send();
    }
    
    void PROMPT(const String& name, int prd = 1000) {
        *_GPP += F("<script>setInterval(function(){var xhttp=new XMLHttpRequest();xhttp.onreadystatechange=function(){\n"
        "if(this.readyState==4&&this.status==200&&this.responseText.length){let res=prompt(this.responseText,'');GP_send('/GP_click?");
        *_GPP += name;
        *_GPP += F("='+res);}};\n xhttp.open('GET','/GP_update?'+'");
        *_GPP += name;
        *_GPP += F("=',true);xhttp.send();},");
        *_GPP += prd;
        *_GPP += F(");</script>\n");
        send();
    }
    void PROMPT(const String& name, const String& text, int prd = 1000) {
        *_GPP += F("<script>setInterval(function(){var xhttp=new XMLHttpRequest();xhttp.onreadystatechange=function(){\n"
        "if(this.readyState==4&&this.status==200&&this.responseText.length){let res=prompt('");
        *_GPP += text;
        *_GPP += F("','');GP_send('/GP_click?");
        *_GPP += name;
        *_GPP += F("='+res);}};\n xhttp.open('GET','/GP_update?'+'");
        *_GPP += name;
        *_GPP += F("=',true);xhttp.send();},");
        *_GPP += prd;
        *_GPP += F(");</script>\n");
        send();
    }
    
    void CONFIRM(const String& name, int prd = 1000) {
        *_GPP += F("<script>setInterval(function(){var xhttp=new XMLHttpRequest();xhttp.onreadystatechange=function(){\n"
        "if(this.readyState==4&&this.status==200&&this.responseText.length){let res=confirm(this.responseText);GP_send('/GP_click?");
        *_GPP += name;
        *_GPP += F("='+(res?'1':'0'));}};\n xhttp.open('GET','/GP_update?'+'");
        *_GPP += name;
        *_GPP += F("=',true);xhttp.send();},");
        *_GPP += prd;
        *_GPP += F(");</script>\n");
        send();
    }
    void CONFIRM(const String& name, const String& text, int prd = 1000) {
        *_GPP += F("<script>setInterval(function(){var xhttp=new XMLHttpRequest();xhttp.onreadystatechange=function(){\n"
        "if(this.readyState==4&&this.status==200&&this.responseText.length){let res=confirm('");
        *_GPP += text;
        *_GPP += F("');GP_send('/GP_click?");
        *_GPP += name;
        *_GPP += F("='+(res?'1':'0'));}};\n xhttp.open('GET','/GP_update?'+'");
        *_GPP += name;
        *_GPP += F("=',true);xhttp.send();},");
        *_GPP += prd;
        *_GPP += F(");</script>\n");
        send();
    }
    
    // ======================= ФОРМА =======================
    void FORM_BEGIN(const String& name) {
        *_GPP += F("<form action='");
        *_GPP += name;
        *_GPP += F("' id='");
        *_GPP += name;
        *_GPP += F("' method='POST'>\n<input type='hidden' name='GP_form' value='");
        *_GPP += name;
        *_GPP += F("'>\n");
        send();
    }
    void FORM_END() {
        SEND(F("</form>\n"));
    }
    void SUBMIT(const String& text, PGM_P st = GP_GREEN, const String& cls = "") {
        *_GPP += F("<input type='submit' value='");
        *_GPP += text;
        if (st != GP_GREEN) {
            *_GPP += F("' style='background:");
            *_GPP += FPSTR(st);
        }
        if (cls.length()) {
            *_GPP += F("' class='");
            *_GPP += cls;
        }
        *_GPP += F("'>\n");
        send();
    }
    void SUBMIT_MINI(const String& text, PGM_P st = GP_GREEN) {
        SUBMIT(text, st, F("miniButton"));
    }
    
    void FORM_SEND(const String& text, const String& url = "", PGM_P st = GP_GREEN, const String& cls = "") {
        *_GPP += F("<input type='button' onclick='GP_sendForm(this.parentNode.id);");
        if (url.length()) {
            *_GPP += F("GP_redirect(\"");
            *_GPP += url;
            *_GPP += F("\");");
        }
        *_GPP += F("' value='");
        *_GPP += text;
        if (st != GP_GREEN) {
            *_GPP += F("' style='background:");
            *_GPP += FPSTR(st);
        }
        if (cls.length()) {
            *_GPP += F("' class='");
            *_GPP += cls;
        }
        *_GPP += F("'>\n");
        send();
    }
    void FORM_SEND_MINI(const String& text, const String& url = "", PGM_P st = GP_GREEN) {
        FORM_SEND(text, url, st, F("miniButton"));
    }
    
    void FORM_SUBMIT(const String& name, const String& text, PGM_P st = GP_GREEN) {
        FORM_BEGIN(name);
        SUBMIT(text, st);
        FORM_END();
    }
    void HIDDEN(const String& name, const String& text) {
        *_GPP += F("<input type='hidden' name='");
        *_GPP += name;
        *_GPP += F("' value='");
        *_GPP += text;
        *_GPP += F("' id='");
        *_GPP += name;
        *_GPP += F("'>\n");
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
        *_GPP += F("<style type='text/css'>@media screen and (max-width:");
        *_GPP += width;
        *_GPP += F("px){\n.grid{display:block;}\n#grid .block{margin:20px 10px;width:unset;}}</style>\n");
        send();
    }
    
    void BLOCK_BEGIN(GPblock type, const String& width = "", const String& text = "", PGM_P st = GP_DEFAULT) {
        *_GPP += F("<div class='blockBase");
        if (type != GP_DIV) {
            *_GPP += F(" block");
            if (text.length()) *_GPP += F(" blockTab");
            if (type == GP_THIN) *_GPP += F(" thinBlock");
        }
        *_GPP += "'";
        if (type == GP_TAB) *_GPP += F(" id='blockBack'");

        if (width.length()) {
            *_GPP += F(" style='max-width:");
            *_GPP += width;
            *_GPP += "'";
        }
        if (type == GP_THIN && st != GP_DEFAULT) {
            *_GPP += F(" style='border:2px solid");
            *_GPP += FPSTR(st);
            *_GPP += "'";
        }
        *_GPP += F(">\n");
        
        if (text.length()) {
            if (type == GP_DIV) {
                LABEL(text);
                HR();
            } else if (type == GP_TAB) {
                *_GPP += F("<div class='blockHeader'");
                if (st != GP_DEFAULT) {
                    *_GPP += F(" style='background:");
                    *_GPP += FPSTR(st);
                    *_GPP += "'";
                }
                *_GPP += ">";
                *_GPP += text;
                *_GPP += F("</div>\n");
            } else if (type == GP_THIN) {
                *_GPP += F("<div class='blockHeader thinTab'>");
                *_GPP += F("<span class='thinText'");
                if (st != GP_DEFAULT) {
                    *_GPP += F(" style='color:");
                    *_GPP += FPSTR(st);
                    *_GPP += "'";
                }
                *_GPP += ">";
                *_GPP += text;
                *_GPP += F("</span></div>\n");
            }
        }
        send();
    }
    
    void BLOCK_BEGIN(const String& width = "") {
        BLOCK_BEGIN(GP_TAB, width);
    }
    
    void BLOCK_TAB_BEGIN(const String& label, const String& width = "", PGM_P st = GP_DEFAULT) {
        BLOCK_BEGIN(GP_TAB, width, label, st);
    }
    void BLOCK_THIN_BEGIN(const String& width = "") {
        BLOCK_BEGIN(GP_THIN, width);
    }
    void BLOCK_THIN_TAB_BEGIN(const String& label, const String& width = "") {
        BLOCK_BEGIN(GP_THIN, width, label);
    }
    void BLOCK_END() {
        SEND(F("</div>\n"));
    }
    
    void BOX_BEGIN(GPalign al = GP_JUSTIFY, const String& w = "100%") {
        *_GPP += F("<div style='width:");
        *_GPP += w;
        *_GPP += F(";justify-content:");
        *_GPP += FPSTR(GPgetAlignFlex(al));
        *_GPP += F("' class='inliner'>\n");
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
    void TAG_RAW(const String& tag, const String& val, const String& name = "") {
        *_GPP += F("<");
        *_GPP += tag;
        if (name.length()) {
            *_GPP += F(" id='");
            *_GPP += name;
            *_GPP += "'";
        }
        *_GPP += F(">");
        *_GPP += val;
        *_GPP += F("</");
        *_GPP += tag;
        *_GPP += F(">\n");
        send();
    }
    
    void TITLE(const String& val, const String& name="") {
        TAG_RAW(F("h2"), val, name);
    }
    void LABEL(const String& val, const String& name="") {
        TAG_RAW(F("label"), val, name);
    }
    
    // legacy
    /*void SPAN(const String& val, const String& align = "", const String& name = "") {
        if (align.length()) {
            *_GPP += F("<div style='text-align:");
            *_GPP += align;
            *_GPP += F("'>");
        } else *_GPP += F("<div>");
        TAG_RAW(F("span"), val, name);
        *_GPP += F("</div>\n");
        send();
    }*/
    
    void SPAN(const String& val, GPalign al = GP_CENTER, const String& name = "") {
        if (al != GP_CENTER) {
            *_GPP += F("<div style='text-align:");
            *_GPP += FPSTR(GPgetAlign(al));
            *_GPP += F("'>");
        } else *_GPP += F("<div>");
        TAG_RAW(F("span"), val, name);
        *_GPP += F("</div>\n");
        send();
    }
    void PLAIN(const String& text) {
        *_GPP += text;
        send();
    }
    void BOLD(const String& text) {
        TAG_RAW(F("strong"), text);
        send();
    }
    void LABEL_BLOCK(const String& val, const String& name = "", PGM_P st = GP_GREEN) {
        *_GPP += F("<label class='display' ");
        if (st != GP_GREEN) {
            *_GPP += F("style='background:");
            *_GPP += FPSTR(st);
            *_GPP += F(";' ");
        }
        if (name.length()) {
            *_GPP += F("id='");
            *_GPP += name;
            *_GPP += "'";
        }
        *_GPP += F(">");
        *_GPP += val;
        *_GPP += F("</label>\n");
        send();
    }
    
    // ======================= ЛЕДЫ =======================
    void LED(const String& name, bool state = 0) {
        *_GPP += F("<input class='ledn' type='radio' disabled ");
        if (state) *_GPP += F("checked ");
        *_GPP += F("name='");
        *_GPP += name;
        *_GPP += F("' id='");
        *_GPP += name;
        *_GPP += F("'>\n");
        send();
    }
    void LED_RED(const String& name, bool state = 0) {
        *_GPP += F("<input class='led red' type='radio' disabled ");
        if (state) *_GPP += F("checked ");
        *_GPP += F("name='");
        *_GPP += name;
        *_GPP += F("' id='");
        *_GPP += name;
        *_GPP += F("'>\n");
        send();
    }
    void LED_GREEN(const String& name, bool state = 0) {
        *_GPP += F("<input class='led green' type='radio' disabled ");
        if (state) *_GPP += F("checked ");
        *_GPP += F("name='");
        *_GPP += name;
        *_GPP += F("' id='");
        *_GPP += name;
        *_GPP += F("'>\n");
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
        *_GPP += F("<div class='navtab'><ul ");
        if (st != GP_GREEN) {
            *_GPP += F("style='background:");
            *_GPP += FPSTR(st);
            *_GPP += "'";
        }
        *_GPP += F(">\n");
        GP_parser n(names);
        GP_parser u(urls);
        while (n.parse()) {
            u.parse();
            *_GPP += F("<li ");
            if (_gp_uri->equals(u.str)) *_GPP += F("style='background:#2a2d35' ");
            *_GPP += F("onclick='location.href=\"");
            *_GPP += u.str;
            *_GPP += F("\";'>");
            *_GPP += n.str;
            *_GPP += F("</li>\n");
        }
        *_GPP += F("</ul></div>\n");
        send();
    }
    
    void NAV_TABS_M(const String& name, const String& list, PGM_P st = GP_GREEN) {
        _gp_nav_pos = 0;
        *_GPP += F("<div class='navtab'><ul ");
        if (st != GP_GREEN) {
            *_GPP += F("style='background:");
            *_GPP += FPSTR(st);
            *_GPP += "' ";
        }
        *_GPP += F(">\n");
        GP_parser p(list);
        while (p.parse()) {
            *_GPP += F("<li ");
            if (!p.count) *_GPP += F("style='background:#2a2d35' ");
            *_GPP += F("' class='");
            *_GPP += name;
            *_GPP += F("' onclick='openTab(\"");
            *_GPP += name;
            *_GPP += '/';
            *_GPP += p.count;
            *_GPP += F("\",this,\"block_");
            *_GPP += name;
            *_GPP += F("\");GP_send(\"/GP_click?");
            *_GPP += name;
            *_GPP += '/';
            *_GPP += p.count;
            *_GPP += F("=\");'>");
            *_GPP += p.str;
            *_GPP += F("</li>\n");
        }
        *_GPP += F("</ul></div>\n");
        send();
    }
    
    void NAV_BLOCK_BEGIN(const String& name, int pos) {
        *_GPP += F("<div class='navblock block_");
        *_GPP += name;
        *_GPP += F("' id='");
        *_GPP += name;
        *_GPP += '/';
        *_GPP += pos;
        *_GPP += "' ";
        if (!pos) *_GPP += F("style='display:block'");
        *_GPP += F(">\n");
        send();
    }
    
    void NAV_TABS(const String& list, PGM_P st = GP_GREEN) {
        _gp_nav_id++;
        _gp_nav_pos = 0;
        *_GPP += F("<div class='navtab'><ul ");
        if (st != GP_GREEN) {
            *_GPP += F("style='background:");
            *_GPP += FPSTR(st);
            *_GPP += "' ";
        }
        *_GPP += F(">\n");
        GP_parser p(list);
        while (p.parse()) {
            *_GPP += F("<li ");
            if (!p.count) *_GPP += F("style='background:#2a2d35' ");
            *_GPP += F("class='nt-");
            *_GPP += _gp_nav_id;
            *_GPP += F("' onclick='openTab(\"ntab-");
            *_GPP += _gp_nav_id;
            *_GPP += '/';
            *_GPP += p.count;
            *_GPP += F("\",this,\"nb-");
            *_GPP += _gp_nav_id;
            *_GPP += F("\")'>");
            *_GPP += p.str;
            *_GPP += F("</li>\n");
        }
        *_GPP += F("</ul></div>\n");
        send();
    }
    
    void NAV_BLOCK_BEGIN() {
        *_GPP += F("<div class='navblock nb-");
        *_GPP += _gp_nav_id;
        *_GPP += F("' id='ntab-");
        *_GPP += _gp_nav_id;
        *_GPP += '/';
        *_GPP += _gp_nav_pos;
        *_GPP += "' ";
        if (!_gp_nav_pos) *_GPP += F("style='display:block'");
        *_GPP += F(">\n");
        send();
        _gp_nav_pos++;
    }
    
    void NAV_BLOCK_END() {
        SEND(F("</div>\n"));
    }
    
    // ======================= ФАЙЛЫ =======================
    void FILE_UPLOAD_RAW(const String& name, const String& text = "", const String& accept = "", const String& options = "", const String& action = "/GP_upload") {
        *_GPP += F("<form action='");
        *_GPP += action;
        *_GPP += F("' method='POST' enctype='multipart/form-data' id='");
        *_GPP += name;
        *_GPP += F("_form' style='display:flex;justify-content:center;'>\n"
        "<div id='ubtn' onclick='saveFile(\"");
        *_GPP += name;
        *_GPP += F("_inp\")'>");
        *_GPP += text;
        *_GPP += F("</div>\n"
        "<div id='ubtnclr'><input ");
        *_GPP += options;
        *_GPP += F("name='");
        *_GPP += name;
        if (accept.length()) {
            *_GPP += F("' accept='");
            *_GPP += accept;
        }
        *_GPP += F("' id='");
        *_GPP += name;
        *_GPP += F("_inp' type='file' onchange='GP_submId(\"");
        *_GPP += name;
        *_GPP += F("_form\")'/></div>\n"
        "</form>\n");
        send();
    }
    
    void FILE_UPLOAD(const String& name, const String& text = "", const String& accept = "") {
        FILE_UPLOAD_RAW(name, "📄 " + text, accept, F("multiple "));
    }
    
    void FOLDER_UPLOAD(const String& name, const String& text = "") {
        FILE_UPLOAD_RAW(name, "📁 " + text, "", F("multiple webkitdirectory allowdirs "));
    }
    
    void OTA_FIRMWARE(const String& text = "OTA firmware", bool page = 0) {
        FILE_UPLOAD_RAW(F("firmware"), "🧱 " + text, F(".bin,.bin.gz"), "", page ? F("/ota_update") : F("/GP_OTAupload"));
    }
    
    void OTA_FILESYSTEM(const String& text = "OTA filesystem", bool page = 0) {
        FILE_UPLOAD_RAW(F("filesystem"), "💽 " + text, F(".bin,.bin.gz"), "", page ? F("/ota_update") : F("/GP_OTAupload"));
    }
    
    void IMAGE(const String& uri, const String& w = "") {
        *_GPP += F("<img src='");
        *_GPP += uri;
        *_GPP += F("' style='width:");
        *_GPP += w;
        *_GPP += F("'>\n");
        send();
    }
    void VIDEO(const String& uri, const String& w = "") {
        *_GPP += F("<video src='");
        *_GPP += uri;
        *_GPP += F("' style='width:");
        *_GPP += w;
        *_GPP += F("' controls>Browser doesn't support video.</video>\n");
        send();
    }
    void EMBED(const String& uri, const String& w = "100%", const String& h = "") {
        *_GPP += F("<embed src='");
        *_GPP += uri;
        *_GPP += F("' style='width:90%;border-radius:5px;background:white;width:");
        *_GPP += w;
        if (h.length()) {
            *_GPP += F(";height:");
            *_GPP += h;
        }
        *_GPP += F(";'>\n");
        send();
    }
    
    
    // ======================= ФАЙЛОВЫЙ МЕНЕДЖЕР =======================
    void _href(const String& url, const String& text) {
        *_GPP += F("<a style='text-decoration:none' href='");
        *_GPP += url;
        *_GPP += F("'>");
        *_GPP += text;
        *_GPP += F("</a>");
    }
    
    void _fileRow(const String& fpath, int size) {
        *_GPP += "<tr>";
        *_GPP += F("<td align='left' style='padding-right:5px'>");
        _href(fpath, fpath);
        *_GPP += F("<td>");
        *_GPP += '[';
        *_GPP += String(size / 1000.0, 1);
        *_GPP += F(" kB]");
        *_GPP += "<td>";
        _href(String("?GP_delete=") + fpath, "❌");
    }

    void _showFiles(fs::FS *fs, const String& path, __attribute__((unused)) uint8_t levels = 0) {
#ifdef ESP8266
        yield();
        Dir dir = fs->openDir(path);
        while (dir.next()) {
            if (dir.isFile() && dir.fileName().length()) {
                String fpath = '/' + path + dir.fileName();
                _fileRow(fpath, dir.fileSize());
            }
            if (dir.isDirectory()) {
                String p = path;
                p += dir.fileName();
                p += '/';
                Dir sdir = fs->openDir(p);
                _showFiles(fs, p);
            }
        }

#else   // ESP32
        File root = fs->open(path.length() ? path.c_str() : ("/"));
        if (!root || !root.isDirectory()) return;
        File file;
        while (file = root.openNextFile()) {
            if (file.isDirectory()) {
                if (levels) _showFiles(fs, file.path(), levels - 1);
            } else {
                _fileRow(path + '/' + file.name(), file.size());
            }
        }
#endif
    }
    
    void FILE_MANAGER(fs::FS *fs) {
        *_GPP += F("<table>");
        _showFiles(fs, "", 5);

#ifdef ESP8266
        FSInfo fsi;
        fs->info(fsi);
        *_GPP += F("<tr><td colspan=3 align=center><hr><strong>Used ");
        *_GPP += String(fsi.usedBytes / 1000.0, 2);
        *_GPP += F(" kB from ");
        *_GPP += String(fsi.totalBytes / 1000.0, 2);
        *_GPP += F(" (");
        *_GPP += (fsi.usedBytes * 100 / fsi.totalBytes);
        *_GPP += F("%)</strong>");
#endif
        *_GPP += F("</table>");
        send();
    }
    
    // ================ СИСТЕМНАЯ ИНФОРМАЦИЯ ================
    void SYSTEM_INFO() {
        TABLE_BEGIN("300px");
        // ===========
        TR();
        TD(GP_CENTER, 3);
        LABEL(F("Network"));
        HR();
        
        TR();
        TD(GP_LEFT); PLAIN(F("WiFi Mode"));
        TD(GP_RIGHT); BOLD(WiFi.getMode() == WIFI_AP ? F("AP") : (WiFi.getMode() == WIFI_STA ? F("STA") : F("AP_STA")));
        
        if (WiFi.getMode() != WIFI_AP) {
            TR();
            TD(GP_LEFT); PLAIN(F("SSID"));
            TD(GP_RIGHT); BOLD(WiFi.SSID());
            
            TR();
            TD(GP_LEFT); PLAIN(F("Local IP"));
            TD(GP_RIGHT); BOLD(WiFi.localIP().toString());
        }
        if (WiFi.getMode() != WIFI_STA) {
            TR();
            TD(GP_LEFT); PLAIN(F("AP IP"));
            TD(GP_RIGHT); BOLD(WiFi.softAPIP().toString());
        }
        TR();
        TD(GP_LEFT); PLAIN(F("Subnet"));
        TD(GP_RIGHT); BOLD(WiFi.subnetMask().toString());
        
        TR();
        TD(GP_LEFT); PLAIN(F("Gateway"));
        TD(GP_RIGHT); BOLD(WiFi.gatewayIP().toString());
        
        TR();
        TD(GP_LEFT); PLAIN(F("MAC Address"));
        TD(GP_RIGHT); BOLD(WiFi.macAddress());
        
        TR();
        TD(GP_LEFT); PLAIN(F("RSSI"));
        TD(GP_RIGHT); BOLD("📶 " + String(constrain(2 * (WiFi.RSSI() + 100), 0, 100)) + '%');
        
        // ===========
        TR();
        TD(GP_CENTER, 3);
        LABEL(F("Memory"));
        HR();
        
        TR();
        TD(GP_LEFT); PLAIN(F("Free Heap"));
        TD(GP_RIGHT); BOLD(String(ESP.getFreeHeap() / 1000.0, 3) + " kB");
        
    #ifdef ESP8266
        TR();
        TD(GP_LEFT); PLAIN(F("Heap Fragmentation"));
        TD(GP_RIGHT); BOLD(String(ESP.getHeapFragmentation()) + '%');
    #endif
        
        TR();
        TD(GP_LEFT); PLAIN(F("Sketch Size"));
        TD(GP_RIGHT); BOLD(String(ESP.getSketchSize() / 1000.0, 1) + " kB (" + String(ESP.getFreeSketchSpace() / 1000.0, 1) + ")");
        
        TR();
        TD(GP_LEFT); PLAIN(F("Flash Size"));
        TD(GP_RIGHT); BOLD(String(ESP.getFlashChipSize() / 1000.0, 1) + " kB");
        
        // ===========
        TR();
        TD(GP_CENTER, 3);
        LABEL(F("System"));
        HR();
        
    #ifdef ESP8266
        TR();
        TD(GP_LEFT); PLAIN(F("Chip ID"));
        TD(GP_RIGHT); BOLD("0x" + String(ESP.getChipId(), HEX));
    #endif
        
        TR();
        TD(GP_LEFT); PLAIN(F("Cycle Count"));
        TD(GP_RIGHT); BOLD(String(ESP.getCycleCount()));
        
        TR();
        TD(GP_LEFT); PLAIN(F("Cpu Freq."));
        TD(GP_RIGHT); BOLD(String(ESP.getCpuFreqMHz()));
        
        TR();
        TD(GP_LEFT); PLAIN(F("Date"));
        GPdate date(_gp_local_unix);
        TD(GP_RIGHT); BOLD(date.encode());
        
        TR();
        TD(GP_LEFT); PLAIN(F("Time"));
        GPtime time(_gp_local_unix);
        TD(GP_RIGHT); BOLD(time.encode());
        
        TR();
        TD(GP_LEFT); PLAIN(F("Uptime"));
        uint32_t sec = millis() / 1000ul;
        uint8_t second = sec % 60ul;
        sec /= 60ul;
        uint8_t minute = sec % 60ul;
        sec /= 60ul;
        uint16_t hour = sec % 24ul;
        String s;
        s += hour;
        s += ':';
        s += minute / 10;
        s += minute % 10;
        s += ':';
        s += second / 10;
        s += second % 10;
        TD(GP_RIGHT); BOLD(s);
        
        // ===========
        TR();
        TD(GP_CENTER, 3);
        LABEL(F("Version"));
        HR();
        
        TR();
        TD(GP_LEFT); PLAIN(F("SDK"));
        TD(GP_RIGHT); BOLD(ESP.getSdkVersion());
        
    #ifdef ESP8266
        TR();
        TD(GP_LEFT); PLAIN(F("Core"));
        TD(GP_RIGHT); BOLD(ESP.getCoreVersion());
    #endif
    
        TR();
        TD(GP_LEFT); PLAIN(F("GyverPortal"));
        TD(GP_RIGHT); BOLD(GP_VERSION);

        TABLE_END();
    }
    
    // ======================= КНОПКА =======================
    void BUTTON_RAW(const String& name, const String& value, const String& tar, PGM_P st, const String& width = "", const String& cls = "", bool dis = 0, bool rel = 0) {
        *_GPP += F("<input type='button' ");
        if (cls.length()) {
            *_GPP += F("class='");
            *_GPP += cls;
            *_GPP += "' ";
        }
        *_GPP += F("style='");
        if (st != GP_GREEN) {
            *_GPP += F("background:");
            *_GPP += FPSTR(st);
            *_GPP += ';';
        }
        if (width.length()) {
            *_GPP += F("width:");
            *_GPP += width;
            *_GPP += ';';
        }
        *_GPP += F("' name='");
        *_GPP += name;
        *_GPP += F("' value='");
        *_GPP += value;
        *_GPP += F("' onmousedown='if(!_touch)GP_clickUD(this,0)' onmouseup='if(!_touch)GP_clickUD(this,1)' ");
        if (!dis) *_GPP += F("ontouchstart='_touch=1;GP_clickUD(this,0)' ontouchend='GP_clickUD(this,1)' ");
        if (tar.length()) {
            *_GPP += F("onclick=\"GP_clickid('");
            *_GPP += name;
            *_GPP += F("','");
            *_GPP += tar;
            *_GPP += F("')\"");
        } else {
            *_GPP += F("onclick='GP_click(this,");
            *_GPP += rel ? reloadTimeout : 0;
            *_GPP += F(")'");
        }
        if (dis) *_GPP += F(" disabled");
        *_GPP += F(">\n");
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
        *_GPP += F("<input type='button' ");
        if (name.length()) {
            *_GPP += F("name='");
            *_GPP += name;
            *_GPP += "' "; 
        }
        if (cls.length()) {
            *_GPP += F("class='");
            *_GPP += cls;
            *_GPP += "' ";
        }
        *_GPP += F("style='");
        if (st != GP_GREEN) {
            *_GPP += F("background:");
            *_GPP += FPSTR(st);
            *_GPP += ';';
        }
        if (width.length()) {
            *_GPP += F("width:");
            *_GPP += width;
            *_GPP += ';';
        }
        *_GPP += F("' value='");
        *_GPP += value;
        
        if (name.length()){
            *_GPP += F("' onclick='GP_click(this);setTimeout(function(){location.href=\"");
            *_GPP += url;
            *_GPP += F("\";},");
            *_GPP += reloadTimeout;
            *_GPP += F(");'>\n");
        } else {
            *_GPP += F("' onclick='location.href=\"");
            *_GPP += url;
            *_GPP += F("\";'>\n");
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
        *_GPP += F("<a style='text-decoration:none;' href='");
        *_GPP += url;
        *_GPP += F("' download><input type='button' value='");
        *_GPP += value;
        *_GPP += "' ";
        if (cls.length()) {
            *_GPP += F("class='");
            *_GPP += cls;
            *_GPP += "' ";
        }
        *_GPP += F("style='");
        if (st != GP_GREEN) {
            *_GPP += F("background:");
            *_GPP += FPSTR(st);
            *_GPP += ';';
        }
        if (width.length()) {
            *_GPP += F("width:");
            *_GPP += width;
            *_GPP += ';';
        }
        *_GPP += F("'>");
        *_GPP += F("</a>\n");
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
        *_GPP += F("<input type='number' step='any' name='");
        *_GPP += name;
        *_GPP += F("' id='");
        *_GPP += name;
        if (value.length()) {
            *_GPP += F("' value='");
            *_GPP += value;
        }
        if (width.length()) {
            *_GPP += F("' style='width:");
            *_GPP += width;
        }
        if (minv.length()) {
            *_GPP += F("' min='");
            *_GPP += minv;
        }
        if (maxv.length()) {
            *_GPP += F("' max='");
            *_GPP += maxv;
        }
        *_GPP += F("' placeholder='");
        *_GPP += place;
        *_GPP += F("' onchange='GP_click(this)'");
        if (dis) *_GPP += F(" disabled");
        *_GPP += F(">\n");
        send();
    }
    void NUMBER(const String& name, const String& place, int value = INT32_MAX, const String& width = "", bool dis = false) {
        NUMBER_RAW(name, place, (value == INT32_MAX ? String("") : String(value)), "", "", width, dis);
    }
    void NUMBER_F(const String& name, const String& place, float value = NAN, uint8_t dec = 2, const String& width = "", bool dis = false) {
        NUMBER_RAW(name, place, (isnan(value) ? String("") : String(value, (uint16_t)dec)), "", "", width, dis);
    }
    
    void TEXT(const String& name, const String& place = "", const String& value = "", const String& width = "", int maxlength = 0, const String& pattern = "", bool dis = false) {
        *_GPP += F("<input type='text' name='");
        *_GPP += name;
        *_GPP += F("' id='");
        *_GPP += name;
        *_GPP += F("' value='");
        *_GPP += value;
        if (width.length()) {
            *_GPP += F("' style='width:");
            *_GPP += width;
        }
        *_GPP += F("' placeholder='");
        *_GPP += place;
        *_GPP += F("' onchange='GP_click(this)'");
        if (dis) *_GPP += F(" disabled");
        if (maxlength) {
            *_GPP += F(" maxlength=");
            *_GPP += maxlength;
        }
        if (pattern.length()) {
            *_GPP += F(" pattern=");
            *_GPP += pattern;
        }
        *_GPP += ">\n";
        send();
    }
    void PASS(const String& name, const String& place = "", const String& value = "", const String& width = "", int maxlength = 0, const String& pattern = "", bool dis = false) {
        *_GPP += F("<input type='password' name='");
        *_GPP += name;
        *_GPP += F("' id='");
        *_GPP += name;
        *_GPP += F("' value='");
        *_GPP += value;
        if (width.length()) {
            *_GPP += F("' style='width:");
            *_GPP += width;
        }
        *_GPP += F("' placeholder='");
        *_GPP += place;
        *_GPP += F("' onchange='GP_click(this)'");
        if (dis) *_GPP += F(" disabled");
        if (maxlength) {
            *_GPP += F(" maxlength=");
            *_GPP += maxlength;
        }
        if (pattern.length()) {
            *_GPP += F(" pattern=");
            *_GPP += pattern;
        }
        *_GPP += ">\n";
        *_GPP += F("<span class='eyepass' onclick='GP_eye(this)'>👁</span>\n");
        send();
    }
    
    void AREA(const String& name, int rows = 1, const String& value = "", bool dis = false) {
        *_GPP += F("<textarea onchange='GP_click(this)' style='height:auto' name='");
        *_GPP += name;
        *_GPP += F("' id='");
        *_GPP += name;
        *_GPP += F("' rows='");
        *_GPP += rows;
        *_GPP += "'";
        if (dis) *_GPP += F(" disabled");
        *_GPP += ">";
        if (value.length()) *_GPP += value;
        *_GPP += F("</textarea>\n");
        send();
    }
    
    void AREA_LOG(int rows = 5, int prd = 1000, const String& w = "") {
        *_GPP += F("<textarea style='height:auto;");
        if (w.length()) {
            *_GPP += F("width:");
            *_GPP += w;
        }
        *_GPP += F("' id='GP_log' rows='");
        *_GPP += rows;
        *_GPP += F("' disabled></textarea>\n");
        *_GPP += F("<script>setInterval(function(){var xhttp=new XMLHttpRequest();\n"
        "xhttp.onreadystatechange=function(){\n"
        "if(this.readyState==4&&this.status==200){\n"
        "var elm=document.getElementById('GP_log');\n"
        "elm.innerHTML+=this.responseText;elm.scrollTop=elm.scrollHeight;}};\n"
        "xhttp.open('GET','/GP_log',true);xhttp.send();},");
        *_GPP += prd;
        *_GPP += F(");</script>\n");
        send();
    }
    
    void AREA_LOG(GPlog& log, int rows = 5, int prd = 1000, const String& w = "") {
        log.clear();
        *_GPP += F("<textarea style='height:auto;");
        if (w.length()) {
            *_GPP += F("width:");
            *_GPP += w;
        }
        *_GPP += F("' id='");
        *_GPP += log.name;
        *_GPP += F("' rows='");
        *_GPP += rows;
        *_GPP += F("' disabled></textarea>\n");
        *_GPP += F("<script>setInterval(function(){var xhttp=new XMLHttpRequest();\n"
        "xhttp.onreadystatechange=function(){\n"
        "if(this.readyState==4&&this.status==200){\n"
        "var elm=document.getElementById('");
        *_GPP += log.name;
        *_GPP += F("');\n"
        "elm.innerHTML+=this.responseText;elm.scrollTop=elm.scrollHeight;}};\n"
        "xhttp.open('GET','/GP_update?");
        *_GPP += log.name;
        *_GPP += F("=',true);xhttp.send();},");
        *_GPP += prd;
        *_GPP += F(");</script>\n");
        send();
    }
    
    // ======================= НАСТРОЙКА =======================
    void CHECK(const String& name, bool state = 0, bool dis = false) {
        *_GPP += F("<input type='checkbox' name='");
        *_GPP += name;
        *_GPP += F("' id='");
        *_GPP += name;
        *_GPP += "' ";
        if (state) *_GPP += F("checked ");
        if (dis) *_GPP += F("disabled ");
        *_GPP += F("onclick='GP_click(this)'>\n");
        *_GPP += F("<input type='hidden' value='0' name='");
        *_GPP += name;
        *_GPP += "'>\n";
        send();
    }
    void SWITCH(const String& name, bool state = 0, bool dis = false, PGM_P st = GP_GREEN) {
        if (st != GP_GREEN) {
            *_GPP += F("<style>#_");
            *_GPP += name;
            *_GPP += F(" input:checked+.slider{background-color:");
            *_GPP += FPSTR(st);
            *_GPP += F("}</style>\n");
        }
        *_GPP += F("<label id='_");
        *_GPP += name;
        *_GPP += "'";
        *_GPP += F(" class='switch'><input type='checkbox' name='");
        *_GPP += name;
        *_GPP += F("' id='");
        *_GPP += name;
        *_GPP += "' ";
        if (state) *_GPP += F("checked ");
        if (dis) *_GPP += F("disabled ");
        *_GPP += F("onclick='GP_click(this)'>\n");
        *_GPP += F("<span class='slider'></span></label>");
        *_GPP += F("<input type='hidden' value='0' name='");
        *_GPP += name;
        *_GPP += "'>\n";
        send();
    }
    
    void DATE(const String& name, bool dis = false) {
        GPdate d;
        DATE(name, d, dis);
    }
    void DATE(const String& name, GPdate d, bool dis = false) {
        *_GPP += F("<input step='any' type='date' name='");
        *_GPP += name;
        *_GPP += F("' id='");
        *_GPP += name;
        if (d.year) {
            *_GPP += F("' value='");
            *_GPP += d.encode();
        }
        *_GPP += "' ";
        if (dis) *_GPP += F("disabled ");
        *_GPP += F("onchange='GP_click(this)'>\n");
        send();
    }
    
    void TIME(const String& name, bool dis = false) {
        *_GPP += F("<input step='any' type='time' name='");
        *_GPP += name;
        *_GPP += F("' id='");
        *_GPP += name;
        *_GPP += "' ";
        if (dis) *_GPP += F("disabled ");
        *_GPP += F("onchange='GP_click(this)'>\n");
        send();
    }
    void TIME(const String& name, GPtime t, bool dis = false) {
        *_GPP += F("<input step='any' type='time' name='");
        *_GPP += name;
        *_GPP += F("' id='");
        *_GPP += name;
        *_GPP += F("' value='");
        *_GPP += t.encode();
        *_GPP += "' ";
        if (dis) *_GPP += F("disabled ");
        *_GPP += F("onchange='GP_click(this)'>\n");
        send();
    }
    
    void SLIDER(const String& name, float value = 0, float min = 0, float max = 100, float step = 1, uint8_t dec = 0, PGM_P st = GP_GREEN, bool dis = 0, bool oninp = 0) {
        *_GPP += F("<input type='range' name='");
        *_GPP += name;
        *_GPP += F("' id='");
        *_GPP += name;
        *_GPP += F("' value='");
        *_GPP += value;
        *_GPP += F("' min='");
        *_GPP += min;
        *_GPP += F("' max='");
        *_GPP += max;
        *_GPP += F("' step='");
        *_GPP += step;
        *_GPP += F("' style='background-image:linear-gradient(");
        *_GPP += FPSTR(st);
        *_GPP += ',';
        *_GPP += FPSTR(st);
        *_GPP += F(");' onload='GP_change(this)' ");
        if (oninp) *_GPP += F("oninput='GP_change(this);GP_click(this)'");
        else *_GPP += F("onchange='GP_click(this)' oninput='GP_change(this)'");
        *_GPP += F(" onmousewheel='GP_wheel(this);GP_change(this);GP_click(this)' ");
        if (dis) *_GPP += F("disabled");
        *_GPP += F(">\n");
        *_GPP += F("<output id='");
        *_GPP += name;
        *_GPP += F("_val' ");
        if (st != GP_GREEN) {
            *_GPP += F("style='background:");
            *_GPP += FPSTR(st);
            *_GPP += F(";'");
        }
        if (dis) *_GPP += F(" class='dsbl'");
        *_GPP += F(">");
        if (!dec) *_GPP += (int)round(value);
        else *_GPP += String(value, (uint16_t)dec);
        *_GPP += F("</output>\n");
        send();
    }
    
    void SLIDER_C(const String& name, float value = 0, float min = 0, float max = 100, float step = 1, uint8_t dec = 0, PGM_P st = GP_GREEN, bool dis = 0) {
        SLIDER(name, value, min, max, step, dec, st, dis, 1);
    }
    
    void SPIN_BTN(const String& name, float step, bool dir, PGM_P st, uint8_t dec, bool dis) {
        *_GPP += F("<input type='button' class='spinBtn ");
        *_GPP += dir ? F("spinR") : F("spinL");
        *_GPP += F("' onclick='GP_spin(\"");
        *_GPP += name;
        *_GPP += F("\",");
        *_GPP += dir ? step : -step;
        *_GPP += ',';
        *_GPP += dec;
        *_GPP += F(")' ");
        *_GPP += F("value='");
        *_GPP += dir ? '+' : '-';
        *_GPP += "' ";
        if (st != GP_GREEN) {
            *_GPP += F(" style='background:");
            *_GPP += FPSTR(st);
            *_GPP += F(";'");
        }
        if (dis) *_GPP += F(" disabled");
        *_GPP += F(">\n");
    }
    void SPINNER(const String& name, float value = 0, float min = NAN, float max = NAN, float step = 1, uint8_t dec = 0, PGM_P st = GP_GREEN, const String& width = "", bool dis = 0) {
        *_GPP += F("<div id='spinner' class='spinner'>\n");
        SPIN_BTN(name, step, false, st, dec, dis);
        *_GPP += F("<input type='number' name='");
        *_GPP += name;
        *_GPP += F("' id='");
        *_GPP += name;
        *_GPP += F("' step='");
        *_GPP += String(step, (uint16_t)dec);
        if (width.length()) {
            *_GPP += F("' style='width:");
            *_GPP += width;
        }
        *_GPP += F("' onchange='GP_click(this)' onmousewheel='GP_wheel(this);GP_click(this)' value='");
        if (!dec) *_GPP += (int)round(value);
        else *_GPP += String(value, (uint16_t)dec);
        if (!isnan(min)) {
            *_GPP += F("' min='");
            *_GPP += min;
        }
        if (!isnan(max)) {
            *_GPP += F("' max='");
            *_GPP += max;
        }
        *_GPP += "'";
        if (dis) *_GPP += F("disabled");
        *_GPP += F(">\n");
        SPIN_BTN(name, step, true, st, dec, dis);
        *_GPP += F("</div>\n");
        send();
    }
    
    void COLOR(const String& name, uint32_t value = 0, bool dis = false) {
        *_GPP += F("<input type='color' name='");
        *_GPP += name;
        *_GPP += F("' id='");
        *_GPP += name;
        *_GPP += F("' value='");
        GPcolor col(value);
        *_GPP += col.encode();
        *_GPP += "' ";
        if (dis) *_GPP += F("disabled ");
        *_GPP += F("onchange='GP_click(this)'>\n");
        send();
    }
    void COLOR(const String& name, GPcolor col, bool dis = false) {
        *_GPP += F("<input type='color' name='");
        *_GPP += name;
        *_GPP += F("' id='");
        *_GPP += name;
        *_GPP += F("' value='");
        *_GPP += col.encode();
        *_GPP += "' ";
        if (dis) *_GPP += F("disabled ");
        *_GPP += F("onchange='GP_click(this)'>\n");
        send();
    }
    
    void SELECT(const String& name, const String& list, int sel = 0, bool nums = 0, bool dis = 0, bool rel = 0) {
        if (sel < 0) sel = 0;
        *_GPP += F("<select name='");
        *_GPP += name;
        *_GPP += F("' id='");
        *_GPP += name;
        *_GPP += "' ";
        if (dis) *_GPP += F("disabled ");
        *_GPP += F("onchange='GP_click(this,");
        *_GPP += rel ? reloadTimeout : 0;
        *_GPP += F(")'>\n");
        
        GP_parser p(list);
        int idx = 0; 
        while (p.parse()) {
            *_GPP += F("<option value='");
            *_GPP += idx;
            *_GPP += "'";
            if (p.count == sel) *_GPP += F(" selected");
            *_GPP += F(">");
            if (nums) {
                *_GPP += idx;
                *_GPP += ". ";
            }
            *_GPP += p.str;
            *_GPP += F("</option>\n");
            idx++;
        }
        *_GPP += F("</select>\n");
        send();
    }
    void SELECT(const String& name, String* list, int sel = 0, bool nums = 0, bool dis = false, bool rel = 0) {
        if (sel < 0) sel = 0;
        *_GPP += F("<select name='");
        *_GPP += name;
        *_GPP += F("' id='");
        *_GPP += name;
        *_GPP += "' ";
        if (dis) *_GPP += F("disabled ");
        *_GPP += F("onchange='GP_click(this,");
        *_GPP += rel ? reloadTimeout : 0;
        *_GPP += F(")'>\n");
        int idx = 0; 
        while (list[idx].length()) {
            *_GPP += F("<option value='");
            *_GPP += idx;
            *_GPP += "'";
            if (idx == sel) *_GPP += F(" selected");
            *_GPP += F(">");
            if (nums) {
                *_GPP += idx;
                *_GPP += ". ";
            }
            *_GPP += list[idx];
            *_GPP += F("</option>\n");
            idx++;
        }
        *_GPP += F("</select>\n");
        send();
    }
    void SELECT(const String& name, char** list, int sel = 0, bool nums = 0, bool dis = false, bool rel = 0) {
        if (sel < 0) sel = 0;
        *_GPP += F("<select name='");
        *_GPP += name;
        *_GPP += F("' id='");
        *_GPP += name;
        *_GPP += "' ";
        if (dis) *_GPP += F("disabled ");
        *_GPP += F("onchange='GP_click(this,");
        *_GPP += rel ? reloadTimeout : 0;
        *_GPP += F(")'>\n");
        int idx = 0; 
        while (list[idx]!=nullptr) {
            *_GPP += F("<option value='");
            *_GPP += idx;
            *_GPP += "'";
            if (idx == sel) *_GPP += F(" selected");
            *_GPP += F(">");
            if (nums) {
                *_GPP += idx;
                *_GPP += ". ";
            }
            *_GPP += list[idx];
            *_GPP += F("</option>\n");
            idx++;
        }
        *_GPP += F("</select>\n");
        send();
    }
    
    // ======================= ГРАФИКИ =======================
    template <int ax, int am>
    void PLOT(const String& id, const char** labels, int16_t vals[ax][am], int dec = 0, int height = 400) {
        *_GPP += F("<script type='text/javascript' src='https://www.gstatic.com/charts/loader.js'></script>\n"
        "<script type='text/javascript'>\n"
        "google.charts.load('current', {'packages':['corechart']});\n"
        "google.charts.setOnLoadCallback(drawChart);\n"
        "function drawChart() {\n"
        "var data = google.visualization.arrayToDataTable([\n");
        send();
        
        *_GPP += '[';
        for (int i = 0; i < ax+1; i++) {
            *_GPP += '\'';
            if (i) *_GPP += labels[i-1];
            *_GPP += "',";
        }
        *_GPP += "],\n";
        for (int j = 0; j < am; j++) {
            *_GPP += '[';
            for (int i = 0; i < ax+1; i++) {
                if (!i) *_GPP += '\'';
                if (!i) *_GPP += j;
                else {
                    if (dec) *_GPP += (float)vals[i-1][j] / dec;
                    else *_GPP += vals[i-1][j];
                }
                if (!i) *_GPP += '\'';
                *_GPP += ',';
            }
            *_GPP += F("],\n");
        }
        send();
        
        *_GPP += F("]);var options = {pointSize:5,curveType:'function','chartArea':{'width':'90%','height':'90%'},\n"
        "backgroundColor:'none',hAxis:{title:''},vAxis:{title:''},\n"
        "legend: {position:'bottom'}};\n"
        "var chart = new google.visualization.LineChart(document.getElementById('");
        *_GPP += id;
        *_GPP += F("'));\n");
        *_GPP += F("chart.draw(data, options);}\n"
        "</script><div id='");
        *_GPP += id;
        *_GPP += F("' class='chartBlock' style='height:");
        *_GPP += height;
        *_GPP += F("px'></div>\n");
        send();
    }
    
    template <int ax, int am>
    void PLOT_DARK(const String& id, const char** labels, int16_t vals[ax][am], int dec = 0, int height = 400) {
        *_GPP += F("<script type='text/javascript' src='https://www.gstatic.com/charts/loader.js'></script>\n"
        "<script type='text/javascript'>\n"
        "google.charts.load('current', {'packages':['corechart']});\n"
        "google.charts.setOnLoadCallback(drawChart);\n"
        "function drawChart() {\n"
        "var data = google.visualization.arrayToDataTable([\n");
        send();
        
        *_GPP += '[';
        for (int i = 0; i < ax+1; i++) {
            *_GPP += '\'';
            if (i) *_GPP += labels[i-1];
            *_GPP += "',";
        }
        *_GPP += "],\n";
        for (int j = 0; j < am; j++) {
            *_GPP += '[';
            for (int i = 0; i < ax+1; i++) {
                if (!i) *_GPP += '\'';
                if (!i) *_GPP += j;
                else {
                    if (dec) *_GPP += (float)vals[i-1][j] / dec;
                    else *_GPP += vals[i-1][j];
                }
                if (!i) *_GPP += '\'';
                *_GPP += ',';
            }
            *_GPP += F("],\n");
        }
        send();
        
        *_GPP += F("]);var options = {pointSize:5,curveType:'function','chartArea':{'width':'90%','height':'90%'},\n"
        "backgroundColor:'none',hAxis:{title:'',titleTextStyle:{color:'#ddd'},textStyle:{color:'#bbb'}},\n"
        "vAxis:{title:'',titleTextStyle:{color:'#ddd'},gridlines:{color:'#777'},textStyle:{color:'#bbb'}},\n"
        "legend: {position:'bottom',textStyle:{color:'#eee'}}};\n"
        "var chart = new google.visualization.LineChart(document.getElementById('");
        *_GPP += id;
        *_GPP += F("'));\n");
        *_GPP += F("chart.draw(data, options);}\n");
        *_GPP += F("</script><div id='");
        *_GPP += id;
        *_GPP += F("' class='chartBlock' style='height:");
        *_GPP += height;
        *_GPP += F("px'></div>\n");
        send();
    }
    
    void AJAX_PLOT(const String& name, int axes, int xamount = 20, int prd = 1000, int height = 400, bool local = 0) {
        *_GPP += F("<script src='");
        
        if (local) *_GPP += F("/gp_data/AJAX_PLOT.js");
        else *_GPP += F("https://code.highcharts.com/highcharts.js");
        
        *_GPP += F("'></script>\n<div id='");
        *_GPP += name;
        *_GPP += F("' class='chartBlock' style='height:");
        *_GPP += height;
        *_GPP += F("px'></div><script>\n var ");
        *_GPP += name;
        *_GPP += F("=new Highcharts.Chart({\n"
        "chart:{borderRadius:10,renderTo:'");
        *_GPP += name;
        *_GPP += F("',style:{fontFamily:'sans-serif'}},\n"
        "title:{text:''},"
        "series:[");
        for (int i = 0; i < axes; i++) {
            *_GPP += F("{data:[]}");
            if (i != axes - 1) *_GPP += ',';
        }
        *_GPP += F("],\n"
        "xAxis:{type:'datetime',dateTimeLabelFormats:{second:'%H:%M:%S'}},\n"
        "yAxis:{title:{enabled:false}},"
        "credits:{enabled:false}});\n"
        "setInterval(function(){var xhttp=new XMLHttpRequest();var ch=");
        *_GPP += name;
        *_GPP += F("\n"
        "xhttp.onreadystatechange=function(){if(this.readyState==4&&this.status==200){\n"
        "var x=new Date().getTime()-new Date().getTimezoneOffset()*60000;"
        "var arr=this.responseText.split(',');"
        "var move=(ch.series[0].data.length>");
        *_GPP += xamount;
        *_GPP += F(");\n"
        "for(let i=0;i<arr.length;i++)ch.series[i].addPoint([x,Number(arr[i])],true,move,true);\n"
        "}};xhttp.open('GET','/GP_update?");
        *_GPP += name;
        *_GPP += F("=',true);xhttp.send();},\n");
        *_GPP += prd;
        *_GPP += F(");</script>\n");
        send();
    }
    
    void AJAX_PLOT_DARK(const String& name, int axes, int xamount = 20, int prd = 1000, int height = 400, bool local = 0) {
        if (local) *_GPP += F("<script src='/gp_data/AJAX_PLOT.js'></script>\n"
        "<script src='/gp_data/AJAX_PLOT_DARK.js'></script>\n");
        else *_GPP += F("<script src='https://code.highcharts.com/highcharts.js'></script>\n"
        "<script src='https://code.highcharts.com/themes/dark-unica.js'></script>\n");
        
        *_GPP += F("<div id='");
        *_GPP += name;
        *_GPP += F("' class='chartBlock' style='height:");
        *_GPP += height;
        *_GPP += F("px'></div><script>\n var ");
        *_GPP += name;
        *_GPP += F("=new Highcharts.Chart({\n"
        "chart:{borderRadius:10,renderTo:'");
        *_GPP += name;
        *_GPP += F("',style:{fontFamily:'sans-serif'}},\n"
        "title:{text:''},"
        "series:[");
        for (int i = 0; i < axes; i++) {
            *_GPP += F("{data:[]}");
            if (i != axes - 1) *_GPP += ',';
        }
        *_GPP += F("],\n"
        "xAxis:{type:'datetime',dateTimeLabelFormats:{second:'%H:%M:%S'}},\n"
        "yAxis:{title:{enabled:false}},"
        "credits:{enabled:false}});\n"
        "setInterval(function(){var xhttp=new XMLHttpRequest();var ch=");
        *_GPP += name;
        *_GPP += F("\n"
        "xhttp.onreadystatechange=function(){if(this.readyState==4&&this.status==200){\n"
        "var x=new Date().getTime()-new Date().getTimezoneOffset()*60000;"
        "var arr=this.responseText.split(',');"
        "var move=(ch.series[0].data.length>");
        *_GPP += xamount;
        *_GPP += F(");\n"
        "for(let i=0;i<arr.length;i++)ch.series[i].addPoint([x,Number(arr[i])],true,move,true);\n"
        "}};xhttp.open('GET','/GP_update?");
        *_GPP += name;
        *_GPP += F("=',true);xhttp.send();},\n");
        *_GPP += prd;
        *_GPP += F(");</script>\n");
        send();
    }
    
    template <int ax, int am>
    void PLOT_STOCK(const String& id, const char** labels, uint32_t* times, int16_t vals[ax][am], int dec = 0, int height = 400, bool local = 0) {
        *_GPP += F("<script src='");
        
        if (local) *_GPP += F("/gp_data/PLOT_STOCK.js");
        else *_GPP += F("https://code.highcharts.com/stock/highstock.js");
        
        *_GPP += F("'></script>\n <div class='chartBlock' style='height:");
        *_GPP += height;
        *_GPP += F("px' id='");
        *_GPP += id;
        *_GPP += F("'></div>");
        *_GPP += F("<script>Highcharts.stockChart('");
        *_GPP += id;
        *_GPP += F("',{chart:{},\n"
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
            *_GPP += F("{name:'");
            *_GPP += labels[axs];
            *_GPP += F("',data:[\n");
            for (int ams = 0; ams < am; ams++) {
                *_GPP += '[';
                *_GPP += times[ams];
                *_GPP += F("000");
                *_GPP += ',';
                if (dec) *_GPP += (float)vals[axs][ams] / dec;
                else *_GPP += vals[axs][ams];
                *_GPP += "],\n";
                send();
            }
            *_GPP += "]},\n";
        }
        *_GPP += F("]});</script>\n");
        send();
    }
    
    template <int ax, int am>
    void PLOT_STOCK_DARK(const String& id, const char** labels, uint32_t* times, int16_t vals[ax][am], int dec = 0, int height = 400, bool local = 0) {
        if (local) *_GPP += F("<script src='/gp_data/PLOT_STOCK.js'></script>\n"
        "<script src='/gp_data/PLOT_STOCK_DARK.js'></script>\n");
        else *_GPP += F("<script src='https://code.highcharts.com/stock/highstock.js'></script>\n"
        "<script src='https://code.highcharts.com/themes/dark-unica.js'></script>\n");
        
        *_GPP += F("<div class='chartBlock' style='height:");
        *_GPP += height;
        *_GPP += F("px' id='");
        *_GPP += id;
        *_GPP += F("'></div>");
        *_GPP += F("<script>Highcharts.stockChart('");
        *_GPP += id;
        *_GPP += F("',{chart:{},\n"
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
            *_GPP += F("{name:'");
            *_GPP += labels[axs];
            *_GPP += F("',data:[\n");
            for (int ams = 0; ams < am; ams++) {
                *_GPP += '[';
                *_GPP += times[ams];
                *_GPP += F("000");
                *_GPP += ',';
                if (dec) *_GPP += (float)vals[axs][ams] / dec;
                else *_GPP += vals[axs][ams];
                *_GPP += "],\n";
                send();
            }
            *_GPP += "]},\n";
        }
        *_GPP += F("]});</script>\n");
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