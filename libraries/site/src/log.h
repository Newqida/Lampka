#pragma once
#include <Print.h>
class log : public Print {
public:
    void start(int n = 64) {
        stop();
        size = n;
        buffer = new char [size];
        state = 1;
    }
    ~log() {
        stop();
    }
    void stop() {
        if (buffer) delete [] buffer;
        state = 0;
    }
    
    virtual size_t write(uint8_t n) {
        if (!state) return 1;
        uint8_t i = (head + 1) % size;
        if (i != tail) {
            buffer[head] = n;
            head = i;
        }
        return 1;
    }

    char read() {
        if (!state) return 0;
        if (head == tail) return 0;
        char val = buffer[tail];
        tail = (tail + 1) % size;
        return val;
    }

    int available() {
        if (!state) return 0;
        else return (size + head - tail) % size;
    }

    void clear() {
        head = tail = 0;
    }
    
    bool state = false;
    
private:
    char* buffer = nullptr;
    uint8_t size;
    int head = 0, tail = 0;
};