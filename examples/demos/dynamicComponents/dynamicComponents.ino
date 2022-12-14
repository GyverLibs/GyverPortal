// динамическое создание и опрос компонентов интерфейса

#define AP_SSID ""
#define AP_PASS ""

#include <GyverPortal.h>
GyverPortal ui;

void build() {
  GP.BUILD_BEGIN();
  GP.THEME(GP_DARK);

  // ОБНОВЛЕНИЯ
  String s;
  // формируем список для UPDATE
  // вида "lbl/0,lbl/1..."
  for (int i = 0; i < 5; i++) {
    s += "lbl/";
    s += i;
    s += ',';
  }
  GP.UPDATE(s);

  // создаём лейблы с именами lbl/0,lbl/1...
  for (int i = 0; i < 5; i++) {
    GP.LABEL_BLOCK("", String("lbl/") + i);
    GP.BREAK();
  }

  // КЛИКИ
  // создём слайдеры с именами sld/0, sld/1 ...
  for (int i = 0; i < 5; i++) {
    GP.SLIDER(String("sld/") + i);
  }

  // создём кнопки с именами btn/0, btn/1 ...
  for (int i = 0; i < 5; i++) {
    GP.BUTTON(String("btn/") + i, String("Button ") + i);
  }

  GP.BUILD_END();
}

void action() {
  if (ui.click()) {
    if (ui.clickSub("sld")) {   // начинается с sld
      Serial.print("Slider ");
      Serial.print(ui.clickNameSub(1)); // получаем цифру
      Serial.print(": ");
      Serial.println(ui.getInt());

    }
    if (ui.clickSub("btn")) {   // начинается с btn
      Serial.print("Click: ");
      Serial.println(ui.clickNameSub(1)); // получаем цифру
    }
  }

  if (ui.update()) {
    if (ui.updateSub("lbl")) {   // начинается с lbl
      // формируем ответ вида "lbl #0: 123"
      String s;
      s += "lbl #";
      s += ui.updateNameSub(1);
      s += ":";
      s += random(10);
      ui.answer(s);
    }
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
}

void loop() {
  ui.tick();
}
