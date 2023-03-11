#ifndef _EncButton_h
#define _EncButton_h
#define _EB_FAST 30
#define _EB_DEB 50
#define _EB_HOLD 1000
#define _EB_STEP 500
#define _EB_CLICK 400
#include <Arduino.h>
#ifndef nullptr
#define nullptr NULL
#endif
#define _EB_setFlag(x) (flags |= 1 << x)
#define _EB_clrFlag(x) (flags &= ~(1 << x))
#define _EB_readFlag(x) ((flags >> x) & 1)
#ifndef EB_FAST
#define EB_FAST _EB_FAST
#endif
#ifndef EB_DEB
#define EB_DEB _EB_DEB
#endif
#ifndef EB_HOLD
#define EB_HOLD _EB_HOLD
#endif
#ifndef EB_STEP
#define EB_STEP _EB_STEP
#endif
#ifndef EB_CLICK
#define EB_CLICK _EB_CLICK
#endif

enum eb_callback {
    TURN_HANDLER,
    LEFT_HANDLER,
    RIGHT_HANDLER,
    LEFT_H_HANDLER,
    RIGHT_H_HANDLER, 
    CLICK_HANDLER,
    HOLDED_HANDLER, 
    STEP_HANDLER,
    PRESS_HANDLER, 
    CLICKS_HANDLER,
    RELEASE_HANDLER,
    HOLD_HANDLER,
    TURN_H_HANDLER, 
};

#define EB_TICK 0
#define EB_CALLBACK 1

#define EB_NO_PIN 255

#define VIRT_ENC 254
#define VIRT_ENCBTN 253
#define VIRT_BTN 252

#ifdef EB_BETTER_ENC
static const int8_t _EB_DIR[] = {
  0, -1,  1,  0,
  1,  0,  0, -1,
  -1,  0,  0,  1,
  0,  1, -1,  0
};
#endif

template < uint8_t _EB_MODE, uint8_t _S1 = EB_NO_PIN, uint8_t _S2 = EB_NO_PIN, uint8_t _KEY = EB_NO_PIN >
class EncButton {
public:
    EncButton(const uint8_t mode = INPUT_PULLUP) {
        if (_S1 < 252 && mode == INPUT_PULLUP) pullUp();
        setButtonLevel(LOW);
    }
    
    void pullUp() {
        if (_S1 < 252) {
            if (_S2 == EB_NO_PIN) {
                pinMode(_S1, INPUT_PULLUP);
            } else if (_KEY == EB_NO_PIN) {
                pinMode(_S1, INPUT_PULLUP);
                pinMode(_S2, INPUT_PULLUP);
            } else {
                pinMode(_S1, INPUT_PULLUP);
                pinMode(_S2, INPUT_PULLUP);
                pinMode(_KEY, INPUT_PULLUP);
            }
        }
    }
    
    void setHoldTimeout(int tout) {
        _holdT = tout >> 7;
    }
    
    void holdEncButton(bool state) {
        if (state) _EB_setFlag(8);
        else _EB_clrFlag(8);
    }
    
    void setButtonLevel(bool level) {
        if (level) _EB_clrFlag(11);
        else _EB_setFlag(11);
    }
    
    uint8_t tick(uint8_t s1 = 0, uint8_t s2 = 0, uint8_t key = 0) {
        tickISR(s1, s2, key);
        checkCallback();
        return EBState;
    }
    
    uint8_t tickISR(uint8_t s1 = 0, uint8_t s2 = 0, uint8_t key = 0) {
        if (!_isrFlag) {
            _isrFlag = 1;
            
            if ((_S1 < 252 && _S2 < 252) || _S1 == VIRT_ENC || _S1 == VIRT_ENCBTN) {
                uint8_t state;
                if (_S1 >= 252) state = s1 | (s2 << 1);
                else state = fastRead(_S1) | (fastRead(_S2) << 1);
                poolEnc(state);
            }

            if ((_S1 < 252 && _S2 == EB_NO_PIN) || _KEY != EB_NO_PIN || _S1 == VIRT_BTN || _S1 == VIRT_ENCBTN) {
                if (_S1 < 252 && _S2 == EB_NO_PIN) _btnState = fastRead(_S1);
                if (_KEY != EB_NO_PIN) _btnState = fastRead(_KEY);
                if (_S1 == VIRT_BTN) _btnState = s1;
                if (_S1 == VIRT_ENCBTN) _btnState = key;
                _btnState ^= _EB_readFlag(11);
                poolBtn();           
            }
        }
        _isrFlag = 0;
        return EBState;
    }
    
    void checkCallback() {
        if (_EB_MODE) {
            if (turn()) exec(0);
            if (turnH()) exec(12);
            if (EBState > 0 && EBState <= 8) exec(EBState);
            if (release()) exec(10);
            if (hold()) exec(11);
            if (checkFlag(6)) {
                exec(9);
                if (clicks == _amount) exec(13);
            }
            EBState = 0;
        }
    }
    
    void attach(eb_callback type, void (*handler)()) {
        _callback[type] = *handler;
    }
    
    void detach(eb_callback type) {
        _callback[type] = nullptr;
    }
    
    void attachClicks(uint8_t amount, void (*handler)()) {
        _amount = amount;
        _callback[13] = *handler;
    }
    
    void detachClicks() {
        _callback[13] = nullptr;
    }
    
    uint8_t getState() { return EBState; }
    void resetState() { EBState = 0; }
    
    bool left() { return checkState(1); } 
    bool right() { return checkState(2); }
    bool leftH() { return checkState(3); }
    bool rightH() { return checkState(4); }
    
    bool fast() { return _EB_readFlag(1); }
    bool turn() { return checkFlag(0); }
    bool turnH() { return checkFlag(9); }
    
    int8_t getDir() { return _dir; }
    int16_t counter = 0;
    
    bool state() { return _btnState; }
    bool press() { return checkState(8); }
    bool release() { return checkFlag(10); }
    bool click() { return checkState(5); }
    
    bool held() { return checkState(6); }
    bool hold() { return _EB_readFlag(4); }
    bool step() { return checkState(7); }
    bool releaseStep() { return checkFlag(12); }
    
    bool held(uint8_t clk) { return (clicks == clk) ? checkState(6) : 0; }
    bool hold(uint8_t clk) { return (clicks == clk) ? _EB_readFlag(4) : 0; }
    bool step(uint8_t clk) { return (clicks == clk) ? checkState(7) : 0; }
    bool releaseStep(uint8_t clk) { return (clicks == clk) ? checkFlag(12) : 0; }
    
    uint8_t clicks = 0;
    bool hasClicks(uint8_t num) { return (clicks == num && checkFlag(7)) ? 1 : 0; }
    uint8_t hasClicks() { return checkFlag(6) ? clicks : 0; }
    
    bool isStep() { return step(); }
    bool isHold() { return hold(); }
    bool isHolded() { return held(); }
    bool isHeld() { return held(); }
    bool isClick() { return click(); }
    bool isRelease() { return release(); }
    bool isPress() { return press(); }
    bool isTurnH() { return turnH(); }
    bool isTurn() { return turn(); }
    bool isFast() { return fast(); }
    bool isLeftH() { return leftH(); }
    bool isRightH() { return rightH(); }
    bool isLeft() { return left(); }
    bool isRight() { return right(); }
    

private:
    bool fastRead(const uint8_t pin) {
#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__)
        if (pin < 8) return bitRead(PIND, pin);
        else if (pin < 14) return bitRead(PINB, pin - 8);
        else if (pin < 20) return bitRead(PINC, pin - 14);
#elif defined(__AVR_ATtiny85__) || defined(__AVR_ATtiny13__)
        return bitRead(PINB, pin);
#else
        return digitalRead(pin);
#endif
        return 0;
    }
    
    void poolEnc(uint8_t state) {
    #ifdef EB_BETTER_ENC
        if (_prev != state) {
            _ecount += _EB_DIR[state | (_prev << 2)];
            _prev = state;
            #ifdef EB_HALFSTEP_ENC
            if ((state == 0x3 || state == 0x0) && _ecount != 0) {
            #else
            if (state == 0x3 && _ecount != 0) {
            #endif
                EBState = (_ecount < 0) ? 1 : 2;
                _ecount = 0;
                if (_S2 == EB_NO_PIN || _KEY != EB_NO_PIN) {
                    if (!_EB_readFlag(4) && (_btnState || _EB_readFlag(8))) EBState += 2;
                }
                _dir = (EBState & 1) ? -1 : 1
                counter += _dir;
                if (EBState <= 2) _EB_setFlag(0);
                else if (EBState <= 4) _EB_setFlag(9);
                if (millis() - _debTimer < EB_FAST) _EB_setFlag(1);
                else _EB_clrFlag(1);
                _debTimer = millis();
            }
        }
    #else
        if (_encRST && state == 0b11) {
            if (_S2 == EB_NO_PIN || _KEY != EB_NO_PIN) {
                if ((_prev == 1 || _prev == 2) && !_EB_readFlag(4)) {
                    EBState = _prev;
                    if (_btnState || _EB_readFlag(8)) EBState += 2;
                }
            } else {
                if (_prev == 1 || _prev == 2) EBState = _prev;
            }
            
            if (EBState > 0) {
                _dir = (EBState & 1) ? -1 : 1;
                counter += _dir;
                if (EBState <= 2) _EB_setFlag(0);
                else if (EBState <= 4) _EB_setFlag(9);
                if (millis() - _debTimer < EB_FAST) _EB_setFlag(1);
                else _EB_clrFlag(1);
            }

            _encRST = 0;
            _debTimer = millis();
        }
        if (state == 0b00) _encRST = 1;
        _prev = state;
    #endif
    }
    
    void poolBtn() {
        uint32_t thisMls = millis();
        uint32_t debounce = thisMls - _debTimer;
        if (_btnState) {
            if (!_EB_readFlag(3)) {
                if (_EB_readFlag(14)) {
                    if (debounce > EB_DEB) {
                        _EB_setFlag(3);
                        EBState = 8;
                        _debTimer = thisMls;
                    }
                } else { 
                    EBState = 0;
                    _EB_setFlag(14);                 
                    if (debounce > EB_CLICK || _EB_readFlag(5)) {
                        clicks = 0;	
                        flags &= ~0b0011000011100000;
                    }
                    _debTimer = thisMls;
                }
            } else {
                if (!_EB_readFlag(4)) {
                    if (debounce < (uint32_t)(_holdT << 7)) {
                        if (EBState != 0 && EBState != 8) _EB_setFlag(2);
                    } else {
                        if (!_EB_readFlag(2)) {
                            EBState = 6;
                            flags |= 0b00110000;
                            _debTimer = thisMls;
                        }
                    }
                } else {
                    if (debounce > EB_STEP) {
                        EBState = 7;
                        _EB_setFlag(13);
                        _debTimer = thisMls;
                    }
                }
            }
        } else {
            if (_EB_readFlag(3)) {
                if (debounce > EB_DEB) {
                    if (!_EB_readFlag(4) && !_EB_readFlag(2)) {
                        EBState = 5;
                        clicks++;
                    }
                    flags &= ~0b00011100;                 
                    _debTimer = thisMls;
                    _EB_setFlag(10);
                    if (checkFlag(13)) _EB_setFlag(12);
                }
            } else if (clicks > 0 && debounce > EB_CLICK && !_EB_readFlag(5)) flags |= 0b11100000;
            checkFlag(14);
        }
    }
    
    bool checkState(uint8_t val) {
        if (EBState == val) {
            EBState = 0;
            return 1;
        } return 0;
    }
    bool checkFlag(uint8_t val) {
        if (_EB_readFlag(val)) {
            _EB_clrFlag(val);
            return 1;
        } return 0;
    }
    void exec(uint8_t num) {
        if (*_callback[num]) _callback[num]();
    }
    
    uint8_t _prev : 2;
    uint8_t EBState : 4;
    bool _btnState : 1;
    bool _encRST : 1;
    bool _isrFlag : 1;
    uint16_t flags = 0;
    
    #ifdef EB_BETTER_ENC
    int8_t _ecount = 0;
    #endif
    
    uint32_t _debTimer = 0;
    uint8_t _holdT = (EB_HOLD >> 7);
    int8_t _dir = 0;
    void (*_callback[_EB_MODE ? 14 : 0])() = {};
    uint8_t _amount = 0;
};

#endif
