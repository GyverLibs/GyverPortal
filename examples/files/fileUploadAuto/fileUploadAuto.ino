// автоматическая загрузка файлов
// также включены: автоматическое скачивание/удаление/переименование для работы менеджера файлов

#define AP_SSID ""
#define AP_PASS ""

// подключить библиотеку файловой системы (до #include GyverPortal)
#include <LittleFS.h>

#include <GyverPortal.h>
GyverPortal ui(&LittleFS);  // передать ссылку на fs (SPIFFS/LittleFS)

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

  ui.attachBuild(build);
  ui.attach(action);
  ui.start();
}

void action() {
  // СИГНАЛ об успешном окончании загрузки
  if (ui.uploadEnd()) {
    Serial.print("Uploaded file: ");
    Serial.print(ui.fileName());      // имя файла
    Serial.print(", from: ");
    Serial.println(ui.uploadName());  // имя формы загрузки
    // файл сохранится В КОРЕНЬ, С ИМЕНЕМ fileName()
    // или с сохранением пути вложенных папок
  }
}

void loop() {
  ui.tick();
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
