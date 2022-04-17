#pragma once
#include <Print.h>

// ======================= ДАТА =======================
extern String* _gp_sptr;
extern void* _gp_ptr;

// ====================== COLOR =======================
struct GPcolor {

	uint8_t r, g, b;

	GPcolor();

	GPcolor(uint32_t col);

	GPcolor(uint8_t nr, uint8_t ng, uint8_t nb);

	void setRGB(uint8_t nr, uint8_t ng, uint8_t nb);

	void operator=(uint32_t col);

	void setHEX(uint32_t col);

	uint32_t getHEX();
};

String encodeColor(uint32_t color);

String encodeColor(GPcolor color);

uint32_t decodeColor(char* hex);

struct GPdate {
	int16_t year;
	uint8_t month, day;
};

void encodeDate(char* str, GPdate& d);

String encodeDate(GPdate& d);

String encodeDate(int16_t year, uint8_t month, uint8_t day);

GPdate decodeDate(char* str);

// ======================= TIME =======================
struct GPtime {
	uint8_t hour, minute, second;
};

void encodeTime(char* str, GPtime& t);

String encodeTime(GPtime& t);

String encodeTime(uint8_t hour, uint8_t minute, uint8_t second);

GPtime decodeTime(char* str);

uint32_t GPunix(uint16_t y, uint8_t m, uint8_t d, uint8_t h, uint8_t mn, uint8_t s, int8_t gmt);

void GPaddInt(int16_t val, int16_t* arr, uint8_t am);

void GPaddUnix(uint32_t val, uint32_t* arr, uint8_t am);

void GPaddUnixS(int16_t val, uint32_t* arr, uint8_t am);

// ===================== DATE-TIME PACK =====================
// упакованная дата-время
struct DateTimeP {
	uint32_t ymdhm; // 0yyyyyyy yyyyymmm mddddhhh hhmmmmmm
	uint8_t ss;     // 00ssssss
	uint16_t year();
	uint8_t month();
	uint8_t day();
	uint8_t hour();
	uint8_t minute();
	uint8_t second();
	void year(uint16_t y);
	void month(uint8_t m);
	void day(uint8_t d);
	void hour(uint8_t h);
	void minute(uint8_t m);
	void second(uint8_t s);
	void set(uint16_t y, uint8_t m, uint8_t d, uint8_t h, uint8_t mn, uint8_t s);
};

void encodeDTP(char* buf, DateTimeP dtp);

String encodeDTP(DateTimeP& dtp);

char* splitList(char* str);

int8_t inList(const char* name, const char* list);

int8_t inList(String& s, const char* list);

int8_t inList(const String& s, const char* list);
