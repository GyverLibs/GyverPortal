// делаем "сериал монитор" в браузере

#define AP_SSID ""
#define AP_PASS ""

#include <GyverPortal.h>
GyverPortal ui;

// конструктор страницы
void build() {
  GP.BUILD_BEGIN();
  GP.THEME(GP_DARK);

  // кнопка отправляет текст из поля txt
  GP.TEXT("txt", "");
  GP.BUTTON_MINI("btn", "Send", "txt");
  GP.BREAK();

  GP.AREA_LOG(5);

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

  ui.start();
  ui.attachBuild(build);
  ui.attach(action);
  ui.log.start(30);   // размер буфера
}

void action() {
  if (ui.click("btn")) {
    // отправляем обратно в "монитор" содержимое поля, оно пришло по клику кнопки
    ui.log.println(ui.getString("btn"));
  }
}

void loop() {
  ui.tick();
}
