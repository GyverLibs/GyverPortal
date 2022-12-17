#include "GyverPortal.h"

#ifdef ESP8266
ESP8266WebServer* _gp_s;
#else
WebServer* _gp_s;
#endif

String* _GPP;
String* _gp_uri;
int _gp_bufsize;
uint32_t _gp_unix_tmr = 0;
uint32_t _gp_local_unix = 0;
Builder GP;
const char* _gp_style = nullptr;
uint8_t _gp_seed;
const char* _gp_mdns = nullptr;