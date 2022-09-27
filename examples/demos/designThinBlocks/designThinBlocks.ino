// пример использования "блоков" для оформления

#define AP_SSID ""
#define AP_PASS ""

#include <GyverPortal.h>
GyverPortal portal;

void build() {
  GP.BUILD_BEGIN(600);
  GP.THEME(GP_DARK);

  // создаём блок вручную
  GP_MAKE_BLOCK_THIN_TAB(
    "MOTOR CONFIG",
    GP_MAKE_BOX(GP.LABEL("Velocity"); GP.SLIDER("vel"););
    GP_MAKE_BOX(GP.LABEL("Accel."); GP.SLIDER("acc"););
  );

  // через макрос
  GP_MAKE_BLOCK_THIN_TAB(
    "MANUAL",
    GP_MAKE_BOX(GP.LABEL("Duty"); GP.SLIDER("spd"););
    GP_MAKE_BOX(GP.BUTTON("bkw", "◄"); GP.BUTTON("frw", "►"););
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
