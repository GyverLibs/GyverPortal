#pragma once
#include <Print.h>
class GPlog : public Print {
public:
    void start(int n = 64) {
        if (buffer) return;
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
            buffer[head] = n;
            head++;
        }
        return 1;
    }
    
    char* read() {
        buffer[head] = '\0';
        head = 0;
        return buffer;
    }
    
    bool available() {
        return (buffer && head);
    }
    
private:
    char* buffer = nullptr;
    int size;
    int head = 0;
};