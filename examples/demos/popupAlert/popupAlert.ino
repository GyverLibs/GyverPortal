// всплывающее окно alert
// активация по событию из скетча и по кнопке в браузере

#define AP_SSID ""
#define AP_PASS ""

#include <GyverPortal.h>
GyverPortal ui;

void build() {
  GP.BUILD_BEGIN();
  GP.THEME(GP_DARK);

  // запрос на обновление каждую секунду
  GP.UPDATE("alt");
  GP.ALERT("alt");

  // вызов алерта по кнопке
  GP.ALERT("altT", "Alert Text");
  GP.BUTTON("btnA", "Alert");
  GP.UPDATE_CLICK("altT", "btnA");

  GP.BUILD_END();
}

bool flag;

void action() {
  if (ui.update()) {
    // вызов с таймера
    if (flag && ui.update("alt")) {
      flag = 0;
      ui.answer(String("Alert! millis(): ") + millis());
    }

    // вызов с кнопки
    if (ui.update("altT")) {
      ui.answer(1); // показать алерт (текст у него задан в конструкторе)
    }
  }
}

void setup() {
  startup();

  ui.attachBuild(build);
  ui.attach(action);
  ui.start();
}

void loop() {
  ui.tick();

  // "вызываем" алерт раз в 5 секунд
  static uint32_t tmr;
  if (millis() - tmr >= 5000) {
    tmr = millis();
    flag = 1;
  }
}

void startup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(AP_SSID, AP_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(WiFi.localIP());
}
