/*
	Простой конструктор веб интерфейса для esp8266 и ESP32
	Документация:
	GitHub: https://github.com/GyverLibs/GyverPortal
	Возможности:
	- Простой конструктор - делаем страницы без знаний HTML и CSS
	- Библиотека является обёрткой для стандартной ESP8266WebServer
	- Позволяет быстро создать вебморду для управления и настройки своего девайса
	- Компактный читаемый код в "скетче", никаких внешних обработчиков и лямбда-функций
	- Конструктор использует стандартные HTML формы, CSS и javascript
	- Элементы конструктора хранятся во Flash памяти
	- Никаких глобальных буферов, всё генерируется на лету
	- Приятный дизайн из коробки + тёмная тема
	- Адаптировано под мобильные устройства и ПК
	- Встроенные инструменты для удобного парсинга значений с формы
	- Возможность настроить автоматическое обновление значений переменных по действию со страницы
	- Встроенные жабаскрипты для AJAX, работа без обновления всей страницы

	v1.0 - релиз
	v1.1 - улучшил графики и стили
	v1.2
	- Блок NUMBER теперь тип number
	- Добавил большое текстовое поле AREA
	- Добавил GPunix
	- Улучшил парсинг
	- Добавил BUTTON_MINI
	- Кнопки могут передавать данные с других компонентов (кроме AREA и чекбоксов)
	- Добавил PLOT_STOCK - статический график с масштабом
	- Добавил AJAX_PLOT_DARK
	- Изменён синтаксис у старых графиков
	- Фичи GPaddUnix и GPaddInt для графиков
	- Убрал default тему
	- Подкрутил стили
	- Добавил окно лога AREA_LOG и функцию лога в целом

	v1.3 - переделал GPunix, мелкие фиксы, для списков можно использовать PSTR
	v1.4 - мелкие фиксы, клик по COLOR теперь отправляет цвет
	v1.5 - добавил блок "слайдер+подпись"
	v1.5.1 - мелкий фикс копирования строк
	v1.6 - добавлены инструменты для работы c цветом. Добавил answer() для даты, времени и цвета
	v1.7 - поддержка ESP32
*/
#ifndef _GyverPortal_h
#define _GyverPortal_h

#include <DNSServer.h>
#ifdef ESP8266
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#else
#include <WiFi.h>
#include <WebServer.h>
#endif

#include "themes.h"
#include "list.h"
#include "utils.h"
#include "portal.h"

// // ======================= ДАТА =======================
extern String* _gp_sptr;
extern void* _gp_ptr;

struct Builder {
	void PAGE_BEGIN();

	void AJAX_CLICK();

	void THEME(const char* style);

	void PAGE_BLOCK_BEGIN();

	void PAGE_BLOCK_END();

	void PAGE_END();

	void AJAX_UPDATE(const char* list, int prd);

	void AREA_LOG(int rows);

	void FORM_BEGIN(const char* action);

	void FORM_END();

	void SUBMIT(const char* value);

	void FORM_SUBMIT(const char* name, const char* value);

	void BLOCK_BEGIN();

	void BLOCK_END();

	void BREAK();

	void HR();

	void TITLE(const char* name, const char* id = "");

	void LABEL(const char* name, const char* id = "");

	void LABEL(int name, const char* id);

	void BUTTON(const char* name, const char* value);

	void BUTTON(const char* name, const char* value, const char* tar);

	void BUTTON_MINI(const char* name, const char* value);

	void BUTTON_MINI(const char* name, const char* value, const char* tar);

	void NUMBER(const char* name, const char* place, int value);

	void AREA(const char* name, int rows, char* value);

	void AREA(const char* name, int rows);

	void TEXT(const char* name, const char* place, const char* value/* = ""*/);

	void TEXT(const char* name, const char* place, String& value);

	void TEXT(const char* name, const char* place, char* value);

	void PASS(const char* name, const char* place, const char* value);

	void PASS(const char* name, const char* place, char* value);

	void CHECK(const char* name, bool x);

	void SWITCH(const char* name, bool x);

	void DATE(const char* name);

	void DATE(const char* name, GPdate d);

	void TIME(const char* name);

	void TIME(const char* name, GPtime t);

	void LABEL_MINI(int text);

	void SLIDER(const char* name, int value, int min, int max, int step = 1);

	void SLIDER(const char* name, const char* label, int value, int min, int max, int step = 1);

	void COLOR(const char* name, uint32_t value);

	void COLOR(const char* name, GPcolor value);

	void SELECT(const char* name, const char* values, int8_t sel);

	void LED_RED(const char* name, bool state);

	void LED_GREEN(const char* name, bool state);

	template <uint8_t ax, uint8_t am>
	void PLOT(const char* id, const char** labels, int16_t vals[ax][am], int dec = 0);

	template <uint8_t ax, uint8_t am>
	void PLOT_DARK(const char* id, const char** labels, int16_t vals[ax][am], int dec = 0);

	template <uint8_t ax, uint8_t am>
	void PLOT_STOCK(const char* id, const char** labels, uint32_t* times, int16_t vals[ax][am], int dec = 0);

	template <uint8_t ax, uint8_t am>
	void PLOT_STOCK_DARK(const char* id, const char** labels, uint32_t* times, int16_t vals[ax][am], int dec = 0);

	void AJAX_PLOT(const char* name, int axes, int xamount, int prd);

	void AJAX_PLOT_DARK(const char* name, int axes, int xamount, int prd);

	void PASS(const char* name, const char* place, String& value);
};

extern Builder add;

void GP_BUILD(String& s);
void GP_SHOW();
void BUILD_BEGIN(String& s);
void BUILD_END();
#endif