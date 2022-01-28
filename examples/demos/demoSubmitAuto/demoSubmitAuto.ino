// демо с формой, значения компонентов связаны
// с переменными в скетче
#include <GyverPortal.h>

// переменные
bool valCheck;
bool valSwitch;
String valText;
int valNum;
char valPass[10];
int valSlider;
GPdate valDate;
GPtime valTime;
int valSelect;
uint32_t valCol;

// билдер страницы
void build() {
  String s;
  BUILD_BEGIN(s);
  add.THEME(GP_DARK);

  add.FORM_BEGIN("/update");
  add.TITLE("Title", "t1");
  add.HR();
  add.LABEL("Value: ");
  add.LABEL("NAN", "val");      add.BREAK();
  add.LABEL("Check: ");
  add.CHECK("ch", valCheck);    add.BREAK();
  add.LABEL("Led: ");
  add.LED_RED("led");           add.BREAK();
  add.LABEL("Switch: ");
  add.SWITCH("sw", valSwitch);  add.BREAK();
  add.TEXT("txt", "text", valText);      add.BREAK();
  add.NUMBER("num", "number", valNum);  add.BREAK();
  add.PASS("pass", "pass", valPass);     add.BREAK();
  add.SLIDER("sld", valSlider, 0, 10);  add.BREAK();
  add.DATE("date", valDate);       add.BREAK();
  add.TIME("time", valTime);       add.BREAK();
  add.SELECT("sel", "val 1,val 2,val 3", valSelect);  add.BREAK();
  add.COLOR("col", valCol);     add.BREAK();
  add.BUTTON("btn", "Button");
  add.SUBMIT("Submit");
  add.FORM_END();

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
  portal.list.init(9);
  portal.list.add(&valCheck, "ch", T_CHECK);
  portal.list.add(&valSwitch, "sw", T_CHECK);
  portal.list.add(&valText, "txt", T_STRING);
  portal.list.add(&valNum, "num", T_INT);
  portal.list.add(&valPass, "pass", T_CSTR);
  portal.list.add(&valSlider, "sld", T_INT);
  portal.list.add(&valDate, "date", T_DATE);
  portal.list.add(&valTime, "time", T_TIME);
  //select нету
  portal.list.add(&valCol, "col", T_COLOR);
}

void loop() {
  portal.tick();

  // одна из форм была submit
  if (portal.form()) {

    // проверяем, была ли это форма "/update"
    if (portal.form("/update")) {
      valSelect = portal.getSelected("sel", "val 1,val 2,val 3");

      // выводим для отладки
      Serial.print(valCheck);
      Serial.print(',');
      Serial.print(valSwitch);
      Serial.print(',');
      Serial.print(valText);
      Serial.print(',');
      Serial.print(valNum);
      Serial.print(',');
      Serial.print(valPass);
      Serial.print(',');
      Serial.print(valSlider);
      Serial.print(',');
      Serial.print(encodeDate(valDate));
      Serial.print(',');
      Serial.print(encodeTime(valTime));
      Serial.print(',');
      Serial.print(valSelect);
      Serial.print(',');
      Serial.println(valCol, HEX);
    }
  }
}
