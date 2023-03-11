// Zmiana animacji jasności
void wink()
{
  if (data.power)
  {
    brightLoop(data.bright, 0, 20);
    brightLoop(0, 255, 20);
    brightLoop(255, 0, 20);
    brightLoop(0, 255, 20);
    brightLoop(255, 0, 20);
    brightLoop(0, data.bright, 20);
  }
}

// Cykl dla płynnej zmiany jasności
void brightLoop(int from, int to, int step)
{
  int val = from;
  for (;;)
  {
    FastLED.setBrightness(val);
    FastLED.show();
    delay(10);
    if (from > to)
    {
      val -= step;
      if (val < to)
        return;
    }
    else
    {
      val += step;
      if (val > to)
        return;
    }
  }
}
// Realizacja animacji w stylu płynnego oddychania
void animation()
{
  static Timer tmr(30);
  static bool breath;
  static uint8_t count;
  if (tmr.period())
  {
    count++;
    if (!onlineTmr.elapsed())
    {
      if (!pirTmr.elapsed())
      {
        if (count % 10 == 0)
          breath = !breath;
      }
      else
      {
        if (count % 30 == 0)
          breath = !breath;
      }
    }
    else
    {
      breath = 1;
    }
    uint8_t curBr = data.power ? (breath ? 255 : 210) : 0;

    CRGB ncol = CHSV(data.color, 255, curBr);
    CRGB ccol = leds[0];
    if (ccol != ncol)
      ccol = blend(ccol, ncol, 17);

    fill_solid(leds, LED_AMOUNT, ccol);
    FastLED.setBrightness(data.bright);
    FastLED.show();
  }
}
// Tworzy lokalny hotspot z interfejsem i miga
void localPortal(IPAddress ip)
{
  Serial.println(F("Create AP"));
  WiFi.mode(WIFI_AP);
  String s(F("Lampka "));
  s += ip.toString();
  WiFi.softAP(s);

  portal.start(WIFI_AP);
  while (portal.tick())
  {
    loadAnimation(CRGB::Purple);
    btn.tick();
    if (checkPortal() || btn.click())
      ESP.reset();
  }
}
// Implementuje migającą animację podczas ładowania z kolorem koloru
void loadAnimation(CRGB color)
{
  static int8_t dir = 1;
  static uint8_t val = 0;
  static Timer tmr(20);
  if (tmr.period())
  {
    val += dir;
    if (val >= 100 || val <= 0)
      dir = -dir;
    fill_solid(leds, LED_AMOUNT, color);
    FastLED.setBrightness(val);
    FastLED.show();
  }
  yield();
}

int getFromIndex(char *str, int idx, char div = ',')
{
  int val = 0;
  uint16_t i = 0;
  int count = 0;
  bool sign = 0;
  while (str[i])
  {
    if (idx == count)
    {
      if (str[i] == div)
        break;
      if (str[i] == '-')
        sign = -1;
      else
      {
        val *= 10L;
        val += str[i] - '0';
      }
    }
    else if (str[i] == div)
      count++;
    i++;
  }
  return sign ? -val : val;
}
