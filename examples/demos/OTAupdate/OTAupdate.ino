// использование встроенного OTA update
// зайди на адрес x.x.x.x/ota_update для открытия страницы обновления
// Скетч/Экспорт бинарного файла (для получения файла прошивки)

#define AP_SSID ""
#define AP_PASS ""

#include <LittleFS.h>

#include <GyverPortal.h>
GyverPortal portal(&LittleFS); // для проверки файлов

// конструктор страницы
void build() {
  GP.BUILD_BEGIN();
  GP.THEME(GP_DARK);
  GP.TITLE("Welcome! v3");
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
  portal.attachBuild(build);
  portal.start();
  portal.enableOTA();   // без пароля
  //portal.enableOTA("admin", "pass");  // с паролем

  if (!LittleFS.begin()) Serial.println("FS Error");
  portal.downloadAuto(true);
}

void loop() {
  portal.tick();
}
