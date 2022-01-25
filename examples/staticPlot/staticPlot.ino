#include <GyverPortal.h>

int arr[10][2];
const char *names[] = {"kek", "puk",};

// билдер страницы
void build() {
  String s;
  BUILD_BEGIN(s);
  add.THEME(GP_DARK);

  for (int i = 0; i < 10; i++)
    for (int j = 0; j < 2; j++)
      arr[i][j] = random(100);
      
  //add.PLOT_DARK<2>("table", names, arr, 10);
  add.PLOT_LIGHT<2>("table", names, arr, 10);
  BUILD_END();
}

GyverPortal portal;

void setup() {
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  WiFi.begin("", "");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(WiFi.localIP());

  // подключаем билдер и запускаем
  portal.attachBuild(build);
  portal.start();
}

void loop() {
  portal.tick();
}
