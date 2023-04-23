#pragma once

// GP Utilities

#include <Arduino.h>

#define GP_PGM(name, val) static const char name[] PROGMEM = val
#define GP_PGM_LIST(name, ...) const char* const name[] PROGMEM = {__VA_ARGS__};
extern String _GP_empty_str;

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
GP_PGM(GP_YELLOW_B, "#ffff00");
GP_PGM(GP_YELLOW, "#b4b700");
GP_PGM(GP_ORANGE, "#b37f0d");
GP_PGM(GP_ORANGE_B, "#ff4500");
GP_PGM(GP_GRAY, "#5e5e5e");
GP_PGM(GP_GRAY_B, "#9c9ea1");
GP_PGM(GP_BLACK, "#13161a");
GP_PGM(GP_WHITE, "#fff");
GP_PGM(GP_DEFAULT, "#fff");

enum GPalign {
    GP_CENTER,
    GP_LEFT,
    GP_RIGHT,
    GP_EDGES,
    GP_JUSTIFY = 3,
    
    GP_CENTER2,
    GP_LEFT2,
    GP_RIGHT2,
    GP_JUSTIFY2,
};

enum GPblock {
    GP_DIV_RAW,
    GP_DIV,
    GP_TAB,
    GP_THIN,
};

// получить align
PGM_P GPgetAlignFlex(GPalign a);
PGM_P GPgetAlign(GPalign a);

// ================ STRING UTILS ==================
struct GP_parser {
    GP_parser() {
        str.reserve(20);
    }
    GP_parser(const String& s) {
        strp = &s;
    }
    
    bool parse(const String& s) {
        strp = &s;
        return parse();
    }
    bool parse() {
        int slen = strp->length();
        if (i > slen - 1) return 0;
        i = strp->indexOf(',', from);
        if (i < 0) i = slen;
        int to = i;
        if (strp->charAt(to - 1) == ' ') to--;
        if (strp->charAt(from) == ' ') from++;
        str = strp->substring(from, to);
        from = i + 1;
        count++;
        return 1;
    }
    
    int i = 0, from = 0;
    int count = -1;
    String str;
    const String* strp;
};

// получить номер, под которым name входит в list вида "val1,val2,val3"
int GPinList(const String& s, const String& list);

// получить строку, которая входит в список list "val1,val2,val3" под номером idx
String GPlistIdx(const String& li, int idx, char div = ',');

// получить тип файла (вида image/png) по его пути uri
String GPfileType(const String& uri);

// ====================== COLOR =======================
struct GPcolor {
    uint8_t r = 0, g = 0, b = 0;
    
    GPcolor() {}
    GPcolor(const GPcolor& col) = default;
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
    GPdate(const GPdate& dat) = default;
    GPdate(uint32_t unix, int16_t gmt = 0) {
        unix = (unix + gmt * 60L) / (60 * 60 * 24) + 719468;
        uint8_t era = unix / 146097ul;
        uint16_t doe = unix - era * 146097ul;
        uint16_t yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
        year = yoe + era * 400;
        uint16_t doy = doe - (yoe * 365 + yoe / 4 - yoe / 100);
        uint16_t mp = (doy * 5 + 2) / 153;
        day = doy - (mp * 153 + 2) / 5 + 1;
        month = mp + (mp < 10 ? 3 : -9);
        year += (month <= 2);
    }
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
    String encodeDMY() {
        String s;
        if (year < 2000) s = F("unset");
        else {
            s.reserve(10+1);
            s += day / 10;
            s += day % 10;
            s += '.';
            s += month / 10;
            s += month % 10;
            s += '.';
            if (!year) s += F("0000");
            else s += year;
        }
        return s;
    }
    void decode(const String& str) {
        if (str.length() > 10) return;
        const char* s = str.c_str();
        year = atoi(s);
        s = strchr(s, '-');
        if (!s) return;
        month = atoi(++s);
        s = strchr(s, '-');
        if (!s) return;
        day = atoi(++s);
    }
};

// ======================= TIME =======================
struct GPtime {
    uint8_t hour = 0, minute = 0, second = 0;
    
    GPtime() {}
    GPtime(const GPtime& tim) = default;
    GPtime(uint32_t unix, int16_t gmt = 0) {
        unix += gmt * 60L;
        second = unix % 60ul;
        unix /= 60ul;
        minute = unix % 60ul;
        unix /= 60ul;
        hour = unix % 24ul;
    }
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
        if (str.length() > 8) return;
        const char* s = str.c_str();
        hour = atoi(s);
        s = strchr(s, ':');
        if (!s) return;
        minute = atoi(++s);
        s = strchr(s, ':');
        if (!s) return;
        second = atoi(++s);
    }
};

struct GPweek {
    uint8_t week = 0;

    GPweek() {}
    GPweek(const GPweek& wk) = default;
    GPweek(uint8_t nweek) {
        week = nweek;
    }
    GPweek(const String& s) {
        decode(s);
    }

    void set(uint8_t idx, uint8_t val) {
        if (idx < 8) bitWrite(week, idx, val);
    }
    uint8_t get(uint8_t idx) {
        if (idx < 8) return bitRead(week, idx);
        else return 0;
    }

    void decode(const String& s) {
        if (s.length() != 7) return;
        week = 0;
        for (int i = 0; i < 7; i++) {
            week |= s[6 - i] - '0';
            week <<= 1;
        }
    }
    String encode() {
        String s;
        s.reserve(7);
        for (int i = 1; i < 8; i++) s += get(i);
        return s;
    }
};

struct GPflags {
    uint16_t flags = 0;
    uint8_t len = 16;

    GPflags() {}
    GPflags(const GPflags& f) = default;
    GPflags(uint8_t nlen) {
        len = nlen;
    }
    GPflags(uint16_t nflags, uint8_t nlen) {
        flags = nflags;
        len = nlen;
    }
    GPflags(const String& s) {
        decode(s);
    }

    uint8_t length() {
        return len;
    }
    void setLength(uint8_t nlen) {
        len = nlen;
    }

    void set(uint8_t idx, uint8_t val) {
        if (idx < 16) bitWrite(flags, idx, val);
    }
    uint8_t get(uint8_t idx) {
        if (idx < 16) return bitRead(flags, idx);
        else return 0;
    }

    void decode(const String& s) {
        if (s.length() > 16) return;
        len = s.length();
        flags = 0;
        for (int i = 0; i < len; i++) {
            flags <<= 1;
            flags |= s[len - 1 - i] - '0';
        }
    }
    String encode() {
        String s;
        s.reserve(len);
        for (int i = 0; i < len; i++) s += get(i);
        return s;
    }
};

// ===================== DATE-TIME UNIX =====================
uint32_t GPunix(uint16_t y, uint8_t m, uint8_t d, uint8_t h, uint8_t mn, uint8_t s, int8_t gmt = 0);
uint32_t GPunix(GPdate d, GPtime t, int8_t gmt = 0);

// добавить новое значение в массив с перемоткой (для графиков)
void GPaddInt(int16_t val, int16_t* arr, uint8_t am);
void GPaddUnix(uint32_t val, uint32_t* arr, uint8_t am);
void GPaddUnixS(int16_t val, uint32_t* arr, uint8_t am);