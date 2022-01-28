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
  String s;
  BUILD_BEGIN(s);
  add.THEME(GP_DARK);

  add.FORM_BEGIN("/login");
  add.TEXT("lg", "Login", lp.ssid);
  add.BREAK();
  add.TEXT("ps", "Password", lp.pass);
  add.SUBMIT("Submit");
  add.FORM_END();

  BUILD_END();
}

void setup() {
  delay(2000);
  Serial.begin(9600);
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
  GyverPortal portal;
  portal.attachBuild(build);
  portal.start(WIFI_AP);

  // работа портала
  while (portal.tick()) {
    if (portal.form("/login")) {      // кнопка нажата
      portal.copyStr("lg", lp.ssid);  // копируем себе
      portal.copyStr("ps", lp.pass);
      EEPROM.put(0, lp);              // сохраняем
      EEPROM.commit();                // записываем
      WiFi.softAPdisconnect();        // отключаем AP
      break;                          // выходим из цикла
    }
  }
}

void loop() {

}
