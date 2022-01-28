// лог данных с записью в EEPROM и выводом графика в браузер
// храним точки графика с привязкой к реальному времени из интернета

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
#include <GyverPortal.h>
#include <EEPROM.h>

// билдер страницы
void build() {
  String s;
  BUILD_BEGIN(s);
  add.THEME(GP_DARK);

  add.PLOT_STOCK_DARK<2, PLOT_SIZE>("plot", names, data.unix, data.vals);
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

  // настраиваем получение времени
  configTime(GMT_OFF * 60ul * 60, 0, "pool.ntp.org");

  // запускаем епром и читаем данные
  //EEPROM.begin(sizeof(data));
  //EEPROM.get(0, data);
}

void loop() {
  portal.tick();
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
