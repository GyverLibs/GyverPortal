#pragma once
#include <Arduino.h>

#define GP_PGM(name, val) static const char name[] PROGMEM = val

// ==================== COLORS =====================
GP_PGM(GP_RED, "#bf1e1e");
GP_PGM(GP_RED_B, "#e11414");
GP_PGM(GP_PINK, "#bb32aa");
GP_PGM(GP_PINK_B, "#db13c1");
GP_PGM(GP_VIOL, "#802ecb");
GP_PGM(GP_VIOL_B, "#a60cf5");
GP_PGM(GP_BLUE, "#3b3ebd");
GP_PGM(GP_BLUE_B, "#353aff");
GP_PGM(GP_CYAN, "#1f8fa9");
GP_PGM(GP_CYAN_B, "#06b3db");
GP_PGM(GP_GREEN, "#37a93c");
GP_PGM(GP_GREEN_B, "#25d52c");
GP_PGM(GP_YELLOW, "#b4b700");
GP_PGM(GP_ORANGE, "#b37f0d");
GP_PGM(GP_ORANGE_B, "#ff4500");
GP_PGM(GP_GRAY, "#5e5e5e");
GP_PGM(GP_BLACK, "#13161a");

enum GPalign {
    GP_CENTER,
    GP_LEFT,
    GP_RIGHT,
    GP_EDGES,
};

// получить align для flex
PGM_P GPgetAlign(GPalign a);

// ================ STRING UTILS ==================
struct GP_parser {
    GP_parser() {
        str.reserve(20);
    }
    
    bool parse(const String& s) {
        if (i >= (int)s.length() - 1) return 0;
        i = s.indexOf(',', from);
        if (i < 0) i = s.length();
        int to = i;
        if (s[to - 1] == ' ') to--;
        if (s[from] == ' ') from++;
        str = s.substring(from, to);
        from = i + 1;
        count++;
        return 1;
    }
    
    int i = 0, from = 0;
    int count = -1;
    String str;
};

// получить номер, под которым name входит в list вида "val1,val2,val3"
int GPinList(const String& s, const String& list);

// получить строку, которая входит в список list "val1,val2,val3" под номером idx
String GPlistIdx(int idx, const String& li, char div = ',');

// получить тип файла (вида image/png) по его пути uri
String GPfileType(const String& uri);

// ====================== COLOR =======================
struct GPcolor {
    uint8_t r = 0, g = 0, b = 0;
    
    GPcolor() {}
    GPcolor(uint32_t col) {
        setHEX(col);
    }
    GPcolor(const String& s) {
        decode(s);
    }
    GPcolor(uint8_t nr, uint8_t ng, uint8_t nb) {
        setRGB(nr, ng, nb);
    }
    
    void setRGB(uint8_t nr, uint8_t ng, uint8_t nb) {
        r = nr;
        g = ng;
        b = nb;
    }
    void operator = (uint32_t col) {
        setHEX(col);
    }
    void setHEX(uint32_t col) {
        r = ((uint32_t)col >> 16) & 0xFF;
        g = ((uint32_t)col >>  8) & 0xFF;
        b = col & 0xFF;
    }
    uint32_t getHEX() {
        return (((uint32_t)r << 16) | ((uint32_t)g << 8) | b);
    }
    
    String encode() {
        uint32_t color = getHEX();
        String s('#');
        s.reserve(7+1);
        for (uint8_t i = 0; i < 6; i++) {
            char p = ((uint32_t)color >> (5 - i) * 4) & 0xF;
            p += (p > 9) ? 87 : 48;
            s += p;
        }
        return s;
    }
    void decode(const String& hex) {
        if (hex.length() < 6) return;
        uint32_t val = 0;
        uint8_t i = (hex[0] == '#') ? 1 : 0;
        for (; i < hex.length(); i++) {
            val <<= 4;
            uint8_t d = hex[i];
            d -= (d <= '9') ? 48 : ((d <= 'F') ? 55 : 87);
            val |= d;
        }
        setHEX(val);
    }
};

// ======================= DATE =======================
struct GPdate {
    uint16_t year = 0;
    uint8_t month = 1, day = 1;
    
    GPdate() {}
    GPdate(int nyear, int nmonth, int nday) {
        set(nyear, nmonth, nday);
    }
    GPdate(const String& str) {
        decode(str);
    }
    
    void set(int nyear, int nmonth, int nday) {
        year = nyear;
        month = nmonth;
        day = nday;
    }
    
    String encode() {
        String s;
        s.reserve(10+1);
        if (!year) s += F("0000");
        else s += year;
        s += '-';
        s += month / 10;
        s += month % 10;
        s += '-';
        s += day / 10;
        s += day % 10;
        return s;
    }
    void decode(const String& str) {
        if (str.length() != 10) return;
        year = atoi(str.c_str());
        month = atoi(str.c_str() + 5);
        day = atoi(str.c_str() + 8);
    }
};

// ======================= TIME =======================
struct GPtime {
    uint8_t hour = 0, minute = 0, second = 0;
    
    GPtime() {}
    GPtime(int nhour, int nminute, int nsecond = 0) {
        set(nhour, nminute, nsecond);
    }
    GPtime(const String& str) {
        decode(str);
    }
    
    void set(int nhour, int nminute, int nsecond = 0) {
        hour = nhour;
        minute = nminute;
        second = nsecond;
    }
    String encode() {
        String s;
        s.reserve(8+1);
        s += hour / 10;
        s += hour % 10;
        s += ':';
        s += minute / 10;
        s += minute % 10;
        s += ':';
        s += second / 10;
        s += second % 10;
        return s;
    }
    void decode(const String& str) {
        if (str.length() != 5 && str.length() != 8) return;
        hour = atoi(str.c_str());
        minute = atoi(str.c_str() + 3);
        if (str.length() == 8) second = atoi(str.c_str() + 6);
        else second = 0;
    }
};

// ===================== DATE-TIME UNIX =====================
uint32_t GPunix(uint16_t y, uint8_t m, uint8_t d, uint8_t h, uint8_t mn, uint8_t s, int8_t gmt = 0);
uint32_t GPunix(GPdate d, GPtime t, int8_t gmt = 0);

// добавить новое значение в массив с перемоткой (для графиков)
void GPaddInt(int16_t val, int16_t* arr, uint8_t am);
void GPaddUnix(uint32_t val, uint32_t* arr, uint8_t am);
void GPaddUnixS(int16_t val, uint32_t* arr, uint8_t am);