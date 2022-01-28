#include <GyverPortal.h>
#define PLOT_SIZE 10
int16_t arr[2][PLOT_SIZE];
const char *names[] = {"kek", "puk",};

// билдер страницы
void build() {
  String s;
  BUILD_BEGIN(s);
  add.THEME(GP_LIGHT);

  //add.PLOT_DARK<2, PLOT_SIZE>("table", names, arr);
  add.PLOT<2, PLOT_SIZE>("table", names, arr);

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

  static uint32_t tmr;
  if (millis() - tmr >= 5000) {
    tmr = millis();
    GPaddInt(random(100), arr[0], PLOT_SIZE);
    GPaddInt(random(100), arr[1], PLOT_SIZE);
  }
}
