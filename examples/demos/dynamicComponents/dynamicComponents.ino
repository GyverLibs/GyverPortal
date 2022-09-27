// динамическое создание и опрос компонентов интерфейса

#define AP_SSID ""
#define AP_PASS ""

#include <GyverPortal.h>
GyverPortal portal;

void build() {
  GP.BUILD_BEGIN();
  GP.THEME(GP_DARK);

  for (int i = 0; i < 5; i++) {
    GP.SLIDER(String("sld-") + i);
  }

  for (int i = 0; i < 5; i++) {
    GP.BUTTON(String("btn-") + i, String("Button ") + i);
  }

  GP.BUILD_END();
}

void action() {
  if (portal.click()) {
    if (portal.clickName().startsWith("sld")) {
      Serial.print("Slider ");
      Serial.print(portal.clickName());
      Serial.print(": ");
      Serial.println(portal.getInt());

    } else if (portal.clickName().startsWith("btn")) {
      Serial.print("Click: ");
      Serial.println(portal.clickName());
    }
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
