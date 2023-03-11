void mqttTick()
{
  if (WiFi.status() != WL_CONNECTED)
    return;
  if (!mqtt.connected())
    connectMQTT();
  else
  {
    if (!startFlag)
    {
      startFlag = 1;
      char str[] = MQTT_HEADER "2";
      mqtt.publish(data.remote, str);
    }
  }
  mqtt.loop();
}

void connectMQTT()
{
  String id("Lampka-");
  id += String(random(0xffffff), HEX);
  if (mqtt.connect(id.c_str()))
    mqtt.subscribe(data.local);
  delay(1000);
}

void callback(char *topic, byte *payload, uint16_t len)
{
  payload[len] = '\0';
  char *str = (char *)payload;
  DEBUGLN(str);

  if (strncmp(str, MQTT_HEADER, hLen))
    return;

  str += hLen;

  switch (getFromIndex(str, 0))
  {
  case 0:
    if (!IGNORE_PIR && getFromIndex(str, 1))
      pirTmr.restart();
    break;

  case 1:
    data.power = getFromIndex(str, 1);
    data.color = getFromIndex(str, 2);
    if (getFromIndex(str, 3))
      wink();
    break;

  case 2:
    sendPacket();
    break;
  }
  onlineTmr.restart();
}

void sendPacket()
{
  String s;
  s.reserve(10);
  s += MQTT_HEADER "1,";
  s += data.power;
  s += ',';
  s += data.color;
  s += ',';
  s += winkFlag;
  winkFlag = 0;
  mqtt.publish(data.remote, s.c_str());
}

void heartbeat()
{
  if (hbTmr.period())
  {
    char str[hLen + 4] = MQTT_HEADER "0,";
    str[hLen + 2] = pirFlag + '0';
    pirFlag = 0;
    mqtt.publish(data.remote, str);
  }
}
