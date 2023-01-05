// пример использования таблиц для оформления

#define AP_SSID ""
#define AP_PASS ""

#include <GyverPortal.h>
GyverPortal ui;

void build() {
  GP.BUILD_BEGIN(600);
  GP.THEME(GP_DARK);

  // включаем границы таблиц (ДЛЯ НАГЛЯДНОСТИ)
  GP.TABLE_BORDER(true);

  // делаем таблицу вручную
  GP.TITLE("Manual Table");
  GP.TABLE_BEGIN();

  GP.TR();
  GP.TD();
  GP.LABEL("Row 1");
  GP.TD();
  GP.CHECK("");

  GP.TR();
  GP.TD();
  GP.LABEL("Row 2");
  GP.TD();
  GP.CHECK("");

  GP.TABLE_END();

  // делаем таблицу с объединением ячеек
  GP.TITLE("Span Table");
  GP.TABLE_BEGIN();

  GP.TR();
  GP.TD(GP_CENTER, 2);
  GP.LABEL("Big table cell");

  GP.TR();
  GP.TD();
  GP.LABEL("First");
  GP.TD();
  GP.LABEL("Second");

  GP.TABLE_END();

  // авто таблица 1
  GP.TITLE("Auto Table 1");
  M_TABLE(

  for (int i = 0; i < 5; i++) {

  // макрос создаёт РЯД таблицы, элементы ЧЕРЕЗ ЗАПЯТУЮ
  M_TR(
    GP.LABEL(String("Switch #") + i),
    GP.SWITCH("")
  );

  } // for
  );


  // авто таблица 2
  // с указанием размеров ячеек
  GP.TITLE("Auto Table 2");
  M_TABLE(
    "30%,20%,50%",
  for (int i = 0; i < 5; i++) {
  M_TR(
    GP.LABEL(String("Label #") + i),
    GP.CHECK(""),
    GP.BUTTON("", "Btn")
  );
  }
  );

  // авто таблица 3
  // с указанием ориентации ячеек
  GP.TITLE("Auto Table 3");
  M_TABLE(
    "",
    GP_ALS(GP_LEFT, GP_CENTER, GP_RIGHT),
  for (int i = 0; i < 3; i++) {
  M_TR(GP.LABEL(String(i)), GP.CHECK(""), GP.LED(""));
  }
  );

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

  ui.attachBuild(build);
  ui.start();
}

void loop() {
  ui.tick();
}
