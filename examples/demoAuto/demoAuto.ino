// демка всех компонентов конструктора
// парсим страницу и обновляем значения
#include "GyverPortal.h"

struct Data {
  int val;
  char pass[10];
  bool check;
  GPdate date;
  GPtime time;
  int sld;
  uint32_t color;
  int sel;
  bool sw;
};

Data data;

void build() {
  String s;
  BUILD_BEGIN(s);
  add.THEME(GP_DARK);
  add.TITLE("My page");
  add.HR();
  add.FORM_BEGIN("/action");
  add.TEXT("txt", "value", data.val);
  add.BREAK();
  add.PASS("pass", "pass", data.pass);
  add.BREAK();
  add.LABEL("Check: ");
  add.CHECK("check", data.check);
  add.BREAK();
  add.LABEL("Switch: ");
  add.SWITCH("sw", data.sw);
  add.BREAK();
  add.LABEL("Date"); add.DATE("date", data.date);
  add.BREAK();
  add.LABEL("Time"); add.TIME("time", data.time);
  add.BREAK();
  add.SLIDER("sld", data.sld, 0, 20);
  add.BREAK();
  add.LABEL("Color");
  add.COLOR("col", data.color);
  add.BREAK();
  add.SELECT("sel", "val 1,val 2,val 3", data.sel);
  add.BREAK();
  add.SUBMIT("Submit!");
  add.FORM_END();
  BUILD_END();
}

void setup() {
  Serial.begin(9600);
  /*
    WiFi.mode(WIFI_AP);
    WiFi.softAP("ESP Portal");
  */

  WiFi.mode(WIFI_STA);
  WiFi.begin("", "");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(WiFi.localIP());

  GyverPortal portal;
  portal.attachBuild(build);
  portal.start();

  portal.list.init(8);
  portal.list.add(&data.val, "/action", "txt", T_INT16);
  portal.list.add(&data.pass, "pass", T_CSTR);
  portal.list.add(&data.check, "check", T_CHECK);
  portal.list.add(&data.sw, "sw", T_CHECK);
  portal.list.add(&data.date, "date", T_DATE);
  portal.list.add(&data.time, "time", T_TIME);
  portal.list.add(&data.sld, "sld", T_INT16);
  portal.list.add(&data.color, "col", T_COLOR); 

  while (portal.tick()) {
    if (portal.action()) {
      if (portal.form("/action")) {
        // для селекта нет авто-парсера, его вручную
        data.sel = portal.getSelected("sel", "val 1,val 2,val 3");

        // выводим в порт
        Serial.println(data.val);
        Serial.println(data.pass);
        Serial.println(data.check);
        Serial.println(data.sw);
        Serial.print(data.date.year);
        Serial.print(',');
        Serial.print(data.date.month);
        Serial.print(',');
        Serial.println(data.date.day);
        Serial.print(data.time.hour);
        Serial.print(':');
        Serial.println(data.time.minute);
        Serial.println(data.sld);
        Serial.println(data.color, HEX);
        Serial.println(data.sel);        
      }
    }
  }
}

void loop() {
}
