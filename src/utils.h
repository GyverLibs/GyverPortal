#pragma once

// ======================= ДАТА =======================
String* _gp_sptr = nullptr;
void* _gp_ptr = nullptr;

// ====================== COLOR =======================
// собрать цвет в String
String encodeColor(uint32_t color) {
    String s('#');
    for (uint8_t i = 0; i < 6; i++) {
      char p = ((uint32_t)color >> (5 - i) * 4) & 0xF;
      p += (p > 9) ? 87 : 48;
      s += p;
    }
    return s;
}

// разобрать цвет в число
uint32_t decodeColor(char* hex) {
    if (strlen(hex) < 2 || strlen(hex) > 7) return 0;
    uint32_t val = 0;
    for (uint8_t i = 0; i < strlen(hex) - 1; i++) {
        val <<= 4;
        uint8_t d = hex[i + 1];
        d -= (d <= '9') ? 48 : ((d <= 'F') ? 55 : 87);
        val |= d;
    }
    return val;
}

// ======================= DATE =======================
struct GPdate {
    int16_t year;
    uint8_t month, day;
};
// склеить дату в строку str[11]
void encodeDate(char* str, GPdate& d) {
    str[0] = d.year / 1000 + '0';
    str[1] = (d.year % 1000) / 100 + '0';
    str[2] = (d.year % 100) / 10 + '0';
    str[3] = d.year % 10 + '0';
    str[4] = '-';
    str[5] = d.month / 10 + '0';
    str[6] = d.month % 10 + '0';
    str[7] = '-';
    str[8] = d.day / 10 + '0';
    str[9] = d.day % 10 + '0';
    str[10] = '\0';
}

// склеить дату в строку String
String encodeDate(GPdate& d) {
    char str[11];
    encodeDate(str, d);
    String s(str);
    return s;
}

// склеить дату в строку String
String encodeDate(int16_t year, uint8_t month, uint8_t day) {
    GPdate d = (GPdate){year, month, day};
    return encodeDate(d);
}

// разобрать строковую дату в структуру
GPdate decodeDate(char* str) {
    GPdate d = (GPdate){0,0,0};
    if (strlen(str) == 10) {
        str[4] = str[7] = '\0';
        d.year = atoi(str);
        d.month = atoi(str+5);
        d.day = atoi(str+8);
        str[4] = str[7] = '-';
    }
    return d;
}
// ======================= TIME =======================
struct GPtime {
    uint8_t hour, minute, second;
};

// склеить время в строку str[9]
void encodeTime(char* str, GPtime& t) {
    str[0] = t.hour / 10 + '0';
    str[1] = t.hour % 10 + '0';
    str[2] = ':';
    str[3] = t.minute / 10 + '0';
    str[4] = t.minute % 10 + '0';
    str[5] = ':';
    str[6] = t.second / 10 + '0';
    str[7] = t.second % 10 + '0';
    str[8] = '\0';
}

// склеить время в строку String
String encodeTime(GPtime& t) {
    char str[9];
    encodeTime(str, t);
    String s(str);
    return s;
}

// склеить время в строку String
String encodeTime(uint8_t hour, uint8_t minute, uint8_t second = 0) {
    GPtime t = (GPtime){hour, minute, second};
    return encodeTime(t);
}

// разобрать строковое время HH:MM:SS в структуру
GPtime decodeTime(char* str) {
    GPtime t = (GPtime){0,0,0};
    if (strlen(str) == 8) {
        str[2] = str[5] = '\0';
        t.hour = atoi(str);
        t.minute = atoi(str+3);
        t.second = atoi(str+6);
        str[2] = str[5] = ':';
    }
    return t;
}

// ======================= LIST UTIL =======================
// разделить строку на подстроки. Цыганские фокусы
char* splitList(char* str) {
    static uint8_t prev, end;
    if (str == NULL) prev = end = 0;
    else {
        if (prev) *(str + prev - 1) = ',';
        char* cur = strchr(str+prev, ',');
        if (cur) {
            *cur = '\0';
            uint8_t b = prev;
            prev = cur - str + 1;
            return str + b;
        } else if (!end) {
            end = 1;
            return str + prev;
        }
    }
    return NULL;
}

// получить номер, под которым name входит в list (вида "val1,val2,val3")
int8_t inList(const char* name, const char* list) {
  char* str = (char*)list;
  int8_t count = 0, pos = -1;
  splitList(NULL);
  while ((str = splitList((char*)list)) != NULL) {
    if (!strcmp(str, name)) pos = count;
    count++;
  }
  return pos;
}

int8_t inList(String& s, const char* list) {
    return inList(s.c_str(), list);
}

int8_t inList(const String& s, const char* list) {
    return inList(s.c_str(), list);
}