// пример использования "блоков" для оформления

#define AP_SSID ""
#define AP_PASS ""

#include <GyverPortal.h>
GyverPortal ui;

void build() {
  GP.BUILD_BEGIN(600);
  GP.THEME(GP_DARK);

  GP.BLOCK_BEGIN(GP_DIV);
  GP.LABEL("Block div");
  GP.BLOCK_END();

  GP.BLOCK_BEGIN(GP_DIV, "", "My div txt");
  GP.LABEL("Block div text");
  GP.BLOCK_END();

  GP.BLOCK_BEGIN(GP_TAB);
  GP.LABEL("Block tab");
  GP.BLOCK_END();

  GP.BLOCK_BEGIN(GP_TAB, "", "My tab txt");
  GP.LABEL("Block tab text");
  GP.BLOCK_END();

  GP.BLOCK_BEGIN(GP_TAB, "", "My tab txt red ", GP_RED);
  GP.LABEL("Block tab text red");
  GP.BLOCK_END();

  GP.BLOCK_BEGIN(GP_THIN);
  GP.LABEL("Block thin");
  GP.BLOCK_END();

  GP.BLOCK_BEGIN(GP_THIN, "", "My thin txt");
  GP.LABEL("Block thin text");
  GP.BLOCK_END();

  GP.BLOCK_BEGIN(GP_THIN, "", "My thin txt red", GP_RED);
  GP.LABEL("Block thin text red");
  GP.BLOCK_END();

  // то же самое макросами

  M_BLOCK(
    GP_DIV,
    GP.LABEL("Block div");
  );

  M_BLOCK(
    GP_DIV, "", "My div txt",
    GP.LABEL("Block div text");
  );

  M_BLOCK(
    GP_TAB,
    GP.LABEL("Block tab");
  );

  M_BLOCK(
    GP_TAB, "", "My tab txt",
    GP.LABEL("Block tab text");
  );

  M_BLOCK(
    GP_TAB, "", "My tab txt red ", GP_RED,
    GP.LABEL("Block tab text red");
  );

  M_BLOCK(
    GP_THIN,
    GP.LABEL("Block thin");
  );

  M_BLOCK(
    GP_THIN, "", "My thin txt",
    GP.LABEL("Block thin text");
  );

  M_BLOCK(
    GP_THIN, "", "My thin txt red", GP_RED,
    GP.LABEL("Block thin text red");
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

  ui.attachBuild(build);
  ui.start();
}

void loop() {
  ui.tick();
}
