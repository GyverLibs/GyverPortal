#pragma once
#include <DNSServer.h>
#ifdef ESP8266
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#else
#include <WiFi.h>
#include <WebServer.h>
#endif
//#include "builder.h"
#include "themes.h"

// ======================== ДАТА ========================
struct CPdate {
    uint16_t year;
    uint8_t month, day;
};

struct CPtime {
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

    // запустить портал. Можно передать WIFI_AP для запуска DNS сервера
    void start(uint8_t mode = WIFI_STA) {
        serverMode = (mode == WIFI_AP);
        active = true;
        server.begin();
        if (serverMode) dnsServer.start(53, "*", WiFi.softAPIP());
        server.onNotFound([this]() {
            root();
        });
    }

    // подключить действие формы
    void attachForm(const char* fname) {
        server.on(fname, HTTP_POST, [this]() {
            uri = server.uri();
            ready = true;
        });
    }    
    
    // подключить функцию-билдер страницы
    void attachBuild(void (*handler)()) {
        build = *handler;
    }

    // показать страницу
    void showPage() {
        server.send(200, "text/html", *_gp_sptr);
    }

    // остановить портал
    void stop() {
        active = false;
        if (serverMode) dnsServer.stop();
        server.stop();
    }

    // тикер портала. Вернёт true, если портал запущен
    bool tick() {
        if (!active) return 0;
        if (ready) {
            root();
            ready = false;
        }
        if (serverMode) dnsServer.processNextRequest();
        server.handleClient();
        yield();
        return 1;
    }

    // вернёт true, если было действие с браузера
    bool action() {
        return ready;
    }

    // вернёт true, если было действие с указанной формы
    bool form(const char* f) {
        return uri.startsWith(f);
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
        strcpy(d, server.arg(n).c_str());
    }

    // получить число с элемента
    int getInt(const char* n) {
        return server.arg(n).toInt();
    }

    // получить состояние чекбокса
    bool getCheck(const char* n) {
        return server.hasArg(n);
    }

    // получить дату с элемента
    CPdate getDate(const char* n) {
        String s = server.arg(n);
        return (CPdate) {
            (uint16_t)s.substring(0, 4).toInt(),
            (uint8_t)s.substring(5, 7).toInt(),
            (uint8_t)s.substring(8, 10).toInt()
        };
    }

    // получить время с элемента
    CPtime getTime(const char* n) {
        String s = server.arg(n);
        return (CPtime) {
            (uint8_t)s.substring(0, 2).toInt(),
            (uint8_t)s.substring(3, 5).toInt()
        };
    }

    // получить цвет с элемента
    uint32_t getColor(const char* n) {
        String hex = server.arg(n);
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
    uint8_t getSelected(const char* n, const char* list) {
        uint8_t prev = 0, i = 0, count = 0;
        char* ptr = (char*)list;
        while (1) {
            bool end = (ptr[i] == '\0');
            if (ptr[i] == ',' || end) {      
                ptr[i] = '\0';
                char* str = ptr + prev;
                if (!strcmp(str, server.arg(n).c_str())) {
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
        return 0;
    }


private:
    void root() {
        _gp_ptr = this;
        if (*build) build();
    }
    String uri;
    bool serverMode = false;
    bool active = false;
    bool ready = false;
    void (*build)();
    IPAddress apIP;
    DNSServer dnsServer;
#ifdef ESP8266
    ESP8266WebServer server;
#else
    WebServer server;
#endif
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
    void DATE(const char* name, CPdate d) {
        *_gp_sptr += F("<input type=\"date\" name=\"");
        *_gp_sptr += name;
        *_gp_sptr += F("\" value=\"");
        *_gp_sptr += d.year;
        *_gp_sptr += '-';
        if (d.month < 10) *_gp_sptr += '0';
        *_gp_sptr += d.month;
        *_gp_sptr += '-';
        if (d.day < 10) *_gp_sptr += '0';
        *_gp_sptr += d.day;
        *_gp_sptr += F("\">\n");
    }
    void TIME(const char* name) {
        *_gp_sptr += F("<input type=\"time\" name=\"");
        *_gp_sptr += name;
        *_gp_sptr += F("\">\n");
    }
    void TIME(const char* name, CPtime t) {
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
    void SELECT(const char* name, const char* values, uint8_t sel = 0) {
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

void CUSTOM_BUILD_BEGIN(String& s) {
    _gp_sptr = &s;
}
void BUILD_BEGIN(String& s) {
    _gp_sptr = &s;
    add.PAGE_BEGIN();
}
void CUSTOM_BUILD_END() {
    (*(GyverPortal*)_gp_ptr).showPage();
}
void BUILD_END() {
    add.PAGE_END();
    (*(GyverPortal*)_gp_ptr).showPage();
}