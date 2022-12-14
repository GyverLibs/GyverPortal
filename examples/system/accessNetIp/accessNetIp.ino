// Ограничение к пунктам конструктора в зависимости от ip адреса клиента

#define AP_SSID ""
#define AP_PASS ""

#include <GyverPortal.h>
GyverPortal ui;

// адрес сети
IPAddress net_ip = IPAddress(192, 168, 1, 1);

// конструктор страницы
void build() {
  if (ui.clientFromNet(net_ip, 24)) {
    GP.BUILD_BEGIN(GP_DARK);
    GP.TITLE("Welcome!");
    GP.LABEL("Your IP: " + ui.clientIP().toString());
    GP.BUILD_END();
  } else {
    GP.BUILD_BEGIN(GP_DARK);
    GP.TITLE("Access Denied");
    GP.LABEL("Your IP: " + ui.clientIP().toString());
    GP.BUILD_END();
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
  ui.attachBuild(build);
  ui.start();

}

void loop() {
  ui.tick();
}
