#pragma once

// GP Update List module

enum GPtype {
    T_CSTR,
    T_STRING,
    T_TIME,
    T_DATE,
    T_CHECK,
    T_BYTE,
    T_INT,
    T_FLOAT,
    T_COLOR,
};

struct GPlist {
    ~GPlist() {
        clear();
    }

    void init(int n) {
        clear();
        vals = new void* [n];
        names = new char* [n];
        forms = new char* [n];
        types = new GPtype [n];
        skips = new bool [n];
        am = n;
    }
    
    void add(void* data, const char* name, GPtype type) {
        if (idx < am) {
            vals[idx] = data;
            names[idx] = (char*)name;
            types[idx] = type;
            skips[idx] = true;
            idx++;
        }
    }
    
    void add(void* data, const char* form, const char* name, GPtype type) {
        if (idx < am) {
            vals[idx] = data;
            forms[idx] = (char*)form;
            names[idx] = (char*)name;
            types[idx] = type;
            skips[idx] = false;
            idx++;
        }
    }

    void clear() {
        if (vals) delete [] vals;
        if (names) delete [] names;
        if (forms) delete [] forms;
        if (types) delete [] types;
        if (skips) delete [] skips;
        idx = am = 0;
    }
    
    int idx = 0;
    int am = 0;
    void** vals = nullptr;
    char** names = nullptr;
    char** forms = nullptr;
    GPtype* types = nullptr;
    bool* skips = nullptr;
};