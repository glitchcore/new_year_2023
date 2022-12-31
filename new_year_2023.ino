#include <Adafruit_NeoPixel.h>

#define LED_COUNT 100  // Number of LEDs in the strip
#define LED_PIN 3     // Pin the LED strip is connected to

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_RGB + NEO_KHZ800);  // Initialize the strip

// Enum for LED states
enum LedState {
  OFF,
  FADING_IN,
  ON,
  FADING_OUT
};

// Struct for LED data
struct LedData {
  LedState state;
  int brightness;
  long lastUpdate;
  uint8_t startColor;
  uint8_t endColor;
  long period;
};

LedData leds[LED_COUNT];  // Array for LED data

uint32_t updateLed(int index) {
  unsigned long currentTime = millis();  // Current time
  LedData& led = leds[index];  // Reference to LED data

  // Update LED state based on elapsed time
  switch (led.state) {
    case OFF:
      // Check if it's time to switch on the LED
      if (currentTime - led.lastUpdate >= led.period + random(1000, 3000)) {
        led.state = FADING_IN;
        led.lastUpdate = currentTime;
      }
    break;

    case FADING_IN:
      // Calculate new brightness
      led.brightness = (currentTime - led.lastUpdate) * 256 / 500;

      // Check if LED is fully on
      if (led.brightness >= 256) {
        led.state = ON;
        led.brightness = 255;
        led.lastUpdate = currentTime;
      }
      break;

    case ON:
      // Check if it's time to switch off the LED
      if (currentTime - led.lastUpdate >= 250) {
        led.state = FADING_OUT;
        led.lastUpdate = currentTime;
      }
    break;

    case FADING_OUT:
      // Calculate new brightness
      led.brightness = 255 - (currentTime - led.lastUpdate) * 256 / 500;

      // Check if LED is fully off
      if (led.brightness <= 0) {
        led.state = OFF;
        led.brightness = 0;
        led.lastUpdate = currentTime;
      }
    break;
  }

  uint8_t a = led.endColor >> 4 & 0xF;
  uint8_t b = led.endColor & 0xF;
  uint16_t saturation = 255 - led.endColor/2;

  if(led.endColor < 200) {
    return strip.ColorHSV(0, 255 - led.endColor/2, led.brightness);
  } else if(led.endColor < 230) {
    return strip.ColorHSV(12000 + (uint16_t)(200 - led.endColor) * 30, 255, led.brightness);
  } else {
    return strip.ColorHSV(39835 + (uint16_t)led.endColor * 100, 255 - (230 - led.endColor)/2, led.brightness);
  }
}

void setup() {
  strip.begin();  // Initialize the strip
  strip.clear();
  strip.setBrightness(255);  // Set brightness to 50%

  randomSeed(analogRead(0));  // Initialize the random number generator

  // Initialize LED data
  for (int i = 0; i < LED_COUNT; i++) {
    leds[i].state = OFF;
    leds[i].brightness = 0;
    leds[i].period = random(2000, 10000);
    leds[i].lastUpdate = 0;
    leds[i].startColor = random(256);  // Random start color
    leds[i].endColor = random(256);  // Random end color
  }
}

void loop() {
  // Iterate over all LEDs
  for (int i = 0; i < LED_COUNT; i++) {
    // Set LED color
    strip.setPixelColor(i, updateLed(i));  // Set pixel to red
  }

  // Update the strip
  strip.show();
}
