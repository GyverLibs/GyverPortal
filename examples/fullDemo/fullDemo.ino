// демка всех компонентов конструктора
// парсим страницу и обновляем значения
#include "GyverPortal.h"

struct Data {
  int val;
  char pass[10];
  bool check;
  CPdate d;
  CPtime t;
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
  add.LABEL("Some text"); add.TEXT("txt", "value", data.val);
  add.BREAK();
  add.PASS("pass", "pass", data.pass);
  add.BREAK();
  add.LABEL("check 1: ");
  add.CHECK("check", data.check);
  add.BREAK();
  add.LABEL("Date"); add.DATE("date", data.d);
  add.BREAK();
  add.LABEL("Time"); add.TIME("time", data.t);
  add.BREAK();
  add.SLIDER("sld", data.sld, 0, 20); add.BREAK();
  add.LABEL("Color"); add.COLOR("col", data.color);
  add.BREAK();
  add.SELECT("sel", "val1,val 2,val 3", data.sel);
  add.BREAK();
  add.SWITCH("sw", data.sw);
  add.BREAK();
  add.BLOCK_BEGIN();
  add.SUBMIT("Submit!");
  add.FORM_END();
  add.FORM_SUBMIT("/exit", "Stop");
  add.BLOCK_END();
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
  portal.attachForm("/action");
  portal.attachForm("/exit");
  portal.start();

  while (portal.tick()) {
    if (portal.action()) {
      if (portal.form("/exit")) {
        String s;
        GP_BUILD(s);
        add.TITLE("Portal stopped");
        
        portal.showPage(s);
        portal.stop();
      }

      if (portal.form("/action")) {
        data.val = portal.getInt("txt");
        portal.copyStr("pass", data.pass);
        data.check = portal.getCheck("check");
        data.d = portal.getDate("date");
        data.t = portal.getTime("time");
        data.sld = portal.getInt("sld");
        data.color = portal.getColor("col");
        data.sel = portal.getSelected("sel", "val 1,val 2,val 3");
        data.sw = portal.getCheck("sw");

        Serial.println(data.val);
        Serial.println(data.pass);
        Serial.println(data.check);
        Serial.print(data.d.year);
        Serial.print(',');
        Serial.print(data.d.month);
        Serial.print(',');
        Serial.println(data.d.day);
        Serial.print(data.t.hour);
        Serial.print(':');
        Serial.println(data.t.minute);
        Serial.println(data.sld);
        Serial.println(data.color, HEX);
        Serial.println(data.sel);
        Serial.println(data.sw);
      }
    }
  }
}

void loop() {
}
