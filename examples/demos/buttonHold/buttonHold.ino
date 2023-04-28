// пример с обработкой удержания кнопки тремя способами

#define AP_SSID ""
#define AP_PASS ""

#include <GyverPortal.h>
GyverPortal ui;

void startup();
void build();
void action();
void asyncPrint(const char* str);

void setup() {
  startup();

  // подключаем конструктор и запускаем
  ui.attachBuild(build);
  ui.attach(action);
  ui.start();
}

void build() {
  GP.BUILD_BEGIN();
  GP.THEME(GP_DARK);
  GP.BUTTON_MINI("btn1", "Button 1");
  GP.BUTTON_MINI("btn2", "Button 2");
  GP.BUTTON_MINI("btn3", "Button 3");
  GP.BUILD_END();
}

bool holdFlag2;  // флаг удержания второй кнопки

void action() {
  // обработчик изменения состояния кнопок
  if (ui.hold()) {
    // обработка отдельно нажатия и отпускания
    if (ui.clickDown("btn1")) Serial.println("Press 1");
    if (ui.clickUp("btn1")) Serial.println("Release 1");
    
    holdFlag2 = ui.hold("btn2");
  }
}

void loop() {
  ui.tick();

  // проверяем через свой флаг
  if (holdFlag2) asyncPrint("hold btn 2");

  // в принципе можно и так, это будет нагружать процессор сравнением строк
  // только пока кнопка удерживается
  if (ui.hold("btn3")) asyncPrint("hold btn 3");
}

// пишет в порт по своему таймеру
void asyncPrint(const char* str) {
  static uint32_t tmr;
  if (millis() - tmr >= 500) {
    tmr = millis();
    Serial.println(str);
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
