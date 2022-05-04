# Gyver Portal
### Most of the library does not work correctly with ESP32, I will fix it as soon as I have time

Simple web interface builder for esp8266 and ESP32
- Simple constructor - make pages without knowledge of HTML and CSS
- The library is a wrapper for the standard ESP8266WebServer
- Allows you to quickly create a webmord to manage and configure your device
- Compact readable code in the "sketch", no external handlers and lambda functions
- The constructor uses standard HTML forms, CSS and javascript
- Elements of the designer are stored in Flash memory
- No global buffers, everything is generated on the fly
- Nice design out of the box + dark theme
- Adapted for mobile devices and PC
- Built-in tools for easy parsing of values ​​from the form
- Ability to configure automatic updating of variable values ​​by action from the page
- Built-in javascripts for AJAX, work without refreshing the entire page:
  - Clicks on components, changing their values
  - Updating components on a timer
  - Real time chart
  - Debug text window (send from program)
- Components of the constructor:
  - Title
  - Signature
  - Delimiter
  - Line break
  - Block for combining components
  - Web form (block)
  - submit button (for forms)
  - Text input field
  - Multiline text input field
  - Password entry field
  - Checkmark (checkbox)
  - Switch
  - Slider
  - Slider with caption
  - Time selection
  - Date selection
  - Selector (otheropbox)
  - Button
  - Mini button
  - "LED" indicator
  - Log window for debugging (web serial port)
  - Multiple chart types (internet required)

![demo](/docs/demoBig.png)

### Opportunities
The library allows you to generate a dynamic web interface to control your electronic device from a browser (you need to go to the IP address of the board in the address bar). Three main ways to interact with the interface and use cases:
- **Form and submit button**: when the submit button is clicked, the page reloads, and the program receives data from all components included in the form (text in the input field, positions of sliders and checkboxes, and so on). Convenient for one-time data entry (connection settings, etc.).
- **Click on a component**: when you click on almost any interface component (when its state or value changes), you can get its current value without reloading the page. Convenient for management and configuration (checkmarks, buttons, sliders).
- **Update** component values ​​and states in real time without reloading the page. Convenient for indicating operation and obtaining current numerical and text values ​​from the program, displaying graphs in real time.

### Compatibility
esp8266, esp32

## Content
- [Install](#install)
- [Initialization](#init)
- [Documentation](#docs)
- [Usage](#usage)
  - [Getting Started](#server)
  - [Constructor](#build)
  - [Form Mode](#form)
  - [Click Mode](#click)
  - [Update Mode](#update)
  - [Auto-update variables](#auto)
  - [Plots](#plots)
  - [Log](#log)
  - [Own components, API](#api)
- [Examples](#example)
- [Versions](#versions)
- [Bugs and feedback](#feedback)

<a id="install"></a>
## Installation
- The library can be found by the name **GyverPortal** and installed through the library manager in:
    - Arduino IDE
    - Arduino IDE v2
    - PlatformIO
- [Download library](https://github.com/GyverLibs/GyverPortal/archive/refs/heads/main.zip) .zip archive for manual installationki:
    - Unzip and put in *C:\Program Files (x86)\Arduino\libraries* (Windows x64)
    - Unzip and put in *C:\Program Files\Arduino\libraries* (Windows x32)
    - Unpack and put in *Documents/Arduino/libraries/*
    - (Arduino IDE) automatic installation from .zip: *Sketch/Include library/Add .ZIP library…* and specify the downloaded archive
- Read more detailed instructions for installing libraries [here] (https://alexgyver.ru/arduino-first/#%D0%A3%D1%81%D1%82%D0%B0%D0%BD%D0%BE% D0%B2%D0%BA%D0%B0_%D0%B1%D0%B8%D0%B1%D0%BB%D0%B8%D0%BE%D1%82%D0%B5%D0%BA)

<a id="init"></a>
## Initialization
`GyverPortal portal;`

<a id="docs"></a>
## Documentation
<details>
<summary>Constructor functions</summary>

```cpp
// page creation
BUILD_BEGIN(string); // start building. Adds initial HTML code
BUILD_END(); // finish building. Adds the final HTML code and opens the page

// themes
add.THEME(theme); // set theme
GP_LIGHT - light theme
GP_DARK - dark theme

// form creation
add.FORM_BEGIN(name); // start form with name (name)
add FORM_END(); // complete the form

// empty form with one button
add.FORM_SUBMIT(name, text); // form with name (name) button (text)

// form components
add.BUTTON(name, text); // button
add.BUTTON(name, text, id); // button (id of the component, the data from which the button will send data on click)
add.BUTTON_MINI(name, text); // mini button
add.BUTTON_MINI(name, text, id); // mini button (id of the component, the data from which the button will send data on click)

add.NUMBER(name, hint, number); // text input field, number
add.TEXT(name, hint, text); // text input field
add.PASS(name, hint, text); // password input field
add.CHECK(name); // checkbox, default switched off
add.CHECK(name, state); // checkbox
add SWITCH(name); // switch, default switched off
add.SWITCH(name, state); // switch
add.DATE(name); // date input, default empty
add.DATE(name, GPdate); // date input
add.TIME(name); // time input, default empty
add.TIME(name, GPtime); // time input
add.SELECT(name, list); // selector (dropbox), comma separated list items. List can be PSTR
add.SELECT(name, list, active); // + current active item
add.SLIDER(name, number, min, max); // slider
add.SLIDER(name, number, min, max, step); // slider
add.SLIDER(name, caption, number, min, max); // slider with caption
add.SLIDER(name, caption, number, min, max, step); // slider with caption
add.COLOR(name); // color selection, default black
add.COLOR(name, number); // color selection
add.SUBMIT(text); // form submit button
add.LED_RED(name); // red LED indicator
add.LED_GREEN(name); // green LED indicator

// other for decoration
add.TITLE(text); // header
add.TITLE(text, name); // + component name (for update())
add.LABEL(text); // label (for buttons, fields, checkboxes, etc.)
add.LABEL(text, name); // + component name (for update())
add.LABEL(number); // signature, number
add.LABEL(number, name); // + component name (for update())
add.AREA(name, lines, text); // large text input field
add.AREA(name, number of lines); // large text input field
add.BREAK(); // move line
add.HR(); // horizontal separator
add BLOCK_BEGIN(); // start block drawing
add BLOCK_END(); // end block drawing

// other
add.AJAX_UPDATE(list, period); // pass the component update list
// for list you can use macros PSTR

// large text input field
add.AREA(name, lines, text);
add.AREA(name, number of lines);
add.AREA(name);

// graphs
// light static graph without scale
add.PLOT<number of axes, number of data>(name, labels, data int16_t, int dec = 0)
add.PLOT_DARK<number of axes, number of data>(name, labels, data int16_t, int dec = 0)

// static chart with scale and time reference
add.PLOT_STOCK<number of axes, number of data>(name, labels, array of times, array of data, int dec = 0)
add.PLOT_STOCK_DARK<number of axes, number of data>(name, labels, array of times, array of data, int dec = 0)

// dynamic graph, calls update
add.AJAX_PLOT(name, number of axes, number of points in X, update period);
add.AJAX_PLOT_DARK(name, number of axes, number of points in X, update period);

// create a custom page
GP_BUILD(string); // run constructor (can be called anywhere)
GP_SHOW(); // render the page (call only inside the constructor function!)
add.PAGE_BEGIN(); // initial HTML code
add.PAGE_BLOCK_BEGIN() // center div block
add.PAGE_BLOCK_END() // center div block
add.PAGE_END(); // final HTML code
add.AJAX_CLICK(); // click handler
```
</details>

<details>
<summary>Class methods</summary>

```cpp
// system
void start(); // start portal
void start(WIFI_AP); // start portal with DNS server (for AP)
void stop(); // stop the portal// show your page
void showPage(String&); // show your page
void show(); // call the constructor and show the page

// connection
void attachBuild(void*); // connect page builder function
void attachForm(void*); // connect a function that is called when submit is clicked
void attachClick(void*); // connect a function that is called when clicked (button,checkbox, switch, slider, selector)
void attachUpdate(void*); // connect a function that is called when AJAX refreshes from the page

// poll
bool tick(); // portal ticker. Returns true if the portal is running

boolform(); // will return true if there was a click on any submit
bool form(char*name); // will return true if there was a submit from the specified form
String&formName(); // get the name of the current submit form

boolclick(); // returns true if there was a click on (button, checkbox, switch, slider, selector)
bool click(char*name); // returns true if the specified element was clicked
String&clickName(); // get the name of the currently clicked component
const String&clickText(); // get the text name of the clicked component

bool update(); // will return true if there was an update
bool update(char*name); // will return true if there was an update from the specified component
String& updateName(); // will return the name of the updated component

void answer(String& s); // send update response
void answer(int s);
void answer(char* s);
void answer(int16_t* v, int am); // int array with dimension am, for chart
void answer(int16_t* v, int am, int dec);// + divisor
void answer(GPcolorcol); // response with color
void answer(GPdate date); // response with date
void answer(GPtimetime); // response over time

bool root(); // will return true if the main page (/) is open
String&uri(); // current request address

// auto update list
list.init(number); // initialize the list, specify the number
list.clear(); // clear the list
list.add(address, name, type); // add a variable, specify the component name and type
list.add(address, form name, name, type); // add variable, form name, specify name tocomponent and type

// list types
T_CSTR - char array
T_STRING - String
T_TIME - time of type GPtime
T_DATE - date of type GPdate
T_CHECK - boolean, for checkbox
T_BYTE - integer 1 byte
T_INT - integer 4 bytes
T_FLOAT - float
T_COLOR - 4 byte integer, for color

// parsers
String getString(char* name); // get a String string from the name component
char* getChars(char* name); //get a char* string from the name component
void copyStr(char* name, char* dest); // rewrite the char string from the name component to itself in dest
long getInt(char*name); // get an integer from the name component
float getFloat(char*name); // get a float number from the name component
bool getCheck(char*name); // get the checkbox state from the name component
GPdate getDate(char*name); // get the date from the name component in GPdate format
GPtime getTime(char*name); // get the time from the name component in GPtime format
uint32_t getColor(char*name); // get the color from the name component
uint8_t getSelected(char* name, char* list); // get the number of the selected item in the list droplist (can be PSTR)
```
</details>

<details>
<summary>Storing and changing time</summary>

```cpp
// structure for storing the date
struct GPdate {
  int16_tyear;
  uint8_t month, day;
};

// structure for storing time
struct GPtime {
  uint8_t hour, minute, second;
};

// get unix time for chart
uint32_t GPunix(year, month, day, hour, minute, second);
uint32_t GPunix(year, month, day, hour, minute, second, gmt);
// gmt - timezone, default 0 (example: Moscow gmt = 3)
// month and day start at 1, not 0!
```
</details>

<details>
<summary>Storing and changing colors</summary>

```cpp
// see gpcolor_demo example

// structure for storing color
struct GPcolor {
    uint8_t r, g, b;
};

// initialization
GPcolor color;
GPcolor color(uint32_t color);
GPcolor color(byte r, byte g, byte b);

// to the structure can beapplied methods
void setRGB(r, g, b); // set color byte by byte
setHEX(uint32_tcol); // set 24 bit color
uint32_t getHEX(); // get 24 bit color

// a uint32_t number can be assigned to the structure
```
</details>

<details>
<summary>Utilities</summary>

```cpp
char*splitList(char*str); // split the string into substrings. Gypsy tricks
int8_t inList(char* name, char* list); // get the number under which name is included in the list (like "val1,val2,val3")
int8_t inList(String& name, char* list);

String encodeDate(GPdate& d); // glue the date into a string String
void encodeDate(char* str, GPdate& d); // glue the date into a string str[11]
String encodeDate(year, month, day); // glue the date into a string String
GPdate decodeDate(char*str); // parse string date[11] into structure

String encodeTime(GPtime&t); // glue the time into a string String
void encodeTime(char* str, GPtime& t); // glue the time into a string str[9]
String encodeTime(hour, minute, second);// glue the time into a String
GPtime decodeTime(char*str); // parse string time[9] into structure

String encodeColor(GPcolor color); // collect GPcolor color into String #rrggbb
String encodeColor(uint32_t color); // gather color into String #rrggbb
uint32_t decodeColor(char* hex); // parse color #rrggbb into a number

// add a new value to the array with rewind (for charts)
GPaddInt(int16_t val, int16_t* arr, uint8_t am); // new value, array, array size
GPaddUnix(uint32_t val, uint32_t* arr, uint8_t am); // new value, array, array size
GPaddUnixS(int16_t val, uint32_t* arr, uint8_t am); // add seconds, array, array size
```
</details>

### Table of operating modes support by components

| Component/Call | form() | click() | update() |
| --------------- |:------:|:-------:|:--------:|
| TITLE | | | ✔ |
| label | | | ✔ |
| BUTTON| | ✔ | |
| BUTTON_MINI | | ✔ | |
| NUMBER | ✔ | ✔ | ✔ |
| TEXT | ✔ | ✔ | ✔ |
| PASS | ✔ | ✔ | ✔ |
| AREA | ✔ | | ✔ |
| CHECK | ✔ | ✔ | ✔ |
| SWITCH | ✔ | ✔ | ✔ |
| DATE | ✔ | ✔ | ✔ |
| time | ✔ | ✔ | ✔ |
| SLIDER | ✔ | ✔ | ✔ |
| COLOR | ✔ | ✔ | ✔ |
| SELECT | ✔ | ✔ | |
| LED_RED | ✔ | | ✔ |
| LED_GREEN | ✔ | | ✔ |

<a id="usage"></a>
<a id="server"></a>
## 1. Server
The library can work both in a local network (esp connects to a router) and in access point mode (smartphone connects to esp).
### 1.1 Connecting to a router
```cpp
  WiFi.mode(WIFI_STA);
  wifi.begin("login", "pass");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(WiFi.localIP());
  // ...
  portal.start(); // start the portal
```
To connect to the portal, you need to go in the browser to the IP address of the board, which was issued to it by the router. In the example above, this address is displayed in the port monitor.

### 1.2 Creating a point
In this mode, when starting the portal, you need to pass **WIFI_AP** to start the DNS server
```cpp
  WiFi.mode(WIFI_AP);
  WiFi.softAP("My Portal");
  portal.start(WIFI_AP); // start the portal with AP mode configured
```
On standard settings, the IP address for connection in this mode will be *192.168.4.1*

### 1.3 Ticker
GyverPortal uses the *ESP8266WebServer* standard library, so
to make the server work, you need to call `portal.tick()` in the program loop. Returns `true` if the server is currently running.

### 1.4 Create GyverPortal globally
```cpp
Gyver Portal portal;
void build() {}

void setup() {
  // connect to the network
  portal.attachBuild(build);
  portal.start();
}

void loop() {
  portal.tick();
  // poll for actions
}
```

### 1.5 Create GyverPortal locally
```cpp
void build() {}

void f() {
  Gyver Portal portal;
  portal.attachBuild(build);
  portal.start();

  while (portal.tick()) {
    // poll for actions
  }
}
```
To exit the loop, you can call `portal.stop()` on a timeout or a signal from the browser.

<a id="build"></a>
## 2. Page Builder
### 2.1. Create a constructor function
1. Create a function like: `void f()`. Further in it:
2. Create an empty string: `String s;`.
3. Run the constructor: `BUILD_BEGIN(s)`. We pass the created string. This is where the initial HTML code is added.
4. (Optional) apply theme: `add.THEME(theme)`, GP_LIGHT/GP_DARK
5. We build the page using the constructor or adding our data to the line.
6. Finish the constructor: `BUILD_END()`. Here, the final HTML code is added and the page is sent to the server.

Constructor function template:
```cpp
void build() {
  String s;
  BUILD_BEGIN(s);
  add THEME(GP_LIGHT);
  // assemble the page
  // ...
  BUILD_END();
}
```

### 2.2 Connecting the constructor function
We pass our page constructor function to the library:
```cpp
portal.attachBuild(build);
```
The library itself will call it when it needs to render the page. There can be several constructor functions (and hence pages) and they can be switched.

<a id="form"></a>
## 3. Using Forms
### 3.1 Building the form page
The main essence of the use of forms:
- The form has its own unique name, must start with **/**
- There can be any number of elements inside the form, but only one button of the **SUBMIT** type
- When SUBMIT is clicked, esp gets the form name and data from all elements inside that form
- When you click on SUBMIT, the page is reloaded, so the values ​​of the page components must be stored in variables and passed on the next page build

An example with two forms, the first one can pass the text from the input box, the second one only the factpressing button:
```cpp
form_1
    text input
    submit button
form_1

form_2
    submit button
form_2
```

In the GyverPortal constructor it will look like this:
```cpp
void build() {
  String s; // create a string
  BUILD_BEGIN(s); // run constructor
  add THEME(GP_LIGHT); // apply theme

  add.FORM_BEGIN("/login"); // start form, pass name
  add.TEXT("txt", "Login", ""); // text input, Login hint, no text
  add.BREAK(); // line break
  add.SUBMIT("Submit"); // submit button
  add FORM_END(); // complete the form

  add.FORM_BEGIN("/exit"); // start form, pass name
  add.SUBMIT("Exit"); // Exit button
  add FORM_END(); // complete the form

  BUILD_END(); // finish building
}
```
Result of the constructor:
![demo](/docs/demo.png)
All constructor tools are described in the documentation above.

### 3.2 Polling actions
- When pressing any *SUBMIT* type button in the browser, the `form()` function will return `true`
- Function should be polled after `tick()`
- To search for the form the signal came from, use `form(name)` - will return `true` if the name matches
  - It's better to wrap the search in `if (form())`, so as not to waste CPU time on string comparison
```cpp
portal.tick();
if (portal.form()) {
  Serial.print("Submit form: ");
  if (portal.form("/login")) Serial.println("Login");
  if (portal.form("/exit")) Serial.println("Exit");
}
```

### 3.3 Connecting a handler
Instead of manually polling `form()`, you can connect your own function like `void f(GyverPortal*)`, it will be called when you click on any SUBMIT.
This function will need to be passed to `attachForm()`.
```cpp
void myAction(GyverPortal*p) {
  // have access to the portal object that sent the call
  if (p -> form("/exit")) Serial.println("exit");
}

void setup() {
  portal.attachForm(myAction);
}
```


### 3.4 Data parsing
The library implements ready-madeTools for getting data from form components (see documentation above). For example, let's output the contents of the text input field to the port:
```cpp
portal.tick();
if (portal.form("/login")) Serial.println(portal.getString("txt"));
// where "txt" is the name of the component
```
**Important!** You can only receive data from the form component inside the condition (here `if (portal.form("/name"))`), since esp does not store the page code in itself, it receives specific data only when user!

<a id="click"></a>
## 4. Using clicks
### 4.1 Difference from forms
The library implements a mechanism that allows you to process actions on the page without reloading it (as when using forms):
- The form allows you to get values ​​from several components by pressing one button. The page will reload.
- Click allows you to get the current (changed) value only from the clicked component. *Page will not reload*.

### 4.2 Polling actions
- When clicking on some components or changing their value (see the table in the documentation), the `click()` function will return `true`
- Function should be polled after `tick()`
- To search for the component from which the signal came, use `click(name)` - will return `true` if the name matches
  - It's better to wrap the search in `if (click())`, so as not to waste CPU time on string comparison
```cpp
portal.tick();
if (portal.click()) {
  if (portal.click("mybutton")) Serial.println("Click!");
}
```

### 4.3 Connecting a handler
Instead of manually polling `click()`, you can connect your own function like `void f(GyverPortal*)`, it will be called when clicking on the components.
This function will need to be passed to `attachClick()`.
```cpp
void myClick(GyverPortal* p) {
  // have access to the portal object that sent the call
  if (p -> click("mybutton")) Serial.println("Click!");
}

void setup() {
  portal.attachClick(myClick);
}
```

### 4.4 Data parsing
Parsing data from clicks can be done using the same functions as for forms.
**Important!** You can receive data from a form componento only inside the condition (here `if (portal.click("/name"))`), since esp does not store the page code in itself, it receives specific data only when the user interacts!

### 4.5 Connecting a button to another component
A button (BUTTON, BUTTON_MINI) can be "connected" to another component: when the button is clicked, the `click` signal will be called with the name of the button and data from the specified component. To connect, you need to specify the name of the component as the third argument when adding a button:
```cpp
add.BUTTON(button name, button text, component name);
add.BUTTON_MINI(button name, button text, component name);
```
Example, clicking on the button sends the text from the txt field:
```cpp
add.TEXT("txt", "");
add.BUTTON_MINI("btn", "Send", "txt");
```

<a id="update"></a>
## 5. Using updates
The library implements a mechanism for script requests from the page by timer. This allows you to update the values ​​of some components and labels.
(see the table in the documentation) without refreshing the page in the browser.

### 5.1 Connecting updates
To enable the update mode, add the *AJAX_UPDATE* block to the top of the page:
```cpp
void build() {
  String s;
  BUILD_BEGIN(s);
  add.AJAX_UPDATE("name1,name2,name3");
  // ...
  add.LABEL("NAN", "val"); // we will update the text
  BUILD_END();
}
```

- The AJAX_UPDATE function takes a comma-separated list of component names.
- DO NOT PUT A SPACE AFTER THE COMMA.
- You can also specify the period of update requests in milliseconds `add.AJAX_UPDATE("name1,name2", 5000);`, default is 1000 (1 second).
- Not all components support update mode (see the table in the documentation).

### 5.2 Polling for updates
- When an update occurs, the `update()` function will return `true`
- Function should be polled after `tick()`
- To find the component from which the signal came, use `update(name)` - will return `true` if the name matches
  - It's better to wrap the search in `if (update())`, so as not to waste CPU time on string comparison
- You need to respond to the update request with`answer()` function. The current value for the component is passed to it.
- If you do not respond to the update until the next `tick()` call, the library will respond with an empty response so that the page does not freeze

```cpp
portal.tick();
if (portal update()) {
  if (portal.update("val")) portal.answer(random(1000));
}
```

### 5.3 Connecting a handler
Instead of manually polling `update()`, you can connect your own function like `void f(GyverPortal*)`, it will be called when any component is updated.
This function will need to be passed to `attachUpdate()`.
```cpp
void myUpdate(GyverPortal* p) {
  if (p -> update("val")) p -> answer(random(1000));
}

void setup() {
  portal.attachUpdate(myUpdate);
}
```

<a id="auto"></a>
## 6. Automatic update of variables
**[Cm. examples demoSubmitAuto and demoClickAuto]** Instead of manual parsing, you can specify the library variables,
which will automatically receive new values ​​from the specified page components. *This works for both forms and clicks.*
- Initialize the list by calling `.list.init(number)`, passing the size of the list in the number of variables.
- Add a variable at its address:
  - `.list.add(&variable, name, type)` - indicating the name of the component and its type
  - `.list.add(&variable, form, name, type)` - specifying the form name, component name and type

The specified variables will update their values ​​when the form with the specified name is acted upon or when clicked.
If the form name is not specified, the component will be parsed upon action from any form. *You don't need to specify a form name to work with clicks.*

### 6.1 Feature support by components, connection with types

| Data type | Type/Component | TEXT/NUMBER | PASS | CHECK | SWITCH | DATE | time | SLIDER | COLOR | SELECT | AREA |
| ------------- | ------------- |:-----------:|:----:|:-----:|:------ :|:----:|:----:|:------:|:-----:|:------:|:----:|
| `char[]` | T_CSTR | ✔ | ✔ | | | ✔ | ✔ | ✔ | ✔ | ✔ | |
| `String` | T_STRING | ✔ |Cranberries ✔ | | | ✔ | ✔ | ✔ | ✔ | ✔ | |
| `gptime` | T_TIME | | | | | | ✔ | | | | |
| `GPdate` | T_DATE | | | | | ✔ | | | | | |
| `bool` | T_CHECK | | | ✔ | ✔ | | | | | | |
| `byte`, `char`| T_BYTE | ✔ | ✔ | | | | | ✔ | | | |
| `int`, `long` | T_INT | ✔ | ✔ | | | | | ✔ | | | |
| float | T_FLOAT | ✔ | ✔ | | | | | ✔ | | | |
| `uint32_t` | T_COLOR | | | | | | | | ✔ | | |

<a id="plots"></a>
## 7. Graphs
### 7.1 General features
#### Compatibility
Plots **AJAX_PLOT** and **PLOT_STOCK** are not compatible in the same interface!

#### Output fractional data
All three types of charts have an argument `dec`, which is 0 by default. This is the divisor by which (if different from 0) the values ​​of the chart points will be divided and converted to `float`.
This way you can display floating point data and not store extra 2 bytes in memory. We got a temperature of 22.5 degrees, multiply by 10 and save to an array. We call the graph with `dec` equal to 10.

#### Multiple axes
All graphs support multi-axis output (common x-axis).

#### Signatures
Signatures are stored in a char array, like this:
```cpp
const char *names[] = {"kek", "puk",};
```
#### Update static charts
Static graphs display data on page reload. Thus, an array with actual values ​​must be passed to the constructor.
The library implements functions for conveniently adding a new value to an array (with automatic "rewind"):
```cpp
GPaddInt(int16_t val, int16_t* arr, uint8_tam); // new value, array, array size
GPaddUnix(uint32_t val, uint32_t* arr, uint8_t am); // new value, array, array size
GPaddUnixS(int16_t val, uint32_t* arr, uint8_t am); // add seconds, array, array size
```

For example, there is an array `int arr[2][20]` - stores 20 values ​​for two chart axes. You can update it and store it in EEPROM, ensuring uninterrupted operation. To add a new value, do according to your timer:
```cpp
GPaddInt(new, arr[0], 20);
GPaddInt(new, arr[1], 20);
```
We pass it to the constructor as
```cpp
add.PLOT<2, 20>("table", names, arr);
```

#### Update dynamic charts
The dynamic graph calls `update`, we answer it with new values ​​and it builds a graph in real time. To pass values ​​along multiple axes, use
`answer(array, size)` or `answer(array, size, dec)`, where dec has the meaning of a divisor (see above).


### 7.2 PLOT Plot
Easy static graph without scale
**[Cm. staticPlot example]**
![demo](/docs/staticPlot.png)
```cpp
add.PLOT<number of axes, number of data>(name, labels, data int16_t, int dec = 0)
add.PLOT_DARK<number of axes, number of data>(name, labels, data int16_t, int dec = 0)
```

### 7.3 Plot PLOT_STOCK
Static chart with scale and time reference
**[Cm. stockplot example]**
![demo](/docs/stockplot.png)
```cpp
add.PLOT_STOCK<number of axes, number of data>(name, labels, array of times, array of data, int dec = 0)
add.PLOT_STOCK_DARK<number of axes, number of data>(name, labels, array of times, array of data, int dec = 0)
```
This graph requires to display a date and time array of type `uint32_t` containing the time in unix format.

### 7.4 AJAX_PLOT Plot
Dynamic chart, calls update by its name, requires a response
**[Cm. ajaxplot example]**
![demo](/docs/ajaxPlot.png)

```cpp
add.AJAX_PLOT(name, number of axes, number of points in X, update period);
add.AJAX_PLOT_DARK(name, number of axes, number of points in X, update period);
```

<a id="log"></a>
## 8. Log
ATlibrary implemented the ability to do `print()` in a special log window on the page:
- You can create only one log window
- Update is automatic, once per second
- Page not updating
- Can send any data as Serial

### 8.1 Connecting a log window
Add `add.AREA_LOG(number of lines)` to the right place on the page

### 8.2 Run log
We call `log.start(buffer size)`. The default buffer size is 64 characters
- Note: this is the size of the buffer *on the side of the library*, that is, the limit on the number of characters per send per page (once per second). The browser page has its own buffer for displaying text!

### 8.3 Data output
We just call `log.print()` or `log.println()` like a regular Serial. See **demoLog** example.

<a id="api"></a>
## 9. Own components, API
### 9.1 Custom constructor
The GyverPortal constructor does not restrict page building in any way: just add any HTML code to the string between the start of `GP_BUILD(s)` and the completion of the `GP_SHOW()` constructor:
```cpp
void build() {
  String s;
  GP_BUILD(s);
  // assemble the page
  // ...
  GP_SHOW();
}
```

For reference:
The standard `BUILD_BEGIN(String)` internally consists of:
```cpp
  GP_BUILD(s);
  add.PAGE_BEGIN();
  add.AJAX_CLICK();
  add.PAGE_BLOCK_BEGIN();
```

The standard `BUILD_END()` internally consists of:
```cpp
 add.PAGE_BLOCK_END();
 add.PAGE_END();
 GP_SHOW();
```

### 9.2 Own code
It is enough to add any HTML code to the string, for example:
```cpp
s += F("<input type=\"email\" class=\"myClass\">");
```
You can wrap it in F macro so as not to take up RAM.

### 9.3 API
To ensure the operability of the library mechanisms in custom components, the following points must be observed:
- If you need click support - add `add.AJAX_CLICK()` to the page
- Form components must have the *name* attribute specified to pass data via submit.
- Clickable components must have an *onclick* attribute with a function parameter: `onclick="GP_click(this)"`. The library itselfwill intercept the call and route it to `click()`.
- Components that need a `click()` signal for data changes must have the *onchange* attribute with the function parameter: `onchange="GP_click(this)"`. The library itself will intercept the call and direct it to `click()`.
- Components that require `update()` updates must have the `id` attribute specified. Its value must also be passed to `add.AJAX_UPDATE()`.
- If you need a click that sends data from another component, specify the attribute with the function `onclick="GP_clickid(btn,tar)"`, where `btn` is the name (for the library) of the clicking component, and `tar` is the `id attribute ` of the target component from which to pass data.
- To manually send a click signal to the library, you need to send an http POST request like `GP_click?name=value`
- To manually send an update signal to the library, you need to send an http GET request like `GP_update?component_id`

<a id="example"></a>
## Examples
```cpp
```

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

<a id="feedback"></a>
## Bugs and feedback
AtIf you find bugs, create an **Issue** for Cranberry, or better, immediately write to [alex@alexgyver.ru](mailto:alex@alexgyver.ru)
The library is open for revision and your **Pull Request**'s!