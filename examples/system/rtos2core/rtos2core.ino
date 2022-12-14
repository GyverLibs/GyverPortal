/*
 Определяем 2 задачи:
 1. Основной алгоритм программы контроллера (В конкретном варианте флгоритм Blink)
 2. WEB интерфей для мониторинга и управления основным алгоритмом
 Передача информации между двумя функциями очуществляется через глобально определённые переменные
 В void setup() запускаем 2 задачи, по 1 задаче на каждое ядро
 */

#define AP_SSID ""      //Имя WiFi сети
#define AP_PASS ""   //Пароль WiFi сети
#define LedPin 18
#include "GyverPortal.h"
int LedPause=500;             //Перпеменная для обмена данными между задачами
//Определяем 2 задачи
TaskHandle_t Task1, Task2;    //Определяем задачи

//Конструктор WEB страницы
void build(GyverPortal& portal) {
  GP.BUILD_BEGIN(GP_DARK);
  GP.TITLE("BlinkDelayTime", "t1");
  GP.HR();
  GP.SLIDER("sld", LedPause, 100, 2500);
  GP.BUILD_END();
}

//Обработчик событий
void action(GyverPortal& portal) {
  if (ui.click()) {
    if (ui.click("sld")){
      LedPause=ui.getInt("sld");
    }
  }
}

//Функция для запуска на ядре 0 (Blink)
void taskCore0(void * pvParameters) {
  for(;;){
    digitalWrite(LedPin,HIGH);
    delay(LedPause);
    digitalWrite(LedPin,LOW);
    delay(LedPause);
  }
}

//Функция для запуска на ядре 1 (Запускаем GyverPortal локально)
void taskCore1(void * pvParameters) {
  for(;;){
    GyverPortal ui;
    ui.attachBuild(build);
    ui.attach(action);
    ui.start();
    Serial.println("Portal run");
    while (ui.tick());
  }
}


void setup() {
  Serial.begin(115200);
  pinMode(LedPin,OUTPUT);         //Инициализирнуем выход подключенного светодиода
  WiFi.mode(WIFI_STA);
  WiFi.begin(AP_SSID, AP_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    Serial.print(".");
  }
  Serial.println(WiFi.localIP());
  
  xTaskCreatePinnedToCore(taskCore0,"Task1",10000,NULL,1,&Task1,0);   //Запускаем void taskCore0 на 0-вом ядре
  delay(500); 
  xTaskCreatePinnedToCore(taskCore1,"Task2",10000,NULL,1,&Task2,1);   //Запускаем void taskCore1 на 1-вом ядре

}
 
void loop() {

}
