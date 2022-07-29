// пример использования "блоков" для оформления
// данные храним, но никак не используем, это просто пример

#define AP_SSID ""
#define AP_PASS ""

#include <GyverPortal.h>
GyverPortal portal;

struct SomeData {
  char ssid[32] = "";
  char pass[32] = "";
  char host[32] = "broker.mqttdashboard.com";
  uint32_t port = 1883;
  bool state;
};

SomeData data;

void build() {
  BUILD_BEGIN();
  GP.THEME(GP_DARK);
  GP.AJAX_UPDATE("led, st");

  GP.LABEL("Settings");
  GP.BLOCK_BEGIN();
  GP.LABEL("Power:");
  GP.SWITCH("sw", data.state);
  GP.BREAK();
  GP.LABEL("Local status:");
  GP.LABEL("OK");
  GP.BLOCK_END();

  GP.FORM_BEGIN("/save");

  GP.LABEL("WiFi");
  GP.BLOCK_BEGIN();
  GP.TEXT("ssid", "SSID", data.ssid);
  GP.BREAK();
  GP.TEXT("pass", "Password", data.pass);
  GP.BLOCK_END();

  GP.LABEL("MQTT");
  GP.BLOCK_BEGIN();
  GP.TEXT("host", "Host", data.host);
  GP.BREAK();
  GP.NUMBER("port", "Port", data.port);
  GP.BLOCK_END();

  GP.SUBMIT("Save");
  GP.FORM_END();

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

  portal.attachBuild(build);
  portal.start();
}

void loop() {
  portal.tick();
}
