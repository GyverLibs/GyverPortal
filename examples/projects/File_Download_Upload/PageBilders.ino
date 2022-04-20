// билдер страницы
void build() {
  String s;
  BUILD_BEGIN(s, "File Server");
  add.THEME(GP_DARK);

  add.AJAX_UPDATE(PSTR("dowl_way_l"));


  add.FORM_BEGIN("/files/upload_file");
  add.BLOCK_BEGIN();
  add.UPLOAD_BUTTON("upload_file", "Upload File", "Send", "upload_label");
  add.BLOCK_END();
  add.FORM_END();


  add.BREAK();//--------------------------------------------------

  add.FORM_BEGIN("/files/download");
  add.BLOCK_BEGIN();
  add.TEXT("dowl_way", "Way to file", TextDowload.c_str());
  add.BREAK();
  add.SUBMIT("Download");
  add.BLOCK_END();
  add.FORM_END();


  add.BREAK();//-------------------------------------------------


  add.BLOCK_BEGIN();
  add.LABEL(String((WayDowload == "No SD card") ? WayDowload : ("SD:" + WayDowload + ((WayDowload != "/") ? "/" : ""))).c_str(), "dowl_way_l");
  add.BREAK();
  add.BLOCK_END();


  add.SCROLL_BOX();
  add.HREF_BEGIN("/");

  ///
  {
    if (SD_present) {

      byte PhaseChekFile;
      File Directory = SD.open(WayDowload);

      if (String(Directory.name()) == "/") PhaseChekFile = 0;
      else PhaseChekFile = 1;

      if (PhaseChekFile == 1) add.BUTTON_SELECT(String("OpenFile:/").c_str(), String("Back to SD Folder").c_str());

      while (true) {
        File entry =  Directory.openNextFile();

        if (! entry) {
          // no more files
          break;
        }

        if (PhaseChekFile == 1 ) {
          if (entry.isDirectory()) add.BUTTON_SELECT(String("OpenFile:" + String(entry.name())).c_str(), String(String(entry.name()) + "  Folder").c_str());
          else add.BUTTON_SELECT(String("OpenFile:" + String(entry.name())).c_str(), String(String(entry.name())).c_str());
        } else PhaseChekFile = 1;
        entry.close();
      }
      Directory.close();

    } else add.LABEL("No SD card");
  }

  ///
  add.HREF_END();
  add.BLOCK_END();


  BUILD_END();
}

void buildUploadPage() {
  String s;
  BUILD_BEGIN(s, "File Server");
  add.THEME(GP_DARK);

  add.OPEN_WEB_PAGE("/", 500);

  BUILD_END();
}

void buildDownloadPage() {
  String s;
  BUILD_BEGIN(s, "File Server");
  add.THEME(GP_DARK);

  add.OPEN_WEB_PAGE("/", 500);

  BUILD_END();
}
