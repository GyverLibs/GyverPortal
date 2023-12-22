This is an automatic translation, may be incorrect in some places. See sources and examples!

# Gyverportal
### v3.6.6 (23.04.2023)
Simple web designer for ESP8266 and ESP32
! [IMG] (/doc/feature.png)

| 🔥🔥🔥 <br> ** Pay attention to the new library [gyverhub] (https://github.com/gyverlibs/gyverhub) - this is like gyverPortal, only much easier, more convenient, works through the Internet and integrates with a smart home!** <br> 🔥🔥🔥 |
|--- |

## compatibility
> ### If you do not compose on ESP32 - upgrade the core, it's time.Yes, [here] (https://github.com/espressif/arduino-esp32)
- ESP8266 (SDK V2.7+)
- ESP32 (SDK V2+)

## Documentation
Now it is in [Wiki repository] (https://github.com/gyverlibs/gyverPortal/wiki).The documentation is currently incomplete, at the spelling!
> [English docs] (https://github-com.translate.goog/gyverlibs/gyverPortal/wiki?_x_tr_sl=ru&_x_tr_tl=en)

## Ideas/Problems/Actual version
- The working version (unstable/experimental/night) can be downloaded [from the repository] (https://github.com/gyverlibs/gyverportal/archive/heads/main.zip)
- Changes in the next version Read/offer in Issue https://github.com/gyverlibs/gyverportal/issues/58
- You can also [discuss on the forum] (https://community.alexgyver.ru/threads/gyverportal.6632/)

### famous bugs
Some elements may ugly display Firefox, becauseMade under Chrome, Safari, Edge, Opera

### Development on the host
Examples can be compiled and executed on the host by launching `../../onhost [-h]` from any catalog of examples.

## Possibilities
! [Demo] (/doc/gyverPortal.jpg)

Universal Web Interface Designer for ESP8266 and ESP32:
- allows you to quickly create a webmord for controlling and setting up an electronic device
- works on the basis of standard ESP libraries, nothing needs to be installed additionally
- the creation of multi-page and dynamic web interfaces in several lines of code directly in the sketch
- does not require knowledge HTML, CSS and JavaScript: all styles and scripts are already laid down in the library
- Written on pure HTML + CSS + JS, no third-party web library
- does not require connection to the Internet
- does not require downloading files in spiffs (but styles and scripts can be downloaded from there to save memory)
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
  - OTA updating the firmware and memory through a browser or CURL (it is possibleCranberries ashita password)

! [Demo] (/doc/demobig.png)

## Content
- [installation] (# Install)
- [versions] (#varsions)
- [bugs and feedback] (#fedback)

<a id="install"> </a>
## Installation
- The library can be found by the name ** GyverPortal ** and installed through the library manager in:
    - Arduino ide
    - Arduino ide v2
    - Platformio
- [download the library] (https://github.com/gyverlibs/gyverPortal/archive/refs/heads/main.zip) .Zip archive for manual installation:
    - unpack and put in * C: \ Program Files (X86) \ Arduino \ Libraries * (Windows X64)
    - unpack and put in * C: \ Program Files \ Arduino \ Libraries * (Windows X32)
    - unpack and put in *documents/arduino/libraries/ *
    - (Arduino id) Automatic installation from. Zip: * sketch/connect the library/add .Zip library ... * and specify downloaded archive
- Read more detailed instructions for installing libraries [here] (https://alexgyver.ru/arduino-first/#%D0%A3%D1%81%D1%82%D0%B0%BD%D0%BE%BE%BE%BED0%B2%D0%BA%D0%B0_%D0%B1%D0%B8%D0%B1%D0%BB%D0%B8%D0%BE%D1%82%D0%B5%D0%BA)

### Update
- I recommend always updating the library: errors and bugs are corrected in the new versions, as well as optimization and new features are added
- through the IDE library manager: find the library how to install and click "update"
- Manually: ** remove the folder with the old version **, and then put a new one in its place.“Replacement” cannot be done: sometimes in new versions, files that remain when replacing are deleted and can lead to errors!
- ** When using files and scripts in spiffs, they also need to be replaced with new **
- After the update, you need to clean the browser cache, just press Ctrl+F5 for the Chrome on the loaded page of the portal

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
- Improved Parsing
- added Button_mini
- buttons can transmit data from other components (except AREA and Chekboxes)
- added PLOT_STOCK - Static schedule with scale
- added ajax_plot_dark
- changed syntax in old graphs
- GPadDunix and GPaddint for graphs
- removed Default theme
- twisted styles
- added the window of the Loga Area_log and the function of the log as a whole

## v1.3
- converted GPunix, small fixes, for lists you can use PSTR

## v1.4
- Small fixes, click on Color now sends color

## v1.5
- added the Slider+signature block

## v1.5.1
- Small fixing fixation

## v1.5.2
- Added *Meta Charset = "UTF-8" *, English Readme (thanks Verzsut)

## v1.6
- Added tools for working with color.Added Answer () for date, time and color

## v1.7
- ESP32 support

## v2.0:
- Great update!The logic of work is slightly changed, renew your sketches!
- A lot of optimization/relief/acceleration
- Full support ESP32
- the logic of a survey of actions (more correctly and optimally + works on ESP32) was redone with the preservation of Legasi
- removed Datetimep (not used in the library) and put it separately to the Datepack library
- Redeled and lightened by the LOG module (log)
- Added MDNS so as not to look for an IP board in the port monitor (see Dok)
- Authority of the WIFI operating mode.Redeled Start () with the preservation of Legasi (see Dok.
- Simplified by the builder, the line is not necessary to create and be transmitted (see document)
- the builder object is now called GP (instead of Add) with the preservation of Legasi
- Boinings are featured
- Added amenities for working with GPColor color, GPDATE and GPTIME time
- Remove the old functions of transformation of color and date-time (see document)
- The portal now returns the color in the GPColor format, the auto renewal of variables also works with GPColor
- All examples are tested on ESP8266 and ESP32

## v2.1
- returned the functions of Root () and URI () for the convenience of creating multi -page
- added an example of the organization of multi -page
- Added button-lifeline button_Link
- added authorization by login-paralle (see Dok.
- Added OTA updating the firmware from the browser, includingwith password (see beforeCranberries)

## v3.0: A lot of new things
- Many thanks to Denyschuhlib and Dak85 for ideas and developments!
- added "object" mode of operation in which the components are more convenient to configure, automatically receive new values and the program code becomes much more compact
- the designer mechanism is completely rewritten, the assembly takes many times less memory in SRAM due to sending the page in parts
- the mechanism for adding custom code to the page was converted
- The designer arguments are now taking COST string & - you can transmit lines, const lines, f macro lines
- Reded string utilities
- Completely converted the slider
- removed the version of the slider with the text and the component Label_mini
- added the possibility of setting width to some components
- Some components have an option "only reading"
- Redizin LED Green/Red LEDs, added LED (red-green)
- Added component BOX_BEGIN/BOX_end, which allows us to conveniently assemble components in groups with the right size and alignment
- added Label_block block to highlight the text
- internal ajax_clicks replaced by js_top
- Reded the main container of the page for the convenience of customization for any interface width
- added an element of navigation by dynamic tabs nav_tabs (+ nav_block_begin and nav_block_end)
- added an element of navigation with nav_tabs_links
- Added support for fontaWesome icons for buttons and navigation panels https://fontawesome.com/v4/icons/
- Filled a bug when using the old scenario of a survey of actions
- Ajax_update is renamed Update while preserving Legashi
- added File_upload block to download files to the server
- Added a convenient mechanism for downloading files from SPIFFS memory with support for 33 types of files
- Added blocks for displaying images, video and text files from Spiffs
- examples are renamed and grouped in meaning, new examples have been added
- Added the REQUEST mechanism
- Connected functions added options with the address on GyverPortal
- Added more convenient options for the SELECT component and the methods of polling it (GetseldDididx)
- Update mechanism now works with Select blocks
- Added a template for a convenient creation of custom blocks
- Fixed the work of clicks and updates on the dungeons
- Added mini-link + buttons for downloading files
- added offline mode for graphs (you do not need to connect to the Internet)
- Added block to add styles from Spiffs
- Slider now knows how to work with Float, add Number_f for Float
- Added an element Spinner
- Area now sends the Click signal
- Added macros for convenient assembly blocks
- and so on

## v3.1
- Filled GetBool ()

## v3.2
- This time, GetBool ()/CopyBool () for Switch/Check is fully filled
- The Update () mechanism is completely redone - now it works several times faster and updates all these components at the same time!

## V3.3 Table Update
- Improved work of parsers
- improved built -in js scripts
- Prefix Macro is reduced from gp_make_ to m_, syntax backlight is replaced by fat
- GP_EDGES is replaced by gp_Justify, at Span the alignment is now also set through GPalign
- Added URL Encode, now you can insert a text with "dangerous" symbols (+#<> `etc.)
- Switch component can now be set color
- Click/Update/Copy Ints now works with all integer types (int, byte, long ...)
- Added Form_Send and Form_Send_Mini - a new version of sending a form without a redirect
- Added Reload_Click to reboot the page by click on the specified components
- Added styles "disconnected" components
- added slider_c, sending values in the process of changing the position
- Pressing and releasing the button now works from a smartphone (touchscreen)
- The style of Spinner has been converted, now it is more compact
- Added tables (Table_Begin, Table_end, TD, TR), Macros (gp_make_table, gp_make_td, gp_make_tr, gp_als)
- You can configure the width of the log
- For Text, attributes "Pattern" and the maximum input length are added.** The last argument has changed in the functionAI **
- Added submit_mini
- added a real local time module (request from the browser), the GetSystemDate (), GetsySteMtime (), as well as Getunix ()
- Improved update, can be sewn through curl
- the UPDate mechanism is slightly optimized, he also cuts out extra spaces in the list
- FOLDER_UPLOAD () now works on ESP32
- added File_manager () - output of the list of files from memory with buttons delete, as well as deletefile (), Deleteauto (), Deletepath () handlers.
- Added components Plain () and Bold () to output the text
- Added component System_info () - output of the table with system information
- Added "eye" for the password entry field
- Added color Gray_b
- Blocks Block ... are now created by one component.Thin block has added color settings

## v3.4 ui update
- Added
  - HTML canvas (drawing in the browser) + Sketch update + Processing API
  - Page marking in the style of control panel with a side menu, components ui_begin, ui_menu, ui_body, ui_end, ui_link
  - update_click () - calls update in the specified components when clicking on the specified components
  - OTA.ERROR () to output the text of the error in any place on the page
  - Eval () execution of the code sent to Update JS
  - GyverPortal :: setfs ()
  - Choosing color for icons, File_upload, folder_upload, OTA_FIRMWARE, OTA_FileSYSTEM, TITLE, LABEL, Span, Plain, Bold and HR
  - Max.Width for Grid
  - js_begin () and js_end ()
  - in Build_begin_file () and Build_Begin () you can convey the topic of design
  - page_title () - changing the name of the tab in the browser, includingAccording to update
  - Color GP_WHITE
  - Icon_button () and icon_file_button () icon icons.
  - online_check () - display of the state of the connection with ESP
  - signatures to the axes of ajax_plot
  - Gyverportal :: Caching (Bool), GyverPortal :: Clearcache ()
  - processing of buttons, separate Hold () processor ()
  - ESP32 CAM support, stream output from the camera (Camstream.h + gp.cam_stream)
- Corrected
  - Small bugs
  - Hint () for Button, Switch and Upload
  - Potential bug in the Update mechanism
  - icon_file ()
- Improved
  - Optimization of styles and scripts
  - Color can now be parsed as int
  - Significantly accelerated (caching) loading the page without scripts/styles in spiffs
  - When updating the library, you do not need to clean the browser cache
  - File_manager (): File deletion now does not change the URL in the browser + added buttons download and rename + choice of directory
  - Improved button release processing
  - Spinner (): Keeping buttons, setting speed in gp.setspinnerperiod ()
  - removed the transfer of the line in the last line in log
  - Actualized Objects, an example has been added
- changed
  - the logic of the work of pop -up windows, the argument is removed
  - Spinner (): the width is removed, auto -haired is made, value in the center
  - Automatic download/load/removal/renaming now turned on by default!
  - sorted examples, Portal is renamed UI for brevity
  - Clickup and Clickdown are taken to the Hold () handler
  - removed the outdated code (up to V3)
  - Obj suffixes were removed from undocumented functions
  - CHECK and Switch can be chosen color, the order of arguments is changed
  - the order of arguments in GPLISTIDX is changed, made more logical

## v3.5 Christmas update

- Added
  - display of the boundaries of the Table_Border () table
  - Dangerous Copy and Click Parsers (for a survey in the condition)
  - Width for area
  - a pop -up window with an error if the "click" did not reach the server
  - You can get a login and password that are introduced during authorization, login () and pass ()
  - Settings of the size/thickness of the text and transfer for text signatures Title, Label, Label_block.SPAN/Bold/Plain signatures are no longer needed
  - ENCODEDMY for gpdate (day. Messenger)
  - Color gp_yellow_b
  - Color setup for LED
  - Reload_click works with Popup (Alert, Prompt, Confirm)
  - check whether the client is connected, the Online () and onlineout () functions are connected
  - The weblog has added buttons for cleaning and stopping scrolling
  - Radio component for selection lists
- Fented
  - Eye in Password () put in place
  - Bugwith macros m_tr10
  - Spinner () obeys alignment
  - The System_info table returned to a compact look
  - Hint (broke in v3.4)
  - incredible bug with eternal Check () eternal loading and the problem with a survey of its value from the form
- Improved
  - Optimized scripts
  - Automatic removal of spaces in the Update () list ()
  - the width of the window Cam_stream () and Cam_stream_window can be set with stringing
  - Design of File_manager signs
  - the mechanism of reloading the page has been converted, now it works much faster and more stable
  - Update requests updates only when the browser window is actively
  - Update, upon loss of communication, signals a pop -up banner
  - hidden empty cells at the table with specified widths
- changed
  - in Spinner () returned width with the penultimate argument, its installation turns off the car nap
  - online_check () now issues a pop -up banner instead of the icon in the name of the page
  - removed setreloadtimeout (), the mechanism is improved, the general timaut in Settimeout () is set
  - Label_block design to differ from buttons
  - The password field with the "eye" is now called by the Pass_eye component

## v3.5.2 Fix
- Added
  - Radio object
- Fented
  - Returned automatic width to the fields input fields
  - Forced display of seconds in Time
  - ESP32 compilation error
  - An example of Wifilogin
- Improved
  - Disabled zoom on mobile devices
  
## v3.5.3 Fix
- Fented
  - Critical bug in GPFileetype and sending a file to 8266
  - did not work through the file manager on ESP32
- Improved
  - The buttons inside the form do not lead to the submit (Bagofica HTML)
  
</details>

## v3.6 minor
- Added
    - Optional alignment at the top of Box
    - Added type of block gp_div_RAW - without indentation
- Improved
    - Date and time decoders
    - Parsers were transferred to a separate file for the needs of other libraries
    
## v3.6.1
- Fixed a Parsers compilation error in IDE V2
- Added more debugging opportunities for OTA

## v3.6.2
- Fixed compilation errors:
    - in Arduino ide v2 for ESP32
    - For ESP8266 version of SDK below 3.1
    
## v3.6.3
- Parser of retention is put into a separate file for the needs of other libraries

## v3.6.4
- Added
  - System Info in the uptime column also displays days
  - GPLOG Settings Autoclear (Bool), silent.enabled, you can turn off the log saves
- Fented
  - Automatic opening of the portal when connecting to ESP in AP mode did not work on some devices
- Improved
  - All Copystr () - if the length is indicated, now the line is recorded, even if the incoming line is longer
  - GPLOG works again like a ring buffer
  
## v3.6.5
- Fixed compilation errors for ESP32 2.0.8
- Fixed Warning on other versions

## v3.6.6
- Fixed a critical error from 3.6.5;)

<a id="feedback"> </a>
## bugs and feedback
Create ** Issue ** when you find the bugs, and better immediately write to the mail [alex@alexgyver.ru] (mailto: alex@alexgyver.ru)
The library is open for refinement and your ** pull Request ** 'ow!

When reporting about bugs or incorrect work of the library, it is necessary to indicate:
- The version of the library
- What is MK used
- SDK version (for ESP)
- version of Arduino ide
- whether the built -in examples work correctly, in which the functions and designs are used, leading to a bug in your code
- what code has been loaded, what work was expected from it and how it works in reality
- Ideally, attach the minimum code in which the bug is observed.Not a canvas of a thousand lines, but a minimum code