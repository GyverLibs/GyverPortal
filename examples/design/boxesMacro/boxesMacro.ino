// использование BOX для вёрстки интерфейса

#define AP_SSID ""
#define AP_PASS ""

#include <GyverPortal.h>
GyverPortal ui;

void build() {
  GP.BUILD_BEGIN();
  GP.THEME(GP_DARK);

  M_BOX(
    GP_JUSTIFY,
    GP.LABEL("Switch");
    M_BOX(
      GP_RIGHT,
      GP.LED("");
      GP.SWITCH("");
      GP.LED("");
    );
  );

  GP.HR();

  M_BOX(
    GP_CENTER,
    GP.SPINNER("sp1");
    GP.SPINNER("sp2");
  );

  GP.HR();

  M_BOX(
    GP_JUSTIFY,
    GP.LABEL("Slider");
    GP.SLIDER("sld");
  );

  GP.HR();

  M_BOX(
    GP_JUSTIFY,
    GP.LABEL("Setting");
    M_BOX(
      GP_RIGHT,
      GP.CHECK("ch1");
      GP.CHECK("ch2");
      GP.CHECK("ch3");
    );
  );

  GP.HR();

  M_BOX(
    GP_JUSTIFY,
    GP.LABEL("Buttons");
    M_BOX(
      GP_RIGHT,
      GP.BUTTON_MINI("b1", "Kek", "", GP_RED);
      GP.BUTTON_MINI("b1", "Puk");
    );
  );

  GP.HR();

  M_BOX(
    GP_JUSTIFY,
    GP.LABEL("Upload settings");
    GP.FILE_UPLOAD("file");
  );

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
