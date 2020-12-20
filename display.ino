#define BLACK RgbColor(0, 0, 0)
#define GREEN RgbColor(0, brightness, 0)
#define YELLOW RgbColor(brightness, brightness, 0)
#define RED RgbColor(brightness, 0, 0)

#define CALIBRATIONCOLOR RgbColor(0, brightness/2, brightness)

int thresholdList[] = {400, 450, 500, 550,
                       600, 650, 700, 750,
                       800, 900, 1000, 1100
                      };

void updateDisplay() {
  if (mhz19calibrated) { // only update LEDs when we have a valid reading
    if (digitalRead(ENABLE_DISPLAY_PIN)) {
      int ppm = doc["co2ppm"].as<int>();

      for (int i = 0; i < LED_COUNT; i++) {
        strip.SetPixelColor(i, BLACK);
      }

      int lengthThresholdList = sizeof(thresholdList) / sizeof(thresholdList[0]);

      for (int i = 0; i < lengthThresholdList; i++) {
        int color = i / LED_COUNT;
        if (ppm >= thresholdList[i]) {
          switch (color) {
            case 0: strip.SetPixelColor(i % LED_COUNT, GREEN); break;
            case 1: strip.SetPixelColor(i % LED_COUNT, YELLOW); break;
            case 2: strip.SetPixelColor(i % LED_COUNT, RED); break;
          }
        }
      }

      strip.Show();
    }
    else {
      for (int i = 0; i < LED_COUNT; i++) {
        strip.SetPixelColor(i, BLACK);
      }
      strip.Show();
    }
  }
}

void displayCalibrated() {
  for (int i = 0; i < LED_COUNT; i++) {
    strip.SetPixelColor(i, RED);
  }
  strip.Show();
  delay(500);
}

void displayInCalibration() {
  for (int i = 0; i < LED_COUNT; i++) {
    strip.SetPixelColor(i, CALIBRATIONCOLOR);
  }
  strip.Show();
  delay(500);
}

void setupLeds() {
  strip.Begin();
}
