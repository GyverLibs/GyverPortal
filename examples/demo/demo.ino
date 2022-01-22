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
bool m1, m2, m3;

void build2() {
  String s;
  BUILD_BEGIN(s);
  add.THEME(GP_DEFAULT);
  add.FORM_SUBMIT("/menu1", "Menu 1");
  if (m1) {
    add.LABEL("Slider 1"); add.BR();
    add.SLIDER("sld", 5, 0, 10); add.BR();
    add.LABEL("Slider 2"); add.BR();
    add.SLIDER("sld", 5, 0, 10); add.BR();
    add.LABEL("Slider 3"); add.BR();
    add.SLIDER("sld", 5, 0, 10); add.BR();
  }
  add.FORM_SUBMIT("/menu2", "Menu 2");
  if (m2) {
    add.LABEL("Check 1"); add.CHECK("ch"); add.BR();
    add.LABEL("Check 2"); add.CHECK("ch"); add.BR();
    add.LABEL("Check 3"); add.CHECK("ch"); add.BR();
  }
  add.FORM_SUBMIT("/menu3", "Menu 3");
  if (m3) {
    add.LABEL("Time 1"); add.TIME("time"); add.BR();
    add.LABEL("Time 2"); add.TIME("time"); add.BR();
    add.LABEL("Time 3"); add.TIME("time"); add.BR();
  }
  add.FORM_SUBMIT("/exit", "Exit");
  BUILD_END();
}

void build1() {
  String s;
  BUILD_BEGIN(s);
  add.THEME(GP_DARK);
  add.TITLE("My page");
  add.HR();
  add.FORM_BEGIN("/action");
  add.LABEL("Some text"); add.TEXT("txt", "value", data.val);
  add.BR();
  add.PASS("pass", "pass", data.pass);
  add.BR();
  add.LABEL("check 1: ");
  add.CHECK("check", data.check);
  add.BR();
  add.LABEL("Date"); add.DATE("date", data.d);
  add.BR();
  add.LABEL("Time"); add.TIME("time", data.t);
  add.BR();
  add.SLIDER("sld", data.sld, 0, 20); add.BR();
  add.LABEL("Color"); add.COLOR("col", data.color);
  add.BR();
  add.SELECT("sel", "val1,val 2,val 3", data.sel);
  add.BR();
  add.SWITCH("sw", data.sw);
  add.BR();
  add.BLOCK_BEGIN();
  add.SUBMIT("Submit!");
  add.FORM_END();
  add.FORM_SUBMIT("/stop", "Stop");
  add.BLOCK_END();
  BUILD_END();
}

void build() {
  String s;
  BUILD_BEGIN(s);
  add.THEME(GP_DEFAULT);
  add.TITLE("Login Page");
  add.HR();
  add.FORM_BEGIN("/login");                // начать форму №1
  add.TEXT("txt", "Login", "");       // ввод текста
  add.BR();                           // перенос строки
  add.SUBMIT("Submit");               // кнопка формы
  add.FORM_END();                          // завершить форму №1
  add.FORM_SUBMIT("/exit", "Exit");
  BUILD_END();
}

void setup() {
  Serial.begin(9600);
  /*298580
    WiFi.mode(WIFI_AP);
    WiFi.softAP("ESP Portal");
  */

  WiFi.mode(WIFI_STA);
  WiFi.begin("LLlypa2", "lolpass12345");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(WiFi.localIP());

  GyverPortal portal;
  portal.attachBuild(build1);
  portal.start(WIFI_STA);
  portal.attachForm("/action");
  portal.attachForm("/stop");
  portal.attachForm("/menu1");
  portal.attachForm("/menu2");
  portal.attachForm("/menu3");
  portal.attachForm("/exit");

  while (portal.tick()) {
    if (portal.action()) {
      if (portal.form("/exit")) portal.stop();
      /*
            m1 = m2 = m3 = 0;
            if (portal.form("/menu1")) {
              m1 = 1;
            }
            if (portal.form("/menu2")) {
              m2 = 2;
            }
            if (portal.form("/menu3")) {
              m3 = 3;
            }
      */
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
