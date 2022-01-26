#include <GyverPortal.h>

// билдер страницы
void build() {
  String s;
  BUILD_BEGIN(s);
  add.THEME(GP_DARK);

  add.AJAX_PLOT("plot1", 1, 20, 1000);
  add.AJAX_PLOT_DARK("plot3", 3, 20, 1000);
  BUILD_END();
}

GyverPortal portal;

void setup() {
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  WiFi.begin("", "");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(WiFi.localIP());

  // подключаем билдер и запускаем
  portal.attachBuild(build);
  portal.start();
}

void loop() {
  portal.tick();
  if (portal.update()) {
    if (portal.update("plot1")) portal.answer(random(200));
    if (portal.update("plot3")) {
      int16_t answ[] = {random(200), random(200), random(200)};
      portal.answer(answ, 3);
    }
  }
}
