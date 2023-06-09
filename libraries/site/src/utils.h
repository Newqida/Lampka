#pragma once

String* sptr = nullptr;
void* ptr = nullptr;

String encodeColor(uint32_t color) {
    String s('#');
    for (uint8_t i = 0; i < 6; i++) {
        char p = ((uint32_t)color >> (5 - i) * 4) & 0xF;
        p += (p > 9) ? 87 : 48;
        s += p;
    }
    return s;
}

uint32_t decodeColor(char* hex) {
    if (strlen(hex) < 6) return 0;
    uint32_t val = 0;
    uint8_t i = (hex[0] == '#') ? 1 : 0;
    for (; i < strlen(hex); i++) {
        val <<= 4;
        uint8_t d = hex[i];
        d -= (d <= '9') ? 48 : ((d <= 'F') ? 55 : 87);
        val |= d;
    }
    return val;
}


struct date {
    int16_t year;
    uint8_t month, day;
};

void encodeDate(char* str, date& d) {
    str[0] = d.year / 1000 + '0';
    str[1] = (d.year % 1000) / 100 + '0';
    str[2] = (d.year % 100) / 10 + '0';
    str[3] = d.year % 10 + '0';
    str[4] = '-';
    str[5] = d.month / 10 + '0';
    str[6] = d.month % 10 + '0';
    str[7] = '-';
    str[8] = d.day / 10 + '0';
    str[9] = d.day % 10 + '0';
    str[10] = '\0';
}

String encodeDate(date& d) {
    char str[11];
    encodeDate(str, d);
    String s(str);
    return s;
}

String encodeDate(int16_t year, uint8_t month, uint8_t day) {
    date d = (date){year, month, day};
    return encodeDate(d);
}

date decodeDate(char* str) {
    date d = (date){0,0,0};
    if (strlen(str) == 10) {
        str[4] = str[7] = '\0';
        d.year = atoi(str);
        d.month = atoi(str+5);
        d.day = atoi(str+8);
        str[4] = str[7] = '-';
    }
    return d;
}

struct time {
    uint8_t hour, minute, second;
};

void encodeTime(char* str, time& t) {
    str[0] = t.hour / 10 + '0';
    str[1] = t.hour % 10 + '0';
    str[2] = ':';
    str[3] = t.minute / 10 + '0';
    str[4] = t.minute % 10 + '0';
    str[5] = ':';
    str[6] = t.second / 10 + '0';
    str[7] = t.second % 10 + '0';
    str[8] = '\0';
}

String encodeTime(time& t) {
    char str[9];
    encodeTime(str, t);
    String s(str);
    return s;
}

String encodeTime(uint8_t hour, uint8_t minute, uint8_t second = 0) {
    time t = (time){hour, minute, second};
    return encodeTime(t);
}

time decodeTime(char* str) {
    time t = (time){0,0,0};
    uint8_t len = strlen(str);
    if (len == 5 || len == 8) {
        str[2] = str[5] = '\0';
        t.hour = atoi(str);
        t.minute = atoi(str+3);
        t.second = atoi(str+6);
        str[2] = str[5] = ':';
    }
    return t;
}

uint32_t unix(uint16_t y, uint8_t m, uint8_t d, uint8_t h, uint8_t mn, uint8_t s, int8_t gmt = 0) {
    int my = (m >= 3) ? 1 : 0;
    y += my - 1970;
    uint16_t dm = 0;
    for (int i = 0; i < m - 1; i++) dm += (i<7)?((i==1)?28:((i&1)?30:31)):((i&1)?31:30);
    return (((d-1+dm+((y+1)>>2)-((y+69)/100)+((y+369)/100/4)+365*(y-my))*24ul+h-gmt)*60ul+mn)*60ul+s;
}

void addInt(int16_t val, int16_t* arr, uint8_t am) {
    for (int i = 0; i < am - 1; i++) arr[i] = arr[i + 1];
    arr[am - 1] = val;
}
void addUnix(uint32_t val, uint32_t* arr, uint8_t am) {
    for (int i = 0; i < am - 1; i++) arr[i] = arr[i + 1];
    arr[am - 1] = val;
}
void addUnixS(int16_t val, uint32_t* arr, uint8_t am) {
    for (int i = 0; i < am - 1; i++) arr[i] = arr[i + 1];
    arr[am - 1] += val;
}

struct DateTimeP {
    uint32_t ymdhm;
    uint8_t ss;
    uint16_t year() {return (ymdhm >> 19) & 0xfff;}
    uint8_t month() {return (ymdhm >> 15) & 0xf;}
    uint8_t day() {return (ymdhm >> 11) & 0xf;}
    uint8_t hour() {return (ymdhm >> 6) & 0x1f;}
    uint8_t minute() {return ymdhm & 0x3f;}
    uint8_t second() {return ss & 0x3f;}
    void year(uint16_t y) {ymdhm = (ymdhm & ~(0xffful << 19)) | ((uint32_t)(y & 0xfff) << 19);}
    void month(uint8_t m) {ymdhm = (ymdhm & ~(0xful << 15)) | ((uint32_t)(m & 0xf) << 15);}
    void day(uint8_t d) {ymdhm = (ymdhm & ~(0xf << 11)) | ((d & 0xf) << 11);}
    void hour(uint8_t h) {ymdhm = (ymdhm & ~(0x1f << 6)) | ((h & 0x1f) << 6);}
    void minute(uint8_t m) {ymdhm = ((ymdhm & (~0x3f)) | (m & 0x3f));}
    void second(uint8_t s) {ss = s & 0x3f;}
    void set(uint16_t y, uint8_t m, uint8_t d, uint8_t h, uint8_t mn, uint8_t s) {
        year(y); month(m); day(d); hour(h); minute(mn); second(s);
    }
};

void encodeDTP(char* buf, DateTimeP dtp) {
    uint16_t v = dtp.year() * 10;
    for (int i = 3; i >= 0; i--) buf[i] = (v /= 10) % 10;
    buf[4] = -3;
    v = dtp.month();
    buf[5] = v / 10;
    buf[6] = v % 10;
    buf[7] = -3;
    v = dtp.day();
    buf[8] = v / 10;
    buf[9] = v % 10;
    buf[10] = 36;
    v = dtp.hour();
    buf[11] = v / 10;
    buf[12] = v % 10;
    buf[13] = 10;
    v = dtp.minute();
    buf[14] = v / 10;
    buf[15] = v % 10;
    buf[16] = 10;
    v = dtp.second();
    buf[17] = v / 10;
    buf[18] = v % 10;
    for (int i = 0; i < 19; i++) buf[i] += '0';
    buf[19] = '\0';
}

String encodeDTP(DateTimeP &dtp) {
    char buf[20];
    encodeDTP(buf, dtp);
    return String(buf);
}

char* splitList(char* str) {
    static uint8_t prev, end;
    if (str == NULL) prev = end = 0;
    else {
        if (prev) *(str + prev - 1) = ',';
        char* cur = strchr(str+prev, ',');
        if (cur) {
            *cur = '\0';
            uint8_t b = prev;
            prev = cur - str + 1;
            return str + b;
        } else if (!end) {
            end = 1;
            return str + prev;
        }
    }
    return NULL;
}

int8_t inList(const char* name, const char* list) {
    char buf[strlen(list) + 1];
    strcpy(buf, list);
    char* str = buf;
    int8_t count = 0, pos = -1;
    splitList(NULL);
    while ((str = splitList((char*)buf)) != NULL) {
        if (!strcmp(str, name)) pos = count;
        count++;
    }
    return pos;
}

int8_t inList(String& s, const char* list) {
    return inList(s.c_str(), list);
}

int8_t inList(const String& s, const char* list) {
    return inList(s.c_str(), list);
}