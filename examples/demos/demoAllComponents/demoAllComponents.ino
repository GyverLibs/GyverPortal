// почти все компоненты

#define AP_SSID ""
#define AP_PASS ""

#include <GyverPortal.h>
GyverPortal ui;

void action();

void build() {
  GP.BUILD_BEGIN();
  GP.THEME(GP_DARK);
  //GP.THEME(GP_LIGHT);

  GP.TITLE("GyverPortal");
  GP.HR();

  GP.NAV_TABS_LINKS("/,/home,/sett,/kek", "Home,Settings,Kek");
  
  M_SPOILER(
    "Spoiler",
    GP.LABEL("Hello!");
  );

  M_BLOCK(
    GP.LABEL("Checks & LED");
    GP.BREAK();
    GP.LABEL_BLOCK("label block");
    GP.LED("");
    GP.CHECK("");
    GP.SWITCH("");
  );

  M_BLOCK_TAB(
    "Block Tab",
    GP.LABEL("Inputs");
    M_BOX(GP.LABEL("Number");   GP.NUMBER("", "", 123);     );
    M_BOX(GP.LABEL("Float");    GP.NUMBER_F("", "", 3.14);  );
    M_BOX(GP.LABEL("Text");     GP.TEXT("", "", "Hello");   );
    M_BOX(GP.LABEL("Password"); GP.PASS("", "", "Pass");    );
    GP.AREA("", 3, "Text area");
  );

  M_BLOCK_THIN(
    M_BOX(GP.LABEL("Date");   GP.DATE("");  );
    M_BOX(GP.LABEL("Time");   GP.TIME("");  );
    M_BOX(GP.LABEL("Color");  GP.COLOR(""); );
  );

  M_BLOCK_THIN_TAB(
    "Thin Tab",
    GP.LABEL("Upload File/Folder");
    M_BOX(
      GP_CENTER,
      GP.FILE_UPLOAD("");
      GP.FOLDER_UPLOAD("");
    );
  );

  M_BOX(GP.LABEL("Select");   GP.SELECT("", "Some,Drop,List");  );
  M_BOX(GP.LABEL("Slider");   GP.SLIDER("");  );
  M_BOX(GP.LABEL("Spinner");  GP.SPINNER(""); );

  GP.BUTTON("", "Button");
  GP.BUTTON_MINI("", "Btn Mini");

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
  ui.attachBuild(build);
  ui.attach(action);
  ui.start();
}

void action() {
}

void loop() {
  ui.tick();
}
