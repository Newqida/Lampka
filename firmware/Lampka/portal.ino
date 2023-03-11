bool checkPortal()
{
  if (portal.click())
  {
    if (portal.click("br"))
      data.bright = portal.getInt("br");
    if (portal.click("sw"))
    {
      data.power = portal.getCheck("sw");
      sendPacket();
    }
    if (portal.click("col"))
    {
      data.color = portal.getInt("col");
      sendPacket();
    }
    if (portal.click())
    {
      writeToEEPROM(0, data_p, data_s);
    }
  }

  if (portal.update())
  {
    if (portal.update("ledL"))
      portal.answer(mqtt.connected());
    if (portal.update("ledR"))
      portal.answer(!onlineTmr.elapsed());
    if (portal.update("ledP"))
      portal.answer((!pirTmr.elapsed() && !onlineTmr.elapsed()));
    if (portal.update("br"))
      portal.answer(data.bright);
    if (portal.update("sw"))
      portal.answer(data.power);
    if (portal.update("col"))
      portal.answer(data.color);
  }

  if (portal.form())
  {
    if (portal.form("/save"))
    {
      portal.copyStr("ssid", data.ssid);
      portal.copyStr("pass", data.pass);
      portal.copyStr("local", data.local);
      portal.copyStr("remote", data.remote);
      portal.copyStr("host", data.host);
      data.port = portal.getInt("port");

      writeToEEPROM(0, data_p, data_s);
      delay(50);
      mqtt.disconnect();
      mqtt.setServer(data.host, data.port);
      connectMQTT();
      return 1;
    }
  }
  return 0;
}
