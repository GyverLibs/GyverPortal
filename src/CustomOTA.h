// модуль разработан DenysChuhlib
#pragma once

#include "builder.h"
extern Builder GP;
extern int _gp_bufsize;
extern String* _gp_page;

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

//#define GP_OTA_NAME F("My_sketch.ino")    // имя бинарника скетча чтобы случайно не загрузить другой
//#define GP_OTA_FILES                      // использолвать файлы стилей и скриптов
//#define GP_OTA_LIGHT                      // светлая тема

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
        
        _server->on(F("/ota_update"), [this]() {
            if (_OTAlogin.length() && _OTApass.length() && !_server->authenticate(_OTAlogin.c_str(), _OTApass.c_str())) return _server->requestAuthentication();
            
            _server->sendHeader(F("Cache-Control"), F("no-cache, no-store, must-revalidate"));
            _server->sendHeader(F("Pragma"), F("no-cache"));
            _server->sendHeader(F("Expires"), F("-1"));
            _server->setContentLength(CONTENT_LENGTH_UNKNOWN);
            _server->send(200, "text/html");
            _gp_s = _server;
            String page;
            _gp_bufsize = 500;
            page.reserve(_gp_bufsize);
            _gp_page = &page;
            if (_OTAbuild) _OTAbuild(_UpdateEnd, _UpdateError);
            else defBuild(_UpdateEnd, _UpdateError);
            _gp_page = nullptr;
            _server->sendContent(page);
            _server->sendContent("");
            _server->client().stop();
            
            if (_UpdateEnd && !_UpdateError.length()) {
                if (_OTAbeforeRestart) _OTAbeforeRestart();
                delay(100);
                ESP.restart();
            } else if (_UpdateEnd && _UpdateError.length()) {
                _UpdateEnd = false;
                _UpdateError.clear();
            }
        });
        //------------------
        _server->on(("/GP_OTAupload"), HTTP_GET,[this]() {
            _server->send(200, "text/html", F("<script>setInterval(function(){window.location.href='/ota_update';},300);</script>"));
        });
        //------------------
        _server->on(("/GP_OTAupload"), HTTP_POST, [this]() {
            _server->send(200, "text/html", F("<script>setInterval(function(){window.location.href='/ota_update';},300);</script>"));
        }, [this]() {
            HTTPUpload& upload = _server->upload();
            if (_UpdateEnd) return;
            if (_OTAlogin.length() && _OTApass.length() && !_server->authenticate(_OTAlogin.c_str(), _OTApass.c_str())) return _server->requestAuthentication();
            if (!(upload.name == F("filesystem") || upload.name == F("firmware"))) return;
            if (!(upload.filename.endsWith(F(".bin")) || upload.filename.endsWith(F(".bin.gz")))) {
                _UpdateError = F("file is not .bin or .bin.gz");
                _UpdateEnd = true;
                return;
            }
            if (upload.status == UPLOAD_FILE_START) {
                if (upload.name == F("filesystem")) {
                #ifdef ESP8266
                    size_t fsSize = ((size_t) &_FS_end - (size_t) &_FS_start);
                    close_all_fs();
                    if (!Update.begin(fsSize, U_FS)) {
                #elif defined ESP32
                    if (!Update.begin(UPDATE_SIZE_UNKNOWN, U_SPIFFS)) {
                #endif
                        StreamString str;
                        Update.printError(str);
                        _UpdateError = str.c_str();
                        _UpdateEnd = true;
                        return;
                    }
                } else /* upload.name == "OTAfirmware" */ {
            #ifdef GP_OTA_NAME
                    if (!upload.filename.startsWith(GP_OTA_NAME)) {
                        _UpdateError = F("File name error");
                        _UpdateEnd = true;
                        return;
                    }
            #endif
                    uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
                    if (!Update.begin(maxSketchSpace, U_FLASH)) { //start with max available size
                        StreamString str;
                        Update.printError(str);
                        _UpdateError = str.c_str();
                        _UpdateEnd = true;
                        return;
                    }
				}
			#ifdef ESP8266
				WiFiUDP::stopAll();
			#endif
			} else if (upload.status == UPLOAD_FILE_WRITE) {
				if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
					StreamString str;
					Update.printError(str);
					_UpdateError = str.c_str();
					_UpdateEnd = true;
					return;
				}
			} else if (upload.status == UPLOAD_FILE_END) {
				if (!Update.end(true)) { //true to set the size to the current progress
                    StreamString str;
                    Update.printError(str);
                    _UpdateError = str.c_str();
				}
				_UpdateEnd = true;
				return;
			} else if (upload.status == UPLOAD_FILE_ABORTED) {
				Update.end();
				_UpdateError = F("Upload aborted");
				_UpdateEnd = true;
                if (_OTAabort) _OTAabort();
				return;
			}
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
    
    // подключить функцию которая вызывается перед рестартом платы
    void detachBeforeRestart() {
        _OTAbeforeRestart = nullptr;
    }
    
    // подключить функцию которая вызывается при прерывании загрузки обновления
    void attachAbort(void (*handler)()) {
        _OTAabort = *handler;
    }
    
    // подключить функцию которая вызывается при прерывании загрузки обновления
    void detachAbort() {
        _OTAabort = nullptr;
    }
    
    void defBuild(bool UpdateEnd, const String& UpdateError) {
    #ifndef	GP_OTA_FILES
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
            GP.OTA_FIRMWARE();
            GP.OTA_FILESYSTEM();
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

private:
#ifdef ESP8266
    ESP8266WebServer* _server;
#else
    WebServer* _server;
#endif
    String _OTAlogin;
    String _OTApass;
    String _UpdateError;
    bool _UpdateEnd;
    void (*_OTAbuild)(bool UpdateEnd, const String& UpdateError) = nullptr;
    void (*_OTAbeforeRestart)() = nullptr;
    void (*_OTAabort)() = nullptr;
};