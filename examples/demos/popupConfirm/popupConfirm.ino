// всплывающее окно confirm
// активация по клику по кнопке

#define AP_SSID ""
#define AP_PASS ""

#include <GyverPortal.h>
GyverPortal portal;

// флаг для передачи действия
bool confirmF, confirmFT;

void build() {
  GP.BUILD_BEGIN();
  GP.THEME(GP_DARK);

  GP.CONFIRM("cfm");
  GP.CONFIRM("cfmT", "Confirm Text");

  GP.BUTTON("btnC", "Confirm");
  GP.BUTTON("btnCT", "Confirm Text");

  GP.BUILD_END();
}

void action() {
  if (portal.update()) {
    if (confirmF && portal.update("cfm")) {
      confirmF = 0;
      portal.answer(String("Confirm! millis(): ") + millis());
    }

    if (confirmFT && portal.update("cfmT")) {
      confirmFT = 0;
      portal.answer(1);
    }
  }

  if (portal.click()) {
    if (portal.click("btnC")) confirmF = 1;
    if (portal.click("btnCT")) confirmFT = 1;
    
    if (portal.click("cfm")) Serial.println(portal.getInt());
    if (portal.click("cfmT")) Serial.println(portal.getInt());
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
