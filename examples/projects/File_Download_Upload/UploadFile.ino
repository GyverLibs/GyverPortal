void UploadPage() {
  Serial.println("Load UploadPage()");
  portal.attachBuild(buildUploadPage);
  portal.show();
  portal.attachBuild(build);
}

void UploadedPage() {
  Serial.println("Load Uploaded_Page()");
  UploadPage();
  portal.server.send(200);
}

File UploadFile;
void FileUpload() { // upload a new file to the Filing system
  HTTPUpload& uploadfile = portal.server.upload();

  if (uploadfile.status == UPLOAD_FILE_START)
  {
    String filename = uploadfile.filename;
    if (!filename.startsWith("/")) filename = "/" + filename;
    Serial.print("Upload File Name: "); Serial.println(filename);
    SD.remove(filename);                         // Remove a previous version, otherwise data is appended the file again
    UploadFile = SD.open(filename, FILE_WRITE);  // Open the file for writing in SPIFFS (create it, if doesn't exist)
    filename = String();
  }
  else if (uploadfile.status == UPLOAD_FILE_WRITE)
  {
    if (UploadFile) UploadFile.write(uploadfile.buf, uploadfile.currentSize); // Write the received bytes to the file
  }
  else if (uploadfile.status == UPLOAD_FILE_END)
  {
    if (UploadFile)         // If the file was successfully created
    {
      UploadFile.close();   // Close the file again
      Serial.print("Upload Size: "); Serial.println(uploadfile.totalSize);
    }
  }
}
