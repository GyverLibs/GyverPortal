#pragma once
#include <DNSServer.h>
#ifdef ESP8266
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#else
#include <WiFi.h>
#include <WebServer.h>
#endif

#include "themes.h"
#include "list.h"

// ======================== ДАТА ========================
struct GPdate {
    uint16_t year;
    uint8_t month, day;
};

struct GPtime {
    uint8_t hour, minute;
};

static String* _gp_sptr;
static void* _gp_ptr;

// ======================= ПОРТАЛ =======================
class GyverPortal {
public:
    GyverPortal() {
        uri.reserve(20);
    }

    ~GyverPortal() {
        stop();
    }
    
    List list;

    // запустить портал. Можно передать WIFI_AP для запуска DNS сервера
    void start(uint8_t mode = WIFI_STA) {
        _mode = (mode == WIFI_AP);
        _active = true;
        server.begin();
        if (_mode) dnsServer.start(53, "*", WiFi.softAPIP());
        server.onNotFound([this]() {
            uri = server.uri();
            if (uri.startsWith(F("/favicon.ico"))) show();
            else _actionF = 1;
        });
    }

    // подключить функцию-билдер страницы
    void attachBuild(void (*handler)()) {
        _build = *handler;
    }
    
    // подключить функцию, которая вызывается при действии в браузере
    void attachAction(void (*handler)(GyverPortal* p)) {
        _action = *handler;
    }

    // остановить портал
    void stop() {
        _active = false;
        if (_mode) dnsServer.stop();
        server.stop();
    }

    // тикер портала. Вернёт true, если портал запущен
    bool tick() {
        if (!_active) return 0;
        if (_actionF) {
            show();
            _actionF = 0;
        }

        if (_mode) dnsServer.processNextRequest();
        server.handleClient();
        if (_actionF) {
            checkList();
            if (*_action) _action(this);
        }
        yield();
        return 1;
    }

    // вернёт true, если было действие с браузера
    bool action() {
        return _actionF;
    }

    // вернёт true, если было действие с указанной формы
    bool form(const char* f) {
        return uri.startsWith(f);
    }
    
    // вернёт true, если открыта главная страница (/)
    bool root() {
        return (uri[0] == '/' && uri[1] == '\0');
    }

    // получить String строку с элемента
    String getString(const char* n) {
        return server.arg(n);
    }
    // получить char* строку с элемента
    const char* getChars(const char* n) {
        return server.arg(n).c_str();
    }
    // переписать char строку с элемента к себе
    void copyStr(const char* n, char* d) {
        if (server.hasArg(n)) strcpy(d, server.arg(n).c_str());
    }

    // получить число с элемента
    int getInt(const char* n) {
        return server.arg(n).toInt();
    }
    
    // получить float с элемента
    float getFloat(const char* n) {
        return server.arg(n).toFloat();
    }

    // получить состояние чекбокса
    bool getCheck(const char* n) {
        return server.hasArg(n);
    }

    // получить дату с элемента
    GPdate getDate(const char* n) {
        String s = server.arg(n);
        return (GPdate) {
            (uint16_t)s.substring(0, 4).toInt(),
            (uint8_t)s.substring(5, 7).toInt(),
            (uint8_t)s.substring(8, 10).toInt()
        };
    }

    // получить время с элемента
    GPtime getTime(const char* n) {
        String s = server.arg(n);
        return (GPtime) {
            (uint8_t)s.substring(0, 2).toInt(),
            (uint8_t)s.substring(3, 5).toInt()
        };
    }

    // получить цвет с элемента
    uint32_t getColor(const char* n) {
        if (!server.hasArg(n)) return 0;
        String hex = server.arg(n);
        if (hex.length() < 2 || hex.length() > 7) return 0;
        uint32_t val = 0;
        for (uint8_t i = 0; i < hex.length() - 1; i++) {
            val <<= 4;
            uint8_t d = hex[i + 1];
            d -= (d <= '9') ? 48 : ((d <= 'F') ? 55 : 87);
            val |= d;
        }
        return val;
    }
    
    // получить номер выбранного пункта в дроплисте
    int8_t getSelected(const char* n, const char* list) {
        return inList(server.arg(n).c_str(), list);
    }
    
    // получить номер, под которым name входит в list (вида "val 1, val 2, val 3")
    int8_t inList(const char* n, const char* list) {
        uint8_t prev = 0, i = 0, count = 0;
        char* ptr = (char*)list;
        while (1) {
            bool end = (ptr[i] == '\0');
            if (ptr[i] == ',' || end) {      
                ptr[i] = '\0';
                char* str = ptr + prev;
                if (!strcmp(str, n)) {
                    if (!end) ptr[i] = ',';
                    return count;
                }
                count++;
                if (end) break;
                ptr[i] = ',';
                prev = i + 1;
            }
            i++;
        }
        return -1;
    }
    
    // показать свою страницу
    void showPage(String &s) {
        server.send(200, F("text/html"), s);
    }
    
    // вызвать конструктор и показать страницу
    void show() {
        _gp_ptr = this;
        if (*_build) _build();
        _gp_ptr = nullptr;
    }
    
#ifdef ESP8266
    ESP8266WebServer server;
#else
    WebServer server;
#endif
private:
    void checkList() {
        if (!list.idx) return;
        for (int i = 0; i < list.idx; i++) {
            if (list.skips[i] || form(list.forms[i])) {
                switch (list.types[i]) {
                case T_CSTR:    copyStr(list.names[i], (char*)list.vals[i]);        break;
                case T_STRING:  *(String*)list.vals[i] = getString(list.names[i]);  break;
                case T_TIME:    *(GPtime*)list.vals[i] = getTime(list.names[i]);    break;
                case T_DATE:    *(GPdate*)list.vals[i] = getDate(list.names[i]);    break;
                case T_CHECK:   *(bool*)list.vals[i] = getCheck(list.names[i]);     break;
                case T_INT8:    *(int8_t*)list.vals[i] = (int8_t)getInt(list.names[i]); break;
                case T_INT:   *(long*)list.vals[i] = getInt(list.names[i]);       break;
                case T_FLOAT:   *(float*)list.vals[i] = getFloat(list.names[i]);    break;
                case T_COLOR:   *(uint32_t*)list.vals[i] = getColor(list.names[i]); break;
                }
            }
        }
    }
    
    String uri;
    bool _mode = false;
    bool _active = false;
    bool _actionF = 0;
    void (*_build)() = nullptr;
    void (*_action)(GyverPortal* p) = nullptr;
    IPAddress apIP;
    DNSServer dnsServer;
};

// ======================= БИЛДЕР =======================
struct Builder {
    void PAGE_BEGIN() {
        *_gp_sptr += F("<!DOCTYPE HTML><html><head>\n"
        "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n"
        "</head><body><div align=\"center\" style=\"margin:auto;max-width:450px;\">\n");
    }
    void THEME(const char* style) {
        *_gp_sptr += FPSTR(style);
    }
    void PAGE_END() {
        *_gp_sptr += F("</div></body></html>");
    }

    void BLOCK_BEGIN() {
        *_gp_sptr += F("<div class=\"block\">\n");
    }
    void BLOCK_END() {
        *_gp_sptr += F("</div>\n");
    }
    void BREAK() {
        *_gp_sptr += F("<br>\n");
    }
    void HR() {
        *_gp_sptr += F("<hr>\n");
    }
    void TITLE(const char* name) {
        *_gp_sptr += F("<h2>");
        *_gp_sptr += name;
        *_gp_sptr += F("</h2>\n");
    }
    void LABEL(const char* name) {
        *_gp_sptr += F("<label>");
        *_gp_sptr += name;
        *_gp_sptr += F("</label>\n");
    }
    void LABEL(int name) {
        *_gp_sptr += F("<label>");
        *_gp_sptr += name;
        *_gp_sptr += F("</label>\n");
    }
    void FORM_BEGIN(const char* action) {
        *_gp_sptr += F("<form action=\"");
        *_gp_sptr += action;
        *_gp_sptr += F("\" method=\"POST\">\n");
    }
    void FORM_END() {
        *_gp_sptr += F("</form>\n");
    }
    void TEXT(const char* name, const char* place, int value) {
        *_gp_sptr += F("<input type=\"text\" name=\"");

        *_gp_sptr += name;
        *_gp_sptr += F("\" value=\"");
        *_gp_sptr += value;
        *_gp_sptr += F("\" placeholder=\"");
        *_gp_sptr += place;
        *_gp_sptr += F("\">\n");
    }
    void TEXT(const char* name, const char* place, const char* value) {
        *_gp_sptr += F("<input type=\"text\" name=\"");
        *_gp_sptr += name;
        *_gp_sptr += F("\" value=\"");
        *_gp_sptr += value;
        *_gp_sptr += F("\" placeholder=\"");
        *_gp_sptr += place;
        *_gp_sptr += F("\">\n");
    }
    void PASS(const char* name, const char* place, const char* value) {
        *_gp_sptr += F("<input type=\"password\" name=\"");
        *_gp_sptr += name;
        *_gp_sptr += F("\" value=\"");
        *_gp_sptr += value;
        *_gp_sptr += F("\" placeholder=\"");
        *_gp_sptr += place;
        *_gp_sptr += F("\">\n");
    }
    void CHECK(const char* name, bool x = 0) {
        *_gp_sptr += F("<input type=\"checkbox\" name=\"");
        *_gp_sptr += name;
        *_gp_sptr += (x ? F("\" checked>\n") : F("\">\n"));
    }
    void DATE(const char* name) {
        *_gp_sptr += F("<input type=\"date\" name=\"");
        *_gp_sptr += name;
        *_gp_sptr += F("\">\n");
    }
    void DATE(const char* name, GPdate d) {
        *_gp_sptr += F("<input type=\"date\" name=\"");
        *_gp_sptr += name;
        *_gp_sptr += F("\" value=\"");
        if (d.year == 0) *_gp_sptr += F("2022");
        else *_gp_sptr += d.year;
        *_gp_sptr += '-';
        if (d.month < 10) *_gp_sptr += 0;
        *_gp_sptr += (d.month == 0) ? 1 : d.month;
        *_gp_sptr += '-';
        if (d.day < 10) *_gp_sptr += 0;
        *_gp_sptr += (d.day == 0) ? 1 : d.day;
        *_gp_sptr += F("\">\n");
    }
    void TIME(const char* name) {
        *_gp_sptr += F("<input type=\"time\" name=\"");
        *_gp_sptr += name;
        *_gp_sptr += F("\">\n");
    }
    void TIME(const char* name, GPtime t) {
        *_gp_sptr += F("<input type=\"time\" name=\"");
        *_gp_sptr += name;
        *_gp_sptr += F("\" value=\"");
        if (t.hour < 10) *_gp_sptr += '0';
        *_gp_sptr += t.hour;
        *_gp_sptr += ':';
        if (t.minute < 10) *_gp_sptr += '0';
        *_gp_sptr += t.minute;
        *_gp_sptr += F("\">\n");
    }
    void SLIDER(const char* name, int value, int min, int max, int step = 1) {
        LABEL(min);
        *_gp_sptr += F("<input type=\"range\" name=\"");
        *_gp_sptr += name;
        *_gp_sptr += F("\" value=\"");
        *_gp_sptr += value;
        *_gp_sptr += F("\" min=\"");
        *_gp_sptr += min;
        *_gp_sptr += F("\" max=\"");
        *_gp_sptr += max;
        *_gp_sptr += F("\" step=\"");
        *_gp_sptr += step;
        *_gp_sptr += F("\">\n");
        LABEL(max);
    }
    void COLOR(const char* name) {
        *_gp_sptr += F("<input type=\"color\" name=\"");
        *_gp_sptr += name;
        *_gp_sptr += F("\">\n");
    }
    void COLOR(const char* name, uint32_t value = 0) {
        *_gp_sptr += F("<input type=\"color\" name=\"");
        *_gp_sptr += name;
        *_gp_sptr += F("\" value=\"");
        char str[8] = "#";
        utoa(value, str + 1, 16);
        *_gp_sptr += str;
        *_gp_sptr += F("\">\n");
    }
    void SUBMIT(const char* value) {
        *_gp_sptr += F("<input type=\"submit\" value=\"");
        *_gp_sptr += value;
        *_gp_sptr += F("\">\n");
    }
    void FORM_SUBMIT(const char* name, const char* value) {
        FORM_BEGIN(name);
        SUBMIT(value);
        FORM_END();
    }
    void SELECT(const char* name, const char* values, int8_t sel = 0) {
        if (sel < 0) sel = 0;
        *_gp_sptr += F("<select name=\"");
        *_gp_sptr += name;
        *_gp_sptr += F("\">\n");    
        uint8_t prev = 0, i = 0, count = 0;
        char* ptr = (char*)values;
        while (1) {
            bool end = (ptr[i] == '\0');
            if (ptr[i] == ',' || end) {      
                ptr[i] = '\0';
                char* str = ptr + prev;
                *_gp_sptr += F("<option value=\"");
                *_gp_sptr += str;
                *_gp_sptr += F("\"");
                if (count++ == sel) *_gp_sptr += F("selected");
                *_gp_sptr += F(">");
                *_gp_sptr += str;
                *_gp_sptr += F("</option>\n");      
                if (end) break;
                ptr[i] = ',';
                prev = i + 1;
            }
            i++;
        }
        *_gp_sptr += F("</select>");
    }
    void SWITCH(const char* name, bool x = 0) {
        *_gp_sptr += F("<label class=\"switch\"><input type=\"checkbox\" name=\"");
        *_gp_sptr += name;
        *_gp_sptr += (x ? F("\" checked>\n") : F("\">\n"));
        *_gp_sptr += F("<span class=\"slider round\"></span></label>");
    }
};

extern Builder add = Builder();

void GP_BUILD(String& s) {
    _gp_sptr = &s;
}
void GP_SHOW() {
    if (_gp_ptr) (*(GyverPortal*)_gp_ptr).showPage(*_gp_sptr);
}

void BUILD_BEGIN(String& s) {
    GP_BUILD(s);
    add.PAGE_BEGIN();
}
void BUILD_END() {
    add.PAGE_END();
    GP_SHOW();
}