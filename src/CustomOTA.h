#pragma once

// GP OTA Update Module
// developed by DenysChuhlib

#include "builder.h"
extern Builder GP;
extern int _gp_bufsize;
extern String* _GPP;

#ifdef ESP8266
#include <WiFiUdp.h>
#include <flash_hal.h>
#include <FS.h>
#include <ESP8266WebServer.h>
#else
#include <Update.h>
#include <WebServer.h>
#endif

#include <StreamString.h>

// Настройки в скетч
//#define GP_OTA_NAME F("My_sketch.ino")    // имя бинарника скетча чтобы случайно не загрузить другой
//#define GP_OTA_FS_NAME F("My_sketch")    	// имя бинарника файловой системы чтобы случайно не загрузить другой
//#define GP_OTA_FILES                      // использолвать файлы стилей и скриптов
//#define GP_OTA_LIGHT                      // светлая тема

// Error Codes
//esp32
/*
* UPDATE_ERROR_OK                 (0)
* UPDATE_ERROR_WRITE              (1)
* UPDATE_ERROR_ERASE              (2)
* UPDATE_ERROR_READ               (3)
* UPDATE_ERROR_SPACE              (4)
* UPDATE_ERROR_SIZE               (5)
* UPDATE_ERROR_STREAM             (6)
* UPDATE_ERROR_MD5                (7)
* UPDATE_ERROR_MAGIC_BYTE         (8)
* UPDATE_ERROR_ACTIVATE           (9)
* UPDATE_ERROR_NO_PARTITION       (10)
* UPDATE_ERROR_BAD_ARGUMENT       (11)
* UPDATE_ERROR_ABORT              (12)
*/
//esp8266
/*
* UPDATE_ERROR_OK                 (0)
* UPDATE_ERROR_WRITE              (1)
* UPDATE_ERROR_ERASE              (2)
* UPDATE_ERROR_READ               (3)
* UPDATE_ERROR_SPACE              (4)
* UPDATE_ERROR_SIZE               (5)
* UPDATE_ERROR_STREAM             (6)
* UPDATE_ERROR_MD5                (7)
* UPDATE_ERROR_FLASH_CONFIG       (8)
* UPDATE_ERROR_NEW_FLASH_CONFIG   (9)
* UPDATE_ERROR_MAGIC_BYTE         (10)
* UPDATE_ERROR_BOOTSTRAP          (11)
* UPDATE_ERROR_SIGN               (12)
* UPDATE_ERROR_NO_DATA            (13)
* UPDATE_ERROR_OOM                (14)
*/
//Module codes
#define CUSTOM_UPDATE_ERROR_TYPE_FILE		(252)
#define CUSTOM_UPDATE_ERROR_NAME_FILE		(253)
#define CUSTOM_UPDATE_ERROR_FS_NAME_FILE	(254)
#define CUSTOM_UPDATE_ERROR_ABORTED			(255)

class CustomOTAUpdate {
public:
    // включить OTA обновление с авторизацией
    void begin(
    #ifdef ESP8266
    ESP8266WebServer* server,
    #else
    WebServer* server,
    #endif
    const String& login = "", const String& pass = "")
    {
        _server = server;
        
        _OTAlogin = login;
        _OTApass = pass;
        
        _server->on(F("/ota_update"), HTTP_GET, [this]() {
            if (_OTAlogin.length() && _OTApass.length() && !_server->authenticate(_OTAlogin.c_str(), _OTApass.c_str())) return _server->requestAuthentication();
            _ShowOTApage();
            _UpdateReload();
        });
        //------------------
        _server->on(F("/ota_update"), HTTP_POST, [this]() {
            if (_OTAlogin.length() && _OTApass.length() && !_server->authenticate(_OTAlogin.c_str(), _OTApass.c_str())) return _server->requestAuthentication();
            _ShowOTApage();
            _UpdateReload();
        }, [this]() {
            HTTPUpload& upload = _server->upload();
            _UploadBin(upload);
        });
        //------------------
        _server->on(F("/GP_OTAupload"), HTTP_GET,[this]() {
            _JSback();
        });
        //------------------
        _server->on(F("/GP_OTAupload"), HTTP_POST, [this]() {
            _JSback();
            _server->client().stop();
            _UpdateReload();
        }, [this]() {
            HTTPUpload& upload = _server->upload();
            _UploadBin(upload);
        });
        //------------------
    }
    
    void enableAuth(const String& login, const String& pass) {
        _OTAlogin = login;
        _OTApass = pass;
    }
    
    void disableAuth() {
        _OTAlogin.clear();
        _OTApass.clear();
    }
    
    // подключить функцию-билдер страницы OTA update
    void attachUpdateBuild(void (*handler)(bool UpdateEnd, const String& UpdateError)) {
        _OTAbuild = *handler;
    }
    
    // отключить функцию-билдер страницы OTA update
    void detachUpdateBuild() {
        _OTAbuild = nullptr;
    }
    
    // подключить функцию которая вызывается перед рестартом платы
    void attachBeforeRestart(void (*handler)()) {
        _OTAbeforeRestart = *handler;
    }
    
    // отключить функцию которая вызывается перед рестартом платы
    void detachBeforeRestart() {
        _OTAbeforeRestart = nullptr;
    }
    
    // подключить функцию которая вызывается при прерывании загрузки обновления
    void attachAbort(void (*handler)()) {
        _OTAabort = *handler;
    }
    
    // отключить функцию которая вызывается при прерывании загрузки обновления
    void detachAbort() {
        _OTAabort = nullptr;
    }
    
    // подключить функцию которая вызывается при ошибке
    void attachError(void (*handler)(const String& UpdateError)) {
        _OTAerror = *handler;
    }
    
    // отключить функцию которая вызывается при ошибке
    void detachError() {
        _OTAerror = nullptr;
    }
    
    // подключить функцию которая используется для текста ошибки
    void attachErrorCode(String (*handler)(uint8_t code)) {
        _OTAerrorCode = *handler;
    }
    
    // отключить функцию которая используется для текста ошибки
    void detachErrorCode() {
        _OTAerrorCode = nullptr;
    }
    
    /*Стандартная функция-билдер для страници /ota_update
    
    Позначка "OTA page" обезательно должна быть "true"
    */
    void defBuild(bool UpdateEnd, const String& UpdateError) {
        #ifndef    GP_OTA_FILES
        GP.BUILD_BEGIN(400);
        
        #ifndef GP_OTA_LIGHT
        GP.THEME(GP_DARK);
        #else
        GP.THEME(GP_LIGHT);
        #endif
        #else
        GP.BUILD_BEGIN_FILE(400);

        #ifndef GP_OTA_LIGHT
        GP.THEME_FILE(F("GP_DARK"));
        #else
        GP.THEME_FILE(F("GP_LIGHT"));
        #endif
        #endif
        
        GP.BLOCK_TAB_BEGIN(F("OTA Update"));
        if (!UpdateEnd) {
            GP.OTA_FIRMWARE(F("OTA firmware"), GP_GREEN, true /*OTA page*/);
            GP.OTA_FILESYSTEM(F("OTA filesystem"), GP_GREEN, true /*OTA page*/);
        } else if (UpdateError.length()) {
            GP.TITLE(String(F("Update error: ")) + UpdateError);
            GP.BUTTON_LINK(F("/ota_update"), F("Refresh"));
        } else {
            GP.TITLE(F("Update Success!"));
            GP.TITLE(F("Rebooting..."));
            GP.BUTTON_LINK(F("/"), F("Home"));
            GP.BUTTON_LINK(F("/ota_update"), F("Refresh"));
        }
        GP.BLOCK_END();
        GP.BUILD_END();
    }
    
    
    /* Пример функции блока для обновления
    
    При ошибке, показивает ее.
    
    Возвращать результат обновления (Успешно ли) может только страница /ota_update из-за многих особенностей Gyver Portal & CustopOTA
    поэтому обновиться пожет незаметно.
    
    */
    void buildBlock() {													
        GP.BLOCK_TAB_BEGIN(F("OTA Update"));
        if (hasError()) {												// OTA.hasError()
            GP.TITLE(String(F("Update error: ")) + error());			// OTA.error()
        }
        GP.OTA_FIRMWARE(F("OTA firmware"), GP_GREEN);
        GP.OTA_FILESYSTEM(F("OTA filesystem"), GP_GREEN);
        GP.BLOCK_END();
    }
    
    /* Возращает ошибку текстом 
    
    default (English)
    либо своим, если подключина функция в "attachErrorCode"
    */
    String error() {
        if (_OTAerrorCode) 
        return _OTAerrorCode(_errCode);
        else
        return getErrorString();
    }
    
    uint8_t getError() { return _errCode; }
    void clearError(){ _errCode = UPDATE_ERROR_OK; }
    bool hasError(){ return _errCode != UPDATE_ERROR_OK; }
    
    // Возращает ошибку текстом
    String getErrorString() {
        String out;
        switch (_errCode) {
        case UPDATE_ERROR_OK:
            out = F("No Error");
            break;
        case UPDATE_ERROR_WRITE:
            out = F("Flash Write Failed");
            break;
        case UPDATE_ERROR_ERASE:
            out = F("Flash Erase Failed");
            break;
        case UPDATE_ERROR_READ:
            out = F("Flash Read Failed");
            break;
        case UPDATE_ERROR_SPACE:
            out = F("Not Enough Space");
            break;
        case UPDATE_ERROR_SIZE:
            out = F("Bad Size Given");
            break;
        case UPDATE_ERROR_STREAM:
            out = F("Stream Read Timeout");
            break;
#if defined(ESP32)
        case UPDATE_ERROR_MD5:
            out = F("MD5 Check Failed");
            break;
        case UPDATE_ERROR_MAGIC_BYTE:
            out = F("Wrong Magic Byte");
            break;
        case UPDATE_ERROR_ACTIVATE:
            out = F("Could Not Activate The Firmware");
            break;
        case UPDATE_ERROR_NO_PARTITION:
            out = F("Partition Could Not be Found");
            break;
        case UPDATE_ERROR_BAD_ARGUMENT:
            out = F("Bad Argument");
            break;
        case UPDATE_ERROR_ABORT:
            out = F("Aborted");
            break;
#elif defined(ESP8266)
        case UPDATE_ERROR_MD5://
            /*out += F("MD5 verification failed: ");
                out += F("expected: ") + _target_md5;
                out += F(", calculated: ") + _md5.toString();*/
            out = F("MD5 Error");
            break;
        case UPDATE_ERROR_FLASH_CONFIG:
            out += F("Flash config wrong: ");
            out += F("real: ");
            out += ESP.getFlashChipRealSize();
            out += F(", SDK: ");
            out += ESP.getFlashChipSize();
            break;
        case UPDATE_ERROR_NEW_FLASH_CONFIG:
            out += F("new Flash config wrong, real size: ");
            out += String(ESP.getFlashChipRealSize(), 10);
            break;
        case UPDATE_ERROR_MAGIC_BYTE:
            out = F("Magic byte is not 0xE9");
            break;
        case UPDATE_ERROR_BOOTSTRAP:
            out = F("Invalid bootstrapping state, reset ESP8266 before updating");
            break;
        case UPDATE_ERROR_SIGN:
            out = F("Signature verification failed");
            break;
            /*
            // v3.1
            case UPDATE_ERROR_NO_DATA:
                out = F("No data supplied");
                break;
            case UPDATE_ERROR_OOM:
                out = F("Out of memory");
                break;*/
#endif				
        case CUSTOM_UPDATE_ERROR_TYPE_FILE:
            out = F("file is not .bin or .bin.gz");
            break;
        case CUSTOM_UPDATE_ERROR_NAME_FILE:	
            out = F("File name error");
            break;
        case CUSTOM_UPDATE_ERROR_FS_NAME_FILE:	
            out = F("FS file name error");
            break;  				
        case CUSTOM_UPDATE_ERROR_ABORTED:
            out = F("Upload aborted");
            break;
            
        default:
            out = F("UNKNOWN");
            break;
        }
        return out;
    }
    
private:
#ifdef ESP8266
    ESP8266WebServer* _server;
#else
    WebServer* _server;
#endif
    String _OTAlogin;
    String _OTApass;
    uint8_t _errCode = 0;
    bool _UpdateEnd = 0;
    
    void (*_OTAbuild)(bool UpdateEnd, const String& UpdateError) = nullptr;
    void (*_OTAbeforeRestart)() = nullptr;
    void (*_OTAabort)() = nullptr;
    void (*_OTAerror)(const String& UpdateError) = nullptr;
    String (*_OTAerrorCode)(uint8_t code) = nullptr;
    
    void _UpdateReload() {
        if (!_UpdateEnd) return;
        if (hasError()) {
            _UpdateEnd = false;
            if (_OTAerror) _OTAerror(error());
        } else {
            if (_OTAbeforeRestart) _OTAbeforeRestart();
            delay(100);
            ESP.restart();
        }
    }
    
    void _JSback() {
        _server->send(200, "text/html", F("<script>setInterval(function(){if(history.length>0)window.history.back();else window.location.href='/';},500);</script>"));
    }
    
    void _ShowOTApage() {
        _server->sendHeader(F("Cache-Control"), F("no-cache, no-store, must-revalidate"));
        _server->sendHeader(F("Pragma"), F("no-cache"));
        _server->sendHeader(F("Expires"), F("-1"));
        _server->setContentLength(CONTENT_LENGTH_UNKNOWN);
        _server->send(200, "text/html");
        
        *_gp_uri = F("/ota_update");
        _gp_s = _server;
        String page;
        _gp_bufsize = 500;
        page.reserve(_gp_bufsize);
        _GPP = &page;
        if (_OTAbuild) _OTAbuild(_UpdateEnd, hasError()? error() :  "");
        else defBuild(_UpdateEnd, hasError()? error() :  "");
        _GPP = nullptr;
        _server->sendContent(page);
        _server->sendContent("");
        _server->client().stop();
    }
    
    void _setError(uint8_t code) {
        _errCode = code;
        _UpdateEnd = true;
    }
    
    void _UploadBin(HTTPUpload& upload) {
        if (_UpdateEnd) return;
        if (_OTAlogin.length() && _OTApass.length() && !_server->authenticate(_OTAlogin.c_str(), _OTApass.c_str())) return _server->requestAuthentication();
        if (!(upload.name == F("filesystem") || upload.name == F("firmware"))) return;
        
        clearError();
        if (!(upload.filename.endsWith(F(".bin")) || upload.filename.endsWith(F(".bin.gz")))) {
            _setError(CUSTOM_UPDATE_ERROR_TYPE_FILE);
            return;
        }
        if (upload.status == UPLOAD_FILE_START) {
            if (upload.name == F("filesystem")) {
                #ifdef GP_OTA_FS_NAME
                if (!upload.filename.startsWith(GP_OTA_FS_NAME)) {
                    _setError(CUSTOM_UPDATE_ERROR_FS_NAME_FILE);
                    return;
                }
                #endif
                
                #ifdef ESP8266
                size_t fsSize = ((size_t) &_FS_end - (size_t) &_FS_start);
                close_all_fs();
                if (!Update.begin(fsSize, U_FS))
                #elif defined ESP32
                if (!Update.begin(UPDATE_SIZE_UNKNOWN, U_SPIFFS))
                #endif
                {
                    _setError(Update.getError());
                    return;
                }
            } else /* upload.name == "firmware" */ {
                #ifdef GP_OTA_NAME
                if (!upload.filename.startsWith(GP_OTA_NAME)) {
                    _setError(CUSTOM_UPDATE_ERROR_NAME_FILE);
                    return;
                }
                #endif
                uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
                if (!Update.begin(maxSketchSpace, U_FLASH)) { //start with max available size
                    _setError(Update.getError());
                    return;
                }
            }
            #ifdef ESP8266
            WiFiUDP::stopAll();
            #endif
        } else if (upload.status == UPLOAD_FILE_WRITE) {
            if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
                _setError(Update.getError());
                return;
            }
        } else if (upload.status == UPLOAD_FILE_END) {
            if (!Update.end(true)) {
                _setError(Update.getError());
            }
            _UpdateEnd = true;
            return;
        } else if (upload.status == UPLOAD_FILE_ABORTED) {
            Update.end();
            _setError(CUSTOM_UPDATE_ERROR_ABORTED);
            if (_OTAabort) _OTAabort();
            return;
        }
    }
};