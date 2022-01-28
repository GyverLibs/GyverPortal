// делаем "сериал монитор" в браузере

#include <GyverPortal.h>
GyverPortal portal;

// билдер страницы
void build() {
  String s;
  BUILD_BEGIN(s);
  add.THEME(GP_DARK);

  // кнопка отправляет текст из поля txt
  add.TEXT("txt", "");
  add.BUTTON_MINI("btn", "Send", "txt");
  add.BREAK();
  
  add.AREA_LOG(5);
  
  BUILD_END();
}

void setup() {
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  WiFi.begin("", "");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(WiFi.localIP());

  portal.attachBuild(build);
  portal.start();
  portal.log.start(30);   // размер буфера
}

void loop() {
  portal.tick();
  if (portal.click("btn")) {
    // отправляем содержимое поля, оно пришло по клику кнопки
    portal.log.println(portal.getString("btn"));
  }
}
