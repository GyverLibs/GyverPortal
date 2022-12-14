// тест иконок

#define AP_SSID ""
#define AP_PASS ""

// подключить библиотеку файловой системы (до #include GyverPortal)
#include <LittleFS.h>

#include <GyverPortal.h>
GyverPortal ui(&LittleFS);

// конструктор страницы
void build() {
  GP.BUILD_BEGIN();
  GP.THEME(GP_DARK);

  // онлайн иконки
  GP.ICON_SUPPORT();
  GP.LABEL(GP.ICON("gear") + "Text");
  GP.BREAK();
  GP.LABEL(GP.ICON("gear", 30) + "Text");
  GP.BREAK();
  GP.LABEL(GP.ICON("gear", 50, GP_RED) + "Text");
  GP.BREAK();
  
  GP.ICON_BUTTON("icn", "gear", 70, GP_GREEN);
  GP.BREAK();

  // оффлайн иконки
  GP.LABEL(GP.ICON_FILE("/gp_data/gear.svg") + "Text");
  GP.BREAK();
  GP.LABEL(GP.ICON_FILE("/gp_data/gear.svg", 30) + "Text");
  GP.BREAK();
  GP.LABEL(GP.ICON_FILE("/gp_data/gear.svg", 50, GP_RED) + "Text");
  GP.BREAK();
  
  GP.ICON_FILE_BUTTON("icnf", "/gp_data/gear.svg", 70, GP_RED);
  GP.BREAK();

  // кнопка с иконкой
  GP.BUTTON("btn", GP.ICON("gear", 30) + "Some Text");

  GP.BUILD_END();
}

void setup() {
  startup();
  ui.attachBuild(build);
  ui.attach(action);
  ui.start();
  LittleFS.begin();
}

void action() {
  if (ui.click()) {
    // вывод имён кликнутых кнопок
    Serial.println(ui.clickName());
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
