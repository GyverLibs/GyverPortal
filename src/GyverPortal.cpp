#include <GyverPortal.h>

// // ======================= ���� =======================
String* _gp_sptr = nullptr;
void* _gp_ptr = nullptr;

// ======================= ������ =======================

    // ======================= �������� =======================
void Builder::PAGE_BEGIN() {

    *_gp_sptr += F("<!DOCTYPE HTML><html><head>\n"
        "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n"
        "<meta charset=\"utf-8\"></head><body>\n");
}
void Builder::AJAX_CLICK() {
    *_gp_sptr += F("<script>function GP_click(arg){var xhttp=new XMLHttpRequest();var v;\n"
        "if(arg.type==\"checkbox\")v=arg.checked?'1':'0';\nelse v=arg.value;\n"
        "if(v.charAt(0)=='#')v=v.substring(1);\n"
        "xhttp.open(\"POST\",\"GP_click?\"+arg.name+\"=\"+v,true);xhttp.send();}\n"
        "function GP_clickid(btn,tar){var xhttp=new XMLHttpRequest();\n"
        "xhttp.open(\"POST\",\"GP_click?\"+btn+\"=\"+document.getElementById(tar).value,true);xhttp.send();\n}"
        "</script>\n");
}
void Builder::THEME(const char* style) {
    *_gp_sptr += FPSTR(style);
}
void Builder::PAGE_BLOCK_BEGIN() {
    *_gp_sptr += F("<div align=\"center\" style=\"margin:auto;max-width:1000px;\">\n");
}
void Builder::PAGE_BLOCK_END() {
    *_gp_sptr += F("</div>");
}
void Builder::PAGE_END() {
    *_gp_sptr += F("</body></html>");
}
void Builder::AJAX_UPDATE(const char* list, int prd = 1000) {
    *_gp_sptr += F("<script>setInterval(function(){\n");
    *_gp_sptr += "var elms=[";
    char buf[strlen(list) + 1];
    strcpy(buf, list);
    char* str = buf;
    splitList(NULL);
    while ((str = splitList(buf)) != NULL) {
        *_gp_sptr += "'";
        if (str[0] == ' ') *_gp_sptr += (str + 1);
        else *_gp_sptr += str;
        *_gp_sptr += "',";
    }
    *_gp_sptr += F("];\n"
        "elms.forEach(function(elm){\n"
        "var xhttp=new XMLHttpRequest();\n"
        "xhttp.onreadystatechange=function(){\n"
        "if(this.readyState==4&&this.status==200){\n"
        "var resp=this.responseText;\n"
        "var item=document.getElementById(elm);\n"
        "if(item.type==\"checkbox\"||item.type==\"radio\")item.checked=Number(resp);\n"
        "else if(item.type==undefined)item.innerHTML=resp;\n"
        "else item.value=resp;\n"
        "}};xhttp.open(\"GET\",\"GP_update?\"+elm,true);xhttp.send();});},");
    *_gp_sptr += prd;
    *_gp_sptr += F(");</script>\n");
}
void Builder::AREA_LOG(int rows = 5) {
    *_gp_sptr += F("<textarea style=\"height:auto\" id=\"GP_log\" rows=\"");
    *_gp_sptr += rows;
    *_gp_sptr += F("\" disabled></textarea>");
    *_gp_sptr += F("<script>let _gplog=\"\";\n"
        "setInterval(function(){var xhttp=new XMLHttpRequest();\n"
        "xhttp.onreadystatechange=function(){\n"
        "if(this.readyState==4&&this.status==200){\n"
        "_gplog+=this.responseText;\n"
        "var elm=document.getElementById(\"GP_log\");\n"
        "elm.innerHTML=_gplog;elm.scrollTop=elm.scrollHeight;}};\n"
        "xhttp.open(\"GET\",\"GP_log?\",true);xhttp.send();},1000);</script>\n");
}

// ======================= ����� =======================
void Builder::FORM_BEGIN(const char* action) {
    *_gp_sptr += F("<form action=\"");
    *_gp_sptr += action;
    *_gp_sptr += F("\" method=\"POST\">\n");
}
void Builder::FORM_END() {
    *_gp_sptr += F("</form>\n");
}
void Builder::SUBMIT(const char* value) {
    *_gp_sptr += F("<input type=\"submit\" value=\"");
    *_gp_sptr += value;
    *_gp_sptr += F("\">\n");
}
void Builder::FORM_SUBMIT(const char* name, const char* value) {
    FORM_BEGIN(name);
    SUBMIT(value);
    FORM_END();
}

// ======================= ���������� =======================
void Builder::BLOCK_BEGIN() {
    *_gp_sptr += F("<div class=\"block\" id=\"blockBack\">\n");
}
void Builder::BLOCK_END() {
    *_gp_sptr += F("</div>\n");
}
void Builder::BREAK() {
    *_gp_sptr += F("<br>\n");
}
void Builder::HR() {
    *_gp_sptr += F("<hr>\n");
}

void Builder::TITLE(const char* name, const char* id) {
    *_gp_sptr += F("<h2 id=\"");
    *_gp_sptr += id;
    *_gp_sptr += F("\">");
    *_gp_sptr += name;
    *_gp_sptr += F("</h2>\n");
}
void Builder::LABEL(const char* name, const char* id) {
    *_gp_sptr += F("<label id=\"");
    *_gp_sptr += id;
    *_gp_sptr += F("\">");
    *_gp_sptr += name;
    *_gp_sptr += F("</label>\n");
}
void Builder::LABEL(int name, const char* id) {
    *_gp_sptr += F("<label id=\"");
    *_gp_sptr += id;
    *_gp_sptr += F("\">");
    *_gp_sptr += name;
    *_gp_sptr += F("</label>\n");
}

// ======================= ���������� =======================
void Builder::BUTTON(const char* name, const char* value) {
    *_gp_sptr += F("<input type=\"button\" value=\"");
    *_gp_sptr += value;
    *_gp_sptr += "\" name=\"";
    *_gp_sptr += name;
    *_gp_sptr += F("\" onclick=\"GP_click(this)\">\n");
}
void Builder::BUTTON(const char* name, const char* value, const char* tar) {
    *_gp_sptr += F("<input type=\"button\" value=\"");
    *_gp_sptr += value;
    *_gp_sptr += "\" name=\"";
    *_gp_sptr += name;
    *_gp_sptr += F("\" onclick=\"GP_clickid('");
    *_gp_sptr += name;
    *_gp_sptr += F("','");
    *_gp_sptr += tar;
    *_gp_sptr += F("')\">\n");
}
void Builder::BUTTON_MINI(const char* name, const char* value) {
    *_gp_sptr += F("<input class=\"miniButton\" type=\"button\" value=\"");
    *_gp_sptr += value;
    *_gp_sptr += "\" name=\"";
    *_gp_sptr += name;
    *_gp_sptr += F("\" onclick=\"GP_click(this)\">\n");
}
void Builder::BUTTON_MINI(const char* name, const char* value, const char* tar) {
    *_gp_sptr += F("<input class=\"miniButton\" type=\"button\" value=\"");
    *_gp_sptr += value;
    *_gp_sptr += "\" name=\"";
    *_gp_sptr += name;
    *_gp_sptr += F("\" onclick=\"GP_clickid('");
    *_gp_sptr += name;
    *_gp_sptr += F("','");
    *_gp_sptr += tar;
    *_gp_sptr += F("')\">\n");
}

void Builder::NUMBER(const char* name, const char* place, int value = INT32_MAX) {
    *_gp_sptr += F("<input type=\"number\" name=\"");
    *_gp_sptr += name;
    *_gp_sptr += F("\" id=\"");
    *_gp_sptr += name;
    if (value != INT32_MAX) {
        *_gp_sptr += F("\" value=\"");
        *_gp_sptr += value;
    }
    *_gp_sptr += F("\" placeholder=\"");
    *_gp_sptr += place;
    *_gp_sptr += F("\" onchange=\"GP_click(this)\">\n");
}
void Builder::AREA(const char* name, int rows, char* value) {
    *_gp_sptr += F("<textarea style=\"height:auto\" name=\"");
    *_gp_sptr += name;
    *_gp_sptr += F("\" id=\"");
    *_gp_sptr += name;
    *_gp_sptr += F("\" rows=\"");
    *_gp_sptr += rows;
    *_gp_sptr += F("\">");
    *_gp_sptr += value;
    *_gp_sptr += F("</textarea>");
}
void Builder::AREA(const char* name, int rows = 1) {
    *_gp_sptr += F("<textarea style=\"height:auto\" name=\"");
    *_gp_sptr += name;
    *_gp_sptr += F("\" id=\"");
    *_gp_sptr += name;
    *_gp_sptr += F("\" rows=\"");
    *_gp_sptr += rows;
    *_gp_sptr += F("\">");
    *_gp_sptr += F("</textarea>");
}
void Builder::TEXT(const char* name, const char* place, const char* value = "") {
    *_gp_sptr += F("<input type=\"text\" name=\"");
    *_gp_sptr += name;
    *_gp_sptr += F("\" id=\"");
    *_gp_sptr += name;
    *_gp_sptr += F("\" value=\"");
    *_gp_sptr += value;
    *_gp_sptr += F("\" placeholder=\"");
    *_gp_sptr += place;
    *_gp_sptr += F("\" onchange=\"GP_click(this)\">\n");
}
void Builder::TEXT(const char* name, const char* place, String& value) {
    TEXT(name, place, (const char*)value.c_str());
}
void Builder::TEXT(const char* name, const char* place, char* value) {
    TEXT(name, place, (const char*)value);
}
void Builder::PASS(const char* name, const char* place, const char* value = "") {
    *_gp_sptr += F("<input type=\"password\" name=\"");
    *_gp_sptr += name;
    *_gp_sptr += F("\" id=\"");
    *_gp_sptr += name;
    *_gp_sptr += F("\" value=\"");
    *_gp_sptr += value;
    *_gp_sptr += F("\" placeholder=\"");
    *_gp_sptr += place;
    *_gp_sptr += F("\" onchange=\"GP_click(this)\">\n");
}
void Builder::PASS(const char* name, const char* place, String& value) {
    PASS(name, place, (const char*)value.c_str());
}
void Builder::PASS(const char* name, const char* place, char* value) {
    PASS(name, place, (const char*)value);
}
void Builder::CHECK(const char* name, bool x = 0) {
    *_gp_sptr += F("<input type=\"checkbox\" name=\"");
    *_gp_sptr += name;
    *_gp_sptr += F("\" id=\"");
    *_gp_sptr += name;
    *_gp_sptr += (x ? F("\" checked") : F("\""));
    *_gp_sptr += F(" onclick=\"GP_click(this)\">\n");
}
void Builder::SWITCH(const char* name, bool x = 0) {
    *_gp_sptr += F("<label class=\"switch\"><input type=\"checkbox\" name=\"");
    *_gp_sptr += name;
    *_gp_sptr += F("\" id=\"");
    *_gp_sptr += name;
    *_gp_sptr += (x ? F("\" checked") : F("\""));
    *_gp_sptr += F(" onclick=\"GP_click(this)\">\n");
    *_gp_sptr += F("<span class=\"slider\"></span></label>");
}
void Builder::DATE(const char* name) {
    *_gp_sptr += F("<input type=\"date\" name=\"");
    *_gp_sptr += name;
    *_gp_sptr += F("\" id=\"");
    *_gp_sptr += name;
    *_gp_sptr += F("\" onchange=\"GP_click(this)\">\n");
}
void Builder::DATE(const char* name, GPdate d) {
    *_gp_sptr += F("<input type=\"date\" name=\"");
    *_gp_sptr += name;
    *_gp_sptr += F("\" id=\"");
    *_gp_sptr += name;
    *_gp_sptr += F("\" value=\"");
    char buf[11];
    encodeDate(buf, d);
    *_gp_sptr += buf;
    *_gp_sptr += F("\" onchange=\"GP_click(this)\">\n");
}
void Builder::TIME(const char* name) {
    *_gp_sptr += F("<input type=\"time\" name=\"");
    *_gp_sptr += name;
    *_gp_sptr += F("\" id=\"");
    *_gp_sptr += name;
    *_gp_sptr += F("\" onchange=\"GP_click(this)\">\n");
}
void Builder::TIME(const char* name, GPtime t) {
    *_gp_sptr += F("<input type=\"time\" name=\"");
    *_gp_sptr += name;
    *_gp_sptr += F("\" id=\"");
    *_gp_sptr += name;
    *_gp_sptr += F("\" value=\"");
    char buf[9];
    encodeTime(buf, t);
    *_gp_sptr += buf;
    *_gp_sptr += F("\" onchange=\"GP_click(this)\">\n");
}
void Builder::LABEL_MINI(int text) {
    *_gp_sptr += F("<label class=\"sldLbl\">");
    *_gp_sptr += text;
    *_gp_sptr += F("</label>");
}
void Builder::SLIDER(const char* name, int value, int min, int max, int step) {
    LABEL_MINI(min);
    *_gp_sptr += F("<input type=\"range\" name=\"");
    *_gp_sptr += name;
    *_gp_sptr += F("\" id=\"");
    *_gp_sptr += name;
    *_gp_sptr += F("\" value=\"");
    *_gp_sptr += value;
    *_gp_sptr += F("\" min=\"");
    *_gp_sptr += min;
    *_gp_sptr += F("\" max=\"");
    *_gp_sptr += max;
    *_gp_sptr += F("\" step=\"");
    *_gp_sptr += step;
    *_gp_sptr += F("\" onchange=\"GP_click(this)\">\n");
    LABEL_MINI(max);
}
void Builder::SLIDER(const char* name, const char* label, int value, int min, int max, int step) {
    *_gp_sptr += F("<div class=\"sldBlock\">");
    *_gp_sptr += F("<label>");
    *_gp_sptr += label;
    *_gp_sptr += F("</label>");
    SLIDER(name, value, min, max, step);
    *_gp_sptr += F("</div>");
}
void Builder::COLOR(const char* name, uint32_t value = 0) {
    *_gp_sptr += F("<input type=\"color\" name=\"");
    *_gp_sptr += name;
    *_gp_sptr += F("\" id=\"");
    *_gp_sptr += name;
    *_gp_sptr += F("\" value=\"");
    *_gp_sptr += encodeColor(value);
    *_gp_sptr += F("\" onchange=\"GP_click(this)\">\n");
}
void Builder::COLOR(const char* name, GPcolor value) {
    *_gp_sptr += F("<input type=\"color\" name=\"");
    *_gp_sptr += name;
    *_gp_sptr += F("\" id=\"");
    *_gp_sptr += name;
    *_gp_sptr += F("\" value=\"");
    *_gp_sptr += encodeColor(value);
    *_gp_sptr += F("\" onchange=\"GP_click(this)\">\n");
}

void Builder::SELECT(const char* name, const char* values, int8_t sel = 0) {
    if (sel < 0) sel = 0;
    *_gp_sptr += F("<select name=\"");
    *_gp_sptr += name;
    *_gp_sptr += F("\" id=\"");
    *_gp_sptr += name;
    *_gp_sptr += F("\" onchange=\"GP_click(this)\">\n");
    char buf[strlen(values) + 1];
    strcpy(buf, values);
    char* str = buf;
    uint8_t count = 0;
    splitList(NULL);
    while ((str = splitList(buf)) != NULL) {
        *_gp_sptr += F("<option value=\"");
        *_gp_sptr += str;
        *_gp_sptr += F("\"");
        if (count++ == sel) *_gp_sptr += F("selected");
        *_gp_sptr += F(">");
        *_gp_sptr += str;
        *_gp_sptr += F("</option>\n");
    }
    *_gp_sptr += F("</select>");
}
void Builder::LED_RED(const char* name, bool state = 0) {
    *_gp_sptr += F("<input class=\"led red\" type=\"radio\" disabled ");
    if (state) *_gp_sptr += F("checked ");
    *_gp_sptr += F("name=\"");
    *_gp_sptr += name;
    *_gp_sptr += F("\" id=\"");
    *_gp_sptr += name;
    *_gp_sptr += F("\">");
}
void Builder::LED_GREEN(const char* name, bool state = 0) {
    *_gp_sptr += F("<input class=\"led green\" type=\"radio\" disabled ");
    if (state) *_gp_sptr += F("checked ");
    *_gp_sptr += F("name=\"");
    *_gp_sptr += name;
    *_gp_sptr += F("\" id=\"");
    *_gp_sptr += name;
    *_gp_sptr += F("\">");
}

// ======================= ������� =======================
template <uint8_t ax, uint8_t am>
void Builder::PLOT(const char* id, const char** labels, int16_t vals[ax][am], int dec/* = 0*/) {
    *_gp_sptr += F("<script type=\"text/javascript\" src=\"https://www.gstatic.com/charts/loader.js\"></script>\n"
        "<script type=\"text/javascript\">\n"
        "google.charts.load('current', {'packages':['corechart']});\n"
        "google.charts.setOnLoadCallback(drawChart);\n"
        "function drawChart() {\n"
        "var data = google.visualization.arrayToDataTable([\n");

    *_gp_sptr += '[';
    for (int i = 0; i < ax + 1; i++) {
        *_gp_sptr += '\'';
        if (i) *_gp_sptr += labels[i - 1];
        *_gp_sptr += "',";
    }
    *_gp_sptr += "],\n";
    for (int j = 0; j < am; j++) {
        *_gp_sptr += '[';
        for (int i = 0; i < ax + 1; i++) {
            if (!i) *_gp_sptr += '\'';
            if (!i) *_gp_sptr += j;
            else {
                if (dec) *_gp_sptr += (float)vals[i - 1][j] / dec;
                else *_gp_sptr += vals[i - 1][j];
            }
            if (!i) *_gp_sptr += '\'';
            *_gp_sptr += ',';
        }
        *_gp_sptr += F("],\n");
    }

    *_gp_sptr += F("]);var options = {pointSize:5,curveType:'function','chartArea':{'width':'90%','height':'90%'},\n"
        "backgroundColor:'none',hAxis:{title:''},vAxis:{title:''},\n"
        "legend: {position:'bottom'}};\n"
        "var chart = new google.visualization.LineChart(document.getElementById('");
    *_gp_sptr += id;
    *_gp_sptr += F("'));\n");
    *_gp_sptr += F("chart.draw(data, options);}\n"
        "</script><div id=\"");
    *_gp_sptr += id;
    *_gp_sptr += F("\" class=\"chartBlock\"></div>\n");
}

template <uint8_t ax, uint8_t am>
void Builder::PLOT_DARK(const char* id, const char** labels, int16_t vals[ax][am], int dec/* = 0*/) {
    *_gp_sptr += F("<script type=\"text/javascript\" src=\"https://www.gstatic.com/charts/loader.js\"></script>\n"
        "<script type=\"text/javascript\">\n"
        "google.charts.load('current', {'packages':['corechart']});\n"
        "google.charts.setOnLoadCallback(drawChart);\n"
        "function drawChart() {\n"
        "var data = google.visualization.arrayToDataTable([\n");

    *_gp_sptr += '[';
    for (int i = 0; i < ax + 1; i++) {
        *_gp_sptr += '\'';
        if (i) *_gp_sptr += labels[i - 1];
        *_gp_sptr += "',";
    }
    *_gp_sptr += "],\n";
    for (int j = 0; j < am; j++) {
        *_gp_sptr += '[';
        for (int i = 0; i < ax + 1; i++) {
            if (!i) *_gp_sptr += '\'';
            if (!i) *_gp_sptr += j;
            else {
                if (dec) *_gp_sptr += (float)vals[i - 1][j] / dec;
                else *_gp_sptr += vals[i - 1][j];
            }
            if (!i) *_gp_sptr += '\'';
            *_gp_sptr += ',';
        }
        *_gp_sptr += F("],\n");
    }

    *_gp_sptr += F("]);var options = {pointSize:5,curveType:'function','chartArea':{'width':'90%','height':'90%'},\n"
        "backgroundColor:'none',hAxis:{title:'',titleTextStyle:{color:'#ddd'},textStyle:{color:'#bbb'}},\n"
        "vAxis:{title:'',titleTextStyle:{color:'#ddd'},gridlines:{color:'#777'},textStyle:{color:'#bbb'}},\n"
        "legend: {position:'bottom',textStyle:{color:'#eee'}}};\n"
        "var chart = new google.visualization.LineChart(document.getElementById('");
    *_gp_sptr += id;
    *_gp_sptr += F("'));\n");
    *_gp_sptr += F("chart.draw(data, options);}\n");
    *_gp_sptr += F("</script><div id=\"");
    *_gp_sptr += id;
    *_gp_sptr += F("\" class=\"chartBlock\"></div>\n");
}

void Builder::AJAX_PLOT(const char* name, int axes, int xamount = 20, int prd = 1000) {
    *_gp_sptr += F(""
        "<script src=\"https://code.highcharts.com/highcharts.js\"></script>\n"
        "<div id=\"");
    *_gp_sptr += name;
    *_gp_sptr += F("\" class=\"chartBlock\"></div><script>\n"
        "var ");
    *_gp_sptr += name;
    *_gp_sptr += F("=new Highcharts.Chart({\n"
        "chart:{borderRadius:10,renderTo:'");
    *_gp_sptr += name;
    *_gp_sptr += F("',style:{fontFamily:\"sans-serif\"}},\n"
        "title:{text:''},"
        "series:[");
    for (int i = 0; i < axes; i++) {
        *_gp_sptr += F("{data:[]}");
        if (i != axes - 1) *_gp_sptr += ',';
    }
    *_gp_sptr += F("],\n"
        "xAxis:{type:'datetime',dateTimeLabelFormats:{second:'%H:%M:%S'}},\n"
        "yAxis:{title:{enabled:false}},"
        "credits:{enabled:false}});\n"
        "setInterval(function(){var xhttp=new XMLHttpRequest();var ch=");
    *_gp_sptr += name;
    *_gp_sptr += F("\n"
        "xhttp.onreadystatechange=function(){if(this.readyState==4&&this.status==200){\n"
        "var x=(new Date()).getTime();"
        "var arr=this.responseText.split(',');"
        "var move=(ch.series[0].data.length>");
    *_gp_sptr += xamount;
    *_gp_sptr += F(");\n"
        "for(let i=0;i<arr.length;i++)ch.series[i].addPoint([x,Number(arr[i])],true,move,true);\n"
        "}};xhttp.open(\"GET\",\"GP_update?");
    *_gp_sptr += name;
    *_gp_sptr += F("\",true);xhttp.send();},\n");
    *_gp_sptr += prd;
    *_gp_sptr += F(");</script>\n");
}

void Builder::AJAX_PLOT_DARK(const char* name, int axes, int xamount = 20, int prd = 1000) {
    *_gp_sptr += F(""
        "<script src=\"https://code.highcharts.com/highcharts.js\"></script>\n"
        "<script src=\"https://code.highcharts.com/themes/dark-unica.js\"></script>\n"
        "<div id=\"");
    *_gp_sptr += name;
    *_gp_sptr += F("\" class=\"chartBlock\"></div><script>\n"
        "var ");
    *_gp_sptr += name;
    *_gp_sptr += F("=new Highcharts.Chart({\n"
        "chart:{borderRadius:10,renderTo:'");
    *_gp_sptr += name;
    *_gp_sptr += F("',style:{fontFamily:\"sans-serif\"}},\n"
        "title:{text:''},"
        "series:[");
    for (int i = 0; i < axes; i++) {
        *_gp_sptr += F("{data:[]}");
        if (i != axes - 1) *_gp_sptr += ',';
    }
    *_gp_sptr += F("],\n"
        "xAxis:{type:'datetime',dateTimeLabelFormats:{second:'%H:%M:%S'}},\n"
        "yAxis:{title:{enabled:false}},"
        "credits:{enabled:false}});\n"
        "setInterval(function(){var xhttp=new XMLHttpRequest();var ch=");
    *_gp_sptr += name;
    *_gp_sptr += F("\n"
        "xhttp.onreadystatechange=function(){if(this.readyState==4&&this.status==200){\n"
        "var x=(new Date()).getTime();"
        "var arr=this.responseText.split(',');"
        "var move=(ch.series[0].data.length>");
    *_gp_sptr += xamount;
    *_gp_sptr += F(");\n"
        "for(let i=0;i<arr.length;i++)ch.series[i].addPoint([x,Number(arr[i])],true,move,true);\n"
        "}};xhttp.open(\"GET\",\"GP_update?");
    *_gp_sptr += name;
    *_gp_sptr += F("\",true);xhttp.send();},\n");
    *_gp_sptr += prd;
    *_gp_sptr += F(");</script>\n");
}

template <uint8_t ax, uint8_t am>
void Builder::PLOT_STOCK(const char* id, const char** labels, uint32_t* times, int16_t vals[ax][am], int dec/* = 0*/) {
    *_gp_sptr += F("<script src=\"https://code.highcharts.com/stock/highstock.js\"></script>\n"
        "<div class=\"chartBlock\" id=\"");
    *_gp_sptr += id;
    *_gp_sptr += F("\"></div>");
    *_gp_sptr += F("<script>Highcharts.stockChart('");
    *_gp_sptr += id;
    *_gp_sptr += F("',{chart:{},\n"
        "rangeSelector:{buttons:[\n"
        "{count:1,type:'minute',text:'1M'},\n"
        "{count:1,type:'hour',text:'1H'},\n"
        "{count:1,type:'day',text:'1D'},\n"
        "{type:'all',text:'All'}],\n"
        "inputEnabled:false,selected:0},\n"
        "time:{useUTC:false},\n"
        "credits:{enabled:false},series:[\n");
    for (int axs = 0; axs < ax; axs++) {
        *_gp_sptr += F("{name:'");
        *_gp_sptr += labels[axs];
        *_gp_sptr += F("',data:[\n");
        for (int ams = 0; ams < am; ams++) {
            *_gp_sptr += '[';
            *_gp_sptr += times[ams];
            *_gp_sptr += F("000");
            *_gp_sptr += ',';
            if (dec) *_gp_sptr += (float)vals[axs][ams] / dec;
            else *_gp_sptr += vals[axs][ams];
            *_gp_sptr += "],\n";
        }
        *_gp_sptr += "]},\n";
    }
    *_gp_sptr += F("]});</script>\n");
}

template <uint8_t ax, uint8_t am>
void Builder::PLOT_STOCK_DARK(const char* id, const char** labels, uint32_t* times, int16_t vals[ax][am], int dec/* = 0*/) {
    *_gp_sptr += F("<script src=\"https://code.highcharts.com/stock/highstock.js\"></script>\n"
        "<script src=\"https://code.highcharts.com/themes/dark-unica.js\"></script>"
        "<div class=\"chartBlock\" id=\"");
    *_gp_sptr += id;
    *_gp_sptr += F("\"></div>");
    *_gp_sptr += F("<script>Highcharts.stockChart('");
    *_gp_sptr += id;
    *_gp_sptr += F("',{chart:{},\n"
        "rangeSelector:{buttons:[\n"
        "{count:1,type:'minute',text:'1M'},\n"
        "{count:1,type:'hour',text:'1H'},\n"
        "{count:1,type:'day',text:'1D'},\n"
        "{type:'all',text:'All'}],\n"
        "inputEnabled:false,selected:0},\n"
        "time:{useUTC:false},\n"
        "credits:{enabled:false},series:[\n");
    for (int axs = 0; axs < ax; axs++) {
        *_gp_sptr += F("{name:'");
        *_gp_sptr += labels[axs];
        *_gp_sptr += F("',data:[\n");
        for (int ams = 0; ams < am; ams++) {
            *_gp_sptr += '[';
            *_gp_sptr += times[ams];
            *_gp_sptr += F("000");
            *_gp_sptr += ',';
            if (dec) *_gp_sptr += (float)vals[axs][ams] / dec;
            else *_gp_sptr += vals[axs][ams];
            *_gp_sptr += "],\n";
        }
        *_gp_sptr += "]},\n";
    }
    *_gp_sptr += F("]});</script>\n");
}


extern Builder add = Builder();

// ======================= ������ =======================
void GP_BUILD(String& s) {
    _gp_sptr = &s;
}
void GP_SHOW() {
    if (_gp_ptr && _gp_sptr) (*(GyverPortal*)_gp_ptr).showPage(*_gp_sptr);
}

void BUILD_BEGIN(String& s) {
    GP_BUILD(s);
    add.PAGE_BEGIN();
    add.AJAX_CLICK();
    add.PAGE_BLOCK_BEGIN();
}
void BUILD_END() {
    add.PAGE_BLOCK_END();
    add.PAGE_END();
    GP_SHOW();
}
