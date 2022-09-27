// всплывающее окно prompt
// активация по клику по кнопке

#define AP_SSID ""
#define AP_PASS ""

#include <GyverPortal.h>
GyverPortal portal;

// флаг для передачи действия
bool promptF, promptFT;

void build() {
  GP.BUILD_BEGIN();
  GP.THEME(GP_DARK);

  GP.PROMPT("prm");
  GP.PROMPT("prmT", "Prompt Text");

  GP.BUTTON("btnP", "Prompt");
  GP.BUTTON("btnPT", "Prompt Text");

  GP.BUILD_END();
}

void action() {
  if (portal.update()) {
    if (promptF && portal.update("prm")) {
      promptF = 0;
      portal.answer(String("Prompt! millis(): ") + millis());
    }

    if (promptFT && portal.update("prmT")) {
      promptFT = 0;
      portal.answer(1);
    }
  }

  if (portal.click()) {
    if (portal.click("btnP")) promptF = 1;
    if (portal.click("btnPT")) promptFT = 1;

    if (portal.click("prm")) Serial.println(portal.getString());
    if (portal.click("prmT")) Serial.println(portal.getString());
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
