// AJAX обновление на странице

#define AP_SSID ""
#define AP_PASS ""

#include <GyverPortal.h>
GyverPortal portal;

// переменные
int valNum;
String valPass;
int valSlider;
float valSpin;
GPdate valDate;
GPtime valTime;
GPcolor valCol;
int valSelect;

// конструктор страницы
void build() {
  GP.BUILD_BEGIN();
  GP.THEME(GP_DARK);

  // список имён компонентов на обновление
  GP.UPDATE("t1,lb,lbb,ch,led,sw,txt,num,pass,sld,date,time,spn,col,sel,seli");

  // обновление случайным числом
  GP.TITLE("Title", "t1");
  GP.HR();
  GP.LABEL("Label: ");
  GP.LABEL("NAN", "lb");        GP.BREAK();
  GP.LABEL("Label Block: ");
  GP.LABEL_BLOCK("NAN", "lbb"); GP.BREAK();
  GP.LABEL("Check: ");
  GP.CHECK("ch");         GP.BREAK();
  GP.LABEL("Led: ");
  GP.LED("led");          GP.BREAK();
  GP.LABEL("Switch: ");
  GP.SWITCH("sw");        GP.BREAK();
  GP.TEXT("txt", "text"); GP.BREAK();

  // обновление из переменной (храним значение)
  GP.NUMBER("num", "number", valNum);   GP.BREAK();
  GP.PASS("pass", "pass", valPass);     GP.BREAK();
  GP.SPINNER("spn");
  GP.SLIDER("sld", valSlider, 0, 10);   GP.BREAK();
  GP.DATE("date", valDate);             GP.BREAK();
  GP.TIME("time", valTime);             GP.BREAK();
  GP.COLOR("col", valCol);              GP.BREAK();
  GP.SELECT("sel", "val 1,val 2,val 3", valSelect);  GP.BREAK();

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
  // было обновление
  if (portal.update()) {
    // 1. ищем, какой компонент запрашивает обновление
    // и вручную отправляем рандомное значение
    if (portal.update("t1")) portal.answer(random(100));
    if (portal.update("lb")) portal.answer(random(1000));
    if (portal.update("lbb")) portal.answer(random(1000));
    if (portal.update("ch")) portal.answer(random(2));
    if (portal.update("led")) portal.answer(random(2));
    if (portal.update("sw")) portal.answer(random(2));
    if (portal.update("txt")) portal.answer(random(1000));

    // 2. автоматическое обновление из переменной
    portal.updateInt("num", valNum);
    portal.updateString("pass", valPass);
    portal.updateInt("spn", valSpin);
    portal.updateInt("sld", valSlider);
    portal.updateDate("date", valDate);
    portal.updateTime("time", valTime);
    portal.updateColor("col", valCol);
    portal.updateInt("sel", valSelect);
  }
}

void loop() {
  portal.tick();

  // имитируем изменение переменных "откуда то из программы"
  static uint32_t tmr;
  if (millis() - tmr >= 500) {
    tmr = millis();
    valNum = random(1000);
    valPass = random(1000);
    valSlider = random(10);
    valSpin = random(50);
    valDate.set(random(2000, 2030), random(13), random(13));
    valTime.set(random(24), random(60), random(60));
    valCol.setHEX(random(0xffffff));
    valSelect = random(3);
  }
}
