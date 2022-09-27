// работа с компонентами в виде объектов (beta)

#define AP_SSID ""
#define AP_PASS ""

#include <GyverPortal.h>
GyverPortal portal;

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
  BUILD_BEGIN();
  GP.THEME(GP_DARK);
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

  BUILD_END();
}

void action() {
  if (portal.form("/save")) {
    portal.copyObj(num);
    portal.copyObj(numf);
    Serial.println(num.value);
    Serial.println(numf.value);

    portal.copyObj(txt);
    portal.copyObj(pas);
    Serial.println(txt.text);
    Serial.println(pas.text);

    portal.copyObj(ar);
    Serial.println(ar.text);

    portal.copyObj(ch);
    portal.copyObj(sw);
    Serial.println(ch.state);
    Serial.println(sw.state);

    portal.copyObj(dat);
    portal.copyObj(tm);
    Serial.println(dat.date.encode());
    Serial.println(tm.time.encode());

    portal.copyObj(col);
    Serial.println(col.color.encode());

    portal.copyObj(sp);
    Serial.println(sp.value);

    portal.copyObj(sl);
    Serial.println(sl.value);

    portal.copyObj(sel);
    Serial.println(sel.selected);
    Serial.println(sel.getValue());
  }

  if (portal.click()) {
    if (portal.clickUp(btn)) Serial.println("down");
    if (portal.clickDown(btn)) Serial.println("up");
    
    if (portal.clickObj(btn)) Serial.println("btn");
    if (portal.clickObj(btnm)) Serial.println("btn mini");

    if (portal.clickObj(num)) Serial.println(num.value);
    if (portal.clickObj(numf)) Serial.println(numf.value);

    if (portal.clickObj(txt)) Serial.println(txt.text);
    if (portal.clickObj(pas)) Serial.println(pas.text);

    if (portal.clickObj(ar)) Serial.println(ar.text);

    if (portal.clickObj(ch)) Serial.println(ch.state);
    if (portal.clickObj(sw)) Serial.println(sw.state);

    if (portal.clickObj(dat)) Serial.println(dat.date.encode());
    if (portal.clickObj(tm)) Serial.println(tm.time.encode());
    if (portal.clickObj(col)) Serial.println(col.color.encode());

    if (portal.clickObj(sp)) Serial.println(sp.value);
    if (portal.clickObj(sl)) Serial.println(sl.value);

    if (portal.clickObj(sel)) {
      Serial.println(sel.selected);
      Serial.println(sel.getValue());
    }
  }

  if (portal.update()) {
    tit.text = random(10);
    lab.text = random(10);
    labb.text = random(10);
    portal.updateObj(tit);
    portal.updateObj(lab);
    portal.updateObj(labb);

    led1.state = random(2);
    led2.state = random(2);
    led3.state = random(2);
    portal.updateObj(led1);
    portal.updateObj(led2);
    portal.updateObj(led3);

    num.value = random(10);
    numf.value = random(100) / 10.0;
    portal.updateObj(num);
    portal.updateObj(numf);

    txt.text = random(100);
    pas.text = random(100);
    portal.updateObj(txt);
    portal.updateObj(pas);

    ar.text = random(10000);
    portal.updateObj(ar);

    ch.state = random(2);
    sw.state = random(2);
    portal.updateObj(ch);
    portal.updateObj(sw);

    dat.date = (GPdate) {
      (uint16_t)random(2000, 2030), (uint8_t)random(13), (uint8_t)random(13)
    };
    tm.time = (GPtime) {
      (uint8_t)random(24), (uint8_t)random(60), (uint8_t)random(60)
    };
    portal.updateObj(dat);
    portal.updateObj(tm);

    col.color = random(0xffffff);
    portal.updateObj(col);

    sp.value = random(50);
    portal.updateObj(sp);

    sl.value = random(10);
    portal.updateObj(sl);
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
  portal.attachBuild(build);
  portal.attach(action);
  portal.start();
}

void loop() {
  portal.tick();
}
