//User has pressed the power button to turn on the system
//Was it an accidental bump or do they really want to turn on?
//Let's make sure they continue to press for 500ms
void powerOnCheck()
{
#ifdef ENABLE_DEVELOPER
  return;
#endif

  powerPressedStartTime = millis();

  while (digitalRead(pin_powerSenseAndControl) == LOW)
  {
    delay(10);

    if (millis() - powerPressedStartTime > 500)
      break;
  }

  if (millis() - powerPressedStartTime < 500)
    powerDown(false); //Power button tap. Returning to off state.

  powerPressedStartTime = 0; //Reset var to return to normal 'on' state
}

//If we have a power button tap, or if the display is not yet started (no I2C!)
//then don't display a shutdown screen
void powerDown(bool displayInfo)
{
  if (online.logging == true)
  {
    //Attempt to write to file system. This avoids collisions with file writing from other functions like recordSystemSettingsToFile()
    //Wait up to 1000ms
    if (xSemaphoreTake(sdCardSemaphore, 1000 / portTICK_PERIOD_MS) == pdPASS)
    {
      //Close down file system
      ubxFile.sync();
      ubxFile.close();
      //xSemaphoreGive(sdCardSemaphore); //Do not release semaphore
    } //End sdCardSemaphore

    online.logging = false;
  }

  if (displayInfo == true)
  {
    displayShutdown();
    delay(2000);
  }

  pinMode(pin_powerSenseAndControl, OUTPUT);
  digitalWrite(pin_powerSenseAndControl, LOW);

  pinMode(pin_powerFastOff, OUTPUT);
  digitalWrite(pin_powerFastOff, LOW);

  while (1)
    delay(1);
}
