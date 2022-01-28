// AJAX обновление на странице
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
  
  // список можно засунуть во Flash через PSTR()
  add.AJAX_UPDATE(PSTR("t1,val,ch,led,sw,txt,num,pass,sld,date,time,col"));

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
    if (portal.update("date")) portal.answer(encodeDate(random(2000, 2030), random(13), random(13)));
    if (portal.update("time")) portal.answer(encodeTime(random(24), random(60), random(60)));
    // селект не работает
    if (portal.update("col")) portal.answer(encodeColor(random(0xffffff)));
  }
}
