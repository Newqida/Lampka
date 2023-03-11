#pragma once
#include "log.h"
class site {
public:
    site() {
        req.reserve(20);
    }

    ~site() {
        stop();
    }
    
    void start(uint8_t mode = WIFI_STA) {
        _mode = (mode == WIFI_AP);
        _active = 1;
        server.begin();
        if (_mode) dnsServer.start(53, "*", WiFi.softAPIP());
        server.onNotFound([this]() {
            req = server.uri();
            if (req.startsWith(F("/favicon.ico"))) show();
            else _formF = 1;
        });
        server.on("/click", [this]() {
            _clickF = 1;
            server.send(200, "text/plane");
        });
        server.on("/update", [this]() {
            _updateF = 1;
        });
        server.on("/log", [this]() {
            if (log.state && log.available()) {
                String s;
                while (log.available()) s += log.read();
                server.send(200, "text/plane", s);
            } else server.send(200, "text/plane");
        });
    }
    
    void stop() {
        _active = 0;
        if (_mode) dnsServer.stop();
        server.stop();
    }
    
    void attachBuild(void (*handler)()) {
        _build = *handler;
    }
    
    void attachClick(void (*handler)(site* p)) {
        _click = *handler;
    }
    
    void attachForm(void (*handler)(site* p)) {
        _form = *handler;
    }
    
    void attachUpdate(void (*handler)(site* p)) {
        _update = *handler;
    }

    bool tick() {
        if (!_active) return 0;
        if (_formF) show();
        if (_updateF) server.send(200, "text/plane");
        _updateF = _clickF = _formF = 0;

        if (_mode) dnsServer.processNextRequest();
        server.handleClient();
        if (_formF) {
            checkList();
            if (*_form) _form(this);
        }
        if (_clickF) {
            checkList();
            if (*_click) _click(this);
        }
        if (_updateF && *_update) _update(this);
        yield();
        return 1;
    }

    bool form() {
        return _formF;
    }
    
    bool form(const char* name) {
        return _formF ? req.equals(name) : 0;
    }
    
    String& formName() {
        return req;
    }
    
    bool click() {
        return _clickF;
    }
    
    bool click(const char* name) {
        return _clickF ? server.argName(0).equals(name) : 0;
    }
    
    const String& clickName() {
        return server.argName(0);
    }
    
    int clickValue() {
        return server.arg(0).toInt();
    }
    
    const String& clickText() {
        return server.arg(0);
    }
    
    bool update() {
        return _updateF;
    }
    
    bool update(const char* name) {
        return _updateF ? server.argName(0).equals(name) : 0;
    }
    
    const String& updateName() {
        return server.argName(0);
    }
    
    void answer(String s) {
        _updateF = 0;
        server.send(200, "text/plane", s);
    }
    void answer(char* s) {
        _updateF = 0;
        server.send(200, "text/plane", s);
    }
    void answer(int v) {
        String s(v);
        answer(s.c_str());
    }
    void answer(int16_t* v, int am, int dec = 0) {
        String s;
        for (int i = 0; i < am; i++) {
            if (dec) s += (float)v[i] / dec;
            else s += v[i];
            if (i != am - 1) s += ',';
        }
        answer(s.c_str());
    }

    bool root() {
        return (req[0] == '/' && req[1] == '\0');
    }
    
    const String& getString(const char* n) {
        return server.arg(n);
    }

    const char* getChars(const char* n) {
        return server.arg(n).c_str();
    }

    void copyStr(const char* n, char* d) {
        if (server.hasArg(n)) strcpy(d, server.arg(n).c_str());
    }

    int getInt(const char* n) {
        return server.arg(n).toInt();
    }
    
    float getFloat(const char* n) {
        return server.arg(n).toFloat();
    }

    bool getCheck(const char* n) {
        if (server.hasArg(n)) return (server.arg(n)[0] == '1' || server.arg(n)[0] == 'o');
        return 0;
    }

    date getDate(const char* n) {
        return decodeDate((char*)server.arg(n).c_str());
    }

    time getTime(const char* n) {
        return decodeTime((char*)server.arg(n).c_str());
    }

    uint32_t getColor(const char* n) {
        if (!server.hasArg(n)) return 0;
        return decodeColor((char*)server.arg(n).c_str());
    }
    
    int8_t getSelected(const char* n, const char* list) {
        return inList(server.arg(n).c_str(), list);
    }
    
    void showPage(String &s) {
        server.send(200, F("text/html"), s);
    }
    
    void show() {
        ptr = this;
        if (*_build) _build();
        ptr = nullptr;
    }
    
    String& uri() {
        return req;
    }
    
#ifdef ESP8266
    ESP8266WebServer server;
#else
    WebServer server;
#endif
    lista list;
    log log;

private:
    void checkList() {
        if (!list.idx) return;
        for (int i = 0; i < list.idx; i++) {
            if (list.skips[i] || form(list.forms[i])) {
                switch (list.types[i]) {
                case T_CSTR:    copyStr(list.names[i], (char*)list.vals[i]);        break;
                case T_STRING:  *(String*)list.vals[i] = getString(list.names[i]);  break;
                case T_TIME:    *(time*)list.vals[i] = getTime(list.names[i]);    break;
                case T_DATE:    *(date*)list.vals[i] = getDate(list.names[i]);    break;
                case T_CHECK:   *(bool*)list.vals[i] = getCheck(list.names[i]);     break;
                case T_BYTE:    *(int8_t*)list.vals[i] = (int8_t)getInt(list.names[i]); break;
                case T_INT:     *(long*)list.vals[i] = getInt(list.names[i]);       break;
                case T_FLOAT:   *(float*)list.vals[i] = getFloat(list.names[i]);    break;
                case T_COLOR:   *(uint32_t*)list.vals[i] = getColor(list.names[i]); break;
                }
            }
        }
    }
    
    bool logF = false;
    String req;
    bool _mode = false;
    bool _active = false;
    bool _formF = 0;
    bool _updateF = 0;
    bool _clickF = 0;
    void (*_build)() = nullptr;
    void (*_click)(site* p) = nullptr;
    void (*_form)(site* p) = nullptr;
    void (*_update)(site* p) = nullptr;
    IPAddress apIP;
    DNSServer dnsServer;
};