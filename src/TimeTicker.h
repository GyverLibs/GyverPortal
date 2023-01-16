#pragma once

// GP Time module

struct TimeTicker {
    void setUnix(uint32_t unixx) {
        _unix = unixx;
        _unixTmr = millis();
    }
    void setGMT(int16_t gmt) {
        _gmt = gmt;
    }
    
    uint32_t getUnix() {
        if (_unix) {
            // защита от переполнения разности через 50 суток
            uint32_t diff = millis() - _unixTmr;
            if (diff > 86400000ul) {
                _unix += diff / 1000ul;
                _unixTmr = millis() - diff % 1000ul;
            }
            return _unix + (millis() - _unixTmr) / 1000ul;
        }
        return 0;
    }
    
    int16_t getGMT() {
        return _gmt;
    }

    bool timeSynced() {
        return (bool)_unix;
    }
    
    int16_t _gmt;
    uint32_t _unix = 0;
    uint32_t _unixTmr = 0;
};