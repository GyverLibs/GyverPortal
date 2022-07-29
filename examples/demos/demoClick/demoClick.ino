// получаем AJAX клики со страницы и значения с компонентов

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
  GP.TEXT("txt", "text");      GP.BREAK();
  GP.NUMBER("num", "number");  GP.BREAK();
  GP.PASS("pass", "pass");     GP.BREAK();
  GP.SLIDER("sld", valSlider, 0, 10);  GP.BREAK();
  GP.DATE("date", valDate);    GP.BREAK();
  GP.TIME("time", valTime);    GP.BREAK();
  GP.SELECT("sel", "val 1,val 2,val 3", valSelect);  GP.BREAK();
  GP.COLOR("col", valCol);     GP.BREAK();
  GP.BUTTON("btn", "Button");

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
}

void action() {
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
      Serial.println(valDate.encode());
    }

    if (portal.click("time")) {
      valTime = portal.getTime("time");
      Serial.print("Time: ");
      Serial.println(valTime.encode());
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
      Serial.println(valCol.encode());
    }

    if (portal.click("btn")) Serial.println("Button");
  }
}

void loop() {
  portal.tick();
}
