// почти все компоненты

#define AP_SSID ""
#define AP_PASS ""

#include <GyverPortal.h>
GyverPortal portal;

void build() {
  GP.BUILD_BEGIN();
  GP.THEME(GP_DARK);
  //GP.THEME(GP_LIGHT);

  GP.TITLE("GyverPortal");
  GP.HR();

  GP.NAV_TABS_LINKS("/,/home,/sett,/kek", "Home,Settings,Kek");
  
  GP_MAKE_SPOILER(
    "Spoiler",
    GP.LABEL("Hello!");
  );

  GP_MAKE_BLOCK(
    GP.LABEL("Checks & LED");
    GP.BREAK();
    GP.LABEL_BLOCK("label block");
    GP.LED("");
    GP.CHECK("");
    GP.SWITCH("");
  );

  GP_MAKE_BLOCK_TAB(
    "Block Tab",
    GP.LABEL("Inputs");
    GP_MAKE_BOX(GP.LABEL("Number");   GP.NUMBER("", "", 123);     );
    GP_MAKE_BOX(GP.LABEL("Float");    GP.NUMBER_F("", "", 3.14);  );
    GP_MAKE_BOX(GP.LABEL("Text");     GP.TEXT("", "", "Hello");   );
    GP_MAKE_BOX(GP.LABEL("Password"); GP.PASS("", "", "Pass");    );
    GP.AREA("", 3, "Text area");
  );

  GP_MAKE_BLOCK_THIN(
    GP_MAKE_BOX(GP.LABEL("Date");   GP.DATE("");  );
    GP_MAKE_BOX(GP.LABEL("Time");   GP.TIME("");  );
    GP_MAKE_BOX(GP.LABEL("Color");  GP.COLOR(""); );
  );

  GP_MAKE_BLOCK_THIN_TAB(
    "Thin Tab",
    GP.LABEL("Upload File/Folder");
    GP_MAKE_BOX(
      GP_CENTER,
      GP.FILE_UPLOAD("");
      GP.FOLDER_UPLOAD("");
    );
  );

  GP_MAKE_BOX(GP.LABEL("Select");   GP.SELECT("", "Some,Drop,List");  );
  GP_MAKE_BOX(GP.LABEL("Slider");   GP.SLIDER("");  );
  GP_MAKE_BOX(GP.LABEL("Spinner");  GP.SPINNER(""); );

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
  portal.attachBuild(build);
  portal.attach(action);
  portal.start();
}

void action() {
}

void loop() {
  portal.tick();
}
