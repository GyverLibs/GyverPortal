// как сделать слайдеры одной ширины с подписью

#define AP_SSID ""
#define AP_PASS ""

#include <GyverPortal.h>
GyverPortal portal;

void build() {
  GP.BUILD_BEGIN();
  GP.THEME(GP_DARK);

  GP_MAKE_BLOCK(
    GP_MAKE_BOX(
      GP.LABEL("Velocity");
      GP_MAKE_BOX(
        GP_RIGHT,
        "220px",
        GP.SLIDER("vel");
      );
    );

    GP_MAKE_BOX(
      GP.LABEL("Accel.");
      GP_MAKE_BOX(
        GP_RIGHT,
        "220px",
        GP.SLIDER("acc");
      );
    );
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
  portal.start();
}

void loop() {
  portal.tick();
}
