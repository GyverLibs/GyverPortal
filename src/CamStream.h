#pragma once

#include <esp_http_server.h>
#include <esp_camera.h>
#include <esp_wifi.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

#define _STREAM_BOUND "==stream=="
static const char* _STREAM_HEADER = "multipart/x-mixed-replace;boundary=" _STREAM_BOUND;
static const char* _CNT_TYPE_LEN = "X-Framerate: 60\r\nContent-Type: image/jpeg\r\nContent-Length: ";
static const char* _RNRN = "\r\n\r\n";
static const char* _STREAM_END = "\r\n--" _STREAM_BOUND "\r\n";

httpd_handle_t _stream_httpd = NULL;
static int _s_port = 90;

static esp_err_t _stream_handler(httpd_req_t *req) {
    camera_fb_t* fb = NULL;
    char buf[72];
    esp_err_t res = httpd_resp_set_type(req, _STREAM_HEADER);

    while (1) {
        if (res != ESP_OK) break;

        fb = esp_camera_fb_get();
        if (!fb) continue;

        strcpy(buf, _CNT_TYPE_LEN);
        utoa(fb->len, buf + strlen(buf), 10);
        strcpy(buf + strlen(buf), _RNRN);

        res = httpd_resp_send_chunk(req, (const char*)buf, strlen(buf));

        if (res == ESP_OK) {
            res = httpd_resp_send_chunk(req, (const char*)fb->buf, fb->len);
        }
        if (res == ESP_OK) {
            res = httpd_resp_send_chunk(req, _STREAM_END, strlen(_STREAM_END));
        }
        esp_camera_fb_return(fb);
        delay(20);
    }
    return res;
}

void cam_stream_begin(int port = 90) {
    esp_wifi_set_ps(WIFI_PS_NONE);  // no power save
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);  // disable brownout
    _s_port = port;
    httpd_config_t cfg = HTTPD_DEFAULT_CONFIG();
    cfg.server_port = _s_port;
    cfg.send_wait_timeout = 1;
    cfg.max_uri_handlers = 16;
    httpd_uri_t index = {"/", HTTP_GET, _stream_handler, NULL};
    if (httpd_start(&_stream_httpd, &cfg) == ESP_OK) {
        httpd_register_uri_handler(_stream_httpd, &index);
    }
}

String cam_stream_window(int width = 500) {
    String s;
    s += F("<img id='_stream' style='max-height:100%;width:");
    s += width;
    s += F("px'>\n<script>window.onload=document.getElementById('_stream').src=window.location.href.slice(0,-1)+':");
    s += _s_port;
    s += F("/';</script>\n");
    return s;
}
String cam_stream_window(const String& width) {
    String s;
    s += F("<img id='_stream' style='max-height:100%;width:");
    s += width;
    s += F("'>\n<script>window.onload=document.getElementById('_stream').src=window.location.href.slice(0,-1)+':");
    s += _s_port;
    s += F("/';</script>\n");
    return s;
}