// пример организации файла со своими компонентами конструктора

#pragma once
#include "GyverPortal.h"

// шаблон "сборного" блока (что-то с надписью или цветом)
void GP_MY_BLOCK(const String& data) {
    String s;               // создай строку
    s += F("some html");    // собери html код
    s += data;              // добавь данные из аргументов
    GP.SEND(s);             // отправь через GP.SEND
}

// шаблон одиночного блока (например скрипт, набор стилей итд)
void GP_MY_BLOCK() {
    // храним html код через макрос PSTR в программной памяти
    // и отправляем через GP.SEND_P
    GP.SEND_P(PSTR("some\n"
    "multi-line\n"
    "html\n"
    "code\n"));
}

// пример "текст-ссылка"
void GP_TEXT_HREF(const String& url, const String& text) {
    String s;
    s += F("<a href='");
    s += url;
    s += F("'>");
    s += text;
    s += F("</a>");
    GP.SEND(s);
}
