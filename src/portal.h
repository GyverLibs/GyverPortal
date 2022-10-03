#pragma once

#ifdef ESP8266
#include <ESP8266WebServer.h>
extern ESP8266WebServer *_gp_s;
#else
#include <WebServer.h>
extern WebServer *_gp_s;
#endif

#include "utils.h"
#include "list.h"
#include "log.h"
#include "objects.h"
#include "CustomOTA.h"

extern int _gp_bufsize;
extern String* _gp_page;
extern String* _gp_uri;

// ============================= CLASS ===========================
class GyverPortal {
public:
    // ======================= КОНСТРУКТОР =======================
    GyverPortal() {
        _uri.reserve(15);
    }
    GyverPortal(fs::FS *useFS) : _fs(useFS) {
        _uri.reserve(15);
    }

    ~GyverPortal() {
        stop();
    }

    // ========================= СИСТЕМА =========================
    // запустить портал. Можно передать имя MDNS (оставь пустым "" если MDNS не нужен) и порт
    void start(__attribute__((unused)) const String& mdns, uint16_t port = 80) {
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
        
        #if defined(FS_H)
        if (_fs) server.serveStatic("/gp_data", *_fs, "/gp_data", GP_CACHE_PRD); 
        #endif
        
        #ifndef GP_NO_DNS
        if (_dns) dnsServer.start(53, "*", WiFi.softAPIP());
        #endif
        
        server.onNotFound([this]() {
            if (_auth && !server.authenticate(_login, _pass)) return server.requestAuthentication();
            _showPage = 0;
            _uri = server.uri();
            
            if (_uri.startsWith(F("/GP_click"))) {              // клик
                _clickF = 1;
                checkList();
                server.send(200, "text/plane");
            #ifdef GP_NO_DOWNLOAD
            } else if (_uri.startsWith(F("/favicon.ico"))) {    // иконка
                server.send(200, "text/plane");
                return;
            #endif
            } else if (_uri.startsWith(F("/GP_update"))) {      // апдейт
                _updateF = _answerF = 1;
            } else if (_uri.startsWith(F("/GP_log"))) {         // лог
                if (log.available()) server.send(200, "text/plane", log.read());
                else server.send(200);
                return;
            } else if (_uri.startsWith(F("/GP_upload"))) {
                server.send(200, "text/html", F("<meta http-equiv='refresh' content='0; url=/'/>"));
                return;
                #if defined(FS_H) && !defined(GP_NO_DOWNLOAD)
            } else if (downOn && !server.args() && _uri[0] == '/' && _uri.indexOf('.') > 0) {   // файл
                if (_autoD && _fs) sendFile(_fs->open(_uri, "r"));  // авто скачивание
                else _fileDF = 1;                                   // ручное скачивание (в action)
                #endif
            } else if (server.argName(0).equals(F("GP_form"))) {    // форма
                _showPage = 1;
                _formF = 1;
                checkList();
            } else {                                                // любой другой запрос
                _showPage = 1;
                _reqF = 1;
            }

            if (_action || _actionR) {                  // подключен новый обработчик действия
                if (_action) _action();                 // вызов обычного
                else if (_actionR) _actionR(*this);     // вызов с объектом
                if (_showPage) show();                  // отправляем страницу
            } else {
                #ifdef ESP32
                if (_showPage) show();                  // затычка для esp32, отправляем страницу даже без обработчика
                #endif
            }
            
            if (_answerF || _fileDF) server.send(200);  // юзер не ответил на update или не отправил файл
            _reqF = _fileDF = _answerF = _updateF = _clickF = _formF = 0;  // скидываем флаги
        });
        
        #if defined(FS_H) && !defined(GP_NO_UPLOAD)
        server.on("/GP_upload", HTTP_POST, [this]() {
            //server.send(200);
            server.send(200, "text/html", F("<script>setInterval(function(){if(history.length>0)window.history.back();else window.location.href='/';},500);</script>"));
        }, [this]() {
            if (!uplOn) return;
            if (!_autoU && !_action && !_actionR) return;
            HTTPUpload& upl = server.upload();
            _filePtr = &upl.filename;
            _namePtr = &upl.name;
            
            switch (upl.status) {
            case UPLOAD_FILE_START:
                if (file) file.close();
                if (_autoU && _fs) {
                    file = _fs->open('/' + upl.filename, "w");
                } else if (_action || _actionR) {
                    _uplF = 1;
                    if (_action) _action();
                    else if (_actionR) _actionR(*this);
                    _uplF = 0;
                }
                break;
            case UPLOAD_FILE_WRITE:
                if (file) file.write(upl.buf, upl.currentSize);
                break;
            case UPLOAD_FILE_END:
                if (file) {
                    file.close();
                    if (_action || _actionR) {
                        _uplEF = 1;
                        if (_action) _action();
                        else if (_actionR) _actionR(*this);
                        _uplEF = 0;
                    }
                }
                //show();
                break;
            case UPLOAD_FILE_ABORTED:
                if (file) file.close();
                _abortF = 1;
                if (_action) _action();
                else if (_actionR) _actionR(*this);
                _abortF = 0;
                //server.send(200, "text/html", F("<h2>Upload Error, <a href='/'>return back</a></h2>"));
                break;
            }
            
        });
        #endif
    }
    
    // запустить портал
    void start(__attribute__((unused)) uint8_t mode = WIFI_STA) {
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
    
    // проверить, запущен ли портал
    bool state() {
        return _active;
    }
    
    
    // ================== IP REMOTE CLIENT ==================
    // вернёт IP адрес клиента
	IPAddress clientIP() {
		return server.client().remoteIP();
	}
    
    // вернёт true, если IP адрес клиента принадлежит указанной сети
    bool clientFromNet(IPAddress NetIP, uint8_t mask) {
		IPAddress RclIP = clientIP();                   // адрес клинета
		uint8_t netmask[4] = {255, 255, 255, 255};      // Делаем маску /32
		for (int r = 0; r < (32 - mask); r++) bitClear(netmask[3 - r / 8],r % 8);    // Конвертируем формат маски
		for (int r = 0; r < 4; r++) {       // Вычисляем адреса сетей относительно маски
			RclIP[r] &= netmask[r];         // сперва клиента
			NetIP[r] &= netmask[r];         // потом адрес сети IP переданного в функцию
		}
		return (NetIP == RclIP);            // Если адреса сетей совпадают, значит они из одной сети
	}
    
    
    // ========================= AUTH =========================
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
    
    
    // ========================== OTA ==========================
    #ifndef GP_NO_OTA
    CustomOTAUpdate OTA; 
    #endif
    
    // включить OTA обновление с авторизацией
    void enableOTA(__attribute__((unused)) const String& login, __attribute__((unused)) const String& pass) {
        #ifndef GP_NO_OTA
        OTA.begin(&server, login, pass);
        #endif
    }
    
    // включить OTA обновление
    void enableOTA() {
       enableOTA("","");
    }
    
    // ======================= АТТАЧИ =======================
    // подключить функцию-обработчик действия
    void attach(void (*handler)()) {
        _action = *handler;
    }
    void attach(void (*handler)(GyverPortal& p)) {
        _actionR = *handler;
    }
    
    // отключить функцию-обработчик загрузки файла
    void detach() {
        _action = nullptr;
        _actionR = nullptr;
    }
    
    // подключить функцию-билдер страницы
    void attachBuild(void (*handler)()) {
        _build = *handler;
    }
    void attachBuild(void (*handler)(GyverPortal& p)) {
        _buildR = *handler;
    }
    
    // отключить функцию-билдер страницы
    void detachBuild() {
        _build = nullptr;
        _buildR = nullptr;
    }
    
    // ======================= ТИКЕР =======================
    // тикер портала. Вернёт true, если портал запущен
    bool tick() {
        if (!_active) return 0;
        
        // deprecated
        #ifdef ESP8266
        if (!_action && !_actionR) {
            if (_showPage) {_showPage = 0; show();}
            if (_updateF) server.send(200, "text/plane");
            _updateF = _clickF = _formF = 0;
        }
        #endif
        // deprecated
        
        #ifndef GP_NO_DNS
        if (_dns) dnsServer.processNextRequest();
        #endif
        #if !defined(GP_NO_MDNS) && defined(ESP8266)
        if (_mdnsF) MDNS.update();
        #endif
        server.handleClient();
        
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
    
    
    // ======================== FILE ========================
    // вкл/выкл поддержку скачивания файлов (по умолч. вкл)
    void downloadMode(bool m) {
        downOn = m;
    }
    
    // вкл/выкл поддержку загрузки файлов (по умолч. вкл)
    void uploadMode(bool m) {
        uplOn = m;
    }
    
    // вернёт true, если был запрос на скачивание файла
    bool download() {
        return _fileDF;
    }
    
    // вернёт true, если был запрос на загрузку файла
    bool upload() {
        return _uplF;
    }
    
    // вернёт true, если был запрос на загрузку файла с указанной кнопки
    bool upload(const String& name) {
        return _uplF && name.equals(uploadName());
    }
    
    // вернёт true, если загрузка файла завершена
    bool uploadEnd() {
        return _uplEF;
    }
    
    // вернёт true, если загрузка файла прервана
    bool uploadAbort() {
        return _abortF;
    }
    
    // автоматическое скачивание файла по uri (по умолч. выкл, false)
    void downloadAuto(bool mode) {
        _autoD = mode;
    }
    
    // автоматическая загрузка файла по uri (по умолч. выкл, false)
    void uploadAuto(bool mode) {
        _autoU = mode;
    }
    
    // имя формы загрузки файла
    String& uploadName() {
        return *_namePtr;
    }
    
    // имя файла при загрузке
    String& fileName() {
        return *_filePtr;
    }
    
    
    // ======================= REQUEST =======================
    // вернёт true, если был http запрос
    bool request() {
        return _reqF;
    }
    
    // вернёт true, если был запрос
    bool request(const String& name) {
        return _reqF ? _uri.equals(name) : 0;
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
    String formName() {
        return _formF ? _uri : String("");
    }
    
    // вернёт часть имени формы, находящейся под номером idx после разделителя /
    String formNameSub(int idx = 1) {
        return _formF ? (GPlistIdx(idx, _uri, '/')) : String("");
    }
    
    // вернёт true, если был submit с форм, имя которых начинмется с name
    bool formSub(const String& name) {
        return _formF ? _uri.startsWith(name) : 0;
    }
    

    // ======================= CLICK =======================
    // вернёт true, если был клик по (кнопка, чекбокс, свитч, слайдер, селектор)
    bool click() {
        return _clickF && server.args();
    }
    
    // вернёт true, если был клик по указанному элементу (кнопка, чекбокс, свитч, слайдер, селектор)
    bool click(const String& name) {
        return click() ? (server.argName(0).equals(name) && server.args() == 1) : 0;
    }
    
    // вернёт true, если имя кликнутого компонента начинается с name
    bool clickSub(const String& name) {
        return click() ? (server.argName(0).startsWith(name) && server.args() == 1) : 0;
    }
    
    // вернёт имя теукщего кликнутого компонента
    String clickName() {
        return click() ? server.argName(0) : String();
    }
    
    // вернёт часть имени кликнутого компонента, находящейся под номером idx после разделителя /
    String clickNameSub(int idx = 1) {
        return click() ? (GPlistIdx(idx, server.argName(0), '/')) : String();
    }
    
    // вернёт true, если кнопка была нажата
    bool clickDown(const String& name) {
        return click() ? (server.argName(0).equals(name) && server.args() == 2 && server.arg(1)[0] == '0') : 0;
    }
    // вернёт true, если кнопка была нажата и имя компонента начинается с указанного
    bool clickDownSub(const String& name) {
        return click() ? (server.argName(0).startsWith(name) && server.args() == 2 && server.arg(1)[0] == '0') : 0;
    }
    
    // вернёт true, если кнопка была отпущена
    bool clickUp(const String& name) {
        return click() ? (server.argName(0).equals(name) && server.args() == 2 && server.arg(1)[0] == '1') : 0;
    }
    // вернёт true, если кнопка была отпущена и имя компонента начинается с указанного
    bool clickUpSub(const String& name) {
        return click() ? (server.argName(0).startsWith(name) && server.args() == 2 && server.arg(1)[0] == '1') : 0;
    }
    
    
    // ===================== CLICK AUTO =====================
    bool clickStr(const String& n, char* t, int len = 0) {
        return click() ? copyStr(n, t, len) : 0;
    }
    bool clickString(const String& n, String& t) {
        return click() ? copyString(n, t) : 0;
    }
    bool clickInt(const String& n, int& t) {
        return click() ? copyInt(n, t) : 0;
    }
    bool clickFloat(const String& n, float& t) {
        return click() ? copyFloat(n, t) : 0;
    }
    bool clickBool(const String& n, bool& t) {
        return click() ? copyBool(n, t) : 0;
    }
    bool clickDate(const String& n, GPdate& t) {
        return click() ? copyDate(n, t) : 0;
    }
    bool clickTime(const String& n, GPtime& t) {
        return click() ? copyTime(n, t) : 0;
    }
    bool clickColor(const String& n, GPcolor& t) {
        return click() ? copyColor(n, t) : 0;
    }

    
    // ===================== CLICK OBJ ======================
    bool clickDown(GP_BUTTON& btn) {
        return clickDown(btn.name);
    }
    
    bool clickUp(GP_BUTTON& btn) {
        return clickUp(btn.name);
    }
    
    bool clickDown(GP_BUTTON_MINI& btn) {
        return clickDown(btn.name);
    }
    
    bool clickUp(GP_BUTTON_MINI& btn) {
        return clickUp(btn.name);
    }
    
    //
    bool clickObj(GP_BUTTON& btn) {
        return click(btn.name);
    }
    bool clickObj(GP_BUTTON_MINI& btn) {
        return click(btn.name);
    }
    
    bool clickObj(GP_NUMBER& num) {
        return click() ? copyObj(num) : 0;
    }
    bool clickObj(GP_NUMBER_F& num) {
        return click() ? copyObj(num) : 0;
    }
    
    bool clickObj(GP_TEXT& txt) {
        return click() ? copyObj(txt) : 0;
    }
    bool clickObj(GP_PASS& pas) {
        return click() ? copyObj(pas) : 0;
    }
    
    bool clickObj(GP_AREA& ar) {
        return click() ? copyObj(ar) : 0;
    }
    
    bool clickObj(GP_CHECK& ch) {
        return click() ? copyObj(ch) : 0;
    }
    bool clickObj(GP_SWITCH& sw) {
        return click() ? copyObj(sw) : 0;
    }
    
    bool clickObj(GP_DATE& d) {
        return click() ? copyObj(d) : 0;
    }
    bool clickObj(GP_TIME& t) {
        return click() ? copyObj(t) : 0;
    }
    bool clickObj(GP_COLOR& c) {
        return click() ? copyObj(c) : 0;
    }
    
    bool clickObj(GP_SPINNER& s) {
        return click() ? copyObj(s) : 0;
    }
    bool clickObj(GP_SLIDER& s) {
        return click() ? copyObj(s) : 0;
    }
    
    bool clickObj(GP_SELECT& s) {
        return click() ? copyObj(s) : 0;
    }
    
    
    // ======================= UPDATE =======================
    // вернёт true, если было обновление
    bool update() {
        return _updateF;
    }
    
    // вернёт true, если было update с указанного компонента
    bool update(const String& name) {
        return update() ? server.argName(0).equals(name) : 0;
    }
    
    // вернёт true, если имя обновляемого компонента НАЧИНАЕТСЯ с указанного
    bool updateSub(const String& name) {
        return update() ? server.argName(0).startsWith(name) : 0;
    }
    
    // вернёт имя обновлённого компонента
    String updateName() {
        return update() ? server.argName(0) : String();
    }
    
    // вернёт часть имени обновляемого компонента, находящейся под номером idx после разделителя /
    String updateNameSub(int idx = 1) {
        return update() ? (GPlistIdx(idx, server.argName(0), '/')) : String();
    }
    
    
    // ===================== UPDATE OBJ =====================
    bool update(GP_TITLE& title) {
        return update(title.name);
    }
    bool update(GP_LABEL& label) {
        return update(label.name);
    }
    bool update(GP_LABEL_BLOCK& label) {
        return update(label.name);
    }
    
    bool update(GP_LED& led) {
        return update(led.name);
    }
    bool update(GP_LED_RED& led) {
        return update(led.name);
    }
    bool update(GP_LED_GREEN& led) {
        return update(led.name);
    }
    
    bool update(GP_NUMBER& num) {
        return update(num.name);
    }
    bool update(GP_NUMBER_F& num) {
        return update(num.name);
    }
    
    bool update(GP_TEXT& txt) {
        return update(txt.name);
    }
    bool update(GP_PASS& pas) {
        return update(pas.name);
    }
    
    bool update(GP_AREA& ar) {
        return update(ar.name);
    }
    
    bool update(GP_CHECK& ch) {
        return update(ch.name);
    }
    bool update(GP_SWITCH& sw) {
        return update(sw.name);
    }
    
    bool update(GP_DATE& d) {
        return update(d.name);
    }
    bool update(GP_TIME& t) {
        return update(t.name);
    }
    bool update(GP_COLOR& c) {
        return update(c.name);
    }
    
    bool update(GP_SPINNER& s) {
        return update(s.name);
    }
    bool update(GP_SLIDER& s) {
        return update(s.name);
    }
    
    // ======================= ANSWER =======================
    // отправить ответ на обновление
    bool answer(const String& s) {
        _updateF = 0;
        _answerF = 0;
        server.send(200, "text/plane", s);
        return 1;
    }
    bool answer(int v) {
        return answer(String(v));
    }
    bool answer(float v, uint8_t dec) {
        return answer(String(v, (uint16_t)dec));
    }
    bool answer(int* v, int am, int dec = 0) {
        String s;
        s.reserve(am * 4);
        for (int i = 0; i < am; i++) {
            if (dec) s += (float)v[i] / dec;
            else s += v[i];
            if (i != am - 1) s += ',';
        }
        return answer(s);
    }
    
    bool answer(GPcolor& col) {
        return answer(col.encode());
    }
    bool answer(GPdate& date) {
        return answer(date.encode());
    }
    bool answer(GPtime& time) {
        return answer(time.encode());
    }
    
    // ==================== UPDATE AUTO =====================
    // автоматическое обновление. Отправит значение из указанной переменной
    // Вернёт true в момент обновления
    bool updateString(const String& n, String& f) {
        return update(n) ? (answer(f), 1) : 0;
    }
    bool updateInt(const String& n, int f) {
        return update(n) ? (answer(f), 1) : 0;
    }
    bool updateFloat(const String& n, float f, int dec = 2) {
        return update(n) ? (answer(f, dec), 1) : 0;
    }
    bool updateBool(const String& n, bool f) {
        return update(n) ? (answer(f), 1) : 0;
    }
    bool updateDate(const String& n, GPdate f) {
        return update(n) ? (answer(f), 1) : 0;
    }
    bool updateTime(const String& n, GPtime f) {
        return update(n) ? (answer(f), 1) : 0;
    }
    bool updateColor(const String& n, GPcolor f) {
        return update(n) ? (answer(f), 1) : 0;
    }
    
    bool updateLog(GPlog& log) {
        return update(log.name) ? (answer(log.read()), 1) : 0;
    }
    
    
    // ===================== ANSWER OBJ =====================
    bool answer(GP_TITLE& title) {
        return answer(title.text);
    }
    bool answer(GP_LABEL& label) {
        return answer(label.text);
    }
    bool answer(GP_LABEL_BLOCK& label) {
        return answer(label.text);
    }
    
    bool answer(GP_LED& led) {
        return answer(led.state);
    }
    bool answer(GP_LED_RED& led) {
        return answer(led.state);
    }
    bool answer(GP_LED_GREEN& led) {
        return answer(led.state);
    }
    
    bool answer(GP_NUMBER& num) {
        return answer(num.value);
    }
    bool answer(GP_NUMBER_F& num) {
        return answer(num.value, num.decimals);
    }
    
    bool answer(GP_TEXT& txt) {
        return answer(txt.text);
    }
    bool answer(GP_PASS& pas) {
        return answer(pas.text);
    }
    
    bool answer(GP_AREA& ar) {
        return answer(ar.text);
    }
    
    bool answer(GP_CHECK& ch) {
        return answer(ch.state);
    }
    bool answer(GP_SWITCH& sw) {
        return answer(sw.state);
    }
    
    bool answer(GP_DATE& d) {
        return answer(d.date);
    }
    bool answer(GP_TIME& t) {
        return answer(t.time);
    }
    bool answer(GP_COLOR& c) {
        return answer(c.color);
    }
    
    bool answer(GP_SPINNER& s) {
        return answer(s.value, s.decimals);
    }
    bool answer(GP_SLIDER& s) {
        return answer(s.value, s.decimals);
    }
    
    
    // ================== UPDATE AUTO OBJ ===================
    bool updateObj(GP_TITLE& title) {
        return (update(title) ? answer(title) : 0);
    }
    bool updateObj(GP_LABEL& label) {
        return (update(label) ? answer(label) : 0);
    }
    bool updateObj(GP_LABEL_BLOCK& label) {
        return (update(label) ? answer(label) : 0);
    }
    
    bool updateObj(GP_LED& led) {
        return (update(led) ? answer(led) : 0);
    }
    bool updateObj(GP_LED_RED& led) {
        return (update(led) ? answer(led) : 0);
    }
    bool updateObj(GP_LED_GREEN& led) {
        return (update(led) ? answer(led) : 0);
    }
    
    bool updateObj(GP_NUMBER& num) {
        return (update(num) ? answer(num) : 0);
    }
    bool updateObj(GP_NUMBER_F& num) {
        return (update(num) ? answer(num) : 0);
    }
    
    bool updateObj(GP_TEXT& txt) {
        return (update(txt) ? answer(txt) : 0);
    }
    bool updateObj(GP_PASS& pas) {
        return (update(pas) ? answer(pas) : 0);
    }
    
    bool updateObj(GP_AREA& ar) {
        return (update(ar) ? answer(ar) : 0);
    }
    
    bool updateObj(GP_CHECK& ch) {
        return (update(ch) ? answer(ch) : 0);
    }
    bool updateObj(GP_SWITCH& sw) {
        return (update(sw) ? answer(sw) : 0);
    }
    
    bool updateObj(GP_DATE& d) {
        return (update(d) ? answer(d) : 0);
    }
    bool updateObj(GP_TIME& t) {
        return (update(t) ? answer(t) : 0);
    }
    bool updateObj(GP_COLOR& c) {
        return (update(c) ? answer(c) : 0);
    }
    
    bool updateObj(GP_SPINNER& s) {
        return (update(s) ? answer(s) : 0);
    }
    bool updateObj(GP_SLIDER& s) {
        return (update(s) ? answer(s) : 0);
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
    
    // true если uri совпадает
    bool uri(const String& s) {
        return _uri.equals(s);
    }
    
    // длина текста в полученном значении
    int argLength(const String& n) {
        return server.arg(n).length();
    }
    int argLength() {
        return server.arg(0).length();
    }
    
    
    // ======================= ПАРСЕРЫ =======================
    // ОПАСНЫЕ ФУНКЦИИ (не проверяют есть ли запрос). Конвертируют и возвращают значение
    
    // получить String строку с компонента
    String getString(const String& n) {
        return String(server.arg(n));
    }
    String getString() {
        return String(server.arg(0));
    }

    // получить число с компонента
    int getInt(const String& n) {
        return server.arg(n).toInt();
    }
    int getInt() {
        return server.arg(0).toInt();
    }
    
    // получить float с компонента
    float getFloat(const String& n) {
        return server.arg(n).toFloat();
    }
    float getFloat() {
        return server.arg(0).toFloat();
    }

    // получить состояние чекбокса
    bool getBool(const String& n) {
        return (server.arg(n)[0] != '0' || server.arg(n)[0] == 'o');
    }
    bool getBool() {
        return (server.arg(0)[0] != '0' || server.arg(0)[0] == 'o');
    }
    
    // получить дату с компонента
    GPdate getDate(const String& n) {
        return GPdate(server.arg(n));
    }
    GPdate getDate() {
        return GPdate(server.arg(0));
    }

    // получить время с компонента
    GPtime getTime(const String& n) {
        return GPtime(server.arg(n));
    }
    GPtime getTime() {
        return GPtime(server.arg(0));
    }

    // получить цвет с компонента
    GPcolor getColor(const String& n) {
        return GPcolor(server.arg(n));
    }
    GPcolor getColor() {
        return GPcolor(server.arg(0));
    }

    
    // ===================== COPY-ПАРСЕРЫ =====================
    // БЕЗОПАСНЫЕ ФУНКЦИИ (проверяют запрос). Копируют данные из запроса в переменную
    bool copyStr(const String& n, char* t, int len = 0) {
        return (server.hasArg(n) && (!len || argLength(n) < len)) ? (strcpy(t, server.arg(n).c_str()), 1) : 0;
    }
    bool copyString(const String& n, String& t) {
        return server.hasArg(n) ? (t = server.arg(n), 1) : 0;
    }
    bool copyInt(const String& n, int& t) {
        return server.hasArg(n) ? (t = getInt(n), 1) : 0;
    }
    bool copyFloat(const String& n, float& t) {
        return server.hasArg(n) ? (t = getFloat(n), 1) : 0;
    }
    bool copyBool(const String& n, bool& t) {
        return server.hasArg(n) ? (t = getBool(n), 1) : 0;
    }
    bool copyDate(const String& n, GPdate& t) {
        return server.hasArg(n) ? (t = getDate(n), 1) : 0;
    }
    bool copyTime(const String& n, GPtime& t) {
        return server.hasArg(n) ? (t = getTime(n), 1) : 0;
    }
    bool copyColor(const String& n, GPcolor& t) {
        return server.hasArg(n) ? (t = getColor(n), 1) : 0;
    }
    
    
    // ===================== COPY OBJ =====================
    bool copyObj(GP_NUMBER& num) {
        return copyInt(num.name, num.value);
    }
    bool copyObj(GP_NUMBER_F& num) {
        return copyFloat(num.name, num.value);
    }
    
    bool copyObj(GP_TEXT& txt) {
        return copyString(txt.name, txt.text);
    }
    bool copyObj(GP_PASS& pas) {
        return copyString(pas.name, pas.text);
    }
    
    bool copyObj(GP_AREA& ar) {
        return copyString(ar.name, ar.text);
    }
    
    bool copyObj(GP_CHECK& ch) {
        return copyBool(ch.name, ch.state);
    }
    bool copyObj(GP_SWITCH& sw) {
        return copyBool(sw.name, sw.state);
    }
    
    bool copyObj(GP_DATE& d) {
        return copyDate(d.name, d.date);
    }
    bool copyObj(GP_TIME& t) {
        return copyTime(t.name, t.time);
    }
    bool copyObj(GP_COLOR& c) {
        return copyColor(c.name, c.color);
    }
    
    bool copyObj(GP_SPINNER& s) {
        return copyFloat(s.name, s.value);
    }
    bool copyObj(GP_SLIDER& s) {
        return copyFloat(s.name, s.value);
    }
    
    bool copyObj(GP_SELECT& s) {
        return copyInt(s.name, s.selected);
    }
    
    
    // ======================= ПРОЧЕЕ =======================    
    // вызвать конструктор и показать страницу
    void show() {
        if (!_build && !_buildR) server.send(200);
        else {
            server.sendHeader(F("Cache-Control"), F("no-cache, no-store, must-revalidate"));
            server.sendHeader(F("Pragma"), F("no-cache"));
            server.sendHeader(F("Expires"), F("-1"));
            server.setContentLength(CONTENT_LENGTH_UNKNOWN);
            server.send(200, "text/html");
            
            _gp_s = &server;
            _gp_uri = &_uri;
            String page;
            page.reserve(_bufsize);
            _gp_bufsize = _bufsize;
            _gp_page = &page;
            if (_build) _build();
            else _buildR(*this);
            _gp_page = nullptr;
            server.sendContent(page);
            
            server.sendContent("");
            server.client().stop();
        }
    }
    
    // задать размер буфера страницы, байт (умолч. 1000)
    void setBufferSize(int sz) {
        _bufsize = sz;
    }

    GPlist list;
    GPlog log;

#ifdef ESP8266
    ESP8266WebServer server;
#else
    WebServer server;
#endif
/*
#ifndef GP_NO_OTA
#ifdef ESP8266
    ESP8266HTTPUpdateServer httpUpdater;
#else
    HTTPUpdateServer httpUpdater;
#endif
#endif
*/
#if defined(FS_H)
    File file;
    
    // установить файл для загрузки
    void saveFile(File f) {
        file = f;
    }
    void saveFile(const String& name) {
        saveFile(_fs->open(name, "w"));
    }
    
    // отправить файл
    void sendFile(File file) {
        _fileDF = 0;
        if (!file) {
            server.send(200);
            return;
        }
        server.streamFile(file, GPfileType(file.name()));
        file.close();
    }
    void sendFile(const String& name) {
        if (_fs) sendFile(_fs->open(name, "r"));
    }
#endif
    
    // ======================= LEGACY =======================
    void attachClick(void (*handler)(GyverPortal* p)) {_click = *handler;}
    void attachForm(void (*handler)(GyverPortal* p)) {_form = *handler;}
    void attachUpdate(void (*handler)(GyverPortal* p)) {_update = *handler;}
    int clickValue() {return server.arg(0).toInt();}
    String clickText() {return String(server.arg(0));}
    int getSelected(const String& n) { return server.arg(n).toInt(); }
    int getSelected() { return server.arg(0).toInt(); }
    bool getCheck(const String& n) { return getBool(n); }
    bool getCheck() { return getBool(); }
    
    // ======================= PRIVATE =======================
private:
    void checkList() {
        if (!list.idx) return;
        for (int i = 0; i < list.idx; i++) {
            if (list.skips[i] || form(list.forms[i])) {
                switch (list.types[i]) {
                case T_CSTR:    copyStr(list.names[i], (char*)list.vals[i]);            break;
                case T_STRING:  *(String*)list.vals[i] = getString(list.names[i]);      break;
                case T_TIME:    *(GPtime*)list.vals[i] = getTime(list.names[i]);        break;
                case T_DATE:    *(GPdate*)list.vals[i] = getDate(list.names[i]);        break;
                case T_CHECK:   *(bool*)list.vals[i] = getBool(list.names[i]);         break;
                case T_BYTE:    *(int8_t*)list.vals[i] = (int8_t)getInt(list.names[i]); break;
                case T_INT:     *(long*)list.vals[i] = getInt(list.names[i]);           break;
                case T_FLOAT:   *(float*)list.vals[i] = getFloat(list.names[i]);        break;
                case T_COLOR:   *(GPcolor*)list.vals[i] = getColor(list.names[i]);      break;
                }
            }
        }
    }
    
    fs::FS *_fs = nullptr;
    
    String _uri;
    String *_namePtr = nullptr;
    String *_filePtr = nullptr;
    
    int _bufsize = 1000;
    
    int _cache = 60 * 60;   // 1 hour
    
    bool _auth = 0;
    const char* _login;
    const char* _pass;
    
    bool _mdnsF = 0, _dns = 0, _active = 0, _showPage = 0;
    bool _formF = 0, _updateF = 0, _clickF = 0, _answerF = 0, _reqF = 0;
    bool _fileDF = 0, _uplEF = 0, _uplF = 0, _abortF = 0, _autoD = 0, _autoU = 0;
    bool downOn = 1, uplOn = 1;
    
    void (*_build)() = nullptr;
    void (*_buildR)(GyverPortal& p) = nullptr;
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
