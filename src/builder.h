#pragma once

// GP Builder

#include "utils.h"
#include "objects.h"
#include "log.h"
#include <FS.h>
#include "version.h"
#include "buildMacro.h"
#include "canvas.h"
#include "scripts.h"

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
extern const char* _gp_style;
extern uint8_t _gp_seed;
extern const char* _gp_mdns;

struct Builder {
    uint8_t _gp_nav_pos = 0;
    uint8_t _gp_nav_id = 0;
    int _spinInt = 200;
    
    // период изменения значения при удержании кнопки спиннера
    void setSpinnerPeriod(int prd) {
        _spinInt = prd;
    }
    
    // установить таймаут ожидания отправки килков и апдейтов
    void setTimeout(int tout) {
        JS_BEGIN();
        *_GPP += F("_tout=");
        *_GPP += tout;
        JS_END();
    }
    
    // ======================= БИЛДЕР =======================
    void BUILD_BEGIN(int width = 350) {
        PAGE_BEGIN();
        JS_TOP();
        PAGE_BLOCK_BEGIN(width);
    }
    void BUILD_BEGIN(PGM_P style, int width = 350) {
        PAGE_BEGIN();
        THEME(style);
        JS_TOP();
        PAGE_BLOCK_BEGIN(width);
    }
    
    void BUILD_BEGIN_FILE(int width = 350) {
        PAGE_BEGIN();
        JS_TOP_FILE();
        PAGE_BLOCK_BEGIN(width);
    }
    void BUILD_BEGIN_FILE(const String& style, int width = 350) {
        PAGE_BEGIN();
        THEME_FILE(style);
        JS_TOP_FILE();
        PAGE_BLOCK_BEGIN(width);
    }
    
    void BUILD_END() {
        PAGE_BLOCK_END();
        JS_BOTTOM();
        PAGE_END();
    }
    
    void PAGE_TITLE(const String& text = "", const String& name = "") {
        if (name.length()) HIDDEN(name, F("_title"), text);
        if (text.length()) {
            JS_BEGIN();
            *_GPP += F("document.title='");
            *_GPP += text;
            *_GPP += "'";
            JS_END();
        }
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

    // ========================== UI БЛОК ==========================
    PGM_P _ui_style = GP_GREEN;
    
    void UI_BEGIN(const String& title, const String& urls, const String& names, PGM_P st = GP_GREEN, int w = 1000) {
        UI_MENU(title, st);
        GP_parser n(names);
        GP_parser u(urls);
        while (n.parse()) {
            u.parse();
            UI_LINK(u.str, n.str);
        }
        UI_BODY(w);
    }
    
    void UI_MENU(const String& title, PGM_P st = GP_GREEN) {
        _ui_style = st;
        *_GPP += F("<style>.mainblock{max-width:100%!important}</style>\n");      // force full width
        *_GPP += F("<div class='headbar'><div class='burgbtn' id='menuToggle' onclick='sdbTgl()'><span></span><span></span><span></span></div>\n<div class='header'>");
        *_GPP += title;
        *_GPP += F("</div></div>\n<nav class='sidebar' id='dashSdb'><div class='sblock'><div class='header header_s'>");
        *_GPP += title;
        *_GPP += F("</div>\n");
        send();
    }
    void UI_BODY(int w = 1000) {
        *_GPP += F("</div></nav>\n<div class='overlay' onclick='sdbTgl()' id='dashOver'></div><div class='page'><div class='ui_block'");
        if (w != 1000) {
            *_GPP += F(" style='max-width:");
            *_GPP += w;
            *_GPP += F("px'");
        }
        *_GPP += ">\n";
        send();
    }
    void UI_END() {
        SEND(F("</div></div>\n"));
    }
    
    void UI_LINK(const String& url, const String& name) {
        *_GPP += F("<a href='");
        *_GPP += url;
        *_GPP += "'";
        if (_gp_uri->equals(url)) {
            *_GPP += F(" class='sbsel' style='background:");
            *_GPP += FPSTR(_ui_style);
            *_GPP += F(" !important;'");
        }
        *_GPP += ">";
        *_GPP += name;
        *_GPP += F("</a>\n");
    }
    
    // ======================= СТРАНИЦА =======================
    void PAGE_BEGIN() {
        _gp_nav_id = 0;
        SEND(F("<!DOCTYPE HTML><html><head>\n"
        "<meta charset='utf-8'>\n"
        "<meta name='viewport' content='width=device-width, initial-scale=1'>\n"
        "<meta name='apple-mobile-web-app-capable' content='yes'/>\n"
        "<meta name='mobile-web-app-capable' content='yes'/>\n"
        "</head><body>\n"));
    }
    void JS_TOP_FILE() {
        SEND(F("<script src='/gp_data/scripts.js?=" GP_VERSION "'></script>\n"));
        updateTime();
    }
    void JS_TOP() {
        *_GPP += F("<script src='/GP_SCRIPT.js?v" GP_VERSION "=");
        *_GPP += _gp_seed;
        *_GPP += F("'></script>\n");
        updateTime();
    }
    void JS_BOTTOM() {
        SEND(F("<script>document.querySelectorAll('input[type=range]').forEach(x=>{GP_change(x)});\n"
        "document.querySelectorAll('.spin_inp').forEach(x=>GP_spinw(x));\n"
        "</script>\n"));
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
        *_GPP += F("<link rel='stylesheet' href='/GP_STYLE.css?v" GP_VERSION "=");
        *_GPP += ((uint32_t)style) & 0xFFFF;
        *_GPP += "'";
        *_GPP += ">\n";
        _gp_style = style;
    }
    void THEME_FILE(const String& style) {
        *_GPP += F("<link rel='stylesheet' href='/gp_data/");
        *_GPP += style;
        *_GPP += F(".css?=" GP_VERSION "'>\n");
        send();
    }
    
    void PAGE_BLOCK_BEGIN(int width = 350) {
        *_GPP += F("<div align='center' class='mainblock'");
        if (width) {
            *_GPP += F(" style='max-width:");
            *_GPP += width;
            *_GPP += F("px'");
        }
        *_GPP += ">\n";
        send();
    }
    void PAGE_BLOCK_END() {
        SEND(F("</div>\n<div id='onlBlock' class='onlBlock'>🚫 Device offline! 🚫</div>\n"));
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
        JS_BEGIN();
        *_GPP += F("GP_hint('");
        *_GPP += name;
        *_GPP += F("','");
        *_GPP += txt;
        *_GPP += F("')");
        JS_END();
        send();
    }
    
    void JS_BEGIN() {
        *_GPP += F("<script>\n");
    }
    void JS_END() {
        *_GPP += F("\n</script>\n");
    }
    
    void ONLINE_CHECK(int prd = 3000) {
        JS_BEGIN();
        *_GPP += F("setInterval(function(){if(!document.hidden){var xhttp=new XMLHttpRequest();xhttp.timeout=");
        *_GPP += prd;
        *_GPP += F(";xhttp.open('GET','/GP_ping?',true);xhttp.send();\n"
        "xhttp.onreadystatechange=function(){onlShow(!this.status)}}},");
        *_GPP += prd;
        *_GPP += F(");\n");
        JS_END();
    }
    
    // ================== CANVAS ==================
    // обновить CANVAS с именем name при клике на компонент из списка list
    void REDRAW_CLICK(const String& name, const String& list) {
        JS_BEGIN();
        *_GPP += F("_clkRedrList['");
        *_GPP += name;
        *_GPP += F("']='");
        *_GPP += list;
        *_GPP += F("';");
        JS_END();
        send();
    }
    void CANVAS(const String& name, int w, int h) {
        *_GPP += F("<canvas class='_canvas' id='");
        *_GPP += name;
        *_GPP += F("' width='");
        *_GPP += w;
        *_GPP += F("' height='");
        *_GPP += h;
        *_GPP += F("'></canvas>\n");
        send();
    }
    
    void CANVAS_SUPPORT() {
        *_GPP += F("<script src='/GP_CANVAS.js?=");
        *_GPP += _gp_seed;
        *_GPP += F("'></script>\n");
    }
    
    void CANVAS_SUPPORT_FILE() {
        SEND(F("<script src='/gp_data/canvas.js?=" GP_VERSION "'></script>\n"));
    }
    
    void CANVAS_BEGIN(const String& name, int w, int h) {
        CANVAS(name, w, h);
        JS_BEGIN();
        *_GPP += F("var cv=getEl('");
        *_GPP += name;
        *_GPP += F("');\n var cx=cv.getContext('2d');eval(GP_canvas(\"");
    }
    void CANVAS_END() {
        *_GPP += F("\"))");
        JS_END();
    }
    
    // ======================= UPDATE =======================
    void UPDATE(const String& list, int prd = 1000) {
        JS_BEGIN();
        *_GPP += F("setInterval(function(){if(!document.hidden)GP_update('");
        *_GPP += list;
        *_GPP += F("')},");
        *_GPP += prd;
        *_GPP += F(");");
        JS_END();
        send();
    }
    
    void JQ_SUPPORT_FILE() {
        SEND(F("<script src='/gp_data/jquery.js'></script>\n"));
    }
    void JQ_SUPPORT() {
        SEND(F("<script src='https://code.jquery.com/jquery-3.6.1.min.js'></script>\n"));
    }
    void JQ_UPDATE_BEGIN(int prd = 1000, int del = 100) {
        JS_BEGIN();
        *_GPP += F("var globalupd=1;\n"
        "$(function(ev){$('#jqupd').click(function (ev){globalupd=0;setTimeout(function(){$('#jqupd').load('# #jqupd');},");
        *_GPP += del;
        *_GPP += F(");});});\n setInterval(function(){if(globalupd){$('#jqupd').load('# #jqupd');}else{globalupd=1;}},");
        *_GPP += prd;
        *_GPP += F(");");
        JS_END();
        *_GPP += F("<div id='jqupd' style='width:100%;'>\n");
        send();
    }
    void JQ_UPDATE_END() {
        SEND(F("</div>\n"));
    }
    
    // перезагрузка. Имя нужно также указать в списке update
    void RELOAD(const String& name) {
        HIDDEN(name, F("_reload"), "");
    }
    
    // список имён компонентов, изменение (клик) которых приведёт к перезагрузке страницы
    void RELOAD_CLICK(const String& list) {
        JS_BEGIN();
        *_GPP += F("_clkRelList='");
        *_GPP += list;
        *_GPP += F("'.split(',');");
        JS_END();
        send();
    }
    
    // вызвать update у компонентов в списке list при клике по компонентам из списка names
    void UPDATE_CLICK(const String& list, const String& names) {
        JS_BEGIN();
        *_GPP += F("_clkUpdList['");
        *_GPP += names;
        *_GPP += F("']='");
        *_GPP += list;
        *_GPP += F("';");
        JS_END();
        send();
    }
    
    // выполнить код по ответу на обновление
    void EVAL(const String& name, const String& code = "") {
        HIDDEN(name, F("_eval"), code);
    }
    
    // ====================== ТАБЛИЦЫ ======================
    GPalign* _als = nullptr;
    int _alsCount = 0;
    
    void TABLE_BORDER(bool show) {
        *_GPP += F("<style>td{border:");
        *_GPP += show ? F("1px solid") : F("none");
        *_GPP += F("}</style>\n");
    }
    
    void TABLE_BEGIN(const String& tdw = "", GPalign* als = nullptr, const String& w = "100%") {
        _als = als;
        *_GPP += F("<table width='");
        *_GPP += w;
        *_GPP += F("'>\n");
        send();
        
        if (tdw.length()) {
            //TR();
            *_GPP += F("<tr style='visibility:collapse;'>\n");
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
    void ALERT(const String& name) {
        HIDDEN(name, F("_alert"), "");
        send();
    }
    void ALERT(const String& name, const String& text) {
        HIDDEN(name, F("_alert"), text);
        send();
    }
    
    // отправит null если нажать отмена
    void PROMPT(const String& name) {
        HIDDEN(name, F("_prompt"), "");
        send();
    }
    void PROMPT(const String& name, const String& text) {
        HIDDEN(name, F("_prompt"), text);
        send();
    }
    
    void CONFIRM(const String& name) {
        HIDDEN(name, F("_confirm"), "");
        send();
    }
    void CONFIRM(const String& name, const String& text) {
        HIDDEN(name, F("_confirm"), text);
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
        *_GPP += F("<input type='button' onclick='GP_sendForm(this.parentNode.id,\"");
        *_GPP += url;
        *_GPP += F("\")' value='");
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
    void HIDDEN(const String& name, const String& value) {
        *_GPP += F("<input type='hidden' name='");
        *_GPP += name;
        *_GPP += F("' value='");
        *_GPP += value;
        *_GPP += F("' id='");
        *_GPP += name;
        *_GPP += F("'>\n");
        send();
    }
    void HIDDEN(const String& id, const String& name, const String& value) {
        *_GPP += F("<input type='hidden' name='");
        *_GPP += name;
        *_GPP += F("' value=\"");
        *_GPP += value;
        *_GPP += F("\" id='");
        *_GPP += id;
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
    void GRID_BEGIN(int width = 0) {
        *_GPP += F("<div class='grid' id='grid'");
        if (width) {
            *_GPP += F(" style='max-width:");
            *_GPP += width;
            *_GPP += F("px'");
        }
        *_GPP += ">\n";
        send();
    }
    void GRID_END() {
        BLOCK_END();
    }
    void GRID_RESPONSIVE(int width) {
        *_GPP += F("<style type='text/css'>@media screen and (max-width:");
        *_GPP += width;
        *_GPP += F("px){\n.grid{display:block;}\n#grid .block{margin:20px 5px;width:unset;}}</style>\n");
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
        *_GPP += ">\n";
        
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
    void HR(PGM_P st) {
        *_GPP += F("<hr style='border-color:");
        *_GPP += FPSTR(st);
        *_GPP += F("'>\n");
    }
    
    // ======================= ТЕКСТ =======================
    void TAG_RAW(const String& tag, const String& val, const String& name = "", PGM_P st = GP_DEFAULT, int size = 0, bool bold = 0, bool wrap = 0, PGM_P back = GP_DEFAULT) {
        *_GPP += F("<");
        *_GPP += tag;
        if (name.length()) {
            *_GPP += F(" id='");
            *_GPP += name;
            *_GPP += "'";
        }
        *_GPP += F(" style='");
        if (st != GP_DEFAULT) {
            *_GPP += F("color:");
            *_GPP += FPSTR(st);
            *_GPP += ';';
        }
        if (back != GP_DEFAULT) {
            *_GPP += F("background-color:");
            *_GPP += FPSTR(back);
            *_GPP += ';';
        }
        if (size) {
            *_GPP += F("font-size:");
            *_GPP += size;
            *_GPP += "px;";
        }
        if (bold) *_GPP += F("font-weight:bold;");
        if (wrap) *_GPP += F("white-space:normal;");
        *_GPP += F("'>");
        *_GPP += val;
        *_GPP += F("</");
        *_GPP += tag;
        *_GPP += ">\n";
        send();
    }
    
    void TITLE(const String& val, const String& name = "", PGM_P st = GP_DEFAULT, int size = 0, bool bold = 0) {
        TAG_RAW(F("h2"), val, name, st, size, bold);
    }
    void LABEL(const String& val, const String& name = "", PGM_P st = GP_DEFAULT, int size = 0, bool bold = 0, bool wrap = 0) {
        TAG_RAW(F("label"), val, name, st, size, bold, wrap);
    }
    void LABEL_BLOCK(const String& val, const String& name = "", PGM_P st = GP_GREEN, int size = 0, bool bold = 0) {
        TAG_RAW(F("label class='display'"), val, name, GP_DEFAULT, size, bold, 0, st);
    }
    
    // устарело
    void SPAN(const String& text, GPalign al = GP_CENTER, const String& name = "", PGM_P st = GP_DEFAULT, int size = 0, bool bold = 0) {
        if (al != GP_CENTER) {
            *_GPP += F("<div style='text-align:");
            *_GPP += FPSTR(GPgetAlign(al));
            *_GPP += F("'>");
        } else *_GPP += F("<div>");
        TAG_RAW(F("span"), text, name, st, size, bold);
        *_GPP += F("</div>\n");
        send();
    }
    void PLAIN(const String& text, const String& name = "", PGM_P st = GP_DEFAULT) {
        TAG_RAW(F("p"), text, name, st);
        send();
    }
    void BOLD(const String& text, const String& name = "", PGM_P st = GP_DEFAULT) {
        TAG_RAW(F("strong"), text, name, st);
        send();
    }
    // устарело
    
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
    void LED(const String& name, bool state, PGM_P st) {
        *_GPP += F("<style>.led_");
        *_GPP += name;
        *_GPP += F(":checked:after{background-color:");
        *_GPP += FPSTR(st);
        *_GPP += F(";box-shadow:inset 0px 3px #fff7,0px 0px 10px 1px ");
        *_GPP += FPSTR(st);
        *_GPP += F(";}</style>\n");
        
        *_GPP += F("<input class='led led_");
        *_GPP += name;
        *_GPP += F("' type='radio' disabled ");
        if (state) *_GPP += F("checked ");
        *_GPP += F("name='");
        *_GPP += name;
        *_GPP += F("' id='");
        *_GPP += name;
        *_GPP += F("'>\n");
        send();
    }
    
    // устарело
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
    
    String ICON(const String& faname, int size = 20, PGM_P st = GP_DEFAULT) {
        String s(F("<i class='fa fa-"));
        s += faname;
        s += F("' style='");
        if (size) {
            s += F("font-size:");
            s += size;
            s += F("px;");
        }
        if (st != GP_DEFAULT) {
            s += F("color:");
            s += FPSTR(st);
            s += ";";
        }
        s += F("'></i>");
        return s;
    }
    String ICON_FILE(const String& uri, int size = 20, PGM_P st = GP_DEFAULT) {
        String s(F("<i class='i_mask' style='-webkit-mask:center/contain no-repeat url("));
        s += uri;
        s += F(");");
        if (st != GP_DEFAULT) {
            s += F("background-color:");
            s += FPSTR(st);
            s += ";";
        }
        if (size) {
            s += F("width:");
            s += size;
            s += F("px;height:");
            s += size;
            s += F("px;");
        }
        s += F("'></i>");
        return s;
    }
    
    void ICON_BUTTON_RAW(const String& name, const String& icon) {
        *_GPP += F("<div class='i_btn' id='");
        *_GPP += name;
        *_GPP += F("' name='");
        *_GPP += name;
        *_GPP += F("' onmousedown='if(!_touch)GP_press(this,1)' onmouseup='if(!_touch&&_pressId)GP_press(this,2)' onmouseleave='if(_pressId&&!_touch)GP_press(this,2);' "
        "ontouchstart='_touch=1;GP_press(this,1)' ontouchend='GP_press(this,2)' onclick='GP_click(this)'>");
        *_GPP += icon;
        *_GPP += F("</div>");
    }
    
    void ICON_BUTTON(const String& name, const String& faname, int size = 0, PGM_P st = GP_DEFAULT) {
        ICON_BUTTON_RAW(name, ICON(faname, size, st));
    }
    void ICON_FILE_BUTTON(const String& name, const String& uri, int size = 0, PGM_P st = GP_DEFAULT) {
        ICON_BUTTON_RAW(name, ICON_FILE(uri, size, st));
    }
    
    // ======================= НАВИГАЦИЯ =======================
    void NAV_TABS_LINKS(const String& urls, const String& names, PGM_P st = GP_GREEN) {
        *_GPP += F("<div class='navtab'><ul ");
        if (st != GP_GREEN) {
            *_GPP += F("style='background:");
            *_GPP += FPSTR(st);
            *_GPP += "'";
        }
        *_GPP += ">\n";
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
        *_GPP += ">\n";
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
        *_GPP += ">\n";
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
        *_GPP += ">\n";
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
        *_GPP += ">\n";
        send();
        _gp_nav_pos++;
    }
    
    void NAV_BLOCK_END() {
        SEND(F("</div>\n"));
    }
    
    // ======================= ФАЙЛЫ =======================
    void FILE_UPLOAD_RAW(const String& name, const String& text = "", PGM_P st = GP_GREEN, const String& accept = "", const String& options = "", const String& action = "/GP_upload") {
        *_GPP += F("<div id='");
        *_GPP += name;
        *_GPP += F("'><form action='");
        *_GPP += action;
        *_GPP += F("' method='POST' enctype='multipart/form-data' id='");
        *_GPP += name;
        *_GPP += F("_form' style='display:flex;justify-content:center;'>\n"
        "<div id='ubtn' onclick='saveFile(\"");
        *_GPP += name;
        *_GPP += F("_inp\")'");
        if (st != GP_GREEN) {
            *_GPP += F(" style='background:");
            *_GPP += FPSTR(st);
            *_GPP += "'";
        }
        *_GPP += ">";
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
        "</form></div>\n");
        send();
    }
    
    void FILE_UPLOAD(const String& name, const String& text = "", const String& accept = "", PGM_P st = GP_GREEN) {
        FILE_UPLOAD_RAW(name, "📄 " + text, st, accept, F("multiple "));
    }
    
    void FOLDER_UPLOAD(const String& name, const String& text = "", PGM_P st = GP_GREEN) {
        FILE_UPLOAD_RAW(name, "📁 " + text, st, "", F("multiple webkitdirectory allowdirs "));
    }
    
    void OTA_FIRMWARE(const String& text = "OTA firmware", PGM_P st = GP_GREEN, bool page = 0) {
        FILE_UPLOAD_RAW(F("firmware"), "🧱 " + text, st, F(".bin,.bin.gz"), "", page ? F("/ota_update") : F("/GP_OTAupload"));
    }
    
    void OTA_FILESYSTEM(const String& text = "OTA filesystem", PGM_P st = GP_GREEN, bool page = 0) {
        FILE_UPLOAD_RAW(F("filesystem"), "💽 " + text, st, F(".bin,.bin.gz"), "", page ? F("/ota_update") : F("/GP_OTAupload"));
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
    
    void CAM_STREAM(int width = 500, int port = 90) {
        *_GPP += F("<img id='_stream' style='max-height:100%;width:");
        *_GPP += width;
        *_GPP += F("px'>\n<script>window.onload=document.getElementById('_stream').src=window.location.href.slice(0,-1)+':");
        *_GPP += port;
        *_GPP += F("/';</script>\n");
        send();
    }
    void CAM_STREAM(const String& width, int port = 90) {
        *_GPP += F("<img id='_stream' style='max-height:100%;width:");
        *_GPP += width;
        *_GPP += F("'>\n<script>window.onload=document.getElementById('_stream').src=window.location.href.slice(0,-1)+':");
        *_GPP += port;
        *_GPP += F("/';</script>\n");
        send();
    }
    
    // ======================= ФАЙЛОВЫЙ МЕНЕДЖЕР =======================
    void _fileRow(const String& fpath, int size) {
        *_GPP += "<tr>";
        *_GPP += F("<td align='left' style='padding-right:5px'>\n"
        "<a style='text-decoration:none' href='");
        *_GPP += fpath;
        *_GPP += F("'>");
        *_GPP += fpath;
        *_GPP += F("</a>\n<td align='right'>[");
        *_GPP += String(size / 1000.0, 1);
        *_GPP += F(" kB]\n"
        "<td align='center'>\n"
        "<span title='Rename' style='cursor:pointer' onclick='GP_rename(\"");
        *_GPP += fpath;
        *_GPP += F("\")'>📝</span>\n"
        "<a style='text-decoration:none' href='");
        *_GPP += fpath;
        *_GPP += F("' download><span title='Download'>📥</span></a>\n"
        "<span title='Delete' style='cursor:pointer' onclick='GP_delete(\"");
        *_GPP += fpath;
        *_GPP += F("\")'>❌</span>\n");
    }

    void _showFiles(fs::FS *fs, const String& path, const String& odir, __attribute__((unused)) uint8_t levels = 0) {
#ifdef ESP8266
        yield();
        Dir dir = fs->openDir(path);
        while (dir.next()) {
            if (dir.isFile() && dir.fileName().length()) {
                String fpath = '/' + path + dir.fileName();
                if (odir.length() && !fpath.startsWith(odir)) continue;
                _fileRow(fpath, dir.fileSize());
            }
            if (dir.isDirectory()) {
                String p = path;
                p += dir.fileName();
                p += '/';
                Dir sdir = fs->openDir(p);
                _showFiles(fs, p, odir);
            }
        }

#else   // ESP32
        File root = fs->open(path.length() ? path.c_str() : ("/"));
        if (!root || !root.isDirectory()) return;
        File file;
        while (file = root.openNextFile()) {
            if (file.isDirectory()) {
                if (levels) _showFiles(fs, file.path(), odir, levels - 1);
            } else {
                String fpath = path + '/' + file.name();
                if (odir.length() && !fpath.startsWith(odir)) continue;
                _fileRow(fpath, file.size());
            }
        }
#endif
    }
    
    void FILE_MANAGER(fs::FS *fs, const String& odir = "") {
        *_GPP += F("<table>");
        _showFiles(fs, "", odir, 5);

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
        *_GPP += F("</table>\n");
        send();
    }
    
    // ================ СИСТЕМНАЯ ИНФОРМАЦИЯ ================
    void SYSTEM_INFO(const String& fwv = "", const String& w = "") {
        TABLE_BEGIN(w);
        // ===========
        TR();
        TD(GP_CENTER, 3);
        LABEL(F("Network"));
        HR();
        
        TR();
        TD(GP_LEFT); BOLD(F("WiFi Mode"));
        TD(GP_RIGHT); SEND(WiFi.getMode() == WIFI_AP ? F("AP") : (WiFi.getMode() == WIFI_STA ? F("STA") : F("AP_STA")));
        
        if (WiFi.getMode() != WIFI_AP) {
            TR();
            TD(GP_LEFT); BOLD(F("SSID"));
            TD(GP_RIGHT); SEND(WiFi.SSID());
            
            TR();
            TD(GP_LEFT); BOLD(F("Local IP"));
            TD(GP_RIGHT); SEND(WiFi.localIP().toString());
        }
        if (WiFi.getMode() != WIFI_STA) {
            TR();
            TD(GP_LEFT); BOLD(F("AP IP"));
            TD(GP_RIGHT); SEND(WiFi.softAPIP().toString());
        }
        
        if (_gp_mdns && strlen(_gp_mdns)) {
            TR();
            TD(GP_LEFT); BOLD(F("mDNS"));
            TD(GP_RIGHT); SEND(String(_gp_mdns) + ".local");
        }
        
        TR();
        TD(GP_LEFT); BOLD(F("Subnet"));
        TD(GP_RIGHT); SEND(WiFi.subnetMask().toString());
        
        TR();
        TD(GP_LEFT); BOLD(F("Gateway"));
        TD(GP_RIGHT); SEND(WiFi.gatewayIP().toString());
        
        TR();
        TD(GP_LEFT); BOLD(F("MAC Address"));
        TD(GP_RIGHT); SEND(WiFi.macAddress());
        
        TR();
        TD(GP_LEFT); BOLD(F("RSSI"));
        TD(GP_RIGHT); SEND("📶 " + String(constrain(2 * (WiFi.RSSI() + 100), 0, 100)) + '%');
        
        // ===========
        TR();
        TD(GP_CENTER, 3);
        LABEL(F("Memory"));
        HR();
        
        TR();
        TD(GP_LEFT); BOLD(F("Free Heap"));
        TD(GP_RIGHT); SEND(String(ESP.getFreeHeap() / 1000.0, 3) + " kB");
        
    #ifdef ESP8266
        TR();
        TD(GP_LEFT); BOLD(F("Heap Fragm."));
        TD(GP_RIGHT); SEND(String(ESP.getHeapFragmentation()) + '%');
    #endif
        
        TR();
        TD(GP_LEFT); BOLD(F("Sketch Size (Free)"));
        TD(GP_RIGHT); SEND(String(ESP.getSketchSize() / 1000.0, 1) + " kB (" + String(ESP.getFreeSketchSpace() / 1000.0, 1) + ")");
        
        TR();
        TD(GP_LEFT); BOLD(F("Flash Size"));
        TD(GP_RIGHT); SEND(String(ESP.getFlashChipSize() / 1000.0, 1) + " kB");
        
        // ===========
        TR();
        TD(GP_CENTER, 3);
        LABEL(F("System"));
        HR();
        
    #ifdef ESP8266
        TR();
        TD(GP_LEFT); BOLD(F("Chip ID"));
        TD(GP_RIGHT); SEND("0x" + String(ESP.getChipId(), HEX));
    #endif
        
        TR();
        TD(GP_LEFT); BOLD(F("Cycle Count"));
        TD(GP_RIGHT); SEND(String(ESP.getCycleCount()));
        
        TR();
        TD(GP_LEFT); BOLD(F("Cpu Freq."));
        TD(GP_RIGHT); SEND(String(ESP.getCpuFreqMHz()) + F(" MHz"));
        
        TR();
        TD(GP_LEFT); BOLD(F("Date"));
        GPdate date(_gp_local_unix);
        TD(GP_RIGHT); SEND(_gp_local_unix ? date.encodeDMY() : String("unset"));
        
        TR();
        TD(GP_LEFT); BOLD(F("Time"));
        GPtime time(_gp_local_unix);
        TD(GP_RIGHT); SEND(_gp_local_unix ? time.encode() : String("unset"));
        
        TR();
        TD(GP_LEFT); BOLD(F("Uptime"));
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
        TD(GP_RIGHT); SEND(s);
        
        // ===========
        TR();
        TD(GP_CENTER, 3);
        LABEL(F("Version"));
        HR();
        
        TR();
        TD(GP_LEFT); BOLD(F("SDK"));
        TD(GP_RIGHT); SEND(ESP.getSdkVersion());
        
    #ifdef ESP8266
        TR();
        TD(GP_LEFT); BOLD(F("Core"));
        TD(GP_RIGHT); SEND(ESP.getCoreVersion());
    #endif
    
        TR();
        TD(GP_LEFT); BOLD(F("GyverPortal"));
        TD(GP_RIGHT); SEND(GP_VERSION);
        
        if (fwv.length()) {
            TR();
            TD(GP_LEFT); BOLD(F("Firmware"));
            TD(GP_RIGHT); SEND(fwv);
        }

        TABLE_END();
    }
    
    // ======================= КНОПКА =======================
    void BUTTON_RAW(const String& name, const String& value, const String& tar, PGM_P st, const String& width = "", const String& cls = "", bool dis = 0, bool rel = 0) {
        *_GPP += F("<button ");
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
        *_GPP += F("' id='");
        *_GPP += name;
        *_GPP += F("' onmousedown='if(!_touch)GP_press(this,1)' onmouseup='if(!_touch&&_pressId)GP_press(this,2)' onmouseleave='if(_pressId&&!_touch)GP_press(this,2);' ");
        if (!dis) *_GPP += F("ontouchstart='_touch=1;GP_press(this,1)' ontouchend='GP_press(this,2)' ");
        if (tar.length()) {
            *_GPP += F("onclick=\"GP_clickid('");
            *_GPP += name;
            *_GPP += F("','");
            *_GPP += tar;
            *_GPP += F("')\"");
        } else {
            *_GPP += F("onclick='GP_click(this,");
            *_GPP += rel;
            *_GPP += F(")'");
        }
        if (dis) *_GPP += F(" disabled");
        *_GPP += ">";
        *_GPP += value;
        *_GPP += F("</button>\n");
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
            *_GPP += F("' id='");
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
            *_GPP += F("' onclick='GP_click(this,\"");
            *_GPP += url;
            *_GPP += F("\");'>\n");
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
    void NUMBER_RAW(const String& name, const String& place = "", const String& value = "", const String& minv = "", const String& maxv = "", const String& width = "", const String& pattern = "", bool dis = 0) {
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
        if (pattern.length()) {
            *_GPP += F("' pattern=");
            *_GPP += pattern;
        }
        *_GPP += F("' placeholder='");
        *_GPP += place;
        *_GPP += F("' onchange='GP_click(this)'");
        if (dis) *_GPP += F(" disabled");
        *_GPP += ">\n";
        send();
    }
    void NUMBER(const String& name, const String& place = "", int value = INT32_MAX, const String& width = "", bool dis = false) {
        NUMBER_RAW(name, place, (value == INT32_MAX ? String("") : String(value)), "", "", width, "", dis);
    }
    void NUMBER_F(const String& name, const String& place = "", float value = NAN, uint8_t dec = 2, const String& width = "", bool dis = false) {
        NUMBER_RAW(name, place, (isnan(value) ? String("") : String(value, (uint16_t)dec)), "", "", width, "", dis);
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
    void PASS(const String& name, const String& place = "", const String& value = "", const String& width = "", int maxlength = 0, const String& pattern = "", bool dis = false, bool eye = 0) {
        *_GPP += F("<div class='inlBlock'><input type='password' name='");
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
        if (eye) *_GPP += F("<span class='eyepass' onclick='GP_eye(this)'>&#x1f441;&#xFE0E;</span>");
        *_GPP += F("</div>\n");
        send();
    }
    void PASS_EYE(const String& name, const String& place = "", const String& value = "", const String& width = "", int maxlength = 0, const String& pattern = "", bool dis = false) {
        PASS(name, place, value, width, maxlength, pattern, dis, true);
    }
    
    void AREA(const String& name, int rows = 1, const String& value = "", const String& width = "", bool dis = false) {
        *_GPP += F("<textarea onchange='GP_click(this)' style='height:auto' name='");
        *_GPP += name;
        *_GPP += F("' id='");
        *_GPP += name;
        *_GPP += F("' rows='");
        *_GPP += rows;
        if (width.length()) {
            *_GPP += F("' style='width:");
            *_GPP += width;
        }
        *_GPP += "'";
        if (dis) *_GPP += F(" disabled");
        *_GPP += ">";
        if (value.length()) *_GPP += value;
        *_GPP += F("</textarea>\n");
        send();
    }
    
    void AREA_LOG_RAW(const String& name, int rows = 5, int prd = 1000, const String& w = "") {
        *_GPP += F("<div class='inlBlock'><textarea name='_gplog' style='height:auto;");
        if (w.length()) {
            *_GPP += F("width:");
            *_GPP += w;
        }
        *_GPP += F("' id='");
        *_GPP += name;
        *_GPP += F("' rows='");
        *_GPP += rows;
        *_GPP += F("' disabled></textarea>\n");
        *_GPP += F("<div class='ctrlBlock'><span class='areaCtrl' style='color:red' onclick='logClear(\"");
        *_GPP += name;
        *_GPP += F("\")'>x</span><span class='areaCtrl' style='color:#888' onclick='logToggle(\"");
        *_GPP += name;
        *_GPP += F("\")'>s</span></div>\n");
        *_GPP += F("</div>\n");
        
        JS_BEGIN();
        *_GPP += F("setInterval(()=>GP_update('");
        *_GPP += name;
        *_GPP += F("'),");
        *_GPP += prd;
        *_GPP += F(");");
        JS_END();
        send();
    }
    void AREA_LOG(int rows = 5, int prd = 1000, const String& w = "") {
        AREA_LOG_RAW("GP_log", rows, prd, w);
    }
    void AREA_LOG(GPlog& log, int rows = 5, int prd = 1000, const String& w = "") {
        log.clear();
        AREA_LOG_RAW(log.name, rows, prd, w);
    }
    
    // ======================= НАСТРОЙКА =======================
    void CHECK(const String& name, bool state = 0, PGM_P st = GP_GREEN, bool dis = false) {
        if (st != GP_GREEN) {
            *_GPP += F("<style>#__");
            *_GPP += name;
            *_GPP += F(" input:checked+span::before{border-color:");
            *_GPP += FPSTR(st);
            *_GPP += F(";background-color:");
            *_GPP += FPSTR(st);
            *_GPP += F("}</style>\n");
        }
        *_GPP += F("<label id='__");
        *_GPP += name;
        *_GPP += F("' class='check_c'><input type='checkbox' name='");
        *_GPP += name;
        *_GPP += F("' id='");
        *_GPP += name;
        *_GPP += "' ";
        if (state) *_GPP += F("checked ");
        if (dis) *_GPP += F("disabled ");
        *_GPP += F("onclick='GP_click(this)'><span></span></label>\n"
        "<input type='hidden' value='0' name='");
        *_GPP += name;
        *_GPP += "'>\n";
        send();
    }
    void SWITCH(const String& name, bool state = 0, PGM_P st = GP_GREEN, bool dis = false) {
        if (st != GP_GREEN) {
            *_GPP += F("<style>#__");
            *_GPP += name;
            *_GPP += F(" input:checked+.slider{background-color:");
            *_GPP += FPSTR(st);
            *_GPP += F("}</style>\n");
        }
        *_GPP += F("<label id='__");
        *_GPP += name;
        *_GPP += F("' class='switch'><input class='_sw_c' type='checkbox' name='");
        *_GPP += name;
        *_GPP += F("' id='");
        *_GPP += name;
        *_GPP += "' ";
        if (state) *_GPP += F("checked ");
        if (dis) *_GPP += F("disabled ");
        *_GPP += F("onclick='GP_click(this)'>\n"
        "<span class='slider' id='_");
        *_GPP += name;
        *_GPP += F("'></span></label>\n"
        "<input type='hidden' value='0' name='");
        *_GPP += name;
        *_GPP += F("'>\n");
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
    
    void _FLOAT_DEC(float val, uint16_t dec) {
        if (!dec) *_GPP += (int)round(val);
        else *_GPP += String(val, (uint16_t)dec);
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
        *_GPP += ">\n";
        *_GPP += F("<output align='center' id='");
        *_GPP += name;
        *_GPP += F("_val' ");
        if (st != GP_GREEN) {
            *_GPP += F("style='background:");
            *_GPP += FPSTR(st);
            *_GPP += F(";'");
        }
        if (dis) *_GPP += F(" class='dsbl'");
        *_GPP += F(">");
        _FLOAT_DEC(value, dec);
        *_GPP += F("</output>\n");
        send();
    }
    
    void SLIDER_C(const String& name, float value = 0, float min = 0, float max = 100, float step = 1, uint8_t dec = 0, PGM_P st = GP_GREEN, bool dis = 0) {
        SLIDER(name, value, min, max, step, dec, st, dis, 1);
    }
    
    void SPIN_BTN(const String& name, float step, PGM_P st, uint8_t dec, bool dis) {
        *_GPP += F("<input type='button' class='spinBtn ");
        *_GPP += (step > 0) ? F("spinR") : F("spinL");
        *_GPP += F("' name='");
        *_GPP += name;
        *_GPP += F("' min='");  // step
        *_GPP += step;
        *_GPP += F("' max='");  // dec
        *_GPP += dec;
        *_GPP += F("' onmouseleave='if(_pressId)clearInterval(_spinInt);_spinF=_pressId=null' onmousedown='_pressId=this.name;_spinInt=setInterval(()=>{GP_spin(this);_spinF=1},");
        *_GPP += _spinInt;
        *_GPP += F(")' onmouseup='clearInterval(_spinInt)' onclick='if(!_spinF)GP_spin(this);_spinF=0' value='");
        *_GPP += (step > 0) ? '+' : '-';
        *_GPP += "' ";
        if (st != GP_GREEN) {
            *_GPP += F(" style='background:");
            *_GPP += FPSTR(st);
            *_GPP += F(";'");
        }
        if (dis) *_GPP += F(" disabled");
        *_GPP += ">\n";
    }
    
    void SPINNER(const String& name, float value = 0, float min = NAN, float max = NAN, float step = 1, uint16_t dec = 0, PGM_P st = GP_GREEN, const String& w = "", bool dis = 0) {
        *_GPP += F("<div id='spinner' class='spinner'>\n");
        SPIN_BTN(name, -step, st, dec, dis);
        *_GPP += F("<input type='number' name='");
        *_GPP += name;
        *_GPP += F("' id='");
        *_GPP += name;
        if (w.length()) {
            *_GPP += F("' style='width:");
            *_GPP += w;
        }
        *_GPP += F("' step='");
        _FLOAT_DEC(step, dec);
        *_GPP += F("' onkeyup='GP_spinw(this)' onkeydown='GP_spinw(this)' onchange='GP_spinw(this);GP_click(this)' onmousewheel='GP_spinw(this);GP_wheel(this);GP_click(this)' value='");
        _FLOAT_DEC(value, dec);
        if (!isnan(min)) {
            *_GPP += F("' min='");
            _FLOAT_DEC(min, dec);
        }
        if (!isnan(max)) {
            *_GPP += F("' max='");
            _FLOAT_DEC(max, dec);
        }
        *_GPP += "' ";
        if (dis) *_GPP += F("disabled ");
        if (!w.length()) *_GPP += F("class='spin_inp'");
        *_GPP += ">\n";
        SPIN_BTN(name, step, st, dec, dis);
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
    
    void RADIO(const String& name, int num, int val = -1, PGM_P st = GP_DEFAULT, bool dis = 0) {
        if (st != GP_DEFAULT) {
            *_GPP += F("<style>.rad_");
            *_GPP += name;
            *_GPP += F(":after{border-color:");
            *_GPP += FPSTR(st);
            *_GPP += F("}.rad_");
            *_GPP += name;
            *_GPP += F(":checked:after{background-color:");
            *_GPP += FPSTR(st);
            *_GPP += F("}</style>\n");
        }
        
        *_GPP += F("<input class='rad rad_");
        *_GPP += name;
        if (dis) *_GPP += F(" dsbl");
        *_GPP += F("' type='radio' name='");
        *_GPP += name;
        *_GPP += F("' id='");
        *_GPP += name;
        *_GPP += '_';
        *_GPP += num;
        *_GPP += F("' value='");
        *_GPP += num;
        *_GPP += F("' onchange='GP_click(this)'");
        if (val == num) *_GPP += F(" checked");
        if (dis) *_GPP += F(" disabled");
        *_GPP += ">\n";
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
        *_GPP += rel;
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
        *_GPP += rel;
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
        *_GPP += rel;
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
    void AJAX_PLOT(const String& name, const char** labels, int axes, int xamount = 20, int prd = 1000, int height = 400, bool local = 0) {
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
            *_GPP += F("{data:[],name:'");
            *_GPP += labels[i];
            *_GPP += F("'}");
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
    void AJAX_PLOT_DARK(const String& name, const char** labels, int axes, int xamount = 20, int prd = 1000, int height = 400, bool local = 0) {
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
            *_GPP += F("{data:[],name:'");
            *_GPP += labels[i];
            *_GPP += F("'}");
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
        TITLE(title.text, title.name, title.style, title.size, title.bold);
    }
    void LABEL(GP_LABEL& label) {
        LABEL(label.text, label.name, label.style, label.size, label.bold, label.wrap);
    }
    void LABEL_BLOCK(GP_LABEL_BLOCK& label) {
        LABEL_BLOCK(label.text, label.name, label.style, label.size, label.bold);
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
        BUTTON(btn.name, btn.text, btn.target, btn.style, btn.width, btn.disabled, btn.reload);
    }
    void BUTTON_MINI(GP_BUTTON_MINI& btn) {
        BUTTON_MINI(btn.name, btn.text, btn.target, btn.style, btn.width);
    }
    
    void NUMBER(GP_NUMBER& num) {
        NUMBER_RAW(num.name, num.placeholder, (num.value == INT32_MAX ? String("") : String(num.value)), num.min, num.max, num.width, num.pattern, num.disabled);
    }
    void NUMBER_F(GP_NUMBER_F& num) {
        NUMBER_F(num.name, num.placeholder, num.value, num.decimals, num.width, num.disabled);
        NUMBER_RAW(num.name, num.placeholder, (isnan(num.value) ? String("") : String(num.value, (uint16_t)num.decimals)), num.min, num.max, num.width, num.pattern, num.disabled);
    }
    
    void TEXT(GP_TEXT& txt) {
        TEXT(txt.name, txt.placeholder, txt.text, txt.width, txt.maxlen, txt.pattern, txt.disabled);
    }
    void PASS(GP_PASS& pas) {
        PASS(pas.name, pas.placeholder, pas.text, pas.width, pas.maxlen, pas.pattern, pas.disabled, pas.eye);
    }
    
    void AREA(GP_AREA& ar) {
        AREA(ar.name, ar.rows, ar.text, ar.width, ar.disabled);
    }
    
    void CHECK(GP_CHECK& ch) {
        CHECK(ch.name, ch.state, ch.style, ch.disabled);
    }
    void SWITCH(GP_SWITCH& sw) {
        SWITCH(sw.name, sw.state, sw.style, sw.disabled);
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
        SPINNER(s.name, s.value, s.min, s.max, s.step, s.decimals, s.style, s.width, s.disabled);
    }
    void SLIDER(GP_SLIDER& s) {
        SLIDER(s.name, s.value, s.min, s.max, s.step, s.decimals, s.style, s.disabled, s.oninput);
    }
    
    void SELECT(GP_SELECT& s) {
        SELECT(s.name, s.list, s.selected, s.numbers, s.disabled, s.reload);
    }
    
    void RADIO(GP_RADIO& r) {
        RADIO(r.name, r.num, r.value, r.style, r.disabled);
    }
};