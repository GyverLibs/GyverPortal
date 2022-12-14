// использование BOX для вёрстки интерфейса

#define AP_SSID ""
#define AP_PASS ""

#include <GyverPortal.h>
GyverPortal ui;

void build() {
  GP.BUILD_BEGIN();
  GP.THEME(GP_DARK);

  GP.BOX_BEGIN(GP_JUSTIFY);
  GP.LABEL("Switch");
  GP.BOX_BEGIN(GP_RIGHT);
  GP.LED("");
  GP.SWITCH("");
  GP.LED("");
  GP.BOX_END();
  GP.BOX_END();

  GP.HR();

  GP.BOX_BEGIN(GP_CENTER);
  GP.SPINNER("sp1");
  GP.SPINNER("sp2");
  GP.BOX_END();

  GP.HR();

  GP.BOX_BEGIN(GP_JUSTIFY);
  GP.LABEL("Slider");
  GP.SLIDER("sld");
  GP.BOX_END();

  GP.HR();

  GP.BOX_BEGIN(GP_JUSTIFY);
  GP.LABEL("Setting");
  GP.BOX_BEGIN(GP_RIGHT);
  GP.CHECK("ch1");
  GP.CHECK("ch2");
  GP.CHECK("ch3");
  GP.BOX_END();
  GP.BOX_END();
  
  GP.HR();

  GP.BOX_BEGIN(GP_JUSTIFY);
  GP.LABEL("Buttons");
  GP.BOX_BEGIN(GP_RIGHT);
  GP.BUTTON_MINI("b1", "Kek", "", GP_RED);
  GP.BUTTON_MINI("b1", "Puk");
  GP.BOX_END();
  GP.BOX_END();

  GP.HR();

  GP.BOX_BEGIN(GP_JUSTIFY);
  GP.LABEL("Upload settings");
  GP.FILE_UPLOAD("file");
  GP.BOX_END();

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
  ui.attach(action);
  ui.start();
}

void action() {
}

void loop() {
  ui.tick();
}
