// пример работы окна лога, выводит случайные hello

#define AP_SSID ""
#define AP_PASS ""

#include <GyverPortal.h>
GyverPortal portal;

// конструктор страницы
void build() {
  GP.BUILD_BEGIN();
  GP.THEME(GP_DARK);
  GP.BUTTON("bt","bt");
  GP.AREA_LOG(5);
  GP.BUILD_END();
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(AP_SSID, AP_PASS);
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
