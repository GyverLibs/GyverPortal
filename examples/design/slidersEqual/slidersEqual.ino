// как сделать слайдеры одной ширины с подписью

#define AP_SSID ""
#define AP_PASS ""

#include <GyverPortal.h>
GyverPortal ui;

void build() {
  GP.BUILD_BEGIN();
  GP.THEME(GP_DARK);

  M_BLOCK(
    M_BOX(
      GP.LABEL("Velocity");
      M_BOX(
        GP_RIGHT,
        "220px",
        GP.SLIDER("vel");
      );
    );

    M_BOX(
      GP.LABEL("Accel.");
      M_BOX(
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

  ui.attachBuild(build);
  ui.start();
}

void loop() {
  ui.tick();
}
