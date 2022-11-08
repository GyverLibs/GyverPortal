// пример с кастомными компонентами
#include "custom.h"

#define AP_SSID ""
#define AP_PASS ""

#include <GyverPortal.h>
GyverPortal portal;

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(AP_SSID, AP_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(WiFi.localIP());

  // подключаем конструктор и запускаем
  portal.attachBuild(build);
  portal.attach(action);
  portal.start();
}

void build() {
  GP.BUILD_BEGIN();
  GP.THEME(GP_DARK);
  GP_TEXT_HREF("/", "Home");
  GP.BUILD_END();
}

void action() {
}

void loop() {
  portal.tick();
}
