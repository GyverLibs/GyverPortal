// пример использования "блоков" и сетки для оформления

#define AP_SSID ""
#define AP_PASS ""

#include <GyverPortal.h>
GyverPortal ui;

void build() {
  GP.BUILD_BEGIN(GP_DARK);

  // создать окно панели управления с кастомным контентом

  GP.UI_MENU("Portal", GP_RED);   // начать меню

  // ссылки меню
  GP.UI_LINK("/", "Home");
  GP.UI_LINK("/settings", "Settings");
  GP.UI_LINK("/update", "Update");

  // кастомный контент
  GP.HR(GP_GRAY);
  GP.LABEL("Some label!");
  GP.BREAK();
  GP.LABEL(ui.getSystemTime().encode());
  GP.BUTTON_MINI("", "Button");

  // начать основное окно
  GP.UI_BODY();

  // далее рандомный интерфейс для теста
  // позволяет "отключить" таблицу при ширине экрана меньше 700px
  GP.GRID_RESPONSIVE(700);

  if (ui.uri("/")) {
    
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
    
  } else if (ui.uri("/settings")) {
    
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
    
  } else if (ui.uri("/update")) {
    
    M_BLOCK_TAB(
      "PLANNER",
      GP.SWITCH("planOn"); GP.BREAK();
      M_BOX(GP.LABEL("Target"); GP.SLIDER("tar"););
      M_BOX(GP.LABEL("Stop zone"); GP.SLIDER("deadz"););
    );
  }
  
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
