// тест графика PLOT/PLOT_DARK

#define AP_SSID ""
#define AP_PASS ""

#include <GyverPortal.h>
GyverPortal ui;

#define PLOT_SIZE 10
int16_t arr[2][PLOT_SIZE];
const char *names[] = {"kek", "puk",};

// конструктор страницы
void build() {
  GP.BUILD_BEGIN();
  GP.THEME(GP_LIGHT);

  //GP.PLOT_DARK<2, PLOT_SIZE>("table", names, arr);
  GP.PLOT<2, PLOT_SIZE>("table", names, arr);

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
  ui.attachBuild(build);
  ui.start();
}

void loop() {
  ui.tick();

  static uint32_t tmr;
  if (millis() - tmr >= 5000) {
    tmr = millis();
    GPaddInt(random(100), arr[0], PLOT_SIZE);
    GPaddInt(random(100), arr[1], PLOT_SIZE);
  }
}
