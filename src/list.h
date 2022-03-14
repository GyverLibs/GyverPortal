#pragma once
// список функций автообновления

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

struct List {
    ~List();

    void init(int n);

    void add(void* data, const char* name, GPtype type);

    void add(void* data, const char* form, const char* name, GPtype type);

    void clear();

    int idx = 0;
    int am = 0;
    void** vals = nullptr;
    char** names = nullptr;
    char** forms = nullptr;
    GPtype* types = nullptr;
    bool* skips = nullptr;
};