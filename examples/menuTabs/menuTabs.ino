// пример реализации меню со вкладками
// значения не парсятся
#include "GyverPortal.h"
bool m1, m2, m3;

void build() {
  String s;
  BUILD_BEGIN(s);
  add.THEME(GP_LIGHT);
  add.FORM_SUBMIT("/menu1", "Menu 1");
  if (m1) {
    add.LABEL("Slider 1"); add.BREAK();
    add.SLIDER("sld", 5, 0, 10); add.BREAK();
    add.LABEL("Slider 2"); add.BREAK();
    add.SLIDER("sld", 5, 0, 10); add.BREAK();
    add.LABEL("Slider 3"); add.BREAK();
    add.SLIDER("sld", 5, 0, 10); add.BREAK();
  }
  add.FORM_SUBMIT("/menu2", "Menu 2");
  if (m2) {
    add.LABEL("Check 1"); add.CHECK("ch"); add.BREAK();
    add.LABEL("Check 2"); add.CHECK("ch"); add.BREAK();
    add.LABEL("Check 3"); add.CHECK("ch"); add.BREAK();
  }
  add.FORM_SUBMIT("/menu3", "Menu 3");
  if (m3) {
    add.LABEL("Time 1"); add.TIME("time"); add.BREAK();
    add.LABEL("Time 2"); add.TIME("time"); add.BREAK();
    add.LABEL("Time 3"); add.TIME("time"); add.BREAK();
  }
  add.FORM_SUBMIT("/exit", "Exit");
  BUILD_END();
}

void setup() {
  Serial.begin(9600);
  /*
    WiFi.mode(WIFI_AP);
    WiFi.softAP("ESP Portal");
  */

  WiFi.mode(WIFI_STA);
  WiFi.begin("", "");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(WiFi.localIP());

  GyverPortal portal;
  portal.attachBuild(build);
  portal.start();

  while (portal.tick()) {
    if (portal.form()) {
      if (portal.form("/menu1")) m1 = !m1;
      if (portal.form("/menu2")) m2 = !m2;
      if (portal.form("/menu3")) m3 = !m3;
      if (portal.form("/exit")) portal.stop();
    }
  }
}

void loop() {
}
