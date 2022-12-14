// демо с авторизацией

#define AP_SSID ""
#define AP_PASS ""

#include <GyverPortal.h>
GyverPortal ui;

// конструктор страницы
void build() {
  GP.BUILD_BEGIN();
  GP.THEME(GP_DARK);
  GP.TITLE("Welcome!");
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
  ui.attachBuild(build);
  ui.start();
  
  // включить авторизацию
  ui.enableAuth("admin", "pass");
}

void loop() {
  ui.tick();
}
