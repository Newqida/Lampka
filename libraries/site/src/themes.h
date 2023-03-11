#pragma once

const char LIGHT[] PROGMEM =
"<style type=\"text/css\">\n"
"body {font-family: sans-serif;margin-top:15px;}\n"
"hr {width:95%;margin-bottom:10px;max-width:400px;}\n"
"h2 {margin:8px 0;}\n"
"span {font-size:20px;}\n"
"label {font-size:20px;margin:0 5px;}\n"
".chartBlock {border-radius:15px;margin:10px;width:90%;max-width:1000px;height:500px}\n"
".block {border-radius:10px;background-color:#f2f2f2;padding:15px 0px;margin-top:3px;margin-bottom:15px;width:90%;max-width:400px;}\n"
".sldBlock{width:90%;display:flex;flex-direction:row;justify-content:right;align-items:center}"
".sldLbl{font-size:15px;}"
"input[type=\"number\"],input[type=\"text\"],input[type=\"password\"],input[type=\"date\"],input[type=\"time\"],input[type=\"range\"],input[type=\"color\"],input[type=\"checkbox\"],select,textarea\n"
"{border-radius:8px;max-width:350px;padding:3px 10px;vertical-align:middle;position:relative;margin:2px 0 7px 0;font-size:20px;height:40px;cursor:pointer;}\n"
"input[type=\"range\"] {width:60%;height:30px;padding:0px;}\n"
"input[type=\"date\"],input[type=\"time\"] {width:180px;}\n"
"input[type=\"color\"] {width:100px;height:40px;}\n"
"input[type=\"checkbox\"] {width:27px;height:27px;margin-bottom:10px;}\n"
"input[type=\"number\"],input[type=\"text\"],input[type=\"password\"],select,textarea {display:inline-block;border:1px solid #ccc;border-radius:8px;box-sizing:border-box;cursor:auto;}\n"
"input[type=\"submit\"],input[type=\"button\"] {max-width:350px;height:60px;font-size:28px;width:90%;margin:8px 5px;background-color:#4CAF50;border:none;border-radius:8px;color:white;cursor:pointer;padding:0px 10px;}\n"
"input[type=submit],input[type=\"button\"]:hover {background-color:#45a049;}\n"
"textarea {width: 100%;}"
".led {margin:20px 18px;}\n"
".led:after {width:25px;height:25px;border-radius:25px;top:-10px;left:-9px;position:relative;content:'';display:inline-block;border:2px solid;border-color:#858585;background-color:#323232}\n"
".led:checked:after {box-shadow:0px 0px 10px;}\n"
".led.red:checked:after {border-color:red !important;color:red !important;background-color:red !important;}\n"
".led.green:checked:after {border-color:lime !important;color:lime !important;background-color:lime !important;}\n"
".miniButton{height:40px !important;font-size:20px!important;width:auto!important;}\n"
".switch{margin:10px 0 5px;position:relative;display:inline-block;width:60px;height:34px}\n"
".switch input{opacity:0;width:0;height:0}\n"
".slider{border-radius:34px;position:absolute;cursor:pointer;top:-5px;left:0;right:0;bottom:5px;background-color:#ccc;-webkit-transition:.1s;transition:.1s}\n"
".slider:before{border-radius:50%;position:absolute;content:\"\";height:26px;width:26px;left:4px;bottom:4px;background-color:#fff;-webkit-transition:.1s;transition:.1s}\n"
"input:checked+.slider{background-color:#2196f3}\n"
"input:checked+.slider:before{-webkit-transform:translateX(26px);-ms-transform:translateX(26px);transform:translateX(26px)}\n"
"</style>\n";

const char DARK[] PROGMEM =
"<style type=\"text/css\">\n"
"#blockBack>input[type=\"password\"]{background:#13161a}\n"
"#blockBack>input[type=\"text\"]{background:#13161a}\n"
"#blockBack>input[type=\"date\"]{background:#13161a}\n"
"#blockBack>input[type=\"time\"]{background:#13161a}\n"
"#blockBack>input[type=\"number\"]{background:#13161a}\n"
"#blockBack>select{background:#13161a}\n"
"body {font-family:sans-serif;background:#13161a;color:#bbb;margin-top:15px;}\n"
"hr {width:95%;margin-bottom:10px;border:none;border-top:2px solid #394048;max-width:400px;}\n"
"h2 {margin:8px 0;}\n"
"span {font-size:20px;}\n"
"label {font-size:20px;margin:0 5px;}\n"
".chartBlock {border-radius:15px;margin:10px;width:90%;max-width:1000px;height:500px}\n"
".block {border-radius:10px;background-color:#2a2d35;padding:15px 0px;margin-top:3px;margin-bottom:15px;width:90%;max-width:400px;}\n"
".sldBlock{width:90%;display:flex;flex-direction:row;justify-content:right;align-items:center}"
".sldLbl{font-size:15px;}"
"input[type=\"number\"],input[type=\"text\"],input[type=\"password\"],input[type=\"date\"],input[type=\"time\"],input[type=\"range\"],input[type=\"color\"],input[type=\"checkbox\"],select,textarea\n"
"{border-radius:8px;max-width:350px;padding:3px 10px;color:#bbb;border:none;background-color:#2a2d35;vertical-align:middle;position:relative;margin:2px 0 7px 0;font-size:20px;height:40px;cursor:pointer;}\n"
"input[type=\"range\"] {width:60%;height:30px;padding:0px;}\n"
"input[type=\"date\"],input[type=\"time\"] {width:180px;}\n"
"input[type=\"color\"] {width:100px;height:40px;}\n"
"input[type=\"checkbox\"] {width:27px;height:27px;margin-bottom:10px;}\n"
"input[type=\"number\"],input[type=\"text\"],input[type=\"password\"],select,textarea {display:inline-block;border-radius:8px;box-sizing:border-box;cursor:auto;}\n"
"input[type=\"submit\"],input[type=\"button\"] {max-width:350px;height:60px;font-size:28px;width:90%;margin:8px 5px;background-color:#4CAF50;border:none;border-radius:8px;color:white;cursor:pointer;}\n"
"input[type=submit],input[type=\"button\"]:hover {background-color:#45a049;}\n"
"input:focus,select,textarea:focus{outline:none;}"
"textarea {width: 100%;}"
".led {margin:20px 18px;}\n"
".led:after {width:25px;height:25px;border-radius:25px;top:-10px;left:-9px;position:relative;content:'';display:inline-block;border:2px solid;border-color:#858585;background-color:#121212}\n"
".led:checked:after {box-shadow:0px 0px 10px 4px;}\n"
".led.red:checked:after {border-color:red !important;color:red !important;background-color:red !important;}\n"
".led.green:checked:after {border-color:lime !important;color:lime !important;background-color:lime !important;}\n"
".miniButton{height:40px !important;font-size:20px!important;width:auto!important;}\n"
".switch{margin:10px 0 5px;position:relative;display:inline-block;width:60px;height:34px}\n"
".switch input{opacity:0;width:0;height:0}\n"
".slider{border-radius:34px;position:absolute;cursor:pointer;top:-5px;left:0;right:0;bottom:5px;background-color:#54545485;-webkit-transition:.1s;transition:.1s}\n"
".slider:before{border-radius:50%;position:absolute;content:\"\";height:26px;width:26px;left:4px;bottom:4px;background-color:#fff;-webkit-transition:.1s;transition:.1s}\n"
"input:checked+.slider{background-color:#2196f3}\n"
"input:checked+.slider:before{-webkit-transform:translateX(26px);-ms-transform:translateX(26px);transform:translateX(26px)}\n"
"</style>\n";