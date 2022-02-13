[![Foo](https://img.shields.io/badge/Version-1.6-brightgreen.svg?style=flat-square)](#versions)
[![Foo](https://img.shields.io/badge/Website-AlexGyver.ru-blue.svg?style=flat-square)](https://alexgyver.ru/)
[![Foo](https://img.shields.io/badge/Support-Aleks-orange.svg?style=flat-square)](https://alexgyver.ru/support_alex/)

# GyverPortal

Easy ot use web interface builder for ESP8266 and ESP32

- Easy constructor - make pages without knowledge of HTML and CSS
- The library is a wrapper for the standard ESP8266WebServer
- Allows you to quickly create a web interface for managing and configuring your device
- Compact readable code in the "sketch", no external handlers and lambda functions
- The constructor uses standard HTML forms, CSS and JavaScript
- Constructor elements are stored in Flash memory
- No global buffers, everything is generated "on the fly"
- Nice design "out of the box" + dark theme
- Adapted for mobile devices and PCs
- Built-in tools for parsing values from the form
- Ability to configure automatic updating of variable values by action from the page
- Built-in scripts for AJAX, work without reloading the page:
  - Clicks on components, changing their values
  - Updating components by timer
  - Real-time graph
  - Debugging text window (send from the program)
- Constructor components:
  - Header
  - Label
  - Separator
  - Line break
  - Block for combining components
  - Web form (block)
  - Submit button (for forms)
  - Text input field
  - Multiline text input field
  - Password input field
  - Check mark (checkbox)
  - Switch
  - Slider
  - Slider with label
  - Time selection
  - Date selection
  - Select (dropbox)
  - Button
  - Mini button
  - "LED" indicator
  - Log window for debugging (Web Serial port)
  - Several types of graphs (Internet required)

![demo](/docs/demoBig.png)

### Features

The library allows you to generate a dynamic web interface to control your electronic device from a browser (you need to go to the IP address of the board in the address bar). There are three main ways to interact with the interface and usage scenarios:

- **The form and the submit button**: when you click the submit button, the page is reloaded, and data from all components included in the form (text in the input field, the positions of sliders and checkboxes, etc) comes to the program. Suitable for single data entry (connection settings and the like).
- **Click on a component**: when you click on almost any component of the interface (when its state or value changes), you can get its current value without reloading the page. Suitable for managing and configuring (checkboxes, buttons, sliders).
- **Updating** values and states of components in real time without reloading the page. Suitable for indicating the operation and getting the current numerical and textual values from the program, displaying graphs in real time.

### Compatibility

ESP8266, ESP32

## Content

- [Installation](#install)
- [Initialization](#init)
- [Documentation](#docs)
- [Usage](#usage)
  - [Getting Started](#server)
  - [Constructor](#build)
  - [Form mode](#form)
  - [Click mode](#click)
  - [Update mode](#update)
  - [Auto-update variables](#auto)
  - [Graphs](#plots)
  - [Log](#log)
  - [Custom components, API](#api)
- [Examples](#example)
- [Versions](#versions)
- [Bugs and Feedback](#feedback)

<a id="install"></a>

## Installation

- The library can be found by the name **GyverPortal** and installed via the library manager in:
  - Arduino IDE
  - Arduino IDE v2
  - PlatformIO
- [Download the library](https://github.com/GyverLibs/GyverPortal/archive/refs/heads/main.zip) .zip archive for manual installation:
  - Unpack and put in *C:\Program Files (x86)\Arduino\libraries* (Windows x64)
  - Unpack and put in *C:\Program Files\Arduino\libraries* (Windows x32)
  - Unpack and put in *Documents/Arduino/libraries/*
  - (Arduino IDE) automatic installation from .zip: *Sketch/Include Library/Add .ZIP library...* and specify the downloaded archive
- Read more detailed instructions for installing libraries [here](https://alexgyver.ru/arduino-first/#%D0%A3%D1%81%D1%82%D0%B0%D0%BD%D0%BE%D0%B2%D0%BA%D0%B0_%D0%B1%D0%B8%D0%B1%D0%BB%D0%B8%D0%BE%D1%82%D0%B5%D0%BA)

<a id="init"></a>

## Initialization

`GyverPortal portal;`

<a id="docs"></a>

## Documentation

<details>
<summary>Constructor functions</summary>

```cpp
// creating a page
BUILD_BEGIN(string);                 // start building. Adds the initial HTML code
BUILD_END();                         // finish building. Adds the final HTML code and opens the page

// themes
add.THEME(theme);                    // set theme
// GP_LIGHT - light theme
// GP_DARK - dark theme

// creating the form
add.FORM_BEGIN(name);                // start the form with the 'name'
add.FORM_END();                      // end the form

// empty form with one button
add.FORM_SUBMIT(name, text);         // form with 'name' and button with 'text'

// form components
add.BUTTON(name, text);
add.BUTTON(name, text, id);          // 'id' of the component that the data with which the button will send data on the click
add.BUTTON_MINI(name, text);
add.BUTTON_MINI(name, text, id);     // 'id' of the component that the data with which the button will send data on the click

add.NUMBER(name, tooltip, number);   // number input
add.TEXT(name, tooltip, text);       // text input
add.PASS(name, tooltip, text);       // password input
add.CHECK(name);                     // checkbox. Default: off
add.CHECK(name, state);
add.SWITCH(name);                    // default: off
add.SWITCH(name, state);
add.DATE(name);                      // date input. Default: empty
add.DATE(name, GPdate);
add.TIME(name);                      // time input. Default: empty
add.TIME(name, GPtime);
add.SELECT(name, list);              // dropbox. The elements of the list separated by a comma. The list can be PSTR
add.SELECT(name, list, selected);    // + current active item
add.SLIDER(name, number, min, max);
add.SLIDER(name, number, min, max, step);
add.SLIDER(name, label, number, min, max);
add.SLIDER(name, label, number, min, max, step);
add.COLOR(name);                     // color select. Default: black
add.COLOR(name, number);
add.SUBMIT(text);                    // form submit button
add.LED_RED(name);                   // red led indicator
add.LED_GREEN(name);                 // green led indicator

// other for decoration
add.TITLE(text);                     // header
add.TITLE(text, name);               // + name of the component (for update())
add.LABEL(text);                     // for buttons, fields, checkboxes, etc
add.LABEL(text, name);               // + name of the component (for update())
add.LABEL(number);
add.LABEL(number, name);             // + name of the component (for update())
add.AREA(name);
add.AREA(name, numberOfLines);       // large text input field
add.AREA(name, numberOfLines, text); // multiline text input field
add.BREAK();                         // break line
add.HR();                            // horizontal separator
add.BLOCK_BEGIN();                   // start block rendering
add.BLOCK_END();                     // finish block rendering

// other
add.AJAX_UPDATE(list, period);       // pass the list of component updates
// you can use the PSTR macro for the list

// graphs
// light static graph without scale
add.PLOT<numberOfAxes, numberOfData>(name, labels, int16_t data, int dec = 0)
add.PLOT_DARK<numberOfAxes, numberOfAata>(name, labels, int16_t data, int dec = 0)

// static graph with the scale and time
add.PLOT_STOCK<numberOfAxes, numberOfData>(name, labels, timeArray, dataArray, int dec = 0)
add.PLOT_STOCK_DARK<numberOfAxes, numberOfData>(name, labels, the timeArray, dataArray, int dec = 0)

// dynamic graph, calls update
add.AJAX_PLOT(name, numberOfAxes, numberOfXPoints, updatePeriod);
add.AJAX_PLOT_DARK(name, number of axes, numberOfXPoints, updatePeriod);

// creating a custom page
GP_BUILD(string);                   // run the constructor (can be called anywhere)
GP_SHOW();                          // display a page (only call inside the constructor function!)
add.PAGE_BEGIN();                   // initial HTML code
add.PAGE_BLOCK_BEGIN()              // central div block
add.PAGE_BLOCK_END()                // central div block
add.PAGE_END();                     // final HTML code
add.AJAX_CLICK();                   // clicks handler
```

</details>

<details>
<summary>Methods of the class</summary>

```cpp
// system
void start();                       // start the portal
void start(WIFI_AP);                // start a portal with a DNS server (for AP)
void stop();                        // stop the portal / show your page
void showPage(String&);             // show your page
void show();                        // call the constructor and show the page

// binding
void attachBuild(void*);            // bind the page builder function
void attachForm(void*);             // bind the function that is called when you click submit
void attachClick(void*);            // bind the function that is called when clicked (button, checkbox, switch, slider, select)
void attachUpdate(void*);           // bind the function that is called when AJAX is updated from the page

// handling
bool tick();                        // portal ticker. Returns true if the portal is running

bool form();                        // returns true if there was a click on any submit
bool form(char* name);              // returns true if there was a submit from the specified form
String& formName();                 // get the name of the current submit form

bool click();                       // returns true if there was a click on (button, checkbox, switch, slider, select)
bool click(char* name);             // returns true if there was a click on the specified element
String& clickName();                // get the name of the clicked component
const String& clickText();          // get the name of the clicked component text

bool update();                      // returns true if there was an update
bool update(char* name);            // returns true if there was an update from the specified component
String& updateName();               // returns the name of the updated component
void answer(String& s);             // send an update response
void answer(int s);
void answer(char* s);
void answer(int16_t* v, int am);    // int array with dimension 'am', for graph
void answer(int16_t* v, int am, int dec); // + divider
void answer(GPcolor col);           // answer with color
void answer(GPdate date);           // answer with date
void answer(GPtime time);           // answer with time

bool root();                        // returns true if the main page is open (/)
String& uri();                      // address of the current request

// auto-update list
list.init(quantity);                // initialize the list, specify the quantity
list.clear();
list.add(address, name, type);      // add variable, specify component name and type
list.add(address, formName, name, type); // add variable, form name, specify component name and type

// types for the list
// - T_CSTR          char array
// - T_STRING        string
// - T_TIME          time (GPtime)
// - T_DATE          date (GPdate)
// - T_CHECK         boolean (for checkbox)
// - T_BYTE          integer (1 byte)
// - T_INT           integer (4 bytes)
// - T_FLOAT         float
// - T_COLOR         integer (4 bytes, for color)

// parsers
String getString(char* name);       // get String string from the name component
char* getChars(char* name);         //get char* string from the name component
void copyStr(char* name, char* dest); // rewrite the char string from the name component to itself in dest
long getInt(char* name);            // get an integer from the name component
float getFloat(char* name);         // get a float number from the name component
bool getCheck(char* name);          // get the status of the checkbox from the name component
GPdate getDate(char* name);         // get the date from the name component in GPdate format
GPtime getTime(char* name);         // get the time from the name component in GPtime format
uint32_t getColor(char* name);      // get the color from the name component
uint8_t getSelected(char* name, char* list); // get the number of the selected item in the list droplist (can be PSTR)
```

</details>

<details>
<summary>Storing and changing time</summary>

```cpp
// structure for storing the date
struct Update {
    int16_t year;
    uint8_t month, day;
};

// structure for storing time
struct GPtime {
    uint8_t hour, minute, second;
};

// get UNIX time for a graph
uint32_t GPunix(year, month, day, hour, minute, second);
uint32_t GPunix(year, month, day, hour, minute, second, gmt);
// 'gmt' is the time zone. By default: 0 (example: Moscow gmt = 3)
// month and day start with 1, not 0!
```

</details>

<details>
<summary>Storing and changing color</summary>

```cpp
// watch example gpcolor_demo

// struct for storing color
struct GPcolor {
    uint8_t r, g, b;
};

// initialization
GPcolor color;
GPcolor color(uint32_t color);
GPcolor color(byte r, byte g, byte b);

// methods
void setRGB(r, g, b);   // set color by bytes
setHEX(uint32_t col);   // set 24 bit color
uint32_t getHEX();      // get 24 bit color

// it is possible to assign uint32_t value to object
```
</details>

<details>
<summary>Utilities</summary>

```cpp
char* splitList(char* str);             // split a string into substrings. Gypsy tricks
int8_t inList(char* name, char* list);  // get the number under which name is included in the list (of the form "val1,val2,val3")
int8_t inList(String& name, char* list);

String encodeDate(GPdate& d);           // convert date to string
void encodeDate(char* str, GPdate& d);  // convert date to str[11]
String encodeDate(year, month, day);    // convert date to string
GPdate decodeDate(char* str);           // convert string date[11] to structure

String encodeTime(GPtime& t);           // convert time to string 
void encodeTime(char* str, GPtime& t);  // convert time to str[9]
String encodeTime(hour, minute, second);// convert time to string
GPtime decodeTime(char* str);           // convert string time[9] to structure

String encodeColor(GPcolor color);      // convert GPcolor color to String #rrggbb
String encodeColor(uint32_t color);     // convert color to string #rrggbb
uint32_t decodeColor(char* hex);        // convert color #rrggbb to number

// add a new value to the array with scroll (for graphs)
GPaddInt(int16_t val, int16_t* arr, uint8_t am);    // new value, array, array size
GPaddUnix(uint32_t val, uint32_t* arr, uint8_t am); // new value, array, array size
GPaddUnixS(int16_t val, uint32_t* arr, uint8_t am); // add seconds, array, array size
```

</details>

### Table of support for operating modes of components

| Component/Call | form() | click() | update() |
| -------------- |:------:|:-------:|:--------:|
| TITLE          |        |         |    ✔    |
| LABEL          |        |         |    ✔    |
| BUTTON         |        |   ✔     |         |
| BUTTON_MINI    |        |   ✔     |         |
| NUMBER         |   ✔   |   ✔     |    ✔    |
| TEXT           |   ✔   |   ✔     |    ✔    |
| PASS           |   ✔   |   ✔     |    ✔    |
| AREA           |   ✔   |          |    ✔    |
| CHECK          |   ✔   |   ✔     |    ✔    |
| SWITCH         |   ✔   |   ✔     |    ✔    |
| DATE           |   ✔   |   ✔     |    ✔    |
| TIME           |   ✔   |   ✔     |    ✔    |
| SLIDER         |   ✔   |   ✔     |    ✔    |
| COLOR          |   ✔   |   ✔     |    ✔    |
| SELECT         |   ✔   |   ✔     |          |
| LED_RED        |   ✔   |          |    ✔    |
| LED_GREEN      |   ✔   |          |    ✔    |

<a id="usage"></a>
<a id="server"></a>

## 1. Server

The library can work both in a local network (esp connects to the router) and in access point mode (smartphone connects to esp).

### 1.1. Connecting to the router

```cpp
WiFi.mode(WIFI_STA);
WiFi.begin("login", "pass");
while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
}
Serial.println(WiFi.localIP());
// ...
portal.start(); // launching the portal
```

To connect to the portal, you need to log into the browser to the IP address of the board that the router gave her. In the example above, this address is output to the port monitor.

### 1.2. Creating a Point

In this mode, when starting the portal, you need to pass **WIFI_AP** to start the DNS server

```cpp
WiFi.mode(WIFI_AP);
WiFi.softAP("My Portal");
portal.start(WIFI_AP); // starting the portal with the AP mode setting
```

On the standard settings, the IP address for connecting in this mode will be *192.168.4.1*

### 1.3. Ticker

GyverPortal uses the standard library *ESP8266WebServer*, so
to ensure the operation of the server, you need to call `portal.tick()` in the program loop. Returns `true` if the server is currently running.

### 1.4. Creating GyverPortal globally

```cpp
GyverPortal portal;
void build() {}

void setup() {
    // connecting to the network
    portal.attachBuild(build);
    portal.start();
}

void loop() {
    portal.tick();
    // action handling
}
```

### 1.5 Creating GyverPortal locally

```cpp
void build() {}

void f() {
    GyverPortal portal;
    portal.attachBuild(build);
    portal.start();

    while (portal.tick()) {
        // action handling
    }
}
```

To exit the loop, you can call `portal.stop()` by timeout or a signal from the browser.

<a id="build"></a>

## 2. Page Builder

### 2.1. Creating a constructor function

1. Create a function of the form: `void f()`. Further in it:
2. Create an empty string: `String s`.
3. Run the constructor: `BUILD_BEGIN(s)`. Passing the created string. The initial HTML code is added here.
4. (Optional) apply the theme: `add.THEME(theme)`, GP_LIGHT/GP_DARK
5. Build the page using the constructor or adding our data to the line.
6. Completing the constructor: `BUILD_END()`. Here the final HTML code is added and the page is sent to the server.

Constructor function template:

```cpp
void build() {
    String s;
    BUILD_BEGIN(s);
    add.THEME(GP_LIGHT);
    // building a page
    // ...
    BUILD_END();
}
```

### 2.2. Connecting the constructor function

Pass our page constructor function to the library:

```cpp
portal.attachBuild(build);
```

The library itself will call it when it needs to display the page. There can be several constructor functions (and hence pages) and they can be switched.

<a id="form"></a>

## 3. Using forms

### 3.1. Building a page with forms

The main purpose of use forms:

- The form has its own unique name, must begin with `/`
- There can be any number of elements inside the form, but only one button of the type **SUBMIT**
- When you click on SUBMIT, esp gets the name of the form and data from all the elements inside this form
- When you click on SUBMIT, the page is reloaded, so the values of the page components need to be stored in variables and passed on the next page build

An example with two forms, the first can transmit text from the input window, the second - only the fact of pressing the button:

```text
form_1
    text_input
    submit_button
form_1

form_2
    submit_button
form_2
```

In the GyverPortal constructor it will look like this:

```cpp
void build() {
    String s;                      // create a string
    BUILD_BEGIN(s);                // start the constructor
    add.THEME(GP_LIGHT);           // apply theme

    add.FORM_BEGIN("/login");      // start form, pass name
    add.TEXT("txt", "Login", "");  // text input, label 'Login', no text
    add.BREAK();                   // line break
    add.SUBMIT("Submit");          // Submit button
    add.FORM_END();                // complete the form

    add.FORM_BEGIN("/exit");       // start form, pass name
    add.SUBMIT("Exit");            // Exit button
    add.FORM_END();                // complete the form

    BUILD_END();                   // finish building
}
```

The result of the designer's work:
![demo](/docs/demo.png)  
All the constructor tools are described in the documentation above.

### 3.2. Actions handling

- When you click any *SUBMIT* button in the browser, the `form()` function returns `true`
- The function should be polled after `tick()`
- To search for the form from which the signal came, use `form(name)`
  - will return `true` if the name matches
  - It is better to wrap the search in `if(form())` so as not to waste CPU time comparing strings

```cpp
portal.tick();
if (portal.form()) {
    Serial.print("Submit form: ");
    if (portal.form("/login")) Serial.println("Login");
    if (portal.form("/exit")) Serial.println("Exit");
}
```

### 3.3. Binding the handler

Instead of manually polling `form()`, you can bind your own function of the form `void f(GyverPortal*)`, it will be called when you click on any SUBMIT.
This function will need to be passed to `attachForm()`.

```cpp
void myHandler(GyverPortal* p) {
    // we have access to the portal object that sent the call
    if (p -> form("/exit")) Serial.println("exit");
}

void setup() {
    portal.attachForm(myHanlder);
}
```

### 3.4. Data parsing

The library implements tools for obtaining data from form components (see the documentation above). For example, output the contents of the text input field to the port:

```cpp
portal.tick();
if (portal.form("/login")) Serial.println(portal.getString("txt"));
// where "txt" is the name of the component
```

<a id="click"></a>

## 4. Handle clicks

### 4.1. Difference from forms

The library implements a mechanism that allows you to process actions on the page without reloading it (as when using forms):

- The form allows you to get values from several components at the click of a button. The page will reload.
- A click allows you to get the current (changed) value only from the clicked component. *The page will not reload*.

### 4.2. Action hanlding

- When clicking on some components or changing their values (see the table in the documentation), the `click()` function returns `true`
- The function should be polled after `tick()`
- To search for the component from which the signal came, use `click(name)`
  - will return `true` if the name matches
  - It is better to wrap the search in `if(click())` so as not to waste CPU time comparing strings

```cpp
portal.tick();
if (portal.click()) {
    if (portal.click("mybutton")) Serial.println("Click!");
}
```

### 4.3. Binding the handler

Instead of manually polling `click()`, you can bind your own function of the form `void f(GyverPortal*)`, it will be called when clicking on components.
This function will need to be passed to `attachClick()`.

```cpp
void myClick(GyverPortal* p) {
// we have access to the portal object that sent the call
    if (p -> click("mybutton")) Serial.println("Click!");
}

void setup() {
    portal.attachClick(myClick);
}
```

### 4.4. Data

Data parsing from clicks can be performed using the same functions as for forms.

### 4.5. Connecting a button to another component

The button can be connected to another component, when the button is clicked, update will be called with the button name and data from the specified location. To connect, you need to specify the name of the component with the third argument:

```cpp
add.BUTTON(buttonName, buttonText, componentName);
add.BUTTON_MINI(buttonName, buttonText, componentName);
```

<a id="update"></a>

## 5. Using updates

The library implements a mechanism for script requests from a page by timer. This allows you to update the values of some components and labels
(see the table in the documentation) without refreshing the page in the browser (like ReactJS).

### 5.1. Enabling updates

To enable the update mode, add the *AJAX_UPDATE* block to the beginning of the page:

```cpp
void build() {
    String s;
    BUILD_BEGIN(s);
    add.AJAX_UPDATE("name1,name2,name3");
    // ...
    add.LABEL("NAN", "val"); // will update the text
    BUILD_END();
}
```

- The AJAX_UPDATE function accepts a comma-separated list of component names.
- WE DO NOT PUT A SPACE AFTER THE COMMA.
- You can also specify the period of update requests in milliseconds `add.AJAX_UPDATE("name1,name2", 5000);`, default is 1000 (1 second).
- Not all components support the update mode (see the table in the documentation).

### 5.2. Update handling

- When an update occurs, the `update()` function returns `true`
- The function should be polled after `tick()`
- To search for the component from which the signal came, use `update(name)`
  - will return `true` if the name matches
  - It is better to wrap the search in `if(update())` so as not to waste CPU time comparing strings
- You need to respond to the update request using the `answer()` function. The actual value for the component is passed to it
- If you do not respond to the update before the next `tick()' call, the library will respond with an empty response so that the page does not hang

```cpp
portal.tick();
if (portal.update()) {
    if (portal.update("val")) portal.answer(random(1000));
}
```

### 5.3. Binding the handler

Instead of manually polling `update()`, you can connect your own function of the form `void f(GyverPortal*)`, it will be called when updating any component.
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

## 6. Automatic updating of variables

**[See demoSubmitAuto and demoClickAuto examples]** Instead of manual parsing, you can specify variables to the library
that will automatically receive new values from the specified page components. *This works for both forms and clicks.*

- Initialize the list by calling `.list.init(quantity)`, pass the size of the list in the number of variables.
- Adding a variable to its address:
  - `.list.add(&variable, name, type)` - specifying the name of the component and its type
  - `.list.add(&variable, form, name, type)` - specifying the form name, component name, and type

The specified variables will update their values when acting from a form with the specified name or when clicked.
If the form name is not specified, the component will be parsed when acting from any form. *To work with clicks, you do not need to specify the name of the form.*

### 6.1. Feature support by components, types

|   Data type   | Type/Component | TEXT/NUMBER | PASS | CHECK | SWITCH | DATE | TIME | SLIDER | COLOR | SELECT | AREA |
| ------------- | ------------- |:-----------:|:----:|:-----:|:------:|:----:|:----:|:------:|:-----:|:------:|:----:|
| `char[]`      | T_CSTR        |  ✔         |  ✔   |       |        |  ✔  |  ✔  |   ✔   |   ✔   |   ✔   |      |
| `String`      | T_STRING      |  ✔         |  ✔   |       |        |  ✔  |  ✔  |   ✔   |   ✔   |   ✔   |      |
| `GPtime`      | T_TIME        |             |      |        |       |      |  ✔  |        |       |        |      |
| `GPdate`      | T_DATE        |             |      |        |       |  ✔  |      |        |       |        |      |
| `bool`        | T_CHECK       |             |      |  ✔    |   ✔   |      |      |        |       |        |      |
| `byte`, `char`| T_BYTE        |  ✔         |  ✔  |        |        |      |      |   ✔   |       |        |      |
| `int`, `long` | T_INT         |  ✔         |  ✔   |       |        |      |      |   ✔   |       |        |      |
| `float`       | T_FLOAT       |  ✔         |  ✔   |       |        |      |      |   ✔   |       |        |      |
| `uint32_t`    | T_COLOR       |             |      |       |        |      |      |        |   ✔   |        |      |

<a id="plots"></a>

## 7. Graphs

### 7.1. General Features

#### Compatibility

The graphs **AJAX_PLOT** and **PLOT_STOCK** are incompatible in the same interface!

#### Output of fractional data

All three types of graphs have an argument `dec`, which is 0 by default. This is the divisor by which (if different from 0) the values of the graph points will be divided and converted to `float`.
In this way, you can display floating-point data and not store extra 2 bytes in memory. We got a temperature of 22.5 degrees, multiply by 10 and save it to an array. Calling the graph with `dec` equal to 10.  

#### Multiple axes

All graphs support output on multiple axes (the common X axis).

#### Labels

Labels are stored in the char array, for example like this:

```cpp
const char *names[] = {"kek", "puk"};
```

#### Updating static graphs

Static graphs display data when the page is reloaded. Thus, an array with the current values should be passed to the constructor.  
The library implements functions for conveniently adding a new value to an array (with automatic "scroll"):

```cpp
GPaddInt(int16_t val, int16_t* arr, uint8_t am);    // new value, array, array size
GPaddUnix(uint32_t val, uint32_t* arr, uint8_t am); // new value, array, array size
GPaddUnixS(int16_t val, uint32_t* arr, uint8_t am); // add seconds, array, array size
```

For example, there is an array `int arr[2][20]` - stores 20 values for two axes of the graph. You can update it and store it in the EEPROM, ensuring uninterrupted operation. To add a new value, we do according to our timer:

```cpp
GPaddInt(new, arr[0], 20);
GPaddInt(new, arr[1], 20);
```

We pass to the constructor as

```cpp
add.PLOT<2, 20>("table", names, arr);
```

#### Updating dynamic charts

The dynamic graph calls `update`, we respond to it with new values and it builds a graph in real time. To transfer values along multiple axes, we use
`answer(array, size)` or `answer(array, size, dec)`, where dec has the meaning of a divisor (see above).

### 7.2. PLOT graph

Lightweight static graph without scale
**[See staticPlot example](/examples/demos/staticPlot/staticPlot.ino)**
![demo](/docs/staticPlot.png)  

```cpp
add.PLOT<numberOfAxes, numberOfData>(name, labels, int16_t data, int dec = 0)
add.PLOT_DARK<numberOfAxes, numberOfData>(name, labels, int16_t data, int dec = 0)
```

### 7.3. PLOT_STOCK graph

Static graph with scale and time reference  
**[See stockPlot example](/examples/demos/stockPlot/stockPlot.ino)**
![demo](/docs/stockPlot.png)  

```cpp
add.PLOT_STOCK<numberOfAxes, numberOfData>(name, labels, timeArray, dataArray, int dec = 0)
add.PLOT_STOCK_DARK<numberOfAxes, numberOfData>(name, labels, timeArray, dataArray, int dec = 0)
```

This graph requires to display an array of date and time of type `uint32_t` containing time in unix format.

### 7.4. AJAX_PLOT Graph

Dynamic graph, calls update by its name, requires a response  
**[See the ajaxPlot example](/examples/demos/ajaxPlot/ajaxPlot.ino)**
![demo](/docs/ajaxPlot.png)  

```cpp
add.AJAX_PLOT(name, numberOfAxes, numberOfXPoints, updatePeriod);
add.AJAX_PLOT_DARK(name, numberOfAxes, numberOfXPoints, updatePeriod);
```

<a id="log"></a>

## 8. Log

The library has the ability to do `print()` in a special log window on the page:

- Only one log window can be created
- Updates occur automatically, once per second
- The page is not reloaded
- You can send any data as Serial

### 8.1. Connecting the log window

Add `add.AREA_LOG(numberOfRows)` to the right place on the page

### 8.2. Starting the log

Call `log.start(buffer size)`. The default buffer size is 64 characters

*Note:* this is the buffer size *on the library side*, that is, the limit on the number of characters per sending per page (once per second). The browser page has its own buffer for displaying text!

### 8.3. Data output

Just call `log.print()` or `log.println()` like a regular Serial. See example **demoLog**.

<a id="api"></a>

## 9. Custom components, API

### 9.1. Custom Constructor

The GyverPortal constructor does not restrict the construction of the page in any way: it is enough to add any HTML code to the line between the launch of `GP_BUILD(s)` and the completion of the `GP_SHOW() constructor`:

```cpp
void build() {
    String s;
    GP_BUILD(s);
    // building a page
    // ...
    GP_SHOW();
}
```

For reference:  
The standard `BUILD_BEGIN(String)` inside consists of:

```cpp
    GP_BUILD(s);
    add.PAGE_BEGIN();
    add.AJAX_CLICK();
    add.PAGE_BLOCK_BEGIN();
```

Standard `BUILD_END()` inside consists of:

```cpp
    add.PAGE_BLOCK_END();
    add.PAGE_END();
    GP_SHOW();
```

### 9.2. Custom page code

It is enough to add any HTML code to the line, for example:

```cpp
s += F("<input type=\"email\" class=\"MyClass\">");
```

You can wrap it in F macro so as not to take up RAM.

### 9.3. API

To ensure the functionality of the library mechanisms in custom components, the following points must be observed:

- If you need click support - add to the page `add.AJAX_CLICK()`
- Form components must have the *name* attribute specified for transmitting data via submit.
- Clickable components must have the *onclick* attribute specified with the function parameter `onclick="GP_click(this)"`. The library itself will intercept the call and direct it to `click()`.
- Components that need a `click()` signal to change data must have the *onchange* attribute specified with the function parameter `onchange="GP_click(this)"`. The library itself will intercept the call and send it to `click()`.
- Components that need `update()` updates must have the `id` attribute specified. Its value must also be passed to `add.UPDATE()`.
- If you need a click that transmits data from another component, specify an attribute with the function `onclick="gp_click(btn,tar)"`, where `btn` is the name (for the library) of the clicking component, and `tar` is the attribute`id` of the target component from which you want to transmit data.
- To manually transmit a click signal to the library, you need to send an http POST request of the form `GP_click?name=value`
- To manually transmit an update signal to the library, you need to send an http GET request of the form `GP_update?componentId`

<a id="example"></a>

## Examples

[See the examples folder](/examples/)

<a id="versions"></a>

## Versions

- v1.0
- v1.1 - improved graphics and styles
- v1.2
  - The NUMBER block is now the number type
  - Added a large text field AREA
  - Added GPunix
  - Improved parsing
  - Added BUTTON_MINI
  - Buttons can transmit data from other components (except AREA and checkboxes)
  - Added PLOT_STOCK - static graph with scale
  - Added AJAX_PLOT_DARK
  - Changed syntax of old graphs
  - GPaddUnix and GPaddInt features for graphs
  - Removed default theme
  - Tweaked styles
  - Added AREA_LOG log window and log function as a whole
- v1.3 - redesigned GPunix, minor fixes, you can use PSTR for lists
- v1.4 - minor fixes, clicking on COLOR now sends color
- v1.5 - added the "slider+signature" block
- v1.5.1 - minor fix for copying strings
- v1.6 - added color utils. Added answer() for date, time and color

<a id="feedback"></a>

## Bugs and feedback

If you find bugs, create **Issue**, or better yet, write to the mail [alex@alexgyver.ru](mailto:alex@alexgyver.ru)
The library is open for revision and your **Pull Requests**!
