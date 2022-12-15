// таблица с рандомным содержимым

#define AP_SSID ""
#define AP_PASS ""

#include <GyverPortal.h>
GyverPortal ui;

void build() {
  GP.BUILD_BEGIN(600);
  GP.THEME(GP_DARK);

  M_TABLE(
    "",
    GP_ALS(GP_LEFT, GP_LEFT),
  for (int i = 0; i < 10; i++) {
  M_TR(rndLabel(), rndComp());
  }
  );

  GP.BUILD_END();
}

void rndComp() {
  switch (random(4)) {
    case 0: GP.SLIDER("sld"); break;
    case 1: GP.SPINNER("spn"); break;
    case 2: GP.SELECT("sel", "1,2,3"); break;
    case 3: GP.SWITCH("sw"); break;
  }
}

void rndLabel() {
  String s;
  int len = random(5, 15);
  for (int i = 0; i < len; i++) {
    s += (char)random(40, 127);
  }
  GP.LABEL(s);
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
