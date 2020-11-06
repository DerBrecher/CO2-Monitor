#define BLACK RgbColor(0, 0, 0)
#define GREEN RgbColor(0, brightness, 0)
#define YELLOW RgbColor(brightness, brightness, 0)
#define RED RgbColor(brightness, 0, 0)

int thresholdList[] = {400, 450, 500, 550,
                       600, 650, 700, 750,
                       800, 900, 1000, 1100
                      };

void updateDisplay(int ppm) {
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

void setupLeds(){
   strip.Begin();
}
