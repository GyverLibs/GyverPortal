// демо с формой, значения компонентов связаны
// с переменными в скетче

#define AP_SSID ""
#define AP_PASS ""

#include <GyverPortal.h>
GyverPortal portal;

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
GPcolor valCol;

// конструктор страницы
void build() {
  BUILD_BEGIN();
  GP.THEME(GP_DARK);

  GP.FORM_BEGIN("/update");
  GP.TITLE("Title", "t1");
  GP.HR();
  GP.LABEL("Value: ");
  GP.LABEL("NAN", "val");      GP.BREAK();
  GP.LABEL("Check: ");
  GP.CHECK("ch", valCheck);    GP.BREAK();
  GP.LABEL("Led: ");
  GP.LED_RED("led");           GP.BREAK();
  GP.LABEL("Switch: ");
  GP.SWITCH("sw", valSwitch);  GP.BREAK();
  GP.TEXT("txt", "text", valText);     GP.BREAK();
  GP.NUMBER("num", "number", valNum);  GP.BREAK();
  GP.PASS("pass", "pass", valPass);    GP.BREAK();
  GP.SLIDER("sld", valSlider, 0, 10);  GP.BREAK();
  GP.DATE("date", valDate);       GP.BREAK();
  GP.TIME("time", valTime);       GP.BREAK();
  GP.SELECT("sel", "val 1,val 2,val 3", valSelect);  GP.BREAK();
  GP.COLOR("col", valCol);        GP.BREAK();
  GP.BUTTON("btn", "Button");     GP.BREAK();
  GP.SUBMIT("Submit");
  GP.FORM_END();

  BUILD_END();
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
  portal.attachBuild(build);
  portal.attach(action);
  portal.start();

  // инициализация авто списка
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

void action() {
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
      Serial.print(valDate.encode());
      Serial.print(',');
      Serial.print(valTime.encode());
      Serial.print(',');
      Serial.print(valSelect);
      Serial.print(',');
      Serial.println(valCol.encode());
    }
  }
}

void loop() {
  portal.tick();
}
