// пример с кастомными компонентами
#include "custom.h"

#define AP_SSID ""
#define AP_PASS ""

#include <GyverPortal.h>
GyverPortal ui;

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
  ui.attachBuild(build);
  ui.attach(action);
  ui.start();
}

void build() {
  GP.BUILD_BEGIN(GP_DARK);
  GP_TEXT_HREF("/", "Home");
  GP.BUILD_END();
}

void action() {
}

void loop() {
  ui.tick();
}
