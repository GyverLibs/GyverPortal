// пример с удерживанием кнопки

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


bool holdFlag;  // глобальный флаг

void build() {
  GP.BUILD_BEGIN();
  GP.THEME(GP_DARK);
  GP.BUTTON("btn", "Button");
  GP.BUILD_END();
}

void action() {
  if (portal.clickDown("btn")) holdFlag = 1;
  if (portal.clickUp("btn")) holdFlag = 0;
}

void loop() {
  portal.tick();

  // печатаем если флаг поднят
  if (holdFlag) asyncPrint();
}

// пишет в порт по своему таймеру
void asyncPrint() {
  static uint32_t tmr;
  if (millis() - tmr >= 500) {
    tmr = millis();
    Serial.println("Hold!");
  }
}
