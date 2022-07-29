// AJAX обновление на странице

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

  GP.AJAX_UPDATE("t1,val,ch,led,sw,txt,num,pass,sld,date,time,col");

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
  GP.TEXT("txt", "text", valText);      GP.BREAK();
  GP.NUMBER("num", "number", valNum);   GP.BREAK();
  GP.PASS("pass", "pass", valPass);     GP.BREAK();
  GP.SLIDER("sld", valSlider, 0, 10);   GP.BREAK();
  GP.DATE("date", valDate);       GP.BREAK();
  GP.TIME("time", valTime);       GP.BREAK();
  GP.SELECT("sel", "val 1,val 2,val 3", valSelect);  GP.BREAK();
  GP.COLOR("col", valCol);        GP.BREAK();
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
  // было обновление
  if (portal.update()) {
    // ищем, какой компонент запрашивает обновление
    // и отправляем рандомное значение
    if (portal.update("t1")) portal.answer(random(100));
    if (portal.update("val")) portal.answer(random(1000));
    if (portal.update("ch")) portal.answer(random(2));
    if (portal.update("led")) portal.answer(random(2));
    if (portal.update("sw")) portal.answer(random(2));
    if (portal.update("txt")) portal.answer(random(1000));
    if (portal.update("num")) portal.answer(random(1000));
    if (portal.update("pass")) portal.answer(random(1000));
    if (portal.update("sld")) portal.answer(random(10));

    if (portal.update("date")) {
      valDate = (GPdate) {
        (uint16_t)random(2000, 2030), (uint8_t)random(13), (uint8_t)random(13)
      };
      portal.answer(valDate);
    }

    if (portal.update("time")) {
      valTime = (GPtime) {
        (uint8_t)random(24), (uint8_t)random(60), (uint8_t)random(60)
      };
      portal.answer(valTime);
    }

    // селект не работает

    if (portal.update("col")) {
      valCol.setHEX(random(0xffffff));
      portal.answer(valCol);
    }
  }
}

void loop() {
  portal.tick();
}
