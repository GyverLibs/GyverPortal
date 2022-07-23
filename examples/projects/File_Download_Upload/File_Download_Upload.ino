
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#include <SD.h>
#include <SPI.h>

#include <GyverPortal.h>
GyverPortal portal;

String TextDowload;
String WayDowload = "/";

bool  SD_present = true;

void setup() {
  Serial.begin(115200);

  WiFi.begin("ssid", "pass");

  Serial.println("Connecting ...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(250); Serial.print('.');
  }
  Serial.println("\nConnected to " + WiFi.SSID() + " Use IP address: " + WiFi.localIP().toString());

  SDLoop();

  portal.server.on("/download", File_Download);
  portal.server.on("/upload_file",  HTTP_POST, UploadedPage , FileUpload);

  portal.attachBuild(build);
  portal.start();

  Serial.println("HTTP server started");

  File root = SD.open("/");

  printDirectory(root, 0);

}

void loop() {
  SDLoop();

  portal.tick();
  if (portal.click()) {
    String name = portal.clickName();
    if (name.startsWith("OpenFile:")) {
      String filename = name.substring(9);
      OpenFileList(&filename);
    }
  }
  if (portal.update()) {
    if (portal.update("dowl_way_l")) portal.answer((WayDowload == "No SD card") ? WayDowload : ("SD:" + WayDowload + ((WayDowload != "/") ? "/" : "")));
  }
}
