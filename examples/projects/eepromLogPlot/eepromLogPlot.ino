// лог данных с записью в EEPROM и выводом графика в браузер
// храним точки графика с привязкой к реальному времени из интернета

#define AP_SSID ""
#define AP_PASS ""

#define GMT_OFF 3       // часовой пояс, +3 для Москвы
#define LOG_PRD 1       // период обновления данных, секунд
#define PLOT_SIZE 100   // длина графика, точек

struct PlotData {
  int16_t vals[2][PLOT_SIZE];
  uint32_t unix[PLOT_SIZE];
};

PlotData data;  // храним всё в структуре для удобства

const char *names[] = {"Random", "Counter"};

#include <ESP8266WiFi.h>
#include "time.h"
#include <EEPROM.h>
#include <GyverPortal.h>
GyverPortal ui;

// конструктор страницы
void build() {
  GP.BUILD_BEGIN();
  GP.THEME(GP_DARK);

  GP.PLOT_STOCK_DARK<2, PLOT_SIZE>("plot", names, data.unix, data.vals);
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

  // настраиваем получение времени
  configTime(GMT_OFF * 60ul * 60, 0, "pool.ntp.org");

  // запускаем епром и читаем данные
  //EEPROM.begin(sizeof(data));
  //EEPROM.get(0, data);
}

void loop() {
  ui.tick();
  dataLog();
}

void dataLog() {
  static uint32_t tmr;   // переменная таймера
  static byte counter;   // счётчик для графика

  // таймер на LOG_PRD секунд
  if (millis() - tmr >= LOG_PRD * 1000ul) {
    tmr = millis();

    // запрашиваем время
    time_t rawtime;
    time(&rawtime);

    // точное время не получено, выходим
    // первое обновление через ~30 секунд после подключения
    if (rawtime < 1600000000) return;

    // добавляем точки в график
    GPaddInt(random(255), data.vals[0], PLOT_SIZE);
    GPaddInt(counter, data.vals[1], PLOT_SIZE);
    GPaddUnix(rawtime, data.unix, PLOT_SIZE);
    counter++;

    // сохраняем в епром
    EEPROM.put(0, data);
    EEPROM.commit();
  }
}
