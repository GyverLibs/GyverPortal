// делаем "сериал монитор" в браузере

#define AP_SSID ""
#define AP_PASS ""

#include <GyverPortal.h>
GyverPortal portal;

// конструктор страницы
void build() {
  BUILD_BEGIN();
  GP.THEME(GP_DARK);

  // кнопка отправляет текст из поля txt
  GP.TEXT("txt", "");
  GP.BUTTON_MINI("btn", "Send", "txt");
  GP.BREAK();

  GP.AREA_LOG(5);

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

  portal.start();
  portal.attachBuild(build);
  portal.attach(action);
  portal.log.start(30);   // размер буфера
}

void action() {
  if (portal.click("btn")) {
    // отправляем обратно в "монитор" содержимое поля, оно пришло по клику кнопки
    portal.log.println(portal.getString("btn"));
  }
}

void loop() {
  portal.tick();
}
