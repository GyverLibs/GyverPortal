#include <GyverPortal.h>

void setup() {
  Serial.begin(9600);

  GPcolor color1;
  GPcolor color2(255, 0, 0);
  GPcolor color3(0xffaa00);

  color1.setRGB(0, 255, 0);
  // ~
  color1.r = 0;
  color1.g = 255;
  color1.b = 0;
  
  color2.setHEX(0xaabbcc);
  color3 = 0xfafafa;

  Serial.print(color1.r);
  Serial.print(',');
  Serial.print(color1.g);
  Serial.print(',');
  Serial.print(color1.b);
  Serial.println();

  Serial.println(color2.getHEX());
}

void loop() {
}
