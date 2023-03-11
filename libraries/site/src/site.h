#include <DNSServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#include "themes.h"
#include "list.h"
#include "utils.h"
#include "portal.h"

struct Builder {
    void PAGE_BEGIN() {
        *sptr += F("<!DOCTYPE HTML><html><head>\n"
        "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n"
        "</head><body>\n");
    }
    void AJAX_CLICK() {
        *sptr += F("<script>function click(arg){var xhttp=new XMLHttpRequest();var v;\n"
        "if(arg.type==\"checkbox\")v=arg.checked?'1':'0';\nelse v=arg.value;\n"
        "if(v.charAt(0)=='#')v=v.substring(1);\n"
        "xhttp.open(\"POST\",\"click?\"+arg.name+\"=\"+v,true);xhttp.send();}\n"
        "function clickid(btn,tar){var xhttp=new XMLHttpRequest();\n"
        "xhttp.open(\"POST\",\"click?\"+btn+\"=\"+document.getElementById(tar).value,true);xhttp.send();\n}"
        "</script>\n");
    }
    void THEME(const char* style) {
        *sptr += FPSTR(style);
    }
    void PAGE_BLOCK_BEGIN() {
        *sptr += F("<div align=\"center\" style=\"margin:auto;max-width:1000px;\">\n");
    }
    void PAGE_BLOCK_END() {
        *sptr += F("</div>");
    }
    void PAGE_END() {
        *sptr += F("</body></html>");
    }
    void AJAX_UPDATE(const char* list, int prd = 1000) {
        *sptr += F("<script>setInterval(function(){\n");
        *sptr += "var elms=[";
        char buf[strlen(list) + 1];
        strcpy(buf, list);
        char* str = buf;
        splitList(NULL);
        while ((str = splitList(buf)) != NULL) {
            *sptr += "'";
            if (str[0] == ' ') *sptr += (str + 1);
            else *sptr += str;
            *sptr += "',";
        }
        *sptr += F("];\n"
        "elms.forEach(function(elm){\n"
        "var xhttp=new XMLHttpRequest();\n"
        "xhttp.onreadystatechange=function(){\n"
        "if(this.readyState==4&&this.status==200){\n"
        "var resp=this.responseText;\n"
        "var item=document.getElementById(elm);\n"
        "if(item.type==\"checkbox\"||item.type==\"radio\")item.checked=Number(resp);\n"
        "else if(item.type==undefined)item.innerHTML=resp;\n"
        "else item.value=resp;\n"
        "}};xhttp.open(\"GET\",\"update?\"+elm,true);xhttp.send();});},");
        *sptr += prd;
        *sptr += F(");</script>\n");
    }
    void AREA_LOG(int rows = 5) {
        *sptr += F("<textarea style=\"height:auto\" id=\"log\" rows=\"");
        *sptr += rows;
        *sptr += F("\" disabled></textarea>");
        *sptr += F("<script>let log=\"\";\n"
        "setInterval(function(){var xhttp=new XMLHttpRequest();\n"
        "xhttp.onreadystatechange=function(){\n"
        "if(this.readyState==4&&this.status==200){\n"
        "log+=this.responseText;\n"
        "var elm=document.getElementById(\"log\");\n"
        "elm.innerHTML=log;elm.scrollTop=elm.scrollHeight;}};\n"
        "xhttp.open(\"GET\",\"log?\",true);xhttp.send();},1000);</script>\n");
    }
    
    void FORM_BEGIN(const char* action) {
        *sptr += F("<form action=\"");
        *sptr += action;
        *sptr += F("\" method=\"POST\">\n");
    }
    void FORM_END() {
        *sptr += F("</form>\n");
    }
    void SUBMIT(const char* value) {
        *sptr += F("<input type=\"submit\" value=\"");
        *sptr += value;
        *sptr += F("\">\n");
    }
    void FORM_SUBMIT(const char* name, const char* value) {
        FORM_BEGIN(name);
        SUBMIT(value);
        FORM_END();
    }

    void BLOCK_BEGIN() {
        *sptr += F("<div class=\"block\" id=\"blockBack\">\n");
    }
    void BLOCK_END() {
        *sptr += F("</div>\n");
    }
    void BREAK() {
        *sptr += F("<br>\n");
    }
    void HR() {
        *sptr += F("<hr>\n");
    }
    
    void TITLE(const char* name, const char* id="") {
        *sptr += F("<h2 id=\"");
        *sptr += id;
        *sptr += F("\">");
        *sptr += name;
        *sptr += F("</h2>\n");
    }
    void LABEL(const char* name, const char* id="") {
        *sptr += F("<label id=\"");
        *sptr += id;
        *sptr += F("\">");
        *sptr += name;
        *sptr += F("</label>\n");
    }
    void LABEL(int name, const char* id="") {
        *sptr += F("<label id=\"");
        *sptr += id;
        *sptr += F("\">");
        *sptr += name;
        *sptr += F("</label>\n");
    }
    
    void BUTTON(const char* name, const char* value) {
        *sptr += F("<input type=\"button\" value=\"");
        *sptr += value;
        *sptr += "\" name=\"";
        *sptr += name;
        *sptr += F("\" onclick=\"click(this)\">\n");
    }
    void BUTTON(const char* name, const char* value, const char* tar) {
        *sptr += F("<input type=\"button\" value=\"");
        *sptr += value;
        *sptr += "\" name=\"";
        *sptr += name;
        *sptr += F("\" onclick=\"clickid('");
        *sptr += name;
        *sptr += F("','");
        *sptr += tar;
        *sptr += F("')\">\n");
    }
    void BUTTON_MINI(const char* name, const char* value) {
        *sptr += F("<input class=\"miniButton\" type=\"button\" value=\"");
        *sptr += value;
        *sptr += "\" name=\"";
        *sptr += name;
        *sptr += F("\" onclick=\"click(this)\">\n");
    }
    void BUTTON_MINI(const char* name, const char* value, const char* tar) {
        *sptr += F("<input class=\"miniButton\" type=\"button\" value=\"");
        *sptr += value;
        *sptr += "\" name=\"";
        *sptr += name;
        *sptr += F("\" onclick=\"clickid('");
        *sptr += name;
        *sptr += F("','");
        *sptr += tar;
        *sptr += F("')\">\n");
    }
    
    void NUMBER(const char* name, const char* place, int value = INT32_MAX) {
        *sptr += F("<input type=\"number\" name=\"");
        *sptr += name;
        *sptr += F("\" id=\"");
        *sptr += name;
        if (value != INT32_MAX) {
            *sptr += F("\" value=\"");
            *sptr += value;
        }
        *sptr += F("\" placeholder=\"");
        *sptr += place;
        *sptr += F("\" onchange=\"click(this)\">\n");
    }
    void AREA(const char* name, int rows, char* value) {
        *sptr += F("<textarea style=\"height:auto\" name=\"");
        *sptr += name;
        *sptr += F("\" id=\"");
        *sptr += name;
        *sptr += F("\" rows=\"");
        *sptr += rows;
        *sptr += F("\">");
        *sptr += value;
        *sptr += F("</textarea>");
    }
    void AREA(const char* name, int rows = 1) {
        *sptr += F("<textarea style=\"height:auto\" name=\"");
        *sptr += name;
        *sptr += F("\" id=\"");
        *sptr += name;
        *sptr += F("\" rows=\"");
        *sptr += rows;
        *sptr += F("\">");
        *sptr += F("</textarea>");
    }
    void TEXT(const char* name, const char* place, const char* value = "") {
        *sptr += F("<input type=\"text\" name=\"");
        *sptr += name;
        *sptr += F("\" id=\"");
        *sptr += name;
        *sptr += F("\" value=\"");
        *sptr += value;
        *sptr += F("\" placeholder=\"");
        *sptr += place;
        *sptr += F("\" onchange=\"click(this)\">\n");
    }
    void TEXT(const char* name, const char* place, String& value) {
        TEXT(name, place, (const char*)value.c_str());
    }
    void TEXT(const char* name, const char* place, char* value) {
        TEXT(name, place, (const char*)value);
    }
    void PASS(const char* name, const char* place, const char* value = "") {
        *sptr += F("<input type=\"password\" name=\"");
        *sptr += name;
        *sptr += F("\" id=\"");
        *sptr += name;
        *sptr += F("\" value=\"");
        *sptr += value;
        *sptr += F("\" placeholder=\"");
        *sptr += place;
        *sptr += F("\" onchange=\"click(this)\">\n");
    }
    void PASS(const char* name, const char* place, String& value) {
        PASS(name, place, (const char*)value.c_str());
    }
    void PASS(const char* name, const char* place, char* value) {
        PASS(name, place, (const char*)value);
    }
    void CHECK(const char* name, bool x = 0) {
        *sptr += F("<input type=\"checkbox\" name=\"");
        *sptr += name;
        *sptr += F("\" id=\"");
        *sptr += name;
        *sptr += (x ? F("\" checked") : F("\""));
        *sptr += F(" onclick=\"click(this)\">\n");
    }
    void SWITCH(const char* name, bool x = 0) {
        *sptr += F("<label class=\"switch\"><input type=\"checkbox\" name=\"");
        *sptr += name;
        *sptr += F("\" id=\"");
        *sptr += name;
        *sptr += (x ? F("\" checked") : F("\""));
        *sptr += F(" onclick=\"click(this)\">\n");
        *sptr += F("<span class=\"slider\"></span></label>");
    }
    void DATE(const char* name) {
        *sptr += F("<input type=\"date\" name=\"");
        *sptr += name;
        *sptr += F("\" id=\"");
        *sptr += name;
        *sptr += F("\" onchange=\"click(this)\">\n");
    }
    void DATE(const char* name, date d) {
        *sptr += F("<input type=\"date\" name=\"");
        *sptr += name;
        *sptr += F("\" id=\"");
        *sptr += name;
        *sptr += F("\" value=\"");
        char buf[11];
        encodeDate(buf, d);
        *sptr += buf;
        *sptr += F("\" onchange=\"click(this)\">\n");
    }
    void TIME(const char* name) {
        *sptr += F("<input type=\"time\" name=\"");
        *sptr += name;
        *sptr += F("\" id=\"");
        *sptr += name;
        *sptr += F("\" onchange=\"click(this)\">\n");
    }
    void TIME(const char* name, time t) {
        *sptr += F("<input type=\"time\" name=\"");
        *sptr += name;
        *sptr += F("\" id=\"");
        *sptr += name;
        *sptr += F("\" value=\"");
        char buf[9];
        encodeTime(buf, t);
        *sptr += buf;
        *sptr += F("\" onchange=\"click(this)\">\n");
    }
    void LABEL_MINI(int text) {
        *sptr += F("<label class=\"sldLbl\">");
        *sptr += text;
        *sptr += F("</label>");
    }
    void SLIDER(const char* name, int value, int min, int max, int step = 1) {
        LABEL_MINI(min);
        *sptr += F("<input type=\"range\" name=\"");
        *sptr += name;
        *sptr += F("\" id=\"");
        *sptr += name;
        *sptr += F("\" value=\"");
        *sptr += value;
        *sptr += F("\" min=\"");
        *sptr += min;
        *sptr += F("\" max=\"");
        *sptr += max;
        *sptr += F("\" step=\"");
        *sptr += step;
        *sptr += F("\" onchange=\"click(this)\">\n");
        LABEL_MINI(max);
    }
    void SLIDER(const char* name, const char* label, int value, int min, int max, int step = 1) {
        *sptr += F("<div class=\"sldBlock\">");
        *sptr += F("<label>");
        *sptr += label;
        *sptr += F("</label>");
        SLIDER(name, value, min, max, step);
        *sptr += F("</div>");
    }
    void COLOR(const char* name, uint32_t value = 0) {
        *sptr += F("<input type=\"color\" name=\"");
        *sptr += name;
        *sptr += F("\" id=\"");
        *sptr += name;
        *sptr += F("\" value=\"");
        *sptr += encodeColor(value);
        *sptr += F("\" onchange=\"click(this)\">\n");
    }

    void SELECT(const char* name, const char* values, int8_t sel = 0) {
        if (sel < 0) sel = 0;
        *sptr += F("<select name=\"");
        *sptr += name;
        *sptr += F("\" id=\"");
        *sptr += name;
        *sptr += F("\" onchange=\"click(this)\">\n");
        char buf[strlen(values) + 1];
        strcpy(buf, values);
        char* str = buf;
        uint8_t count = 0;
        splitList(NULL);
        while ((str = splitList(buf)) != NULL) {
            *sptr += F("<option value=\"");
            *sptr += str;
            *sptr += F("\"");
            if (count++ == sel) *sptr += F("selected");
            *sptr += F(">");
            *sptr += str;
            *sptr += F("</option>\n");
        }
        *sptr += F("</select>");
    }
    void LED_RED(const char* name, bool state = 0) {
        *sptr += F("<input class=\"led red\" type=\"radio\" disabled ");
        if (state) *sptr += F("checked ");
        *sptr += F("name=\"");
        *sptr += name;
        *sptr += F("\" id=\"");
        *sptr += name;
        *sptr += F("\">");
    }
    void LED_GREEN(const char* name, bool state = 0) {
        *sptr += F("<input class=\"led green\" type=\"radio\" disabled ");
        if (state) *sptr += F("checked ");
        *sptr += F("name=\"");
        *sptr += name;
        *sptr += F("\" id=\"");
        *sptr += name;
        *sptr += F("\">");
    }
    
    template <uint8_t ax, uint8_t am>
    void PLOT(const char* id, const char** labels, int16_t vals[ax][am], int dec = 0) {
        *sptr += F("<script type=\"text/javascript\" src=\"https://www.gstatic.com/charts/loader.js\"></script>\n"
        "<script type=\"text/javascript\">\n"
        "google.charts.load('current', {'packages':['corechart']});\n"
        "google.charts.setOnLoadCallback(drawChart);\n"
        "function drawChart() {\n"
        "var data = google.visualization.arrayToDataTable([\n");
        
        *sptr += '[';
        for (int i = 0; i < ax+1; i++) {
            *sptr += '\'';
            if (i) *sptr += labels[i-1];
            *sptr += "',";
        }
        *sptr += "],\n";
        for (int j = 0; j < am; j++) {
            *sptr += '[';
            for (int i = 0; i < ax+1; i++) {
                if (!i) *sptr += '\'';
                if (!i) *sptr += j;
                else {
                    if (dec) *sptr += (float)vals[i-1][j] / dec;
                    else *sptr += vals[i-1][j];
                }
                if (!i) *sptr += '\'';
                *sptr += ',';
            }
            *sptr += F("],\n");
        }
        
        *sptr += F("]);var options = {pointSize:5,curveType:'function','chartArea':{'width':'90%','height':'90%'},\n"
        "backgroundColor:'none',hAxis:{title:''},vAxis:{title:''},\n"
        "legend: {position:'bottom'}};\n"
        "var chart = new google.visualization.LineChart(document.getElementById('");
        *sptr += id;
        *sptr += F("'));\n");
        *sptr += F("chart.draw(data, options);}\n"
        "</script><div id=\"");
        *sptr += id;
        *sptr += F("\" class=\"chartBlock\"></div>\n");
    }
    
    template <uint8_t ax, uint8_t am>
    void PLOT_DARK(const char* id, const char** labels, int16_t vals[ax][am], int dec = 0) {
        *sptr += F("<script type=\"text/javascript\" src=\"https://www.gstatic.com/charts/loader.js\"></script>\n"
        "<script type=\"text/javascript\">\n"
        "google.charts.load('current', {'packages':['corechart']});\n"
        "google.charts.setOnLoadCallback(drawChart);\n"
        "function drawChart() {\n"
        "var data = google.visualization.arrayToDataTable([\n");
        
        *sptr += '[';
        for (int i = 0; i < ax+1; i++) {
            *sptr += '\'';
            if (i) *sptr += labels[i-1];
            *sptr += "',";
        }
        *sptr += "],\n";
        for (int j = 0; j < am; j++) {
            *sptr += '[';
            for (int i = 0; i < ax+1; i++) {
                if (!i) *sptr += '\'';
                if (!i) *sptr += j;
                else {
                    if (dec) *sptr += (float)vals[i-1][j] / dec;
                    else *sptr += vals[i-1][j];
                }
                if (!i) *sptr += '\'';
                *sptr += ',';
            }
            *sptr += F("],\n");
        }
        
        *sptr += F("]);var options = {pointSize:5,curveType:'function','chartArea':{'width':'90%','height':'90%'},\n"
        "backgroundColor:'none',hAxis:{title:'',titleTextStyle:{color:'#ddd'},textStyle:{color:'#bbb'}},\n"
        "vAxis:{title:'',titleTextStyle:{color:'#ddd'},gridlines:{color:'#777'},textStyle:{color:'#bbb'}},\n"
        "legend: {position:'bottom',textStyle:{color:'#eee'}}};\n"
        "var chart = new google.visualization.LineChart(document.getElementById('");
        *sptr += id;
        *sptr += F("'));\n");
        *sptr += F("chart.draw(data, options);}\n");
        *sptr += F("</script><div id=\"");
        *sptr += id;
        *sptr += F("\" class=\"chartBlock\"></div>\n");
    }
    
    void AJAX_PLOT(const char* name, int axes, int xamount = 20, int prd = 1000) {
        *sptr += F(""
        "<script src=\"https://code.highcharts.com/highcharts.js\"></script>\n"
        "<div id=\"");
        *sptr += name;
        *sptr += F("\" class=\"chartBlock\"></div><script>\n"
        "var ");
        *sptr += name;
        *sptr += F("=new Highcharts.Chart({\n"
        "chart:{borderRadius:10,renderTo:'");
        *sptr += name;
        *sptr += F("',style:{fontFamily:\"sans-serif\"}},\n"
        "title:{text:''},"
        "series:[");
        for (int i = 0; i < axes; i++) {
            *sptr += F("{data:[]}");
            if (i != axes - 1) *sptr += ',';
        }
        *sptr += F("],\n"
        "xAxis:{type:'datetime',dateTimeLabelFormats:{second:'%H:%M:%S'}},\n"
        "yAxis:{title:{enabled:false}},"
        "credits:{enabled:false}});\n"
        "setInterval(function(){var xhttp=new XMLHttpRequest();var ch=");
        *sptr += name;
        *sptr += F("\n"
        "xhttp.onreadystatechange=function(){if(this.readyState==4&&this.status==200){\n"
        "var x=(new Date()).getTime();"
        "var arr=this.responseText.split(',');"
        "var move=(ch.series[0].data.length>");
        *sptr += xamount;
        *sptr += F(");\n"
        "for(let i=0;i<arr.length;i++)ch.series[i].addPoint([x,Number(arr[i])],true,move,true);\n"
        "}};xhttp.open(\"GET\",\"update?");
        *sptr += name;
        *sptr += F("\",true);xhttp.send();},\n");
        *sptr += prd;
        *sptr += F(");</script>\n");
    }
    
    void AJAX_PLOT_DARK(const char* name, int axes, int xamount = 20, int prd = 1000) {
        *sptr += F(""
        "<script src=\"https://code.highcharts.com/highcharts.js\"></script>\n"
        "<script src=\"https://code.highcharts.com/themes/dark-unica.js\"></script>\n"
        "<div id=\"");
        *sptr += name;
        *sptr += F("\" class=\"chartBlock\"></div><script>\n"
        "var ");
        *sptr += name;
        *sptr += F("=new Highcharts.Chart({\n"
        "chart:{borderRadius:10,renderTo:'");
        *sptr += name;
        *sptr += F("',style:{fontFamily:\"sans-serif\"}},\n"
        "title:{text:''},"
        "series:[");
        for (int i = 0; i < axes; i++) {
            *sptr += F("{data:[]}");
            if (i != axes - 1) *sptr += ',';
        }
        *sptr += F("],\n"
        "xAxis:{type:'datetime',dateTimeLabelFormats:{second:'%H:%M:%S'}},\n"
        "yAxis:{title:{enabled:false}},"
        "credits:{enabled:false}});\n"
        "setInterval(function(){var xhttp=new XMLHttpRequest();var ch=");
        *sptr += name;
        *sptr += F("\n"
        "xhttp.onreadystatechange=function(){if(this.readyState==4&&this.status==200){\n"
        "var x=(new Date()).getTime();"
        "var arr=this.responseText.split(',');"
        "var move=(ch.series[0].data.length>");
        *sptr += xamount;
        *sptr += F(");\n"
        "for(let i=0;i<arr.length;i++)ch.series[i].addPoint([x,Number(arr[i])],true,move,true);\n"
        "}};xhttp.open(\"GET\",\"update?");
        *sptr += name;
        *sptr += F("\",true);xhttp.send();},\n");
        *sptr += prd;
        *sptr += F(");</script>\n");
    }
    
    template <uint8_t ax, uint8_t am>
    void PLOT_STOCK(const char* id, const char** labels, uint32_t* times, int16_t vals[ax][am], int dec = 0) {
        *sptr += F("<script src=\"https://code.highcharts.com/stock/highstock.js\"></script>\n"
        "<div class=\"chartBlock\" id=\"");
        *sptr += id;
        *sptr += F("\"></div>");
        *sptr += F("<script>Highcharts.stockChart('");
        *sptr += id;
        *sptr += F("',{chart:{},\n"
        "rangeSelector:{buttons:[\n"
        "{count:1,type:'minute',text:'1M'},\n"
        "{count:1,type:'hour',text:'1H'},\n"
        "{count:1,type:'day',text:'1D'},\n"
        "{type:'all',text:'All'}],\n"
        "inputEnabled:false,selected:0},\n"
        "time:{useUTC:false},\n"
        "credits:{enabled:false},series:[\n");
        for (int axs = 0; axs < ax; axs++) {
            *sptr += F("{name:'");
            *sptr += labels[axs];
            *sptr += F("',data:[\n");
            for (int ams = 0; ams < am; ams++) {
                *sptr += '[';
                *sptr += times[ams];
                *sptr += F("000");
                *sptr += ',';
                if (dec) *sptr += (float)vals[axs][ams] / dec;
                else *sptr += vals[axs][ams];
                *sptr += "],\n";
            }
            *sptr += "]},\n";
        }
        *sptr += F("]});</script>\n");
    }
    
    template <uint8_t ax, uint8_t am>
    void PLOT_STOCK_DARK(const char* id, const char** labels, uint32_t* times, int16_t vals[ax][am], int dec = 0) {
        *sptr += F("<script src=\"https://code.highcharts.com/stock/highstock.js\"></script>\n"
        "<script src=\"https://code.highcharts.com/themes/dark-unica.js\"></script>"
        "<div class=\"chartBlock\" id=\"");
        *sptr += id;
        *sptr += F("\"></div>");
        *sptr += F("<script>Highcharts.stockChart('");
        *sptr += id;
        *sptr += F("',{chart:{},\n"
        "rangeSelector:{buttons:[\n"
        "{count:1,type:'minute',text:'1M'},\n"
        "{count:1,type:'hour',text:'1H'},\n"
        "{count:1,type:'day',text:'1D'},\n"
        "{type:'all',text:'All'}],\n"
        "inputEnabled:false,selected:0},\n"
        "time:{useUTC:false},\n"
        "credits:{enabled:false},series:[\n");
        for (int axs = 0; axs < ax; axs++) {
            *sptr += F("{name:'");
            *sptr += labels[axs];
            *sptr += F("',data:[\n");
            for (int ams = 0; ams < am; ams++) {
                *sptr += '[';
                *sptr += times[ams];
                *sptr += F("000");
                *sptr += ',';
                if (dec) *sptr += (float)vals[axs][ams] / dec;
                else *sptr += vals[axs][ams];
                *sptr += "],\n";
            }
            *sptr += "]},\n";
        }
        *sptr += F("]});</script>\n");
    }
};

extern Builder add = Builder();

void BUILD(String& s) {
    sptr = &s;
}
void SHOW() {
    if (ptr && sptr) (*(site*)ptr).showPage(*sptr);
}

void BUILD_BEGIN(String& s) {
    BUILD(s);
    add.PAGE_BEGIN();
    add.AJAX_CLICK();
    add.PAGE_BLOCK_BEGIN();
}
void BUILD_END() {
    add.PAGE_BLOCK_END();
    add.PAGE_END();
    SHOW();
}
#endif