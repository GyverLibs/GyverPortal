#pragma once
#include "log.h"
#include "GyverPortal.h"

class GyverPortal {
public:
	GyverPortal();

	~GyverPortal();

	void start(uint8_t mode = WIFI_STA);

	void stop();

	void attachBuild(void(*handler)());

	void attachClick(void(*handler)(GyverPortal* p));

	void attachForm(void(*handler)(GyverPortal* p));

	void attachUpdate(void(*handler)(GyverPortal* p));

	bool tick();

	bool form();

	bool form(const char* name);

	String& formName();

	bool click();

	bool click(const char* name);

	const String& clickName();

	int clickValue();

	const String& clickText();

	bool update();

	bool update(const char* name);

	const String& updateName();

	void answer(String s);

	void answer(char* s);

	void answer(int v);

	void answer(int16_t* v, int am, int dec);

	void answer(GPcolor col);

	void answer(GPdate date);

	void answer(GPtime time);

	bool root();

	const String& getString(const char* n);

	const char* getChars(const char* n);

	void copyStr(const char* n, char* d);

	int getInt(const char* n);

	float getFloat(const char* n);

	bool getCheck(const char* n);

	GPdate getDate(const char* n);

	GPtime getTime(const char* n);

	uint32_t getColor(const char* n);

	int8_t getSelected(const char* n, const char* list);

	void showPage(String& s);

	void show();

	String& uri();

#ifdef ESP8266
	ESP8266WebServer server;
#else
	WebServer server;
#endif
	List list;
	GPlog log;

private:
	void checkList();

	bool logF = false;
	String req, buf;
	bool _mode = false;
	bool _active = false;
	bool _formF = 0;
	bool _updateF = 0;
	bool _clickF = 0;
	void (*_build)() = nullptr;
	void (*_click)(GyverPortal* p) = nullptr;
	void (*_form)(GyverPortal* p) = nullptr;
	void (*_update)(GyverPortal* p) = nullptr;
	IPAddress apIP;
	DNSServer dnsServer;
};