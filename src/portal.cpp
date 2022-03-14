#include <portal.h>

// ======================= ����������� =======================
GyverPortal::GyverPortal() {
    req.reserve(20);
    buf.reserve(64);
}

GyverPortal::~GyverPortal() {
    stop();
}
// ======================= ������� =======================
// ��������� ������. ����� �������� WIFI_AP ��� ������� DNS �������
void GyverPortal::start(uint8_t mode) {
    _mode = (mode == WIFI_AP);
    _active = 1;
    server.begin();
    if (_mode) dnsServer.start(53, "*", WiFi.softAPIP());
    server.onNotFound([this]() {
        req = server.uri();
        if (req.startsWith(F("/favicon.ico"))) show();
        else _formF = 1;
#ifdef ESP32
        show();
#endif
        });
    server.on("/GP_click", [this]() {
        _clickF = 1;
        server.send(200, "text/plane");
        });
    server.on("/GP_update", [this]() {
        _updateF = 1;
#ifdef ESP32
        show();
#endif
        });
    server.on("/GP_log", [this]() {
        if (log.state && log.available()) {
            String s;
            while (log.available()) s += log.read();
            server.send(200, "text/plane", s);
        }
        else server.send(200, "text/plane");
        });
}

// ���������� ������
void GyverPortal::stop() {
    _active = 0;
    if (_mode) dnsServer.stop();
    server.stop();
}

// ======================= ������ =======================
// ���������� �������-������ ��������
void GyverPortal::attachBuild(void (*handler)()) {
    _build = *handler;
}

// ���������� �������, ������� ���������� ��� ����� (������, �������, �����, �������, ��������)
void GyverPortal::attachClick(void (*handler)(GyverPortal* p)) {
    _click = *handler;
}

// ���������� �������, ������� ���������� ��� ������� submit
void GyverPortal::attachForm(void (*handler)(GyverPortal* p)) {
    _form = *handler;
}

// ���������� �������, ������� ���������� ��� AJAX ���������� �� ��������
void GyverPortal::attachUpdate(void (*handler)(GyverPortal* p)) {
    _update = *handler;
}

// ======================= ����� =======================
// ����� �������. ����� true, ���� ������ �������
bool GyverPortal::tick() {
    if (!_active) return 0;
    if (_formF) show();
    if (_updateF) server.send(200, "text/plane");
    _updateF = _clickF = _formF = 0;

    if (_mode) dnsServer.processNextRequest();
    server.handleClient();
    if (_formF) {
        checkList();
        if (*_form) _form(this);
    }
    if (_clickF) {
        checkList();
        if (*_click) _click(this);
    }
    if (_updateF && *_update) _update(this);
    yield();
    return 1;
}

// ======================= FORM =======================
// ����� true, ���� ���� ������� �� ����� submit
bool GyverPortal::form() {
    return _formF;
}

// ����� true, ���� ��� submit � ��������� �����
bool GyverPortal::form(const char* name) {
    return _formF ? req.equals(name) : 0;
}

// ����� ��� ������� submit �����
String& GyverPortal::formName() {
    return req;
}

// ======================= CLICK =======================
// ����� true, ���� ��� ���� �� (������, �������, �����, �������, ��������)
bool GyverPortal::click() {
    return _clickF;
}

// ����� true, ���� ��� ���� �� ���������� �������� (������, �������, �����, �������, ��������)
bool GyverPortal::click(const char* name) {
    return _clickF ? server.argName(0).equals(name) : 0;
}

// ����� ��� �������� ���������� ����������
const String& GyverPortal::clickName() {
    buf = server.argName(0);
    return buf;
}

// �������� �������� ���������� ����������
int GyverPortal::clickValue() {
    int val = server.arg(0).toInt();
    return val;
}

// �������� ����� ���������� ����������
const String& GyverPortal::clickText() {
    buf = server.arg(0);
    return buf;
}

// ======================= UPDATE =======================
// ����� true, ���� ���� ����������
bool GyverPortal::update() {
    return _updateF;
}

// ����� true, ���� ���� update � ���������� ����������
bool GyverPortal::update(const char* name) {
    return _updateF ? server.argName(0).equals(name) : 0;
}

// ����� ��� ����������� ����������
const String& GyverPortal::updateName() {
    buf = server.argName(0);
    return buf;
}

// ��������� ����� �� ����������
void GyverPortal::answer(String s) {
    _updateF = 0;
    server.send(200, "text/plane", s);
}
void GyverPortal::answer(char* s) {
    _updateF = 0;
    server.send(200, "text/plane", s);
}
void GyverPortal::answer(int v) {
    String s(v);
    answer(s.c_str());
}
void GyverPortal::answer(int16_t* v, int am, int dec = 0) {
    String s;
    for (int i = 0; i < am; i++) {
        if (dec) s += (float)v[i] / dec;
        else s += v[i];
        if (i != am - 1) s += ',';
    }
    answer(s.c_str());
}
void GyverPortal::answer(GPcolor col) {
    answer(encodeColor(col.getHEX()));
}
void GyverPortal::answer(GPdate date) {
    answer(encodeDate(date));
}
void GyverPortal::answer(GPtime time) {
    answer(encodeTime(time));
}

// ======================= MISC =======================
// ����� true, ���� ������� ������� �������� (/)
bool GyverPortal::root() {
    return (req[0] == '/' && req[1] == '\0');
}

// ======================= ������� =======================
// �������� String ������ � ����������
const String& GyverPortal::getString(const char* n) {
    buf = server.arg(n);
    return buf;
}
// �������� char* ������ � ����������
const char* GyverPortal::getChars(const char* n) {
    buf = server.arg(n);
    return buf.c_str();
}
// ���������� char ������ � ���������� � ����
void GyverPortal::copyStr(const char* n, char* d) {
    if (server.hasArg(n)) strcpy(d, server.arg(n).c_str());
}

// �������� ����� � ����������
int GyverPortal::getInt(const char* n) {
    int val = server.arg(n).toInt();
    return val;
}

// �������� float � ����������
float GyverPortal::getFloat(const char* n) {
    float val = server.arg(n).toFloat();
    return val;
}

// �������� ��������� ��������
bool GyverPortal::getCheck(const char* n) {
    if (server.hasArg(n)) return (server.arg(n)[0] == '1' || server.arg(n)[0] == 'o');
    return 0;
}

// �������� ���� � ����������
GPdate GyverPortal::getDate(const char* n) {
    return decodeDate((char*)server.arg(n).c_str());
}

// �������� ����� � ����������
GPtime GyverPortal::getTime(const char* n) {
    return decodeTime((char*)server.arg(n).c_str());
}

// �������� ���� � ����������
uint32_t GyverPortal::getColor(const char* n) {
    if (!server.hasArg(n)) return 0;
    return decodeColor((char*)server.arg(n).c_str());
}

// �������� ����� ���������� ������ � ���������
int8_t GyverPortal::getSelected(const char* n, const char* list) {
    buf = server.arg(n);
    return inList(buf.c_str(), list);
}

// ======================= ������ =======================
// �������� ���� ��������
void GyverPortal::showPage(String& s) {
    server.send(200, F("text/html"), s);
}

// ������� ����������� � �������� ��������
void GyverPortal::show() {
    _gp_ptr = this;
    if (*_build) _build();
    _gp_ptr = nullptr;
}

// ����� ������� (������ action)
String& GyverPortal::uri() {
    return req;
}

// ======================= PRIVATE =======================
// private:
void GyverPortal::checkList() {
    if (!list.idx) return;
    for (int i = 0; i < list.idx; i++) {
        if (list.skips[i] || form(list.forms[i])) {
            switch (list.types[i]) {
            case T_CSTR:    copyStr(list.names[i], (char*)list.vals[i]);        break;
            case T_STRING:  *(String*)list.vals[i] = getString(list.names[i]);  break;
            case T_TIME:    *(GPtime*)list.vals[i] = getTime(list.names[i]);    break;
            case T_DATE:    *(GPdate*)list.vals[i] = getDate(list.names[i]);    break;
            case T_CHECK:   *(bool*)list.vals[i] = getCheck(list.names[i]);     break;
            case T_BYTE:    *(int8_t*)list.vals[i] = (int8_t)getInt(list.names[i]); break;
            case T_INT:     *(long*)list.vals[i] = getInt(list.names[i]);       break;
            case T_FLOAT:   *(float*)list.vals[i] = getFloat(list.names[i]);    break;
            case T_COLOR:   *(uint32_t*)list.vals[i] = getColor(list.names[i]); break;
            }
        }
    }
}