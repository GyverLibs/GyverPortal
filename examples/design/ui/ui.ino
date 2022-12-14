// пример использования "блоков" и сетки для оформления

#define AP_SSID ""
#define AP_PASS ""

#include <GyverPortal.h>
GyverPortal ui;

void build() {
  GP.BUILD_BEGIN(GP_DARK);

  // создать окно панели управления только с заданными ссылками
  GP.UI_BEGIN("Portal", "/,/settings,/update", "Home,Settings,Update");

  // далее рандомный интерфейс для теста
  
  // "отключить" таблицу при ширине экрана меньше 700px
  GP.GRID_RESPONSIVE(700);

  M_GRID(
    M_BLOCK_TAB(
      "MOTOR CONFIG",
      M_BOX(GP.LABEL("Velocity"); GP.SLIDER("vel", 30););
      M_BOX(GP.LABEL("Accel."); GP.SLIDER("acc"););
    );
    M_BLOCK_TAB(
      "MANUAL",
      M_BOX(GP.LABEL("Duty"); GP.SLIDER("spd"););
      M_BOX(GP.BUTTON("bkw", "◄"); GP.BUTTON("frw", "►"););
    );
  );

  M_GRID(
    M_BLOCK_TAB(
      "PID CONFIG",
      M_BOX(GP.LABEL("Kp"); GP.SPINNER("kp"););
      M_BOX(GP.LABEL("Ki"); GP.SPINNER("ki"););
      M_BOX(GP.LABEL("Kd"); GP.SPINNER("kd"););
    );

    M_BLOCK_TAB(
      "SPEED",
      GP.SWITCH("spdOn"); GP.BREAK();
      M_BOX(GP.LABEL("Speed"); GP.SLIDER("spdPid"); );
      M_BOX(GP.BUTTON("bkwPid", "◄"); GP.BUTTON("frwPid", "►"););
    );
  );

  M_BLOCK_TAB(
    "PLANNER",
    GP.SWITCH("planOn"); GP.BREAK();
    M_BOX(GP.LABEL("Target"); GP.SLIDER("tar"););
    M_BOX(GP.LABEL("Stop zone"); GP.SLIDER("deadz"););
  );

  GP.UI_END();    // завершить окно панели управления <<<<-------------
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

  ui.attachBuild(build);
  ui.start();
}

void loop() {
  ui.tick();
}
