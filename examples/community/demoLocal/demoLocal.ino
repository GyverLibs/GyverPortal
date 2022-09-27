// локальная работа портала

#define AP_SSID ""
#define AP_PASS ""

#include <GyverPortal.h>

// конструктор страницы
void build(GyverPortal& p) {
  BUILD_BEGIN();
  GP.THEME(GP_DARK);

  GP.BUTTON("clk", "Click");  GP.BREAK();
  GP.BUTTON("ext", "Exit");   GP.BREAK();
//  GP.FILE_UPLOAD("test.txt"); Требует предворительного монтирования файловой системы
  BUILD_END();
}

//Обработчик событий
void action(GyverPortal& p) {
  if (p.click("clk")) Serial.println("Button click");
  if (p.click("ext")) {
    Serial.println("Exit portal");
    p.stop();
  }
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

  GyverPortal portal;
  portal.attachBuild(build);
  portal.attach(action);
  portal.start();
  Serial.println("Portal run");
  while (portal.tick());

  Serial.println("Setup end");
}


void loop() {
}
