// тест графика PLOT_STOCK

#define AP_SSID ""
#define AP_PASS ""

#include <GyverPortal.h>
GyverPortal ui;

#define PLOT_SIZE 100
int16_t arr[2][PLOT_SIZE];
uint32_t dates[PLOT_SIZE];
const char *names[] = {"kek", "puk",};

// конструктор страницы
void build() {
  GP.BUILD_BEGIN();
  GP.THEME(GP_DARK);

  GP.PLOT_STOCK_DARK<2, PLOT_SIZE>("plot", names, dates, arr);
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

  // ставим последний элемент массива на текущую дату
  // gmt 3 для Москвы
  dates[PLOT_SIZE - 1] = GPunix(2022, 1, 22, 21, 59, 0, 3);

  for (int i = 0; i < PLOT_SIZE; i++) {
    GPaddInt(i * 2, arr[0], PLOT_SIZE);
    GPaddInt(i * 5, arr[1], PLOT_SIZE);

    // добавляем значения графика по 5 секунд
    GPaddUnixS(5, dates, PLOT_SIZE);
  }

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
    GPaddUnixS(5, dates, PLOT_SIZE);
  }
}
