// демо с формой, значения компонентов связаны с переменными в скетче

#define AP_SSID ""
#define AP_PASS ""

#include <GyverPortal.h>
GyverPortal ui;

// переменные для получения данных с формы
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
  GP.BUILD_BEGIN();
  GP.THEME(GP_DARK);

  GP.FORM_BEGIN("/update");
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
  
  GP.SUBMIT("Submit");
  GP.FORM_END();

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
  // одна из форм была submit
  if (ui.form()) {
    // проверяем, была ли это форма "/update"
    if (ui.form("/update")) {
      // забираем значения и обновляем переменные
      // 1. получаем и присваиваем вручную
      valCheck = ui.getBool("ch");
      valSwitch = ui.getBool("sw");
      valText = ui.getString("txt");
      valNum = ui.getInt("num");

      // 2. также можно переписать через copyXX
      ui.copyStr("pass", valPass);
      ui.copyFloat("spn", valSpin);
      ui.copyInt("sld", valSlider);
      ui.copyDate("date", valDate);
      ui.copyTime("time", valTime);
      ui.copyColor("col", valCol);
      ui.copyInt("sel", valSelect);
      ui.copyInt("rad", valRad);
      
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
      Serial.print(valSpin);
      Serial.print(',');
      Serial.print(valSlider);
      Serial.print(',');
      Serial.print(valDate.encode());
      Serial.print(',');
      Serial.print(valTime.encode());
      Serial.print(',');
      Serial.print(valCol.encode());
      Serial.print(',');
      Serial.print(valSelect);
      Serial.print(',');
      Serial.println(valRad);
    }
  }
}

void loop() {
  ui.tick();
}
