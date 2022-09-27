// Ограничение к пунктам конструктора в зависимости от ip адреса клиента

#define AP_SSID ""
#define AP_PASS ""

#include <GyverPortal.h>
GyverPortal portal;

// конструктор страницы
void build() {
  if (portal.clientFromNet({192,168,88,0},24)){
    BUILD_BEGIN();
    GP.THEME(GP_DARK);
    GP.TITLE("Welcome!");
    GP.LABEL("Your IP: "+portal.clientIP().toString());
    BUILD_END();
  } else {
    BUILD_BEGIN();
    GP.THEME(GP_DARK);
    GP.TITLE("Acceess Denied");
    GP.LABEL("Your IP: "+portal.clientIP().toString());
    BUILD_END();
  }
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
  portal.start();

}

void loop() {
  portal.tick();
}
