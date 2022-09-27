#include "legacy.h"
extern Builder GP;

// собирать страницу
void BUILD_BEGIN(int width) {
    GP.BUILD_BEGIN(width);
}

// завершить сборку
void BUILD_END() {
    GP.BUILD_END();
}

void GP_BUILD(__attribute__((unused)) String& s) {}
void BUILD_BEGIN(__attribute__((unused)) String& s) {
    BUILD_BEGIN();
}
void GP_SHOW() {
}

// encoders
String encodeDate(GPdate d) {
    return d.encode();
}
String encodeTime(GPtime t) {
    return t.encode();
}
String encodeColor(GPcolor c) {
    return c.encode();
}