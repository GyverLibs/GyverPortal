#pragma once
#include "log.h"

#define GP_UNUSED __attribute__((unused)) 

// ======================= ПОРТАЛ =======================
class GyverPortal {
public:
    // ======================= КОНСТРУКТОР =======================
    GyverPortal() {
        _uri.reserve(15);
    }

    ~GyverPortal() {
        stop();
    }
    
    // ========================= СИСТЕМА =========================
    // запустить портал. Можно передать имя MDNS (оставь пустым "" если MDNS не нужен) и порт
    void start(GP_UNUSED const String& mdns, uint16_t port = 80) {
        _active = 1;
        _dns = (WiFi.getMode() == WIFI_AP);
    
    #ifndef GP_NO_MDNS
        if (mdns.length()) {
            _mdnsF = 1;
            MDNS.begin(mdns.c_str());  
            MDNS.addService("http", "tcp", port);
        }
    #endif
        
        server.begin(port);
    
    #ifndef GP_NO_DNS
        if (_dns) dnsServer.start(53, "*", WiFi.softAPIP());
    #endif
        
        server.onNotFound([this]() {
            if (_auth && !server.authenticate(_login, _pass)) return server.requestAuthentication();
            _showPage = 0;
            _uri = server.uri();
            if (_uri.startsWith(F("/favicon.ico"))) {       // не знаю почему этот запрос приходит
                server.send(200, "text/plane");             // но мы его игнорируем
                return;
            } else if (_uri.startsWith(F("/GP_click"))) {   // клик
                _clickF = 1;
                checkList();
                server.send(200, "text/plane");             // отвечаем ничем
            } else if (_uri.startsWith(F("/GP_update"))) {  // апдейт
                _updateF = 1;
                _answerF = 1;                               // запомнили что апдейт
            } else if (_uri.startsWith(F("/GP_log"))) {     // лог
                if (log.available()) server.send(200, "text/plane", log.read());
                else server.send(200, "text/plane");
                return;
            } else {                                        // форма или любая страница
                if (server.args()) {                        // есть данные в запросе, это форма
                    _formF = 1;
                    checkList();
                }
                _showPage = 1;
            }

            // подключен новый обработчик действия
            if (_action || _actionR) {
                if (_action) _action();         // вызов обычного
                if (_actionR) _actionR(*this);  // вызов с объектом
                if (_answerF) server.send(200, "text/plane");   // юзер не ответил на update - отвечаем за него
                if (_showPage) show();                           // отправляем страницу
                _answerF = _updateF = _clickF = _formF = 0;     // скидываем флаги
            } else {
            #ifdef ESP32
                if (_showPage) show(); // затычка для esp32, отправляем страницу даже без обработчика
            #endif
            }
        });
    }
    void start(GP_UNUSED uint8_t mode = WIFI_STA) {
        start("");
    }
    
    // остановить портал
    void stop() {
        _active = 0;
    #ifndef GP_NO_DNS
        if (_dns) dnsServer.stop();
    #endif
        server.stop();
    }
    
    // =========================== AUTH ===========================
    // включить авторизацию по логину-паролю
    void enableAuth(const char* login, const char* pass) {
        _auth = 1;
        _login = login;
        _pass = pass;
    }
    
    // отключить авторизацию
    void disableAuth() {
        _auth = 0;
    }
    
    // ============================ OTA ============================
    void enableOTA() {
    #ifndef GP_NO_OTA
        httpUpdater.setup(&server, F("/ota_update"));
    #endif
    }

    void enableOTA(GP_UNUSED const String& login, GP_UNUSED const String& pass) {
    #ifndef GP_NO_OTA
        httpUpdater.setup(&server, F("/ota_update"), login, pass);
    #endif
    }
    
    // ======================= АТТАЧИ =======================
    // подключить функцию-обработчик действия
    void attach(void (*handler)()) {
        _action = *handler;
    }
    void attach(void (*handler)(GyverPortal& p)) {
        _actionR = *handler;
    }
    
    // подключить функцию-билдер страницы
    void attachBuild(void (*handler)()) {
        _build = *handler;
    }

    // ======================= ТИКЕР =======================
    // тикер портала. Вернёт true, если портал запущен
    bool tick() {
        if (!_active) return 0;
        
        // deprecated
    #ifdef ESP8266
        if (!_action && !_actionR) {
            if (_showPage) show();
            if (_updateF) server.send(200, "text/plane");
            _updateF = _clickF = _formF = 0;
        }
    #endif
        // deprecated
        
        // server poll
    #ifndef GP_NO_DNS
        if (_dns) dnsServer.processNextRequest();
    #endif
    #if !defined(GP_NO_MDNS) && defined(ESP8266)
        if (_mdnsF) MDNS.update();
    #endif
        server.handleClient();
        // server poll
        
        // deprecated
    #ifdef ESP8266
        if (!_action && !_actionR) {
            if (_formF && _form) _form(this);
            if (_clickF && _click) _click(this);
            if (_updateF && _update) _update(this);
        }
    #endif
        // deprecated
        
        yield();
        return 1;
    }

    // ======================= FORM =======================
    // вернёт true, если было нажатие на любой submit
    bool form() {
        return _formF;
    }
    
    // вернёт true, если был submit с указанной формы
    bool form(const String& name) {
        return _formF ? _uri.equals(name) : 0;
    }
    
    // вернёт имя теукщей submit формы
    String& formName() {
        return _uri;
    }
    
    // ======================= CLICK =======================
    // вернёт true, если был клик по (кнопка, чекбокс, свитч, слайдер, селектор)
    bool click() {
        return _clickF;
    }
    
    // вернёт true, если был клик по указанному элементу (кнопка, чекбокс, свитч, слайдер, селектор)
    bool click(const String& name) {
        return _clickF ? server.argName(0).equals(name) : 0;
    }
    
    // вернёт имя теукщего кликнутого компонента
    String clickName() {
        return String(server.argName(0));
    }
    
    // получить значение кликнутого компонента
    int clickValue() {
        return server.arg(0).toInt();
    }
    
    // получить текст кликнутого компонента
    String clickText() {
        return String(server.arg(0));
    }
    
    // ======================= UPDATE =======================
    // вернёт true, если было обновление
    bool update() {
        return _updateF;
    }
    
    // вернёт true, если было update с указанного компонента
    bool update(const String& name) {
        return _updateF ? server.argName(0).equals(name) : 0;
    }
    
    // вернёт имя обновлённого компонента
    String updateName() {
        return String(server.argName(0));
    }
    
    // отправить ответ на обновление
    void answer(const String& s) {
        _updateF = 0;
        _answerF = 0;
        server.send(200, "text/plane", s);
    }
    void answer(int v) {
        answer(String(v));
    }
    void answer(int* v, int am, int dec = 0) {
        String s;
        s.reserve(am * 4);
        for (int i = 0; i < am; i++) {
            if (dec) s += (float)v[i] / dec;
            else s += v[i];
            if (i != am - 1) s += ',';
        }
        answer(s);
    }
    
    void answer(GPcolor& col) {
        answer(col.encode());
    }
    void answer(GPdate& date) {
        answer(date.encode());
    }
    void answer(GPtime& time) {
        answer(time.encode());
    }

    // ======================= MISC =======================
    // вернёт true, если открыта главная страница (/)
    bool root() {
        return (_uri[0] == '/' && _uri[1] == '\0');
    }
    // адрес запроса
    String& uri() {
        return _uri;
    }
    
    // ======================= ПАРСЕРЫ =======================
    // получить String строку с компонента
    String getString(const String& n) {
        return String(server.arg(n));
    }
    // переписать char строку с компонента к себе
    void copyStr(const String& n, char* d) {
        if (server.hasArg(n)) strcpy(d, server.arg(n).c_str());
    }

    // получить число с компонента
    int getInt(const String& n) {
        return server.arg(n).toInt();
    }
    
    // получить float с компонента
    float getFloat(const String& n) {
        return server.arg(n).toFloat();
    }

    // получить состояние чекбокса
    bool getCheck(const String& n) {
        if (server.hasArg(n)) return (server.arg(n)[0] == '1' || server.arg(n)[0] == 'o');
        return 0;
    }

    // получить дату с компонента
    GPdate getDate(const String& n) {
        return GPdate(server.arg(n));
    }

    // получить время с компонента
    GPtime getTime(const String& n) {
        return GPtime(server.arg(n));
    }

    // получить цвет с компонента
    GPcolor getColor(const String& n) {
        return GPcolor(server.arg(n));
    }
    
    // получить номер выбранного пункта в дроплисте
    int8_t getSelected(const String& n, const String& list) {
        return inList(server.arg(n).c_str(), list.c_str());
    }
    
    // ======================= ПРОЧЕЕ =======================    
    // вызвать конструктор и показать страницу
    void show() {
        String s;
        _GP = &s;
        if (*_build) _build();
        server.send(200, F("text/html"), *_GP);
    }

#ifdef ESP8266
    ESP8266WebServer server;
#else
    WebServer server;
#endif

#ifndef GP_NO_OTA
#ifdef ESP8266
    ESP8266HTTPUpdateServer httpUpdater;
#else
    HTTPUpdateServer httpUpdater;
#endif
#endif

    GPlist list;
    GPlog log;
    
    // ======================= DEPRECATED =======================
    // подключить функцию, которая вызывается при клике (кнопка, чекбокс, свитч, слайдер, селектор)
    void attachClick(void (*handler)(GyverPortal* p)) {
        _click = *handler;
    }
    
    // подключить функцию, которая вызывается при нажатии submit
    void attachForm(void (*handler)(GyverPortal* p)) {
        _form = *handler;
    }
    
    // подключить функцию, которая вызывается при AJAX обновлении со страницы
    void attachUpdate(void (*handler)(GyverPortal* p)) {
        _update = *handler;
    }

    // ======================= PRIVATE =======================
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
                case T_BYTE:    *(int8_t*)list.vals[i] = (int8_t)getInt(list.names[i]); break;
                case T_INT:     *(long*)list.vals[i] = getInt(list.names[i]);       break;
                case T_FLOAT:   *(float*)list.vals[i] = getFloat(list.names[i]);    break;
                case T_COLOR:   *(GPcolor*)list.vals[i] = getColor(list.names[i]);  break;
                }
            }
        }
    }
    bool _mdnsF = false;
    bool _dns = false;
    bool _active = false;
    bool _showPage = 0;
    
    bool _auth = false;
    const char* _login;
    const char* _pass;
    
    bool _formF = 0;
    bool _updateF = 0;
    bool _clickF = 0;
    bool _answerF = 0;
    String _uri;
    
    void (*_build)() = nullptr;
    void (*_action)() = nullptr;
    void (*_actionR)(GyverPortal& p) = nullptr;
    
#ifndef GP_NO_DNS
    DNSServer dnsServer;
#endif

    // ======================= DEPRECATED =======================
    void (*_click)(GyverPortal* p) = nullptr;
    void (*_form)(GyverPortal* p) = nullptr;
    void (*_update)(GyverPortal* p) = nullptr;
};