#include <GyverPortal.h>
/*
const char plt[] PROGMEM = R"rawliteral(
<div id="chart-distance" class="container"></div>
<script>
var chartT = new Highcharts.Chart({chart:{renderTo:'chart-distance'},
title:{text:'HC-SR04 Distance'},series:[{showInLegend:false,data:[]}],
plotOptions: {line:{animation:false,dataLabels:{enabled:true}},series:{color: '#059e8a'}},
xAxis:{type:'datetime',dateTimeLabelFormats:{second:'%H:%M:%S'}},
yAxis:{title:{text:'Distance(CM)'}},credits:{enabled:false}});

setInterval(function(){var xhttp=new XMLHttpRequest();
xhttp.onreadystatechange=function(){if(this.readyState==4&&this.status==200){
var x=(new Date()).getTime(),y=parseFloat(this.responseText);
if(chartT.series[0].data.length > 40) chartT.series[0].addPoint([x, y],true,true,true);
else chartT.series[0].addPoint([x, y],true,false,true);
}};xhttp.open("GET","_GP_update?",true);xhttp.send();},1000);
</script>
)rawliteral";
*/
// билдер страницы
void build() {
  String s;
  BUILD_BEGIN(s);
  add.AJAX_PLOT("plot1", "Plotter", "Random value", 500);
  BUILD_END();
}

GyverPortal portal;

void setup() {
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  WiFi.begin("", "");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(WiFi.localIP());

  // подключаем билдер и запускаем
  portal.attachBuild(build);
  portal.start();
}

void loop() {
  portal.tick();
  if (portal.update()) {
    if (portal.update("plot1")) portal.answer(random(200));
  }
}
