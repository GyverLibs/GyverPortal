#pragma once
#include <Print.h>

class GPlog : public Print {
public:
    void start(int n/* = 64*/);
    ~GPlog();
    void stop();

    // запись в буфер
    virtual size_t write(uint8_t n);

    // чтение из буфера
    char read();

    // вернёт количество непрочитанных элементов
    int available();

    // "очистка" буфера
    void clear();

    bool state = false;

private:
    char* buffer = nullptr;
    uint8_t size;
    int head = 0, tail = 0;
};