// пример ручного использования лога

#define AP_SSID ""
#define AP_PASS ""

#include <GyverPortal.h>
GyverPortal ui;

GPlog glog1("log1");
GPlog glog2("log2");

// конструктор страницы
void build() {
  GP.BUILD_BEGIN();
  GP.THEME(GP_DARK);

  GP.AREA_LOG(glog1);
  GP.AREA_LOG(glog2);

  GP.BUILD_END();
}

void action() {
  if (ui.update()) {
    ui.updateLog(glog1);
    ui.updateLog(glog2);
  }
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
  ui.attach(action);
  ui.start();

  glog1.start(30);   // передали размер буфера
  glog2.start();
}

void loop() {
  ui.tick();

  static uint32_t tmr1, tmr2;
  if (millis() - tmr1 > 2000) {
    tmr1 = millis();
    glog1.print("Hello: #");
    glog1.println(random(100));
  }
  if (millis() - tmr2 > 1000) {
    tmr2 = millis();
    glog2.print("millis: ");
    glog2.println(millis());
  }
}
