// пример использования "блоков" и сетки для оформления

#define AP_SSID ""
#define AP_PASS ""

#include <GyverPortal.h>
GyverPortal portal;

void build() {
  GP.BUILD_BEGIN(600);
  GP.THEME(GP_DARK);

  // позволяет "отключить" таблицу при ширине экрана меньше 700px
  GP.GRID_RESPONSIVE(700);
  
  GP_MAKE_GRID(
    GP_MAKE_BLOCK_TAB(
      "MOTOR CONFIG",
      GP_MAKE_BOX(GP.LABEL("Velocity"); GP.SLIDER("vel"););
      GP_MAKE_BOX(GP.LABEL("Accel."); GP.SLIDER("acc"););
    );
    GP_MAKE_BLOCK_TAB(
      "MANUAL",
      GP_MAKE_BOX(GP.LABEL("Duty"); GP.SLIDER("spd"););
      GP_MAKE_BOX(GP.BUTTON("bkw", "◄"); GP.BUTTON("frw", "►"););
    );
  );

  GP_MAKE_GRID(
    GP_MAKE_BLOCK_TAB(
      "PID CONFIG",
      GP_MAKE_BOX(GP.LABEL("Kp"); GP.SPINNER("kp"););
      GP_MAKE_BOX(GP.LABEL("Ki"); GP.SPINNER("ki"););
      GP_MAKE_BOX(GP.LABEL("Kd"); GP.SPINNER("kd"););
    );

    GP_MAKE_BLOCK_TAB(
      "SPEED",
      GP.SWITCH("spdOn"); GP.BREAK();
      GP_MAKE_BOX(GP.LABEL("Speed"); GP.SLIDER("spdPid"); );
      GP_MAKE_BOX(GP.BUTTON("bkwPid", "◄"); GP.BUTTON("frwPid", "►"););
    );
  );

  GP_MAKE_BLOCK_TAB(
    "PLANNER",
    GP.SWITCH("planOn"); GP.BREAK();
    GP_MAKE_BOX(GP.LABEL("Target"); GP.SLIDER("tar"););
    GP_MAKE_BOX(GP.LABEL("Stop zone"); GP.SLIDER("deadz"););
  );

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

  portal.attachBuild(build);
  portal.start();
}

void loop() {
  portal.tick();
}
