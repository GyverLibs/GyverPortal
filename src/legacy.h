#pragma once

#include "builder.h"
#include "utils.h"

void BUILD_BEGIN(int width = 350);
void BUILD_END();
void GP_BUILD(__attribute__((unused)) String& s);
void BUILD_BEGIN(__attribute__((unused)) String& s);
void GP_SHOW();

String encodeDate(GPdate d);
String encodeTime(GPtime t);
String encodeColor(GPcolor c);