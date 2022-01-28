// получаем AJAX клики со страницы и значения с компонентов
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
  add.TEXT("txt", "text");      add.BREAK();
  add.NUMBER("num", "number");  add.BREAK();
  add.PASS("pass", "pass");     add.BREAK();
  add.SLIDER("sld", valSlider, 0, 10);  add.BREAK();
  add.DATE("date", valDate);       add.BREAK();
  add.TIME("time", valTime);       add.BREAK();
  add.SELECT("sel", "val 1,val 2,val 3", valSelect);  add.BREAK();
  add.COLOR("col", valCol);     add.BREAK();
  add.BUTTON("btn", "Button");

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
}

void loop() {
  portal.tick();

  // был клик по компоненту
  if (portal.click()) {
    // проверяем компоненты и обновляем переменные
    if (portal.click("ch")) {
      valCheck = portal.getCheck("ch");
      Serial.print("Check: ");
      Serial.println(valCheck);
    }

    if (portal.click("sw")) {
      valSwitch = portal.getCheck("sw");
      Serial.print("Switch: ");
      Serial.println(valSwitch);
    }

    if (portal.click("txt")) {
      valText = portal.getString("txt");
      Serial.print("Text: ");
      Serial.println(valText);
    }

    if (portal.click("num")) {
      valNum = portal.getInt("num");
      Serial.print("Number: ");
      Serial.println(valNum);
    }

    if (portal.click("pass")) {
      portal.copyStr("pass", valPass);
      Serial.print("Password: ");
      Serial.println(valPass);
    }

    if (portal.click("sld")) {
      valSlider = portal.getInt("sld");
      Serial.print("Slider: ");
      Serial.println(valSlider);
    }

    if (portal.click("date")) {
      valDate = portal.getDate("date");
      Serial.print("Date: ");
      Serial.println(encodeDate(valDate));
    }

    if (portal.click("time")) {
      valTime = portal.getTime("time");
      Serial.print("Time: ");
      Serial.println(encodeTime(valTime));
    }

    if (portal.click("sel")) {
      valSelect = portal.getSelected("sel", "val 1,val 2,val 3");
      Serial.print("Select: ");
      Serial.print(portal.getString("sel"));
      Serial.print(", index: ");
      Serial.println(valSelect);
    }

    if (portal.click("col")) {
      valCol = portal.getColor("col");
      Serial.print("Color: ");
      Serial.println(valCol, HEX);
    }

    if (portal.click("btn")) Serial.println("Button");
  }
}
