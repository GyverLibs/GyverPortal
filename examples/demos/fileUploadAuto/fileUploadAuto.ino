// автоматическая загрузка файлов

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

  GP.FILE_UPLOAD("file_upl");    // кнопка загрузки
  GP.FOLDER_UPLOAD("folder_upl");// кнопка загрузки
  GP.FILE_MANAGER(&LittleFS);    // файловый менеджер

  GP.BUILD_END();
}

void setup() {
  startup();

  if (!LittleFS.begin()) Serial.println("FS Error");

  portal.attachBuild(build);
  portal.attach(action);
  portal.uploadAuto(true);  // включить автозагрузку
  portal.deleteAuto(true);  // включить автоудаление
  portal.start();
}

void action() {
  // успешное окончание загрузки
  if (portal.uploadEnd()) {
    Serial.print("Uploaded file: ");
    Serial.print(portal.fileName());      // имя файла
    Serial.print(", from: ");
    Serial.println(portal.uploadName());  // имя формы загрузки
    // файл сохранится В КОРЕНЬ, С ИМЕНЕМ fileName()
  }

  // ===== ФАЙЛОВЫЙ МЕНЕДЖЕР =====
  // обработчик скачивания файлов (для открытия в браузере)
  // отправляем просто по имени, т.к. указана fs
  if (portal.download()) portal.sendFile(portal.uri());
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
