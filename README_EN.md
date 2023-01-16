This is an automatic translation, may be incorrect in some places. See sources and examples!

# Gyverportal
Simple web interfaces designer for ESP8266 and ESP32
! [IMG] (/doc/feature.png)

## compatibility
> ### If it does not comply with the ESP32 - upgrade the core, it's time
- ESP8266 (SDK V2.7+)
- ESP32 (SDK V2+)

## Documentation
Now it is in [Wiki repository] (https://github.com/gyverlibs/gyverPortal/wiki).The documentation is currently incomplete, at the stage of writing!
> [English docs] (https://github-com.translate.goog/gyverlibs/gyverPortal/wiki?_x_tr_sl=ru&_x_tr_tl=en)

## Ideas/Problems/what will happen in the next update
Read/offer in Issue https://github.com/gyverlibs/gyverPortal/issues/55
- You can also [discuss on the forum] (https://community.alexgyver.ru/threads/gyverportal.6632/)

### famous bugs
Some elements may ugly display Firefox, becauseMade under Chrome, Safari, Edge, Opera

### Development on host
Examples can be compiled and excuted on host by starting `../../onhost [-h]` from any example directory

## Capabilities
! [Demo] (/doc/gyverportal.jpg)

Universal web designer for ESP8266 and ESP32:
- allows you to quickly create webmord for controlling and setting up an electronic device
- works on the basis of standard ESP libraries, nothing needs to be installed additionally
- the creation of multi-page and dynamic web interfaces in several lines of code right in the sketch
- does not require knowledge HTML, CSS and JavaScript: all styles and scripts are already laid down in the library
- written on pure HTML + CSS + JS, no third-party web library
- does not require connection to the Internet
- does not require downloading files in spiffs (but styles and scripts can be downloaded from there to accelerate work)
- relatively stylish design, light and dark themes + “control panel” template with a drop -down menu, customization of components (size, color)
- Built -in modules:
  - Interaction with the browser: receiving and sending values on components, indication "offline", change in the name of the page
  - several dozen standard components, instruments of miles and navigation (buttons, icons, graphs, sliders, tables, tabs ...)
  - Automated download of files from Spiffs + Caching
  - Automated file download in spiffs
  - System Information output block and file manager (SD, Littlefs, Spifs ...)
  - HTML Canvas API with the ability to draw from a sketch + drawing API as in Processing
  - Real time hours (based on browser time)
  - Automatic survey and updating of variables from the list
  - Authorization on the server by login-Parol
  - DNS server (for work as an access point)
  - MDNS (to open the interface at a given address instead of IP)
  - OTA updating the firmware and memory through a browser or Curl (password protection is possible)

! [Demo] (/doc/demobig.png)

## Content
- [installation] (# Install)
- [versions] (# Versions)
- [bugs and feedback] (#fedback)

<a id="install"> </a>
## Installation
- The library can be found by the name ** GyverPortal ** and installed through the library manager in:
    - Arduino IDE
    - Arduino ide v2
    - Platformio
- [download the library] (https://github.com/gyverlibs/gyverPortal/archive/refs/heads/main.zip) .ZIP archive for manual installation:
    - unpack and put in * C: \ Program Files (X86) \ Arduino \ Libraries * (Windows X64)
    - unpack and put in * C: \ Program Files \ Arduino \ Libraries * (Windows X32)
    - unpack and put in *documents/arduino/libraries/*
    - (Arduino id) Automatic installation from .Zip: * sketch/connect the library/add .Zip library ... * and specify downloaded archive
- Read more detailed instructions for installing libraries [here] (https://alexgyver.ru/arduino-first/#%D0%A3%D1%81%D1%82%D0%B0%BD%D0%BE%BE%BE%BE%BE%BED0%B2%D0%BA%D0%B0_%D0%B1%D0%B8%D0%B1%D0%BB%D0%B8%D0%BE%D1%82%D0%B5%D0%BA)

### Update
- I recommend always updating the library: errors and bugs are corrected in the new versions, as well as optimization and new features are added
- through the IDE library manager: find the library how to install and click "update"
- Manually: ** remove the folder with the old version **, and then put a new one in its place.“Replacement” cannot be done: sometimes in new versions, files that remain when replacing are deleted and can lead to errors!
- ** when using files and scripts, they also need to be replaced with new **

<a id="versions"> </a>
## versions
<details>
<summary> Old </ Summary>

## v1.1
- improved graphs and styles

## v1.2
- Number block now Number type
- added a large text field Area
- added GPunix
- I improved Parsing
- added Button_mini
- buttons can transmit data from other components (except AREA and checkboxes)
- added PLOT_STOCK - Static schedule with scale
- added ajax_plot_dark
- changed syntax in old graphs
- GPADDUNIX and GPADDINT for graphs
- removed Default theme
- twisted styles
- added the window of the Loga Area_log and the function of the log as a whole

## v1.3
- converted GPunix, small fixes, for lists you can use PSTR

## v1.4
- Small fixes, click on Color now sends color

## v1.5
- added the block "Slider+signature"

## v1.5.1
- Small fixing fixing lines

## v1.5.2
- Added *Meta Charset = "UTF-8" *, English Readme (thanks Verzsut)

## v1.6
- Added tools for working with color.Added Answer () for date, time and color

## v1.7
- ESP32 support

## v2.0:
- Great update!The logic of work is slightly changed, renew your sketches!
- A lot of optimization/relief/acceleration
- full support ESP32
- the logic of a survey of actions (more correctly and optimally + works on ESP32) was redone with the preservation of Legasi
- removed Datetimep (not used in the library) and put it separately into the Datepack library
- Redeled and lightened by the LOG module (log)
- Added MDNS so as not to look for an IP board in the port monitor (see Dok)
- Authority of the WIFI operating mode.Redeled Start () with the preservation of Legashi (see Dok)
- a builder is simplified, the line is not necessary to create and transmit (see document)
- the builder object is now called GP (instead of Add) with the preservation of Legashi
- Varinings are featured
- Added amenities for working with GPColor color, GPDATE and GPTIME time
- the old functions of color transformation and date-time are removed (see document)
- The portal now returns the color in GPColor format, the auto -updating of variables also works with GPColor
- All examples are tested on ESP8266 and ESP32

## v2.1
- returned the functions of Root () and URI () for the convenience of creating multi -page
- added an example of the organization of multi -page
- Added button-link Button_Link
- added authorization by login-paralle (see Dok)
- Added OTA updating the firmware from the browser, includingwith password (see Dok)

## v3.0: A lot of new things
- Many thanks to Denyschuhlib and Dak85 for ideas and developments!
- added "object" mode of operation in which the components are more convenient to configure, automatically receive new values and the program code becomes much more compact
- the designer mechanism is completely rewritten, the assembly takes many times less memory in SRAM due to sending the page in parts
- the mechanism for adding custom code to the page was converted
- The designer arguments are now taking COST String & - you can transmit lines, const lines, f macro lines
- Reded string utals
- Completely converted the slider
- the version of the slider with the text and the component Label_mini is removed
- Added the ability to set the width of some components
- Some components have the option "only reading"
- RediZain LED Green/Red, added LED (red-green)
- Added component Box_begin/Box_end, which allows us to conveniently assemble components in groups with the right size and alignment
- added Label_block block to highlight the text
- internal ajax_clicks replaced by js_top
- Reded the main container of the page for the convenience of customization for any interface width
- added an element of navigation by dynamic tabs nav_tabs (+ nav_block_begin and nav_block_end)
- added an element of navigation with nav_tabs_links
- Added support for fontaWesome icons for buttons and navigation panels https://fontawesome.com/v4/icons/
- Fent a bug when using the old scenario of a survey of actions
- Ajax_update is renamed Update while preserving Legashi
- added File_upload block to download files to the server
- Added a convenient mechanism for downloading files from spifs memory with support for 33 types of files
- Added blocks for displaying images, video and text files from Spiffs
- examples are renamed and grouped in meaning, new examples have been added
- Added the REQUEST mechanism
- connected functions added options with the address on GyverPortal
- Added more convenient options for the SELECT component and the methods of polling it (GETSELECTEDIDIDX)
- Update mechanism now works with Select blocks
- added a template for a convenient creation of custom blocks
- Fixed the work of clicks and updates on the dungeons
- Added mini-link + buttons for downloading files
- added offline mode for graphs (no connection to the Internet)
- Added block to add styles from Spiffs
- Slider now knows how to work with Float, add Number_f for Float
- Added Spinner element
- Area now sends the Click signal
- added macros for convenient assembly blocks
- and so on

## v3.1
- Filled GetBool ()

## v3.2
- This time, GetBool ()/CopyBool () for Switch/Check is fully featured
- The Update () mechanism is completely redone - now it works several times faster and updates all these components at the same time!

## V3.3 Table Update
- improved the work of parsers
- improved built -in js scripts
- Prefix Macro is reduced from gp_make_ to m_, syntax backlight is replaced by fat
- GP_edges is replaced by gp_Justify, span has alignment now also set through GPalign
- Added URL Encode, now you can insert a text with "dangerous" symbols (+#<> `etc.)
- Switch component can now be set color
- Click/Update/Copy Ints now works with all integer types (int, byte, long ...)
- Added Form_Send and Form_Send_Mini - a new version of sending a form without a redirect
- Added Reload_Click to reboot the page by click on the specified components
- Added styles "disconnected" components
- added Slider_C, sending values in the process of changing the position
- Pressing and releasing a button now works from a smartphone (touchscreen)
- The style of Spinner has been converted, now it is more compact
- Added tables (Table_Begin, Table_end, TD, TR), Macros (gp_make_table, gp_make_td, gp_make_tr, gp_als)
- You can configure the width of the log
- For Text, attributes "Pattern" and the maximum input length are added.** The last argument has changed in the function **
- Added submit_mini
- added a real local time module (a request from the browser), the GetsyStemDate (), Getsystemtime (), as well as Getunix ()
- Improved update, can be sewn through Curl
- the Update mechanism is slightly optimized, he also cuts out extra spaces in the list
- Folder_upload () now works on ESP32
- added File_manager () - output of the list of files from memory with buttons delete, as well as deletefile (), Deleteauto (), Deletepath () handlers.
- Added components Plain () and Bold () to output the text
- Added component System_info () - output of the table with systemic information
- Added "Eye" for the Password Entry Field
- Added color Gray_b
- Blocks Block ... are now created by one component.Thin block has added color settings
</details>

## v3.4 ui update
- Added
  - HTML CanVAS (drawing in a browser) + Sketch update + Processing API
  - Page marking in the style of control panel with a side menu, components ui_begin, ui_menu, ui_body, ui_end, ui_link
  - update_click () - calls update in the specified components when clicking on the specified components
  - OTA.ERROR () to output the text of the error in any place on the page
  - Eval () execution of the code sent to Update JS
  - GyverPortal :: setfs ()
  - Choosing color for icons, File_upload, folder_upload, OTA_FIRMWARE, OTA_FileSYSTEM, TITLE, LABEL, Span, Plain, Bold and HR
  - Max.Width for Grid
  - js_begin () and js_end ()
  - in Build_begin_file () and Build_begin () you can convey the topic of design
  - page_title () - changing the name of the tab in the browser, includingAccording to update
  - Color GP_WHITE
  - Icon_button () and icon_file_button () icon icons.
  - online_check () - display of the state of the connection with ESP
  - signatures to the axes of ajax_plot
  - GyverPortal :: Caching (Bool), GyverPortal :: Clearcache ()
  - processing of buttons, separate Hold () processor ()
- Corrected
  - Small bugs
  - Hint () for Button, Switch and Upload
  - Potential bug in the Update mechanism
  - icon_file ()
- Improved
  - Color can now be parsed as int
  - Significantly accelerated (caching) loading the page without scripts/styles in spiffs
  - When updating the library, you do not need to clean the browser cache
  - File_manager (): File deletion now does not change the URL in the browser + added buttons download and rename + choice of directory
  - Improved button release processing
  - Spinner (): Keeping buttons, setting speed in gp.setspinnerperiod ()
  - removed the transfer of the line in the last line to the log
- changed
  - The logic of the work of pop -up windows, the argument is cleaned!
  - Spinner (): the width is removed, auto -width is made, the value in the center
  - Automatic download/load/deletion/renaming now turned on by default!
  - sorted examples, Portal is renamed UI for brevity
  - Clickup and Clickdown are put into Hold () handler

<a id="feedback"> </a>
## bugs and feedback
Create ** Issue ** when you find the bugs, but better immediately write to the mail [alex@alexgyver.ru] (mailto: alex@alexgyver.ru)
The library is open for refinement and your ** pull Request ** 'OV!

When reporting about bugs or incorrect work of the library, it is necessary to indicate:
- The version of the library
- What is MK used
- SDK version (for ESP)
- version of Arduino IDE
- whether the built -in examples work correctly, in which the functions and designs are used, leading to a bug in your code
- what code has been loaded, what work was expected from it and how it works in reality
- Ideally, attach the minimum code in which the bug is observed.Not a canvas of a thousand lines, but a minimum code