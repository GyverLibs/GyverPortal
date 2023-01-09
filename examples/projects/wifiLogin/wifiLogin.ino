// пример с настройкой логина-пароля
// если при запуске нажата кнопка на D2 (к GND)
// открывается точка WiFi Config с формой ввода
// храним настройки в EEPROM

#include <GyverPortal.h>
#include <EEPROM.h>

struct LoginPass {
  char ssid[20];
  char pass[20];
};

LoginPass lp;

void build() {
  GP.BUILD_BEGIN();
  GP.THEME(GP_DARK);

  GP.FORM_BEGIN("/login");
  GP.TEXT("lg", "Login", lp.ssid);
  GP.BREAK();
  GP.TEXT("ps", "Password", lp.pass);
  GP.SUBMIT("Submit");
  GP.FORM_END();

  GP.BUILD_END();
}

void setup() {
  delay(2000);
  Serial.begin(115200);
  Serial.println();

  // читаем логин пароль из памяти
  EEPROM.begin(100);
  EEPROM.get(0, lp);

  // если кнопка нажата - открываем портал
  pinMode(D2, INPUT_PULLUP);
  if (!digitalRead(D2)) loginPortal();

  // пытаемся подключиться
  Serial.print("Connect to: ");
  Serial.println(lp.ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(lp.ssid, lp.pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected! Local IP: ");

  Serial.println(WiFi.localIP());
}

void loginPortal() {
  Serial.println("Portal start");

  // запускаем точку доступа
  WiFi.mode(WIFI_AP);
  WiFi.softAP("WiFi Config");

  // запускаем портал
  GyverPortal ui;
  ui.attachBuild(build);
  ui.start();
  ui.attach(action);

  // работа портала
  while (ui.tick());
}

void action(GyverPortal& p) {
  if (p.form("/login")) {      // кнопка нажата
    p.copyStr("lg", lp.ssid);  // копируем себе
    p.copyStr("ps", lp.pass);
    EEPROM.put(0, lp);              // сохраняем
    EEPROM.commit();                // записываем
    WiFi.softAPdisconnect();        // отключаем AP
  }
}

void loop() {
}
