void File_Download() {
  String name = portal.getString("dowl_way");
  if (name == "") name = TextDowload;
  SD_file_download(&name);
  Serial.println("Load UploadPage()");
  portal.attachBuild(buildDownloadPage);
  portal.show();
  portal.attachBuild(build);
}

void SD_file_download(String* filename) {
  Serial.println("Start Download:" + String(*filename));
  if (SD_present) {
    if (!SD.exists(String(*filename))) return;
    File download = SD.open(String(*filename));
    if (download) {
      portal.server.sendHeader("Content-Type", "text/text");
      portal.server.sendHeader("Content-Disposition", "attachment; filename=" + String(*filename));
      portal.server.sendHeader("Connection", "close");
      portal.server.streamFile(download, "application/octet-stream");
      download.close();
    }
  }
}
