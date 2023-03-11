void buttonTick()
{
  btn.tick();

  switch (btn.hasClicks())
  {

  case 1:
    data.power = !data.power;
    sendPacket();
    delay(100);
    writeToEEPROM(0, data_p, data_s);
    break;

  case 2:
    data.color += 32;
    sendPacket();
    delay(100);
    writeToEEPROM(0, data_p, data_s);
    break;

  case 3:
    winkFlag = 1;
    sendPacket();
    break;
  }

  static int8_t dir = 10;
  if (btn.step())
  {
    data.bright = constrain(data.bright + dir, 0, 255);
    if (data.bright == 255)
    {
      FastLED.setBrightness(0);
      FastLED.show();
      delay(150);
      FastLED.setBrightness(255);
      FastLED.show();
      delay(150);
    }
  }
  if (btn.releaseStep())
  {
    dir = -dir;
    delay(100);
    writeToEEPROM(0, data_p, data_s);
  }
}
