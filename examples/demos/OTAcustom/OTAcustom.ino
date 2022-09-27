// кастомные кнопки OTA прошивки

#define AP_SSID ""
#define AP_PASS ""

// подключить библиотеку файловой системы (до #include GyverPortal)
#include <LittleFS.h>

#include <GyverPortal.h>
GyverPortal portal(&LittleFS);  // передать ссылку на fs (SPIFFS/LittleFS)

// конструктор страницы
void build() {
  GP.BUILD_BEGIN();
  GP.THEME(GP_DARK);

  GP.LABEL("v1");

  GP.OTA_FIRMWARE();
  GP.OTA_FILESYSTEM();

  GP.BUILD_END();
}

void setup() {
  startup();

  if (!LittleFS.begin()) Serial.println("FS Error");

  portal.attachBuild(build);
  portal.attach(action);
  portal.start();
  portal.enableOTA();
}

void action() {
}

void loop() {
  portal.tick();
}

void startup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(AP_SSID, AP_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(WiFi.localIP());
}
