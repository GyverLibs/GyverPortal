// пример организации многостраничности с панелью кнопок-ссылок

#define AP_SSID ""
#define AP_PASS ""

#include <GyverPortal.h>
GyverPortal portal;

// конструктор страницы
void build() {
  GP.BUILD_BEGIN();
  GP.THEME(GP_DARK);

  // portal.uri() возвращает текущий адрес страницы
  // используем его в конструкции с if для постройки страницы
  // переход на страницы может осуществляться в адресной строке браузера
  // или по кнопке-ссылке BUTTON_LINK

  // страница с формой
  // ВАЖНО: url страницы должен совпадать с именем формы!
  // чтобы форма отображалась в браузере после Submit
  GP.NAV_TABS_LINKS("/,/save,/clicks,/updates", "Home,Form,Clicks,Updates");
  
  if (portal.uri("/save")) {
    GP.FORM_BEGIN("/save");
    GP.TEXT("txt", "text", ""); GP.BREAK();
    GP.SUBMIT("Submit");
    GP.FORM_END();

    // страница с кнопкой, на которую можно кликнуть
  } else if (portal.uri("/clicks")) {
    GP.BUTTON("btn", "Button");

    // страница с лампочкой, которая сама переключается
  } else if (portal.uri("/updates")) {
    GP.UPDATE("led");
    GP.LABEL("LED: ");
    GP.LED_RED("led", 0);

    // главная страница, корень, "/"
  } else {
    GP.LABEL("Home");
  }

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

bool led;
void action() {
  if (portal.form("/save")) Serial.println(portal.getString("txt"));
  if (portal.click("btn")) Serial.println("Button");
  if (portal.update("led")) portal.answer(led = !led);
}

void loop() {
  portal.tick();
}
