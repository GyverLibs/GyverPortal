// демо с формой, значения компонентов связаны с переменными в скетче

#define AP_SSID ""
#define AP_PASS ""

#include <GyverPortal.h>
GyverPortal portal;

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
  portal.attachBuild(build);
  portal.attach(action);
  portal.start();
}

void action() {
  // одна из форм была submit
  if (portal.form()) {
    // проверяем, была ли это форма "/update"
    if (portal.form("/update")) {
      // забираем значения и обновляем переменные
      // 1. получаем и присваиваем вручную
      valCheck = portal.getBool("ch");
      valSwitch = portal.getBool("sw");
      valText = portal.getString("txt");
      valNum = portal.getInt("num");

      // 2. также можно переписать через copyXX
      portal.copyStr("pass", valPass);
      portal.copyFloat("spn", valSpin);
      portal.copyInt("sld", valSlider);
      portal.copyDate("date", valDate);
      portal.copyTime("time", valTime);
      portal.copyColor("col", valCol);
      portal.copyInt("sel", valSelect);
      
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
      Serial.println(valSelect);
    }
  }
}

void loop() {
  portal.tick();
}
