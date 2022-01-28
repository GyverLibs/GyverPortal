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
  String s;
  BUILD_BEGIN(s);
  add.THEME(GP_DARK);
  add.AJAX_UPDATE("led, st");

  add.LABEL("Settings");
  add.BLOCK_BEGIN();
  add.LABEL("Power:");
  add.SWITCH("sw", data.state);
  add.BREAK();
  add.LABEL("Local status:");
  add.LABEL("OK");
  add.BLOCK_END();

  add.FORM_BEGIN("/save");

  add.LABEL("WiFi");
  add.BLOCK_BEGIN();
  add.TEXT("ssid", "SSID", data.ssid);
  add.BREAK();
  add.TEXT("pass", "Password", data.pass);
  add.BLOCK_END();

  add.LABEL("MQTT");
  add.BLOCK_BEGIN();
  add.TEXT("host", "Host", data.host);
  add.BREAK();
  add.NUMBER("port", "Port", data.port);
  add.BLOCK_END();

  add.SUBMIT("Save");
  add.FORM_END();

  BUILD_END();
}

void setup() {
  portal.attachBuild(build);
  portal.start();
}

void loop() {
  portal.tick();
}
