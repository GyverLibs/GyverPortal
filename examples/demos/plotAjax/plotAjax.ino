// тест графиков AJAX_PLOT, вывод случайных значений

#define AP_SSID ""
#define AP_PASS ""

#include <GyverPortal.h>
GyverPortal portal;

// конструктор
void build() {
  GP.BUILD_BEGIN();
  GP.THEME(GP_DARK);
  GP.AJAX_PLOT("plot1", 1, 20, 1000);
  GP.AJAX_PLOT_DARK("plot3", 3, 20, 1000);
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

  // подключаем конструктор и запускаем
  portal.attachBuild(build);
  portal.attach(action);
  portal.start();
}

void action() {
  if (portal.update("plot1")) portal.answer(random(200));
  if (portal.update("plot3")) {
    int answ[] = {random(200), random(200), random(200)};
    portal.answer(answ, 3);
  }
}

void loop() {
  portal.tick();
}
