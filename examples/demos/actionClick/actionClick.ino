// получаем клики со страницы и значения с компонентов

#define AP_SSID ""
#define AP_PASS ""

#include <GyverPortal.h>
GyverPortal ui;

// переменные
bool valCheck;
bool valSwitch;
String valText;
int valNum;
char valPass[10];
float valSpin;
int valSlider;
GPdate valDate;
GPtime valTime;
GPcolor valCol;
int valSelect;
int valRad;

// конструктор страницы
void build() {
  GP.BUILD_BEGIN(GP_DARK);

  GP.TITLE("Title", "t1");
  GP.HR();

  GP.LABEL("Value: ");
  GP.LABEL("NAN", "val");       GP.BREAK();
  GP.LABEL("Check: ");
  GP.CHECK("ch", valCheck);     GP.BREAK();
  GP.LABEL("Switch: ");
  GP.SWITCH("sw", valSwitch);   GP.BREAK();
  GP.TEXT("txt", "text", valText);    GP.BREAK();
  GP.NUMBER("num", "number", valNum); GP.BREAK();
  GP.PASS("pass", "pass", valPass);   GP.BREAK();
  GP.SPINNER("spn", valSpin);
  GP.SLIDER("sld", valSlider, 0, 10); GP.BREAK();
  GP.DATE("date", valDate);     GP.BREAK();
  GP.TIME("time", valTime);     GP.BREAK();
  GP.COLOR("col", valCol);      GP.BREAK();
  GP.SELECT("sel", "val 1,val 2,val 3", valSelect);  GP.BREAK();
  GP.RADIO("rad", 0, valRad); GP.LABEL("Value 0"); GP.BREAK();
  GP.RADIO("rad", 1, valRad); GP.LABEL("Value 1"); GP.BREAK();
  GP.RADIO("rad", 2, valRad); GP.LABEL("Value 2"); GP.BREAK();
  GP.RADIO("rad", 3, valRad); GP.LABEL("Value 3"); GP.BREAK();
  GP.BREAK();
  GP.BUTTON("btn", "Button");

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

  // подключаем конструктор и запускаем
  ui.attachBuild(build);
  ui.attach(action);
  ui.start();
}

void action() {
  // был клик по компоненту
  if (ui.click()) {
    // проверяем компоненты и обновляем переменные

    // 1. переписали вручную
    if (ui.click("ch")) {
      valCheck = ui.getBool("ch");
      Serial.print("Check: ");
      Serial.println(valCheck);
    }

    // 2. автоматическое обновление переменной
    if (ui.clickBool("sw", valSwitch)) {
      Serial.print("Switch: ");
      Serial.println(valSwitch);
    }

    if (ui.clickString("txt", valText)) {
      Serial.print("Text: ");
      Serial.println(valText);
    }

    if (ui.clickInt("num", valNum)) {
      Serial.print("Number: ");
      Serial.println(valNum);
    }

    if (ui.clickStr("pass", valPass)) {
      Serial.print("Password: ");
      Serial.println(valPass);
    }

    if (ui.clickFloat("spn", valSpin)) {
      Serial.print("Spinner: ");
      Serial.println(valSpin);
    }

    if (ui.clickInt("sld", valSlider)) {
      Serial.print("Slider: ");
      Serial.println(valSlider);
    }

    if (ui.clickDate("date", valDate)) {
      Serial.print("Date: ");
      Serial.println(valDate.encode());
    }

    if (ui.clickTime("time", valTime)) {
      Serial.print("Time: ");
      Serial.println(valTime.encode());
    }

    if (ui.clickColor("col", valCol)) {
      Serial.print("Color: ");
      Serial.println(valCol.encode());
    }

    if (ui.clickInt("sel", valSelect)) {
      Serial.print("Select: ");
      Serial.println(valSelect);
    }
    if (ui.clickInt("rad", valRad)) {
      Serial.print("Radio: ");
      Serial.println(valRad);
    }

    if (ui.click("btn")) Serial.println("Button click");
  }
}

void loop() {
  ui.tick();
}
