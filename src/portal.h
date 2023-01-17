#pragma once

// GP Portal

#ifdef ESP8266
#include <ESP8266WebServer.h>
extern ESP8266WebServer *_gp_s;
#else
#include <WebServer.h>
extern WebServer *_gp_s;
#endif

#include <FS.h>
#include "utils.h"
#include "list.h"
#include "log.h"
#include "objects.h"
#include "CustomOTA.h"
#include "TimeTicker.h"
#include "canvas.h"
#include "scripts.h"

extern int _gp_bufsize;
extern String* _GPP;
extern String* _gp_uri;
extern uint32_t _gp_unix_tmr;
extern uint32_t _gp_local_unix;
extern const char* _gp_style;
extern uint8_t _gp_seed;
extern const char* _gp_mdns;

#define GP_DEBUG_EN
#ifdef GP_DEBUG_EN
#define GP_DEBUG(x) Serial.println(x)
#else
#define GP_DEBUG(x)
#endif

// ============================= CLASS ===========================
class GyverPortal : public TimeTicker {
public:
    // ======================= КОНСТРУКТОР =======================
    GyverPortal() {
        _uri.reserve(15);
        _hold.reserve(10);
    }
    GyverPortal(fs::FS *useFS) : _fs(useFS) {
        _uri.reserve(15);
        _hold.reserve(10);
    }

    ~GyverPortal() {
        stop();
        _gp_unix_tmr = 0;
    }
    
    void setFS(fs::FS *useFS) {
        _fs = useFS;
    }
    
    // кеширование встроенных стилей/скриптов (умолч. вкл)
    void caching(bool v) {
        _cache = v;
    }
    
    // сбросить кеш встроенных стилей/скриптов в браузере (для разработчиков)
    void clearCache() {
        _gp_seed = random(0xff);
    }

    // ========================= СИСТЕМА =========================
    // запустить портал. Можно передать имя MDNS (оставь пустым "" если MDNS не нужен) и порт
    void start(const char* mdns = "", uint16_t port = 80) {
        _gp_mdns = mdns;
        _gp_unix_tmr = 0;
        _active = 1;
        _dns = (WiFi.getMode() == WIFI_AP);
        
        #ifndef GP_NO_MDNS
        if (strlen(mdns)) {
            _mdnsF = 1;
            MDNS.begin(mdns);  
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
            _onlTmr = millis();
            if (_auth && !server.authenticate(_login, _pass)) return server.requestAuthentication();
            _showPage = 0;
            _uri = server.uri();
            if (_uri.startsWith(F("/GP_click"))) {              // клик
                _clickF = 1;
                checkList();
                server.send(200);
            } else if (_uri.startsWith(F("/GP_press"))) {       // нажатие
                _holdF = server.arg(0)[0] - '0';
                //_clickF = 1;
                if (_holdF == 1) _hold = server.argName(0);
                else _hold = "";
                server.send(200);
                
                #ifdef GP_NO_DOWNLOAD
            } else if (_uri.startsWith(F("/favicon.ico"))) {    // иконка
                server.send(200);
                return;
                #endif
            } else if (_uri.startsWith(F("/GP_ping"))) {        // пинг
                server.send(200);
                return;
            } else if (_uri.startsWith(F("/GP_SCRIPT.js"))) {   // скрипты
                sendFile_P(GP_JS_TOP, "text/javascript");
                return;
            } else if (_uri.startsWith(F("/GP_STYLE.css"))) {   // стили
                sendFile_P(_gp_style, "text/css");
                return;
            } else if (_uri.startsWith(F("/GP_CANVAS.js"))) {   // канвас
                sendFile_P(GP_JS_CANVAS, "text/javascript");
                return;
            } else if (_uri.startsWith(F("/GP_update"))) {      // апдейт
                if (server.argName(0) == "GP_log") {            // лог
                    if (log.available()) server.send(200, "text/plain", log.read());
                    else server.send(200);
                    return;
                }
                String name = server.argName(0);        // тут будет список имён
                String answ;                            // строка с ответом
                _answPtr = &answ;                       // указатель на неё
                if (name.indexOf(',') < 0) {            // один компонент
                    _updPtr = &name;
                    if (_action) _action();             // внутри answer() прибавляет к answ
                    else if (_actionR) _actionR(*this);
                } else {
                    GP_parser n(name);                  // парсер
                    _updPtr = &n.str;                   // указатель на имя (в парсинге)
                    while (n.parse()) {                 // парсим
                        if (_action) _action();         // внутри answer() прибавляет к answ
                        else if (_actionR) _actionR(*this);
                        answ += '\1';
                        yield();
                    }
                    answ.remove(answ.length() - 1);     // удаляем последний разделитель
                }
                server.send(200, "text/plain", answ);
                _answPtr = nullptr;
                _updPtr = nullptr;
                return;
            } else if (_uri.startsWith(F("/GP_time"))) {    // время
                setUnix(server.arg(0).toInt());
                setGMT(server.arg(1).toInt());
                _gp_unix_tmr = millis();
                server.send(200);
                return;
            } else if (_uri.startsWith(F("/GP_delete"))) {  // удаление
                if (_autoDel && _fs) {
                    #if defined(FS_H)
                    _fs->remove(server.argName(0));
                    #endif
                } else _delF = 1;
                _showPage = 1;
            } else if (_uri.startsWith(F("/GP_rename"))) {  // переименовать
                if (_autoRen && _fs) {
                    #if defined(FS_H)
                    _fs->rename(server.argName(0), server.arg(0));
                    #endif
                } else _renF = 1;
                _showPage = 1;
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
                if (log.available()) log.clear();
            }
            
            _gp_local_unix = getUnix() + getGMT() * 60;

            if (_action) _action();                 // вызов обычного обработчика действий
            else if (_actionR) _actionR(*this);     // вызов обработчика действий с объектом
            if (_showPage) show();                  // показать страницу            

            if (_fileDF) server.send(200);  // юзер не ответил на update или не отправил файл
            _reqF = _fileDF = _clickF = _formF = _delF = _renF = 0;     // скидываем флаги
            _holdF = 0;
        });
        
        #if defined(FS_H) && !defined(GP_NO_UPLOAD)
        server.on("/GP_upload", HTTP_POST, [this]() {
            //server.send(200);
            server.send(200, "text/html", F("<script>setInterval(function(){if(history.length>0)window.history.back();else window.location.href='/';},500);</script>"));
        }, [this]() {
            if (!uplOn) return;
            if (!_autoU && !_action && !_actionR) return;
            HTTPUpload& upl = server.upload();
            if (!upl.filename.length()) return;
            _filePtr = &upl.filename;
            _namePtr = &upl.name;
            
            switch (upl.status) {
            case UPLOAD_FILE_START:
                if (file) file.close();
                #ifdef ESP32
                {
                    char path[upl.filename.length() + 2] = "/";
                    strcpy(path + 1, upl.filename.c_str());
                    if (!_fs->exists(path)) {
                        if (strchr(path, '/')) {
                            char *pathStr = strdup(path);
                            if (pathStr) {
                                char *ptr = strchr(pathStr, '/');
                                while (ptr) {
                                    *ptr = 0;
                                    _fs->mkdir(pathStr);
                                    *ptr = '/';
                                    ptr = strchr(ptr + 1, '/');
                                }
                            }
                            free(pathStr);
                        }
                    }
                }
                #endif
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
    
    // проверить, подключен ли клиент (браузер)
    bool online() {
        return (millis() - _onlTmr < _onlPrd);
    }
    
    // установить таймаут онлайна (умолч. 1500)
    void onlineTimeout(uint16_t prd) {
        _onlPrd = prd;
    }
    
    
    // ================== IP REMOTE CLIENT ==================
    // вернёт IP адрес клиента
    IPAddress clientIP() {
        return server.client().remoteIP();
    }
    
    // вернёт true, если IP адрес клиента принадлежит указанной сети
    bool clientFromNet(IPAddress NetIP, uint8_t mask) {
        uint8_t netmask[4] = {255, 255, 255, 255};      // делаем маску /32
        for (int r = 0; r < (32 - mask); r++) bitClear(netmask[3 - r / 8], r % 8);    // Конвертируем формат маски
        return clientFromNet(NetIP, IPAddress(netmask));
    }
    
    bool clientFromNet(IPAddress NetIP, IPAddress netmask = IPAddress(255, 255, 255, 0)) {
        IPAddress RclIP = clientIP();       // адрес клинета
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
    
    // получить логин авторизации
    const char* login() {
        return _login;
    }
    
    // получить пароль авторизации
    const char* pass() {
        return _pass;
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
        
        #ifndef GP_NO_DNS
        if (_dns) dnsServer.processNextRequest();
        #endif
        #if !defined(GP_NO_MDNS) && defined(ESP8266)
        if (_mdnsF) MDNS.update();
        #endif
        server.handleClient();
        
        yield();
        return 1;
    }
    
    // ======================= ВРЕМЯ ========================
    GPdate getSystemDate() {
        return timeSynced() ? GPdate(getUnix(), getGMT()) : GPdate();
    }
    
    GPtime getSystemTime() {
        return timeSynced() ? GPtime(getUnix(), getGMT()) : GPtime();
    }
    
    // ======================== FILE ========================
    // DELETE
    // вернёт true при запросе на удаление файла
    bool deleteFile() {
        return _delF;
    }
    
    // автоматическое удаление файла по uri (по умолч. вкл, true)
    void deleteAuto(bool m) {
        _autoDel = m;
    }
    
    // имя (путь) файла для удаления. Начинается с '/'
    String deletePath() {
        return deleteFile() ? server.argName(0) : String();
    }
    
    
    // RENAME
    // вернёт true при запросе на переименование файла
    bool renameFile() {
        return _renF;
    }
    
    // автоматическое переименование файла по uri (по умолч. вкл, true)
    void renameAuto(bool m) {
        _autoRen = m;
    }
    
    // имя (путь) файла для переименования. Начинается с '/'
    String renamePath() {
        return renameFile() ? server.argName(0) : String();
    }
    
    // новое имя (путь) файла
    String renamePathTo() {
        return renameFile() ? server.arg(0) : String();
    }
    
    
    // DOWNLOAD
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
    
    // автоматическое скачивание файла по uri (по умолч. вкл, true)
    void downloadAuto(bool mode) {
        _autoD = mode;
    }
    
    
    // UPLOAD
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
    
    // автоматическая загрузка файла по uri (по умолч. вкл, true)
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
        return form() ? _uri.equals(name) : 0;
    }
    
    // вернёт имя теукщей submit формы
    String formName() {
        return form() ? _uri : String("");
    }
    
    // вернёт часть имени формы, находящейся под номером idx после разделителя /
    String formNameSub(int idx = 1) {
        return form() ? (GPlistIdx(_uri, idx + 1, '/')) : String("");
    }
    
    // вернёт true, если был submit с форм, имя которых начинмется с name
    bool formSub(const String& name) {
        return form() ? _uri.startsWith(name) : 0;
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
        return click() ? (GPlistIdx(server.argName(0), idx, '/')) : String();
    }
    
    
    // HOLD
    // вернёт true, если статус удержания кнопки изменился (нажата/отпущена)
    bool hold() {
        return _holdF && server.args();
    }
    
    // вернёт true, если кнопка удерживается
    bool hold(const String& name) {
        return _hold.length() ? _hold.equals(name) : 0;
    }
    
    // вернёт имя удерживаемой кнопки
    String holdName() {
        return _hold.length() ? _hold : String();
    }
    
    // вернёт часть имени hold компонента, находящейся под номером idx после разделителя /
    String holdNameSub(int idx = 1) {
        return _hold.length() ? (GPlistIdx(_hold, idx, '/')) : String();
    }
    
    // вернёт true, если кнопка удерживается и имя компонента начинается с указанного
    bool holdSub(const String& name) {
        return _hold.length() ? _hold.startsWith(name) : 0;
    }
    
    // вернёт true, если кнопка была нажата
    bool clickDown(const String& name) {
        return hold() ? (_holdF == 1 && server.argName(0).equals(name)) : 0;
    }
    // вернёт true, если кнопка была нажата и имя компонента начинается с указанного
    bool clickDownSub(const String& name) {
        return hold() ? (_holdF == 1 && server.argName(0).startsWith(name)) : 0;
    }
    
    // вернёт true, если кнопка была отпущена
    bool clickUp(const String& name) {
        return hold() ? (_holdF == 2 && server.argName(0).equals(name)) : 0;
    }
    // вернёт true, если кнопка была отпущена и имя компонента начинается с указанного
    bool clickUpSub(const String& name) {
        return hold() ? (_holdF == 2 && server.argName(0).startsWith(name)) : 0;
    }
    
    
    // ===================== CLICK AUTO =====================
    // нулевой аргумент (для вызова в условии)
    bool clickStr(char* t, int len = 0) {
        return click() ? copyStr(t, len) : 0;
    }
    bool clickString(String& t) {
        return click() ? copyString(t) : 0;
    }
    template <typename T>
    bool clickInt(T& t) {
        return click() ? copyInt(t) : 0;
    }
    template <typename T>
    bool clickFloat(T& t) {
        return click() ? copyFloat(t) : 0;
    }
    template <typename T>
    bool clickBool(T& t) {
        return click() ? copyBool(t) : 0;
    }
    bool clickDate(GPdate& t) {
        return click() ? copyDate(t) : 0;
    }
    bool clickTime(GPtime& t) {
        return click() ? copyTime(t) : 0;
    }
    bool clickColor(GPcolor& t) {
        return click() ? copyColor(t) : 0;
    }
    
    // с указанием имени компонента
    bool clickStr(const String& n, char* t, int len = 0) {
        return click() ? copyStr(n, t, len) : 0;
    }
    bool clickString(const String& n, String& t) {
        return click() ? copyString(n, t) : 0;
    }
    template <typename T>
    bool clickInt(const String& n, T& t) {
        return click() ? copyInt(n, t) : 0;
    }
    template <typename T>
    bool clickFloat(const String& n, T& t) {
        return click() ? copyFloat(n, t) : 0;
    }
    template <typename T>
    bool clickBool(const String& n, T& t) {
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
    bool click(GP_BUTTON& btn) {
        return click(btn.name);
    }
    bool click(GP_BUTTON_MINI& btn) {
        return click(btn.name);
    }
    
    bool click(GP_NUMBER& num) {
        return click() ? copy(num) : 0;
    }
    bool click(GP_NUMBER_F& num) {
        return click() ? copy(num) : 0;
    }
    
    bool click(GP_TEXT& txt) {
        return click() ? copy(txt) : 0;
    }
    bool click(GP_PASS& pas) {
        return click() ? copy(pas) : 0;
    }
    
    bool click(GP_AREA& ar) {
        return click() ? copy(ar) : 0;
    }
    
    bool click(GP_CHECK& ch) {
        return click() ? copy(ch) : 0;
    }
    bool click(GP_SWITCH& sw) {
        return click() ? copy(sw) : 0;
    }
    
    bool click(GP_DATE& d) {
        return click() ? copy(d) : 0;
    }
    bool click(GP_TIME& t) {
        return click() ? copy(t) : 0;
    }
    bool click(GP_COLOR& c) {
        return click() ? copy(c) : 0;
    }
    
    bool click(GP_SPINNER& s) {
        return click() ? copy(s) : 0;
    }
    bool click(GP_SLIDER& s) {
        return click() ? copy(s) : 0;
    }
    
    bool click(GP_SELECT& s) {
        return click() ? copy(s) : 0;
    }
    
    
    // ======================= UPDATE =======================
    // вернёт true, если было обновление
    bool update() {
        return (bool)_updPtr;
    }
    
    // вернёт true, если было update с указанного компонента
    bool update(const String& name) {
        return update() ? _updPtr->equals(name) : 0;
    }
    
    // вернёт true, если имя обновляемого компонента НАЧИНАЕТСЯ с указанного
    bool updateSub(const String& name) {
        return update() ? _updPtr->startsWith(name) : 0;
    }
    
    // вернёт имя обновлённого компонента
    String updateName() {
        return update() ? String(*_updPtr) : String();
    }
    
    // вернёт часть имени обновляемого компонента, находящейся под номером idx после разделителя /
    String updateNameSub(int idx = 1) {
        return update() ? (GPlistIdx(*_updPtr, idx, '/')) : String();
    }
     
    
    // ======================= ANSWER =======================
    // отправить ответ на обновление
    bool answer(const String& s) {
        if (_answPtr) *_answPtr += s;
        return (bool)_answPtr;
    }
    bool answer(int v) {
        if (_answPtr) *_answPtr += v;
        return (bool)_answPtr;
    }
    bool answer(float v, uint8_t dec) {
        return answer(String(v, (uint16_t)dec));
    }
    bool answer(double v, uint8_t dec) {
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
    
    bool answer(GPcolor col) {
        return answer(col.encode());
    }
    bool answer(GPdate date) {
        return answer(date.encode());
    }
    bool answer(GPtime time) {
        return answer(time.encode());
    }
    bool answer(GPcanvas& cv) {
        return answer(cv._read());
    }
    
    // ==================== UPDATE AUTO =====================
    // автоматическое обновление. Отправит значение из указанной переменной
    // Вернёт true в момент обновления
    bool updateString(const String& n, String& f) {
        return update(n) ? (answer(f), 1) : 0;
    }
    template <typename T>
    bool updateInt(const String& n, T f) {
        return update(n) ? (answer((int)f), 1) : 0;
    }
    template <typename T>
    bool updateFloat(const String& n, T f, int dec = 2) {
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
    
    
    // ================== UPDATE AUTO OBJ ===================
    bool update(GP_TITLE& title) {
        return (update(title.name) ? answer(title.text) : 0);
    }
    bool update(GP_LABEL& label) {
        return (update(label.name) ? answer(label.text) : 0);
    }
    bool update(GP_LABEL_BLOCK& label) {
        return (update(label.name) ? answer(label.text) : 0);
    }
    
    bool update(GP_LED& led) {
        return (update(led.name) ? answer(led.state) : 0);
    }
    bool update(GP_LED_RED& led) {
        return (update(led.name) ? answer(led.state) : 0);
    }
    bool update(GP_LED_GREEN& led) {
        return (update(led.name) ? answer(led.state) : 0);
    }
    
    bool update(GP_NUMBER& num) {
        return (update(num.name) ? answer(num.value) : 0);
    }
    bool update(GP_NUMBER_F& num) {
        return (update(num.name) ? answer(num.value, num.decimals) : 0);
    }
    
    bool update(GP_TEXT& txt) {
        return (update(txt.name) ? answer(txt.text) : 0);
    }
    bool update(GP_PASS& pas) {
        return (update(pas.name) ? answer(pas.text) : 0);
    }
    
    bool update(GP_AREA& ar) {
        return (update(ar.name) ? answer(ar.text) : 0);
    }
    
    bool update(GP_CHECK& ch) {
        return (update(ch.name) ? answer(ch.state) : 0);
    }
    bool update(GP_SWITCH& sw) {
        return (update(sw.name) ? answer(sw.state) : 0);
    }
    
    bool update(GP_DATE& d) {
        return (update(d.name) ? answer(d.date) : 0);
    }
    bool update(GP_TIME& t) {
        return (update(t.name) ? answer(t.time) : 0);
    }
    bool update(GP_COLOR& c) {
        return (update(c.name) ? answer(c.color) : 0);
    }
    
    bool update(GP_SPINNER& s) {
        return (update(s.name) ? answer(s.value, s.decimals) : 0);
    }
    bool update(GP_SLIDER& s) {
        return (update(s.name) ? answer(s.value, s.decimals) : 0);
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
    
    // true если uri начинается с
    bool uriSub(const String& s) {
        return _uri.startsWith(s);
    }
    
    // вернёт часть uri, находящейся под номером idx после разделителя /
    String uriNameSub(int idx = 1) {
        return GPlistIdx(_uri, idx + 1, '/');
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
        return getIntUniv(server.arg(n));
    }
    int getInt() {
        return getIntUniv(server.arg(0));
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
        return (server.arg(n)[0] == '1' || server.arg(n)[0] == 'o');
    }
    bool getBool() {
        return (server.arg(0)[0] == '1' || server.arg(0)[0] == 'o');
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
    // ОПАСНЫЕ парсеры (не проверяют запрос). Использовать только в условии!
    bool copyStr(char* t, int len = 0) {
        return (server.args() && (!len || argLength() < len)) ? (strcpy(t, server.arg(0).c_str()), 1) : 0;
    }
    bool copyString(String& t) {
        return server.args() ? (t = server.arg(0), 1) : 0;
    }
    template <typename T>
    bool copyInt(T& t) {
        return server.args() ? (t = getInt(), 1) : 0;
    }
    template <typename T>
    bool copyFloat(T& t) {
        return server.args() ? (t = getFloat(), 1) : 0;
    }
    template <typename T>
    bool copyBool(T& t) {
        return server.args() ? (t = getBool(), 1) : 0;
    }
    bool copyDate(GPdate& t) {
        return server.args() ? (t = getDate(), 1) : 0;
    }
    bool copyTime(GPtime& t) {
        return server.args() ? (t = getTime(), 1) : 0;
    }
    bool copyColor(GPcolor& t) {
        return server.args() ? (t = getColor(), 1) : 0;
    }
    
    // БЕЗОПАСНЫЕ парсеры (проверяют запрос). Копируют данные из запроса в переменную
    bool copyStr(const String& n, char* t, int len = 0) {
        return (server.hasArg(n) && (!len || argLength(n) < len)) ? (strcpy(t, server.arg(n).c_str()), 1) : 0;
    }
    bool copyString(const String& n, String& t) {
        return server.hasArg(n) ? (t = server.arg(n), 1) : 0;
    }
    template <typename T>
    bool copyInt(const String& n, T& t) {
        return server.hasArg(n) ? (t = getInt(n), 1) : 0;
    }
    template <typename T>
    bool copyFloat(const String& n, T& t) {
        return server.hasArg(n) ? (t = getFloat(n), 1) : 0;
    }
    template <typename T>
    bool copyBool(const String& n, T& t) {
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
    bool copy(GP_NUMBER& num) {
        return copyInt(num.name, num.value);
    }
    bool copy(GP_NUMBER_F& num) {
        return copyFloat(num.name, num.value);
    }
    
    bool copy(GP_TEXT& txt) {
        return copyString(txt.name, txt.text);
    }
    bool copy(GP_PASS& pas) {
        return copyString(pas.name, pas.text);
    }
    
    bool copy(GP_AREA& ar) {
        return copyString(ar.name, ar.text);
    }
    
    bool copy(GP_CHECK& ch) {
        return copyBool(ch.name, ch.state);
    }
    bool copy(GP_SWITCH& sw) {
        return copyBool(sw.name, sw.state);
    }
    
    bool copy(GP_DATE& d) {
        return copyDate(d.name, d.date);
    }
    bool copy(GP_TIME& t) {
        return copyTime(t.name, t.time);
    }
    bool copy(GP_COLOR& c) {
        return copyColor(c.name, c.color);
    }
    
    bool copy(GP_SPINNER& s) {
        return copyFloat(s.name, s.value);
    }
    bool copy(GP_SLIDER& s) {
        return copyFloat(s.name, s.value);
    }
    
    bool copy(GP_SELECT& s) {
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
            page.reserve(_bufsize + 200);
            _gp_bufsize = _bufsize;
            _GPP = &page;
            if (_build) _build();
            else _buildR(*this);
            _GPP = nullptr;
            server.sendContent(page);
            
            server.sendContent("");
            server.client().stop();
        }
    }
    
    void sendFile_P(PGM_P file_P, const char* type) {
        if (_cache) server.sendHeader(F("Cache-Control"), GP_CACHE_PRD);
        server.send_P(200, type, file_P, strlen_P(file_P));
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
    #ifdef ESP8266
        server.streamFile(file, mime::getContentType(file.name()));
    #else
        server.streamFile(file, GPfileType(file.name()));
    #endif
        
        file.close();
    }
    void sendFile(const String& name) {
        if (_fs) sendFile(_fs->open(name, "r"));
    }
#endif
    
    // ======================= LEGACY =======================
    int clickValue() { return server.arg(0).toInt(); }
    String clickText() { return String(server.arg(0)); }
    int getSelected(const String& n) { return server.arg(n).toInt(); }
    int getSelected() { return server.arg(0).toInt(); }
    bool getCheck(const String& n) { return getBool(n); }
    bool getCheck() { return getBool(); }
    
    // ======================= PRIVATE =======================
private:
    int getIntUniv(const String& s) {
        if (s[0] == '#') {
            GPcolor col(s);
            return col.getHEX();
        } else return s.toInt();
    }
    
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
    String _hold;
    String *_namePtr = nullptr;
    String *_filePtr = nullptr;
    String *_answPtr = nullptr;
    String *_updPtr = nullptr;
    
    int _bufsize = 1000;
    bool _cache = 1;
    bool _auth = 0;
    const char* _login;
    const char* _pass;
    
    bool _mdnsF = 0, _dns = 0, _active = 0, _showPage = 0;
    bool _formF = 0, _clickF = 0, _reqF = 0, _delF = 0, _renF = 0;
    bool _fileDF = 0, _uplEF = 0, _uplF = 0, _abortF = 0, _autoD = 1, _autoU = 1, _autoDel = 1, _autoRen = 1;
    bool downOn = 1, uplOn = 1;
    uint8_t _holdF = 0;

    uint32_t _onlTmr = 0;
    uint16_t _onlPrd = 1500;
    
    void (*_build)() = nullptr;
    void (*_buildR)(GyverPortal& p) = nullptr;
    void (*_action)() = nullptr;
    void (*_actionR)(GyverPortal& p) = nullptr;
    
#ifndef GP_NO_DNS
    DNSServer dnsServer;
#endif
};
