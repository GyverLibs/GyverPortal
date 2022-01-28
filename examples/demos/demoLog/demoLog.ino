#include <GyverPortal.h>
GyverPortal portal;

// билдер страницы
void build() {
  String s;
  BUILD_BEGIN(s);
  add.THEME(GP_DARK);
  add.AREA_LOG(5);
  BUILD_END();
}

void setup() {
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  WiFi.begin("", "");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(WiFi.localIP());

  portal.attachBuild(build);
  portal.start();
  portal.log.start(30);   // передали размер буфера
}

void loop() {
  portal.tick();

  static uint32_t tmr;
  if (millis() - tmr > 2000) {
    tmr = millis();
    portal.log.print("Hello: #");
    portal.log.println(random(100));
  }
}
