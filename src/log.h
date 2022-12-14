#pragma once

// GP LOG module

#include <Print.h>
class GPlog : public Print {
public:
    GPlog() {}
    GPlog(const char* nname) : name(nname) {}

    void start(int n = 64) {
        if (buffer) delete [] buffer;
        head = 0;
        size = n;
        buffer = new char [size];
    }
    
    ~GPlog() {
        stop();
    }
    
    void stop() {
        if (buffer) {
            delete [] buffer;
            buffer = nullptr;
        }
    }
    
    // запись в буфер
    virtual size_t write(uint8_t n) {
        if (buffer && head <= size - 2) {
            if (newline) {
                newline = 0;
                buffer[head++] = '\r';
                buffer[head++] = '\n';
            }
            buffer[head++] = n;
        }
        return 1;
    }
    
    char* read() {
        if (buffer[head - 2] == '\r') {
            head -= 2;
            newline = 1;
        }
        buffer[head] = '\0';
        head = 0;
        return buffer;
    }
    
    void clear() {
        head = 0;
        buffer[head] = '\0';
    }
    
    bool available() {
        return (buffer && head);
    }
    
    bool state() {
        return buffer;
    }
    
    const char* name = nullptr;
    char* buffer = nullptr;
    
private:
    bool newline = 0;
    int size;
    int head = 0;
};