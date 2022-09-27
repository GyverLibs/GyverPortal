// использование BOX для вёрстки интерфейса

#define AP_SSID ""
#define AP_PASS ""

#include <GyverPortal.h>
GyverPortal portal;

void build() {
  GP.BUILD_BEGIN();
  GP.THEME(GP_DARK);

  GP_MAKE_BOX(
    GP_EDGES,
    GP.LABEL("Switch");
    GP_MAKE_BOX(
      GP_RIGHT,
      GP.LED("");
      GP.SWITCH("");
      GP.LED("");
    );
  );

  GP.HR();

  GP_MAKE_BOX(
    GP_CENTER,
    GP.SPINNER("sp1");
    GP.SPINNER("sp2");
  );

  GP.HR();

  GP_MAKE_BOX(
    GP_EDGES,
    GP.LABEL("Slider");
    GP.SLIDER("sld");
  );

  GP.HR();

  GP_MAKE_BOX(
    GP_EDGES,
    GP.LABEL("Setting");
    GP_MAKE_BOX(
      GP_RIGHT,
      GP.CHECK("ch1");
      GP.CHECK("ch2");
      GP.CHECK("ch3");
    );
  );

  GP.HR();

  GP_MAKE_BOX(
    GP_EDGES,
    GP.LABEL("Buttons");
    GP_MAKE_BOX(
      GP_RIGHT,
      GP.BUTTON_MINI("b1", "Kek", "", GP_RED);
      GP.BUTTON_MINI("b1", "Puk");
    );
  );

  GP.HR();

  GP_MAKE_BOX(
    GP_EDGES,
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

  portal.attachBuild(build);
  portal.attach(action);
  portal.start();
}

void action() {
}

void loop() {
  portal.tick();
}
