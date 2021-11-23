#include <M5StickC.h>

#define VOL_PIN 36
#define smoothing_ratio 0.1

int raw_value = 0;
float smoothing_value = 0;

void setup() {
  M5.begin();
  Serial.begin(115200);
  pinMode(VOL_PIN, INPUT);
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

  delay(500);
}
