// пример работы окна лога, выводит случайные hello

#define AP_SSID ""
#define AP_PASS ""

#include <GyverPortal.h>
GyverPortal ui;

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

  ui.attachBuild(build);
  ui.start();
  ui.log.start(30);   // передали размер буфера
}

void loop() {
  ui.tick();

  static uint32_t tmr;
  if (millis() - tmr > 2000) {
    tmr = millis();
    ui.log.print("Hello: #");
    ui.log.println(random(100));
  }
}
