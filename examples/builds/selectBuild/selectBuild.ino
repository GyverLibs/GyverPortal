// вывод компонентов и значений в зависимости от выбора в SELECT
// хранение текста в char[] для удобства сохранения в EEPROM

#define AP_SSID ""
#define AP_PASS ""

#include <GyverPortal.h>
GyverPortal ui;

int idx;
char dates[10][10];
char times[10][10];

void build() {
  GP.BUILD_BEGIN(GP_DARK);

  GP.SELECT("idx", "1,2,3,4,5", idx);
  GP.RELOAD_CLICK("idx");
  GP.BREAK();

  GP.TEXT(String("date/") + idx, "", dates[idx]);
  GP.BREAK();
  GP.TEXT(String("time/") + idx, "", times[idx]);

  GP.BUILD_END();
}

void action() {
  if (ui.click()) {
    ui.clickInt("idx", idx);  // поймать и записать индекс

    if (ui.clickSub("date")) {
      // clickName() вернёт полное имя с индексом, например date/1
      // ui.clickNameSub(1) вернёт сам индекс после /
      ui.copyStr(ui.clickName(), dates[ui.clickNameSub(1).toInt()]);
    }
    if (ui.clickSub("time")) {
      ui.copyStr(ui.clickName(), times[ui.clickNameSub(1).toInt()]);
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

  ui.attachBuild(build);
  ui.attach(action);
  ui.start();
}

void loop() {
  ui.tick();
}
