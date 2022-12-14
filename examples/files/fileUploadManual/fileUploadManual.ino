// ручная загрузка файлов
// также выключаем скачивание/удаление/переименование и сделаем это вручную (для примера)

#define AP_SSID ""
#define AP_PASS ""

// подключить библиотеку файловой системы (до #include GyverPortal)
#include <LittleFS.h>

#include <GyverPortal.h>
GyverPortal ui;

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

  ui.attachBuild(build);
  ui.attach(action);
  ui.start();
  
  ui.uploadAuto(0);  // выключить автозагрузку
  ui.deleteAuto(0);  // выключить автоудаление
  ui.downloadAuto(0);// выключить автоскачивание
  ui.renameAuto(0);  // выключить автопереименование
}

void action() {
  // начало загрузки
  if (ui.upload()) {
    Serial.print("Upload: ");
    Serial.print(ui.fileName());      // имя файла
    Serial.print(", from: ");
    Serial.println(ui.uploadName());  // имя формы загрузки

    // любым способом открыть и передать файл типа File
    // сохраним в корень по имени файла
    ui.saveFile(LittleFS.open('/' + ui.fileName(), "w"));  // в корень, по имени файла
    //ui.saveFile(LittleFS.open('/' + ui.uploadName(), "w"));   // в корень, по имени формы загрузки
    
    // использовать имя формы как каталог, имя файла - как имя файла
    //ui.saveFile(LittleFS.open('/' + ui.uploadName() + '/' + ui.fileName(), "w"));
  }

  // успешное окончание загрузки
  if (ui.uploadEnd()) {
    Serial.print("Uploaded file: ");
    Serial.print(ui.fileName());
    Serial.print(", from: ");
    Serial.println(ui.uploadName());
  }

  // ===== ФАЙЛОВЫЙ МЕНЕДЖЕР =====
  // обработчик скачивания файлов (для открытия в браузере)
  if (ui.download()) ui.sendFile(LittleFS.open(ui.uri(), "r"));

  // обработчик удаления файлов
  if (ui.deleteFile()) {
    LittleFS.remove(ui.deletePath());
    Serial.println(ui.deletePath());
  }
  
  // обработчик переименования файлов
  if (ui.renameFile()) LittleFS.rename(ui.renamePath(), ui.renamePathTo());
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
