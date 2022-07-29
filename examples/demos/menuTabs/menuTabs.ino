// пример реализации меню со вкладками
// значения не парсятся и не сохраняются

#define AP_SSID ""
#define AP_PASS ""

#include <GyverPortal.h>
GyverPortal portal;
bool m1, m2, m3;

void build() {
  BUILD_BEGIN();
  GP.THEME(GP_LIGHT);

  GP.FORM_SUBMIT("/menu1", "Menu 1");
  if (m1) {
    GP.LABEL("Slider 1");       GP.BREAK();
    GP.SLIDER("sld", 5, 0, 10); GP.BREAK();
    GP.LABEL("Slider 2");       GP.BREAK();
    GP.SLIDER("sld", 5, 0, 10); GP.BREAK();
    GP.LABEL("Slider 3");       GP.BREAK();
    GP.SLIDER("sld", 5, 0, 10); GP.BREAK();
  }

  GP.FORM_SUBMIT("/menu2", "Menu 2");
  if (m2) {
    GP.LABEL("Check 1"); GP.CHECK("ch"); GP.BREAK();
    GP.LABEL("Check 2"); GP.CHECK("ch"); GP.BREAK();
    GP.LABEL("Check 3"); GP.CHECK("ch"); GP.BREAK();
  }

  GP.FORM_SUBMIT("/menu3", "Menu 3");
  if (m3) {
    GP.LABEL("Time 1"); GP.TIME("time"); GP.BREAK();
    GP.LABEL("Time 2"); GP.TIME("time"); GP.BREAK();
    GP.LABEL("Time 3"); GP.TIME("time"); GP.BREAK();
  }

  BUILD_END();
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
  portal.attach(action);
  portal.start();
}

void action() {
  if (portal.form()) {
    if (portal.form("/menu1")) m1 = !m1;
    if (portal.form("/menu2")) m2 = !m2;
    if (portal.form("/menu3")) m3 = !m3;
  }
}

void loop() {
  portal.tick();
}
