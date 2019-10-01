#ifdef ESP_USE_BUTTON
bool brightDirection;

void buttonTick()
{
  touch.tick();
  uint8_t clickCount = touch.hasClicks() ? touch.getClicks() : 0;

  if (clickCount == 1)
  {
    if (dawnFlag)
    {
      manualOff = true;
      dawnFlag = false;
      FastLED.setBrightness(modes[currentMode].Brightness);
      changePower();
    }
    else
    {
      ONflag = !ONflag;
      changePower();
    }
    settChanged = true;
    eepromTimeout = millis();
    loadingFlag = true;
  }

  if (ONflag && clickCount == 2)
  {
    if (++currentMode >= MODE_AMOUNT) currentMode = 0;
    FastLED.setBrightness(modes[currentMode].Brightness);
    loadingFlag = true;
    settChanged = true;
    eepromTimeout = millis();
    FastLED.clear();
    delay(1);
  }

  if (ONflag && clickCount == 3)                            // вывод времени на лампу
  {
    if (ESP_MODE == 1U)
    {
      loadingFlag = true;


      hrs = timeClient.getHours();
      mins = timeClient.getMinutes();

      String sHrs = "0" + String(hrs);
      String sMin = "0" + String(mins);
      if (sHrs.length() > 2) sHrs = sHrs.substring(1);
      if (sMin.length() > 2) sMin = sMin.substring(1);
      //
      String text = sHrs + ":" + sMin;
      const char *text2 = text.c_str();
      while (!fillString(text2)) delay(1);
      loadingFlag = true;
    }
  }

  if (ONflag && clickCount == 4) //вывод погоды на лампу

  {
      loadingFlag = true;
    
    loop_weather();
  }
  
 
  
  if (ONflag && clickCount == 5)
  {
    if (--currentMode < 0) currentMode = MODE_AMOUNT - 1;
    FastLED.setBrightness(modes[currentMode].Brightness);
    loadingFlag = true;
    settChanged = true;
    eepromTimeout = millis();
    FastLED.clear();
    delay(1);
  }

  if (ONflag && clickCount == 6)                            // вывод времени на лампу
  {
    
       #ifdef OTA
      if (otaManager.RequestOtaUpdate())
      {
      currentMode = EFF_MATRIX;                             // принудительное включение режима "Матрица" для индикации перехода в режим обновления по воздуху
      FastLED.clear();
      delay(1);
      }
      #endif
    
  }
  
  
  if (ONflag && touch.isHolded())
  {
    brightDirection = !brightDirection;
  }

  if (ONflag && touch.isStep())
  {
    if (brightDirection)
    {
      if (modes[currentMode].Brightness < 10) modes[currentMode].Brightness += 1;
      else if (modes[currentMode].Brightness < 250) modes[currentMode].Brightness += 5;
      else modes[currentMode].Brightness = 255;
    }
    else
    {
      if (modes[currentMode].Brightness > 15) modes[currentMode].Brightness -= 5;
      else if (modes[currentMode].Brightness > 1) modes[currentMode].Brightness -= 1;
      else modes[currentMode].Brightness = 0;
    }
    FastLED.setBrightness(modes[currentMode].Brightness);
    settChanged = true;
    eepromTimeout = millis();

#ifdef GENERAL_DEBUG
    Serial.printf_P(PSTR("New brightness value: %d\n"), modes[currentMode].Brightness);
#endif
  }
}
#endif
