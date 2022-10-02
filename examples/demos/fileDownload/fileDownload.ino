// скачивание файлов

#define AP_SSID ""
#define AP_PASS ""

#include <LittleFS.h>

#include <GyverPortal.h>
GyverPortal portal(&LittleFS);

// конструктор страницы
void build() {
  GP.BUILD_BEGIN();
  GP.THEME(GP_DARK);
 
  GP.EMBED("/test.txt");
  GP.BUTTON_LINK("/test.txt", "Открыть");
  GP.BUTTON_DOWNLOAD("/test.txt", "Скачать");
  
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

  if (!LittleFS.begin()) Serial.println("FS Error");
  
  portal.attachBuild(build);
  portal.attach(action);
  portal.start();
}

void action() {
  if (portal.download()) {
    Serial.println(portal.uri());
    
    // при указанной библиотеке файловой системы
    // можно качать файлы просто по имени
    portal.sendFile(portal.uri());
  }
}

void loop() {
  portal.tick();
}
