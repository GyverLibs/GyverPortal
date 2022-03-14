#include <log.h>

void GPlog::start(int n = 64) {
    stop();
    size = n;
    buffer = new char[size];
    state = 1;
}
GPlog::~GPlog() {
    stop();
}
void GPlog::stop() {
    if (buffer) delete[] buffer;
    state = 0;
}

// ������ � �����
size_t GPlog::write(uint8_t n) {
    if (!state) return 1;
    uint8_t i = (head + 1) % size;  // ��������� ������ �������� � ������
    if (i != tail) {         		// ���� ���� ��������
        buffer[head] = n; 		    // ����� � �����
        head = i;              		// ������� ������
    }
    return 1;
}

// ������ �� ������
char GPlog::read() {
    if (!state) return 0;
    if (head == tail) return 0;   // ����� ����
    char val = buffer[tail];      // ���� � ������
    tail = (tail + 1) % size;     // ����� �������
    return val;                   // ����������
}

// ����� ���������� ������������� ���������
int GPlog::available() {
    if (!state) return 0;
    else return (size + head - tail) % size;
}

// "�������" ������
void GPlog::clear() {
    head = tail = 0;
}