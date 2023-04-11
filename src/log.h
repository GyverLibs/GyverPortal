#pragma once

// GP LOG module

#include <Print.h>
class GPlog : public Print {
public:
    GPlog() {}
    GPlog(const char* nname) : name(nname) {}

    void start(int n = 64) {
        stop();
        len = head = 0;
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
        /*if (newline) {
            newline = 0;
            _write('\n');
        }*/
        _write(n);
        return 1;
    }
    
    String read() {
        String s;
        if (!len) return s;
        s.reserve(len + 1);
        if (newline) {
            newline = 0;
            s += '\n';
        }
        for (int i = 0; i < len; i++) {
            char c = read(i);
            if (i == len - 2 && c == '\r') {
                newline = 1;
                break;
            }
            if (c == '\r') continue;
            s += c;
        }
        if (clear_f) clear();
        return s;
    }
    
    void clear() {
        len = head = 0;
    }
    
    void autoClear(bool f) {
        clear_f = f;
    }
    
    bool available() {
        return (buffer && len);
    }
    
    bool state() {
        return buffer;
    }

    int length() {
        return len;
    }
    
    const char* name = nullptr;
    char* buffer = nullptr;
    
private:
    void _write(uint8_t n) {
        if (len < size) len++;
        buffer[head] = n;
        if (++head >= size) head = 0;
    }
    char read(int num) {
        return buffer[(len < size) ? num : ((head + num) % size)];
    }
    
    bool newline = 0;
    bool clear_f = 1;
    int size;
    int len = 0;
    int head = 0;
};