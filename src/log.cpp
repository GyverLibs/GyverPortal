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

// запись в буфер
size_t GPlog::write(uint8_t n) {
    if (!state) return 1;
    uint8_t i = (head + 1) % size;  // положение нового значения в буфере
    if (i != tail) {         		// если есть местечко
        buffer[head] = n; 		    // пишем в буфер
        head = i;              		// двигаем голову
    }
    return 1;
}

// чтение из буфера
char GPlog::read() {
    if (!state) return 0;
    if (head == tail) return 0;   // буфер пуст
    char val = buffer[tail];      // берём с хвоста
    tail = (tail + 1) % size;     // хвост двигаем
    return val;                   // возвращаем
}

// вернёт количество непрочитанных элементов
int GPlog::available() {
    if (!state) return 0;
    else return (size + head - tail) % size;
}

// "очистка" буфера
void GPlog::clear() {
    head = tail = 0;
}