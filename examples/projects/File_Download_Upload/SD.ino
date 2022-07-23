void SDLoop() {
  SD.end(false);
  if (!SD.begin(D8)) {
    if (SD_present) {
      Serial.println("SD card initialization failed!");
      WayDowload = "No SD card";
      SD_present = false;
    }
  }  else  {
    if (!SD_present) {
      Serial.println("SD card initialization done.");
      WayDowload = "/";
      SD_present = true;
    }
  }
}
