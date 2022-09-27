// всплывающее окно alert
// активация по клику по кнопке

#define AP_SSID ""
#define AP_PASS ""

#include <GyverPortal.h>
GyverPortal portal;

// флаг для передачи действия
bool alertF, alertFT;

void build() {
  GP.BUILD_BEGIN();
  GP.THEME(GP_DARK);

  GP.ALERT("alt");
  GP.ALERT("altT", "Alert Text");

  GP.BUTTON("btnA", "Alert");
  GP.BUTTON("btnAT", "Alert Text");

  GP.BUILD_END();
}

void action() {
  if (portal.update()) {
    if (alertF && portal.update("alt")) {
      alertF = 0;
      portal.answer(String("Alert! millis(): ") + millis());
    }

    if (alertFT && portal.update("altT")) {
      alertFT = 0;
      portal.answer(1);
    }
  }

  if (portal.click()) {
    if (portal.click("btnA")) alertF = 1;
    if (portal.click("btnAT")) alertFT = 1;
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

  portal.attachBuild(build);
  portal.attach(action);
  portal.start();
}

void loop() {
  portal.tick();
}
