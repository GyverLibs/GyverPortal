#pragma once

// GP Parsers

#include <Arduino.h>
#include "utils.h"
#include "objects.h"

class ArgParser {
public:
    virtual int args() {}                           // amount
    virtual const String& arg(const String& n) {}   // value from name
    virtual const String& arg() {}                  // value from 0
    virtual const String& argName() {}              // name from 0
    virtual bool hasArg(const String& n) {}         // check
    virtual bool clickF() {}
    
    // ===================== CLICK =====================
    // вернёт true, если был клик по (кнопка, чекбокс, свитч, слайдер, селектор)
    bool click() {
        return clickF() && args();
    }
    
    // вернёт true, если был клик по указанному элементу (кнопка, чекбокс, свитч, слайдер, селектор)
    bool click(const String& name) {
        return click() ? (argName().equals(name) && args() == 1) : 0;
    }
    
    // вернёт true, если имя кликнутого компонента начинается с name
    bool clickSub(const String& name) {
        return click() ? (argName().startsWith(name) && args() == 1) : 0;
    }
    
    // вернёт имя теукщего кликнутого компонента
    String clickName() {
        return click() ? argName() : String();
    }
    
    // вернёт часть имени кликнутого компонента, находящейся под номером idx после разделителя /
    String clickNameSub(int idx = 1) {
        return click() ? (GPlistIdx(argName(), idx, '/')) : String();
    }
    
    // ===================== CLICK AUTO =====================
    // нулевой аргумент (для вызова в условии)
    bool clickStr(char* t, int len = 0) {
        return click() ? copyStr(t, len) : 0;
    }
    bool clickString(String& t) {
        return click() ? copyString(t) : 0;
    }
    template <typename T>
    bool clickInt(T& t) {
        return click() ? copyInt(t) : 0;
    }
    template <typename T>
    bool clickFloat(T& t) {
        return click() ? copyFloat(t) : 0;
    }
    template <typename T>
    bool clickBool(T& t) {
        return click() ? copyBool(t) : 0;
    }
    bool clickDate(GPdate& t) {
        return click() ? copyDate(t) : 0;
    }
    bool clickTime(GPtime& t) {
        return click() ? copyTime(t) : 0;
    }
    bool clickColor(GPcolor& t) {
        return click() ? copyColor(t) : 0;
    }
    
    // с указанием имени компонента
    bool clickStr(const String& n, char* t, int len = 0) {
        return click() ? copyStr(n, t, len) : 0;
    }
    bool clickString(const String& n, String& t) {
        return click() ? copyString(n, t) : 0;
    }
    template <typename T>
    bool clickInt(const String& n, T& t) {
        return click() ? copyInt(n, t) : 0;
    }
    template <typename T>
    bool clickFloat(const String& n, T& t) {
        return click() ? copyFloat(n, t) : 0;
    }
    template <typename T>
    bool clickBool(const String& n, T& t) {
        return click() ? copyBool(n, t) : 0;
    }
    bool clickDate(const String& n, GPdate& t) {
        return click() ? copyDate(n, t) : 0;
    }
    bool clickTime(const String& n, GPtime& t) {
        return click() ? copyTime(n, t) : 0;
    }
    bool clickColor(const String& n, GPcolor& t) {
        return click() ? copyColor(n, t) : 0;
    }
    
    // ======================= ПАРСЕРЫ =======================
    // ОПАСНЫЕ ФУНКЦИИ (не проверяют есть ли запрос). Конвертируют и возвращают значение
    // получить String строку с компонента
    String getString(const String& n) {
        return String(arg(n));
    }
    String getString() {
        return String(arg());
    }

    // получить число с компонента
    int getInt(const String& n) {
        return getIntUniv(arg(n));
    }
    int getInt() {
        return getIntUniv(arg());
    }
    
    // получить float с компонента
    float getFloat(const String& n) {
        return arg(n).toFloat();
    }
    float getFloat() {
        return arg().toFloat();
    }

    // получить состояние чекбокса
    bool getBool(const String& n) {
        return (arg(n)[0] == '1' || arg(n)[0] == 'o' || arg(n)[0] == 't');
    }
    bool getBool() {
        return (arg()[0] == '1' || arg()[0] == 'o' || arg()[0] == 't');
    }
    
    // получить дату с компонента
    GPdate getDate(const String& n) {
        return GPdate(arg(n));
    }
    GPdate getDate() {
        return GPdate(arg());
    }

    // получить время с компонента
    GPtime getTime(const String& n) {
        return GPtime(arg(n));
    }
    GPtime getTime() {
        return GPtime(arg());
    }

    // получить цвет с компонента
    GPcolor getColor(const String& n) {
        return GPcolor(arg(n));
    }
    GPcolor getColor() {
        return GPcolor(arg());
    }

    
    // ===================== COPY-ПАРСЕРЫ =====================
    // ОПАСНЫЕ парсеры (не проверяют запрос). Использовать только в условии!
    bool copyStr(char* t, int len = 0) {
        return (args() && (!len || arg().length() < len)) ? (strcpy(t, arg().c_str()), 1) : 0;
    }
    bool copyString(String& t) {
        return args() ? (t = arg(), 1) : 0;
    }
    template <typename T>
    bool copyInt(T& t) {
        return args() ? (t = getInt(), 1) : 0;
    }
    template <typename T>
    bool copyFloat(T& t) {
        return args() ? (t = getFloat(), 1) : 0;
    }
    template <typename T>
    bool copyBool(T& t) {
        return args() ? (t = getBool(), 1) : 0;
    }
    bool copyDate(GPdate& t) {
        return args() ? (t = getDate(), 1) : 0;
    }
    bool copyTime(GPtime& t) {
        return args() ? (t = getTime(), 1) : 0;
    }
    bool copyColor(GPcolor& t) {
        return args() ? (t = getColor(), 1) : 0;
    }
    
    // БЕЗОПАСНЫЕ парсеры (проверяют запрос). Копируют данные из запроса в переменную
    bool copyStr(const String& n, char* t, int len = 0) {
        return (hasArg(n) && (!len || arg(n).length() < len)) ? (strcpy(t, arg(n).c_str()), 1) : 0;
    }
    bool copyString(const String& n, String& t) {
        return hasArg(n) ? (t = arg(n), 1) : 0;
    }
    template <typename T>
    bool copyInt(const String& n, T& t) {
        return hasArg(n) ? (t = getInt(n), 1) : 0;
    }
    template <typename T>
    bool copyFloat(const String& n, T& t) {
        return hasArg(n) ? (t = getFloat(n), 1) : 0;
    }
    template <typename T>
    bool copyBool(const String& n, T& t) {
        return hasArg(n) ? (t = getBool(n), 1) : 0;
    }
    bool copyDate(const String& n, GPdate& t) {
        return hasArg(n) ? (t = getDate(n), 1) : 0;
    }
    bool copyTime(const String& n, GPtime& t) {
        return hasArg(n) ? (t = getTime(n), 1) : 0;
    }
    bool copyColor(const String& n, GPcolor& t) {
        return hasArg(n) ? (t = getColor(n), 1) : 0;
    }
    
    
    // ===================== COPY OBJ =====================
    bool copy(GP_NUMBER& num) {
        return copyInt(num.name, num.value);
    }
    bool copy(GP_NUMBER_F& num) {
        return copyFloat(num.name, num.value);
    }
    
    bool copy(GP_TEXT& txt) {
        return copyString(txt.name, txt.text);
    }
    bool copy(GP_PASS& pas) {
        return copyString(pas.name, pas.text);
    }
    
    bool copy(GP_AREA& ar) {
        return copyString(ar.name, ar.text);
    }
    
    bool copy(GP_CHECK& ch) {
        return copyBool(ch.name, ch.state);
    }
    bool copy(GP_SWITCH& sw) {
        return copyBool(sw.name, sw.state);
    }
    
    bool copy(GP_DATE& d) {
        return copyDate(d.name, d.date);
    }
    bool copy(GP_TIME& t) {
        return copyTime(t.name, t.time);
    }
    bool copy(GP_COLOR& c) {
        return copyColor(c.name, c.color);
    }
    
    bool copy(GP_SPINNER& s) {
        return copyFloat(s.name, s.value);
    }
    bool copy(GP_SLIDER& s) {
        return copyFloat(s.name, s.value);
    }
    
    bool copy(GP_SELECT& s) {
        return copyInt(s.name, s.selected);
    }
    
    
    // ===================== CLICK OBJ =====================
    bool click(GP_BUTTON& btn) {
        return click(btn.name);
    }
    bool click(GP_BUTTON_MINI& btn) {
        return click(btn.name);
    }
    
    bool click(GP_NUMBER& num) {
        return click() ? copy(num) : 0;
    }
    bool click(GP_NUMBER_F& num) {
        return click() ? copy(num) : 0;
    }
    
    bool click(GP_TEXT& txt) {
        return click() ? copy(txt) : 0;
    }
    bool click(GP_PASS& pas) {
        return click() ? copy(pas) : 0;
    }
    
    bool click(GP_AREA& ar) {
        return click() ? copy(ar) : 0;
    }
    
    bool click(GP_CHECK& ch) {
        return click() ? copy(ch) : 0;
    }
    bool click(GP_SWITCH& sw) {
        return click() ? copy(sw) : 0;
    }
    
    bool click(GP_DATE& d) {
        return click() ? copy(d) : 0;
    }
    bool click(GP_TIME& t) {
        return click() ? copy(t) : 0;
    }
    bool click(GP_COLOR& c) {
        return click() ? copy(c) : 0;
    }
    
    bool click(GP_SPINNER& s) {
        return click() ? copy(s) : 0;
    }
    bool click(GP_SLIDER& s) {
        return click() ? copy(s) : 0;
    }
    
    bool click(GP_SELECT& s) {
        return click() ? copy(s) : 0;
    }
    
private:
    int getIntUniv(const String& s) {
        if (s[0] == '#') {
            GPcolor col(s);
            return col.getHEX();
        } else return s.toInt();
    }
};