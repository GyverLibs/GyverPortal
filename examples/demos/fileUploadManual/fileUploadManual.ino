// ручная загрузка файлов

#define AP_SSID ""
#define AP_PASS ""

// подключить библиотеку файловой системы (до #include GyverPortal)
#include <LittleFS.h>

#include <GyverPortal.h>
GyverPortal portal;

// конструктор страницы
void build() {
  GP.BUILD_BEGIN();
  GP.THEME(GP_DARK);

  GP.FILE_UPLOAD("my_file.txt"); // кнопка загрузки
  GP.FILE_UPLOAD("file_upl");    // кнопка загрузки
  
  GP.FILE_MANAGER(&LittleFS);    // файловый менеджер
  
  GP.BUILD_END();
}

void setup() {
  startup();

  if (!LittleFS.begin()) Serial.println("FS Error");

  portal.attachBuild(build);
  portal.attach(action);
  portal.start();
}

void action() {
  // начало загрузки
  if (portal.upload()) {
    Serial.print("Upload: ");
    Serial.print(portal.fileName());      // имя файла
    Serial.print(", from: ");
    Serial.println(portal.uploadName());  // имя формы загрузки

    // любым способом открыть и передать файл типа File
    // сохраним в корень по имени файла
    portal.saveFile(LittleFS.open('/' + portal.fileName(), "w"));  // в корень, по имени файла
    //portal.saveFile(LittleFS.open('/' + portal.uploadName(), "w"));   // в корень, по имени формы загрузки
    
    // использовать имя формы как каталог, имя файла - как имя файла
    //portal.saveFile(LittleFS.open('/' + portal.uploadName() + '/' + portal.fileName(), "w"));
  }

  // успешное окончание загрузки
  if (portal.uploadEnd()) {
    Serial.print("Uploaded file: ");
    Serial.print(portal.fileName());
    Serial.print(", from: ");
    Serial.println(portal.uploadName());
  }

  // ===== ФАЙЛОВЫЙ МЕНЕДЖЕР =====
  // обработчик скачивания файлов (для открытия в браузере)
  if (portal.download()) portal.sendFile(LittleFS.open(portal.uri(), "r"));

  // обработчик удаления файлов
  if (portal.deleteFile()) LittleFS.remove(portal.deletePath());
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
