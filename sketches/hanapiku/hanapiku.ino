#include <M5StickC.h>
#include <FastLED.h>
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "secret.h"

#define VOL_PIN 36
#define LED_PIN 32
#define SMOOTHING_RATIO 0.1
#define NUM_LEDS 3

int raw_value = 0;
float smoothing_value = 0;
int is_hanapiku_on = 0;
CRGB leds_color[NUM_LEDS];
WiFiMulti wifiMulti;
HTTPClient http;
const int capacity = JSON_OBJECT_SIZE(2);
StaticJsonDocument<capacity> json_request;
char buffer[255];
unsigned long counter = 0;
unsigned long tick = 0;

void setup() {
  M5.begin();

  Serial.begin(115200);
  pinMode(VOL_PIN, INPUT);

  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds_color, NUM_LEDS);
  FastLED.setBrightness(20);
  for (int i = 0; i < NUM_LEDS; i++) {
    leds_color[i] = CRGB::White;
  }

  wifiMulti.addAP(WIFI_SSID, WIFI_PASS);
  if ((wifiMulti.run() == WL_CONNECTED)) { // wait for WiFi connection.
    M5.Lcd.print("connect successful");
  } else {
    M5.Lcd.print("connect failed");
  }

  //  M5.Lcd.print("Hello HANAPIKU!\n\n");
}

void change_to_off_color() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds_color[i] = CRGB::White;
  }
}

void change_to_on_color() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds_color[i] = CRGB::Red;
  }
}

void update_sensor_value() {
  raw_value = analogRead(VOL_PIN);
  smoothing_value = SMOOTHING_RATIO * smoothing_value + (1 - SMOOTHING_RATIO) * (float)raw_value;
}

void show_sensor_value() {
  Serial.print("raw_value: ");
  Serial.print(raw_value);
  Serial.print(", ");

  Serial.print("smoothing_value: ");
  Serial.print(smoothing_value);
  Serial.print("\n");
}

void get_http_request() {
  M5.Lcd.print("[HTTP] begin...\n");
  http.begin("https://httpbin.org/get");
  M5.Lcd.print("[HTTP] GET...\n");
  int httpCode = http.GET();  // start connection and send HTTP header.
  if (httpCode > 0) { // httpCode will be negative on error.
    Serial.printf("[HTTP] GET... code: %d\n", httpCode);
    M5.Lcd.print("Please see Serial.");
    if (httpCode == HTTP_CODE_OK) { // file found at server.
      String payload = http.getString();
      Serial.println(payload);
    }
  } else {
    M5.Lcd.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }
  http.end();
}

void post_http_request() {
  counter++;
  tick = millis();

  json_request["counter"] = counter;
  json_request["tick"] = tick;

  serializeJson(json_request, Serial);
  Serial.println("");

  serializeJson(json_request, buffer, sizeof(buffer));

  M5.Lcd.print("[HTTP] begin...\n");
  http.begin("https://httpbin.org/post");
  M5.Lcd.print("[HTTP] POST...\n");
  int httpCode = http.POST((uint8_t*)buffer, strlen(buffer));  // start connection and send HTTP header.
  if (httpCode > 0) { // httpCode will be negative on error.
    Serial.printf("[HTTP] POST... code: %d\n", httpCode);
    M5.Lcd.print("Please see Serial.");
    if (httpCode == HTTP_CODE_OK) { // file found at server.
      Stream* resp = http.getStreamPtr();

      DynamicJsonDocument json_response(255);
      deserializeJson(json_response, *resp);

      serializeJson(json_response, Serial);
      Serial.println("");
    }
  } else {
    M5.Lcd.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }
  http.end();
}

void handle_hanapiku_on() {
  is_hanapiku_on = 1;
  change_to_on_color();
//  get_http_request();
//  M5.Lcd.print("HANAPIKU!\n");
}

void handle_hanapiku_off() {
  is_hanapiku_on = 0;
  change_to_off_color();
//  post_http_request();
//  M5.Lcd.print("none...\n");
}

void handle_sensor_value() {
  if (smoothing_value > 500) {
    if (is_hanapiku_on == 0) {
      handle_hanapiku_on();
    }
  } else {
    if (is_hanapiku_on == 1) {
      handle_hanapiku_off();
    }
  }
}

void loop() {
  update_sensor_value();
  show_sensor_value();
  handle_sensor_value();
  FastLED.show();

  delay(500);
}
