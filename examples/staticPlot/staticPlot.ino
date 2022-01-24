#include <GyverPortal.h>
int arr[][3] = {
  {10, 15, 1},
  {20, 25, 2},
  {30, 35, 3},
  {40, 45, 4},
  {50, 55, 5},
};
const char *names[] = {"value x", "kek", "puk",};

// билдер страницы
void build() {
  String s;
  BUILD_BEGIN(s);
  add.THEME(GP_LIGHT);
  add.PLOT_DARK<3>("table", "Vaxis", names, arr, 5);
  add.PLOT_LIGHT<3>("table2", "Vaxis", names, arr, 5);
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
}
