#pragma once
struct Timer {
  Timer (uint32_t nprd, bool nstate = true) {
    state = nstate;
    setPeriod(nprd);
    tmr = millis();
  }

  void setPeriod(uint32_t nprd) {
    prd = nprd;
  }

  void restart() {
    tmr = millis();
    state = true;
  }

  bool elapsed() {ś
    return (!state || check());
  }

  bool period() {
    if (state && check()) {
      restart();
      return 1;
    } return 0;
  }

  bool check() {
    return millis() - tmr >= prd;
  }

  uint32_t tmr, prd;
  bool state = 1;
};
