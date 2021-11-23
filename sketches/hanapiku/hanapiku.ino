#include <M5StickC.h>
#include <FastLED.h>

#define VOL_PIN 36
#define LED_PIN 32
#define smoothing_ratio 0.1
#define NUM_LEDS 3

int raw_value = 0;
float smoothing_value = 0;
int is_led_on = 0;
CRGB leds_color[NUM_LEDS];

void setup() {
  M5.begin();
  M5.Lcd.print("Hello HANAPIKU!\n\n");
  Serial.begin(115200);
  pinMode(VOL_PIN, INPUT);

  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds_color, NUM_LEDS);
  FastLED.setBrightness(20);

  for(int i=0; i<NUM_LEDS; i++) {
    leds_color[i] = CRGB::White;
  }
}

void change_to_off_color() {
  for(int i=0; i<NUM_LEDS; i++) {
    leds_color[i] = CRGB::White;
  }
}

void change_to_on_color() {
  for(int i=0; i<NUM_LEDS; i++) {
    leds_color[i] = CRGB::Red;
  }
}

void loop() {
  raw_value = analogRead(VOL_PIN);
  smoothing_value = smoothing_ratio * smoothing_value + (1 - smoothing_ratio) * (float)raw_value;

  Serial.print("raw_value: ");
  Serial.print(raw_value);
  Serial.print(", ");

  Serial.print("smoothing_value: ");
  Serial.print(smoothing_value);
  Serial.print("\n");

  if (smoothing_value > 500) {
    if (is_led_on == 0) {
      change_to_on_color();
      is_led_on = 1;
      M5.Lcd.print("HANAPIKU!\n");
    }
  } else {
    if (is_led_on == 1) {
      change_to_off_color();
      is_led_on = 0;
      M5.Lcd.print("none...\n");
    }
  }

  FastLED.show();

  delay(500);
}
