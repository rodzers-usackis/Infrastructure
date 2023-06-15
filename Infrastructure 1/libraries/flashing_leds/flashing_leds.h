void enableLed ( int lednumber );

void lightUpLed ( int lednumber );

void lightDownLed ( int lednumber );

void enableLeds (uint8_t leds);

void lightUpLeds (uint8_t leds);

void lightDownLeds (uint8_t leds);

void enableAllLeds ();

void lightUpAllLeds ();

void lightDownAllLeds ();

void dimLed (int lednumber, int percentage, int duration);

void dimLedsWithoutDuration (uint8_t leds, int percentage);

void fadeInLed (int lednumber, int duration);

void flashingLed (int lednumber, int numberOfFlashes, int delay);

void flashingLeds (uint8_t leds, int numberOfFlashes, int delay);

void ledOnAndOffWithDelay (int lednumber, int delay);

void ledsOnAndOffWithDelay (uint8_t leds, int delay);

void flashingLedsForDuration(uint8_t leds, int delay, int duration);

void knightriderLedTheme();