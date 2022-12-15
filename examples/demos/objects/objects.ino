// работа с "объектами" компонентов
// их можно создавать, настраивать, отправлять на страницу и опрашивать в action

#define AP_SSID ""
#define AP_PASS ""

#include <GyverPortal.h>
GyverPortal ui;

GP_LABEL_BLOCK lbl("lbl");
GP_BUTTON btn("btn", "Button");
GP_TEXT txt("txt");
GP_SELECT sel("sel");

// конструктор страницы
void build() {
  GP.BUILD_BEGIN(GP_DARK);

  GP.LABEL("Random text:");

  // лейбл с апдейтом
  GP.UPDATE(lbl.name);
  GP.LABEL_BLOCK(lbl);
  GP.BREAK();

  GP.BUTTON(btn);
  GP.TEXT(txt);
  GP.SELECT(sel);
  
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

  // подключаем конструктор и запускаем
  ui.attachBuild(build);
  ui.attach(action);
  ui.start();

  // настройка селекта
  sel.list = F("Sel 1,Sel2,Sel 3, Sel 4");
  sel.numbers = true;
}

void action() {
  if (ui.update()) {
    ui.update(lbl);   // автоматически обновить лейбл на странице
  }
  
  if (ui.click()) {
    if (ui.click(btn)) Serial.println("Click");
    if (ui.click(txt)) Serial.println(txt.text);
    if (ui.click(sel)) Serial.println(sel.getValue());
  }
}

void loop() {
  ui.tick();

  // имитируем изменение строки в программе
  static uint32_t tmr;
  if (millis() - tmr >= 500) {
    tmr = millis();
    lbl.text = "";
    for (int i = 0; i < 5; i++) lbl.text += (char)random(48, 126);
  }
  
}
