// работа с компонентами в виде объектов (beta)

#define AP_SSID ""
#define AP_PASS ""

#include <GyverPortal.h>
GyverPortal ui;

GP_TITLE tit("t1");
GP_LABEL lab("l1");
GP_LABEL_BLOCK labb("lb1");

GP_LED led1("led1");
GP_LED_RED led2("led2");
GP_LED_GREEN led3("led3");

GP_BUTTON btn("b1");
GP_BUTTON_MINI btnm("b2");

GP_NUMBER num("num");
GP_NUMBER_F numf("numf");

GP_TEXT txt("txt");
GP_PASS pas("pas");

GP_AREA ar("ar");

GP_CHECK ch("ch");
GP_SWITCH sw("sw");

GP_DATE dat("dat");
GP_TIME tm("tm");
GP_COLOR col("col");

GP_SPINNER sp("sp");
GP_SLIDER sl("sl");

GP_SELECT sel("sel");

void build() {
  GP.BUILD_BEGIN(GP_DARK);
  //GP.UPDATE("t1,l1,lb1,led1,led2,led3,num,numf,txt,pas,ar,ch,sw,dat,tm,col,sp,sl");

  GP.TITLE(tit);
  GP.LABEL(lab);
  GP.LABEL_BLOCK(labb);

  GP.LED(led1);
  GP.LED_RED(led2);
  GP.LED_GREEN(led3);

  GP.BUTTON(btn);
  GP.BUTTON_MINI(btnm);

  GP.FORM_BEGIN("/save");
  GP.NUMBER(num);
  GP.NUMBER_F(numf);

  GP.TEXT(txt);
  GP.PASS(pas);

  GP.AREA(ar);  GP.BREAK();

  GP.CHECK(ch);
  GP.SWITCH(sw);

  GP.DATE(dat);
  GP.TIME(tm);
  GP.COLOR(col);

  GP.SPINNER(sp);
  GP.SLIDER(sl);

  GP.SELECT(sel);

  GP.SUBMIT("Submit");
  GP.FORM_END();

  GP.BUILD_END();
}

void action() {
  if (ui.form("/save")) {
    ui.copy(num);
    ui.copy(numf);
    Serial.println(num.value);
    Serial.println(numf.value);

    ui.copy(txt);
    ui.copy(pas);
    Serial.println(txt.text);
    Serial.println(pas.text);

    ui.copy(ar);
    Serial.println(ar.text);

    ui.copy(ch);
    ui.copy(sw);
    Serial.println(ch.state);
    Serial.println(sw.state);

    ui.copy(dat);
    ui.copy(tm);
    Serial.println(dat.date.encode());
    Serial.println(tm.time.encode());

    ui.copy(col);
    Serial.println(col.color.encode());

    ui.copy(sp);
    Serial.println(sp.value);

    ui.copy(sl);
    Serial.println(sl.value);

    ui.copy(sel);
    Serial.println(sel.selected);
    Serial.println(sel.getValue());
  }

  if (ui.click()) {
    if (ui.clickUp(btn)) Serial.println("down");
    if (ui.clickDown(btn)) Serial.println("up");
    
    if (ui.click(btn)) Serial.println("btn");
    if (ui.click(btnm)) Serial.println("btn mini");

    if (ui.click(num)) Serial.println(num.value);
    if (ui.click(numf)) Serial.println(numf.value);

    if (ui.click(txt)) Serial.println(txt.text);
    if (ui.click(pas)) Serial.println(pas.text);

    if (ui.click(ar)) Serial.println(ar.text);

    if (ui.click(ch)) Serial.println(ch.state);
    if (ui.click(sw)) Serial.println(sw.state);

    if (ui.click(dat)) Serial.println(dat.date.encode());
    if (ui.click(tm)) Serial.println(tm.time.encode());
    if (ui.click(col)) Serial.println(col.color.encode());

    if (ui.click(sp)) Serial.println(sp.value);
    if (ui.click(sl)) Serial.println(sl.value);

    if (ui.click(sel)) {
      Serial.println(sel.selected);
      Serial.println(sel.getValue());
    }
  }

  if (ui.update()) {
    tit.text = random(10);
    lab.text = random(10);
    labb.text = random(10);
    ui.update(tit);
    ui.update(lab);
    ui.update(labb);

    led1.state = random(2);
    led2.state = random(2);
    led3.state = random(2);
    ui.update(led1);
    ui.update(led2);
    ui.update(led3);

    num.value = random(10);
    numf.value = random(100) / 10.0;
    ui.update(num);
    ui.update(numf);

    txt.text = random(100);
    pas.text = random(100);
    ui.update(txt);
    ui.update(pas);

    ar.text = random(10000);
    ui.update(ar);

    ch.state = random(2);
    sw.state = random(2);
    ui.update(ch);
    ui.update(sw);

    dat.date = (GPdate) {
      (uint16_t)random(2000, 2030), (uint8_t)random(13), (uint8_t)random(13)
    };
    tm.time = (GPtime) {
      (uint8_t)random(24), (uint8_t)random(60), (uint8_t)random(60)
    };
    ui.update(dat);
    ui.update(tm);

    col.color = random(0xffffff);
    ui.update(col);

    sp.value = random(50);
    ui.update(sp);

    sl.value = random(10);
    ui.update(sl);
  }
}

void setup() {
  btn.text = "button 1";
  btnm.text = "button 2";
  numf.decimals = 3;
  sp.decimals = 0;
  sl.decimals = 2;
  sl.step = 0.5;

  sel.list = "val 1,val 2,val 3";

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

void loop() {
  ui.tick();
}
