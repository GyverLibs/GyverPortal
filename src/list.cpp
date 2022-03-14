#include <list.h>

List::~List() {
    clear();
}

void List::init(int n) {
    clear();
    vals = new void* [n];
    names = new char* [n];
    forms = new char* [n];
    types = new GPtype[n];
    skips = new bool[n];
    am = n;
}

void List::add(void* data, const char* name, GPtype type) {
    if (idx < am) {
        vals[idx] = data;
        names[idx] = (char*)name;
        types[idx] = type;
        skips[idx] = true;
        idx++;
    }
}

void List::add(void* data, const char* form, const char* name, GPtype type) {
    if (idx < am) {
        vals[idx] = data;
        forms[idx] = (char*)form;
        names[idx] = (char*)name;
        types[idx] = type;
        skips[idx] = false;
        idx++;
    }
}

void List::clear() {
    if (vals) delete[] vals;
    if (names) delete[] names;
    if (forms) delete[] forms;
    if (types) delete[] types;
    if (skips) delete[] skips;
    idx = am = 0;
}