void startup()
{
  delay(1000);
  Serial.begin(9600);        // Uruchomienie portu debugowania na 9600
  portal.attachBuild(build); // Ustawienie funkcji, aby utworzyć interfejs sieciowy
  EEPROM.begin(data_s);      // Inicjalizacja eepromu z długiej struktury
  writeToEEPROM(0, data_p, data_s); // Zapisanie do pamięci
  readFromEEPROM(0, data_p, data_s); // Korekta i pamięć
  IPAddress ip = IPAddress(data.ip[0], data.ip[1], data.ip[2], data.ip[3]); // Konwersja tablicy na typ IPAddress

  // Inicjalizacja diody LED
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, LED_AMOUNT).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(50);
  FastLED.show();

  btn.setButtonLevel(BTN_LEVEL);   // Ustawienie poziomu przyciska
  Timer tmr(2000);                 // Tymczasowy zegar do oczekiwania na włączenie zasilania

   // Oczekuje 2 sekundy
   // Miga na czerwono i czeka na przycisk
   // Wciśnięcie przycisku spowoduje uruchomienie interfejsu z lokalnym adresem IP
  while (!tmr.period())
  {
    loadAnimation(CRGB::Red);
    btn.tick();
    if (btn.click())
      localPortal(ip);
  }

  // Skonfiguruwanie ESP w trybie WIFI_STA i połączenie się z data.ssid i data.pass
  Serial.println("Connecting...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(data.ssid, data.pass);

  // Odnowienie zegara
  tmr.setPeriod(15000);
  tmr.restart();

  // Dopóki nie upłynie 15 sekund
  // Miga na czerwono
  // Oczekiwanie na przycisk w celu utworzenia punktu dostępu
  while (WiFi.status() != WL_CONNECTED)
  {
    loadAnimation(CRGB::Red);
    btn.tick();
    if (btn.click() || tmr.period())
    {
      WiFi.disconnect();
      localPortal(ip);
    }
  }

  // Odnowienie diodów
  FastLED.clear();
  FastLED.show();

  // Wyświetla w porcie szeregowym otrzymany lokalny adres IP
  Serial.print(F("Connected! IP: "));
  Serial.println(WiFi.localIP());

  // Przepisuje adres IP do struktury i zapisuje go w pamięci
  if (ip != WiFi.localIP())
  {
    ip = WiFi.localIP();
    for (int i = 0; i < 4; i++)
      data.ip[i] = ip[i];
    writeToEEPROM(0, data_p, data_s);
  }

  // Ustawia serwer i wywołanie zwrotne na żądanie
  mqtt.setServer(data.host, data.port);
  mqtt.setCallback(callback);

  randomSeed(micros());

  // Inicjalizacja pracy portal
  portal.start();

  // Ustawia jasność diodu
  FastLED.setBrightness(data.bright);
}
