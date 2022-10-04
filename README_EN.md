This is an automatic translation, may be incorrect in some places. See sources and examples!

# Gyver Portal v3
> **Library updated to version 3.0!**
There are changes in the logic of work and partial incompatibility with previous versions. Read the documentation, see examples.

## Documentation
Filed in [Repository Wiki](https://github.com/GyverLibs/GyverPortal/wiki)
> [English docs](https://github-com.translate.goog/GyverLibs/GyverPortal/wiki?_x_tr_sl=ru&_x_tr_tl=en)

![demo](/docs/GyverPortal.jpg)

Simple web interface builder for ESP8266 and ESP32
- Allows you to quickly create a universal web muzzle to manage and configure your device
- Ability to create multi-page and dynamic web interfaces in a few lines of code
- Does not require knowledge of HTML, CSS and JavaScript. All styles and scripts are already included in the library
- Does not require uploading files to SPIFFS, but styles and scripts can be loaded from there
- The page is assembled from ready-made components of the constructor directly in the sketch
- Lightweight, low dynamic memory usage during page generation
- Works on the basis of standard esp libraries, nothing additional needs to be installed
- Relatively stylish design, light and dark themes, the ability to customize some components
- Built-in modules:
  - Automated file upload
  - Automated file download
  - Caching files from SPIFFS memory
  - AJAX and jQuery (optional) updating values ​​on the page
  - Automatic polling and updating of variables
  - Reload page from sketch
  - Authorization on the server by login-password
  - DNS server (to work as an access point)
  - mDNS (to open an interface at a given address instead of IP)
  - OTA firmware and memory update via browser (password protection possible)
- Components of the constructor:
  - Decor
    - Title
    - Signature
    - Separator
    - Line break
  - Page layout
    - Block for combining components vertically (2 styles with a title, 2 without)
    - Block for combining components horizontally with alignment settings
    - Combine vertical blocks horizontally + responsive
    - Block macros to simplify constructor code
  - The form
    - Web form
    - submit button
  - Other components
    - Text on colored background
    - Text input field
    - Multiline text input field
    - Digit input field
    - Password entry field
    - Spinner (a field with a number and buttons + -)
    - Checkmark (checkbox)
    - Switch
    - Slider
    - Spoiler
    - Time selection
    - Date selection
    - Color selection
    - Dropdown list (dropbox)
    - Button
    - Link button
    - File download button
    - Button for uploading a file to the server
    - "LED" indicator of three types
    - Log window for debugging (web serial port)
    - Several types of graphs
    - FontAwesome or local icons for buttons
    - Dynamic navigation block with tabs
    - Navigation block with links
    - Blocks for displaying images, videos and text files from SPIFFS
    - Pop-ups: alert, prompt, confirm with sending action to the program
    - Tooltips for all components

![demo](/docs/demoBig.png)

### Compatibility
esp8266, esp32

### Known bugs
Some elements may not display nicely on Firefox. made for Chrome, SafariCranberry, Edge, Opera

## Content
- [Install](#install)
- [Versions](#versions)
- [Bugs and feedback](#feedback)

<a id="install"></a>
## Installation
- The library can be found by the name **GyverPortal** and installed through the library manager in:
    - Arduino IDE
    - Arduino IDE v2
    - PlatformIO
- [Download Library](https://github.com/GyverLibs/GyverPortal/archive/refs/heads/main.zip) .zip archive for manual installation:
    - Unzip and put in *C:\Program Files (x86)\Arduino\libraries* (Windows x64)
    - Unzip and put in *C:\Program Files\Arduino\libraries* (Windows x32)
    - Unpack and put in *Documents/Arduino/libraries/*
    - (Arduino IDE) automatic installation from .zip: *Sketch/Include library/Add .ZIP library…* and specify the downloaded archive
- Read more detailed instructions for installing libraries [here] (https://alexgyver.ru/arduino-first/#%D0%A3%D1%81%D1%82%D0%B0%D0%BD%D0%BE% D0%B2%D0%BA%D0%B0_%D0%B1%D0%B8%D0%B1%D0%BB%D0%B8%D0%BE%D1%82%D0%B5%D0%BA)
### Update
- I recommend always updating the library: new versions fix errors and bugs, as well as optimize and add new features
- Through the IDE library manager: find the library as during installation and click "Update"
- Manually: **delete the folder with the old version**, and then put the new one in its place. "Replacement" cannot be done: sometimes new versions delete files that remain after replacement and can lead to errors!


<a id="versions"></a>
## Versions
- v1.0
- v1.1 - improved graphics and styles
- v1.2
    - NUMBER block is now number type
    - Added a large text field AREA
    - added GPunix
    - Improved parsing
    - added BUTTON_MINI
    - Buttons can pass data from other components (except AREA and checkboxes)
    - added PLOT_STOCK - static chart with scale
    - Added AJAX_PLOT_DARK
    - Changed syntax for old graphs
    - Features GPaddUnix and GPaddInt for charts
    - Removed default theme
    - Tweaked styles
    - Added AREA_LOG log window and log function in general
- v1.3 - redesigned GPunix, minor fixes, PSTR can be used for lists
- v1.4 - minor fixes, click on COLOR now sends color
- v1.5 - added "slider+caption" block
- v1.5.1 - minor fix for copying strings
- v1.5.2 - added *meta charset="utf-8"*, English README (thanks to VerZsuT)
- v1.6 - added tools for working with color. Added answer() for date, time and color
- v1.7 - ESP32 support

- v2.0: Big update! The logic of work has been slightly changed, update your sketches!
    - Lots of optimization/lightening/speeding up
    - Full support for ESP32
    - Redesigned action polling logic (more correct and optimal + works on ESP32) while maintaining legacy
    - Removed DateTimeP (not used in the library) and moved separately to the DatePack library
    - Redesigned and simplified log module (log)
    - Added MDNS to not lookup board IP in port monitor (see doc)
    - Autodetect WiFi mode. Redesigned start() with legacy preservation (see doc)
    - Simplified builder, no need to create and pass a string (see doc)
    - Builder object is now called GP (instead of add) with preservation of legacy
    - Warnings fixed
    - Added conveniences for working with color GPcolor, date GPdate and time GPtime
    - Removed old color and date-time conversion functions (see doc)
    - Portal now returns color in GPcolor format, auto-update of variables also works with GPcolor
    - All examples tested on esp8266 and esp32

- v2.1
    - Returned the root() and uri() functions for the convenience of creating multi-page
    - Added example of multi-page organization
    - Added link button BUTTON_LINK
    - Added login-password authorization (see doc)
    - Added OTA firmware update from the browser, incl. with password (see doc)
    
- v3.0: A lot of new things, I couldn't list them all =)
    - Many thanks to DenysChuhlib and DAK85 for ideas and developments!
    - Added "object" mode rabots, in which components are more conveniently configured, automatically receive new values ​​and the program code becomes much more compact
    - The constructor mechanism has been completely rewritten, the assembly takes up many times less memory in SRAM due to sending the page in parts
    - The mechanism for adding custom code to the page has been redesigned
    - Constructor arguments now accept const String& - you can pass strings, const strings, F macro strings
    - Redesigned string utilities
    - Completely redesigned slider
    - Removed slider option with text and LABEL_MINI component
    - Added ability to set width for some components
    - Some components now have a "read-only" option
    - Redesign of LED GREEN / RED, added LED (red-green)
    - Added component BOX_BEGIN / BOX_END, which allows you to conveniently assemble components into groups with the desired size and alignment
    - Added LABEL_BLOCK block for highlighting text
    - Internal AJAX_CLICKS replaced with JS_TOP
    - The main page container has been redesigned for ease of customization for any interface width
    - Added navigation element for dynamic tabs NAV_TABS (+ NAV_BLOCK_BEGIN and NAV_BLOCK_END)
    - Added navigation element with NAV_TABS_LINKS link buttons
    - Added support for FontAwesome icons for buttons and navigation bar https://fontawesome.com/v4/icons/
    - Fixed a bug when using the old action polling script
    - AJAX_UPDATE renamed to UPDATE with preservation of legacy
    - Added FILE_UPLOAD block for uploading files to the server
    - Added a convenient mechanism for downloading files from SPIFFS memory with support for 33 file types
    - Added blocks for displaying images, videos and text files from SPIFFS
    - Examples renamed and grouped by meaning, new examples added
    - Added request mechanism
    - Added options for pluggable functions with address on GyverPortal
    - Added more convenient options for the SELECT component and ways to poll it (getSelectedIdx)
    - The update mechanism now works with SELECT blocks
    - Added a template for easy creation of custom blocks
    - Fixed work of clicks and updates on subpages
    - Added mini button-link + buttons for downloading files
    - Added offline mode for graphs (no internet connection needed)
    - Added block for adding styles from spiffs
    - SLIDER can now work with floats, added NUMBER_F for floats
    - Added element SPINNER
    - AREA now sends a click signal
    - Added macros for easy assembly of blocks
    - And other stuff

<a id="feedback"></a>
## Bugs and feedback
When you find bugs, create an **Issue**, or better, immediately write to the mail [alex@alexgyver.ru](mailto:alex@alexgyver.ru)
The library is open for revision and your **Pull Request**'s!

When reporting bugs or incorrect work of the library, it is necessary to indicate:
- Library version
- What MK is used
- SDK version (for ESP)
- Arduino IDE Version
- Whether built-in examples work correctly, which use functions and constructs that lead to a bug in your code
- What code was loaded, what work was expected from it and how it works in reality
- Ideally, attach the minimum code in which the bug is observed. Not a canvas of a thousand lines, but a minimal code
