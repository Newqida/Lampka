#define LED_PIN D1    // pin taśmy
#define BTN_PIN D2    // pin czujnika dotykowego
#define PIR_PIN D5    // pin czujnika PIR
#define LED_AMOUNT 12 // wpisać ilość diodów
#define BTN_LEVEL 1   
#define USE_PIR 1     // 1 - PIR prauje, 0 - jest wyłączony
#define IGNORE_PIR 0  // 1 - ignorować czujnik, 0 - nie ignorować

#define MQTT_HEADER "NQW:" // nazwa dla paketów MQTT

#include <EEPROM.h>       // biblioteka dla pracy z zależnością energetyczną
#include <ESP8266WiFi.h>  // biblioteka dla pracy z Wi-Fi
#include <PubSubClient.h> // biblioteka dla pracy z MQTT brokerom
#include <FastLED.h>      // biblioteka dla pracy z taśmą LED
#define EB_STEP 100       
#include <EncButton.h>    // biblioteka dla pracy z czujnikami
#include "Timer.h"        //biblioteka dla pracy z timer
#include <site.h>         //biblioteka strony internetowej

#if 1                     // 1 - dla wyświetlenia do konsoli, 0 - nie wyświetla 
#define DEBUG(x) Serial.print(x)
#define DEBUGLN(x) Serial.println(x)
#else
#define DEBUG(x)
#define DEBUGLN(x)
#endif

struct LampkaData         // Struktura dla czytelności niezbędnych danych podczas pracy
{
  // zmienne służące do tworzenia punktu dostępu lub łączenia się z sięcią
  char ssid[32] = "";
  char pass[32] = "";
  char local[20] = "Lampka-w61882"; // nazwa lampy lokalnej 
  char remote[20] = "Lampka-w61756"; // nazwa lampy zdalnej
  char host[32] = "broker.mqttdashboard.com"; // nazwa i port hosta mqtt, przez który są podłączone lampy
  uint16_t port = 1883;
  uint8_t ip[4] = {0, 0, 0, 0}; // ip adres lampy lokalnej
  bool power = 1;
  uint8_t bright = 50;
  uint8_t color = 0;
};

LampkaData data;

uint8_t *data_p = (uint8_t *)&data; // Wskaźnik do struktury
uint16_t data_s = sizeof(data);     // Długość struktury

EncButton<EB_TICK, BTN_PIN> btn;    // Tworzenie obiektu przycisk

CRGB leds[LED_AMOUNT];
WiFiClient espClient;
PubSubClient mqtt(espClient);
site portal;

void writeToEEPROM(uint16_t eepromAddr, uint8_t *data, uint16_t len)
{
  for (uint16_t i = 0; i < len; i++)
  {
    EEPROM.write(eepromAddr + i, data[i]);
  }
  EEPROM.commit();
}

void readFromEEPROM(uint16_t eepromAddr, uint8_t *data, uint16_t len)
{
  for (uint16_t i = 0; i < len; i++)
  {
    data[i] = EEPROM.read(eepromAddr + i);
  }
}

bool pirFlag = 0;
bool winkFlag = 0;
bool startFlag = 0;
const uint8_t hLen = strlen(MQTT_HEADER);

Timer onlineTmr(18000, false); // 18-sekundowy limit czasu online
Timer pirTmr(60000, false);    // 1 minuta limitu czasu czujnika PIR
Timer hbTmr(8000);             // 8 sekund na wysłanie pakietu
Timer updtTmr(5000);           // 5 sekundowy okres aktualizacji pamięci eeprom

void setup()
{
  startup();
}

void loop()
{
  if (USE_PIR && digitalRead(PIR_PIN))
    pirFlag = 1;
  heartbeat();
  if (updtTmr.check())
  {
    writeToEEPROM(0, data_p, data_s);
    updtTmr.restart();
  }
  animation();
  buttonTick();
  mqttTick();
  portal.tick();
  checkPortal();
}
