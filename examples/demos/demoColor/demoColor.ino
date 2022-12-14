// работа с цветом

#define AP_SSID ""
#define AP_PASS ""

#include <GyverPortal.h>
GyverPortal ui;

void build() {
  GP.BUILD_BEGIN();
  GP.THEME(GP_DARK);

  // доступ к каналам цвета
  GPcolor color1(255, 0, 0);
  GP.COLOR("c1", color1);
  
  color1.r = 0;
  color1.g = 255;
  color1.b = 255;
  GP.COLOR("c2", color1);

  // HEX
  GPcolor color2;
  color2 = 0x00ff00;
  GP.COLOR("c3", color2);

  // WEB HEX
  GPcolor color3("#ffff00");
  GP.COLOR("c4", color3);

  GP.BUILD_END();
}

void action() {
  if (ui.click()) {
    // первый цвет, выведем в порт
    if (ui.click("c1")) Serial.println(ui.getString());

    // второй перепишем в буфер и выведем каналами
    GPcolor buf;
    if (ui.copyColor("c2", buf)) {
      Serial.print(buf.r);
      Serial.print(',');
      Serial.print(buf.g);
      Serial.print(',');
      Serial.println(buf.b);
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

  // подключаем конструктор и запускаем
  ui.attachBuild(build);
  ui.attach(action);
  ui.start();
}

void loop() {
  ui.tick();
}
