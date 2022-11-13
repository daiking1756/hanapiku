#include <M5StickC.h>
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "secret.h"
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

#define VOL_PIN 36
#define LED_PIN 32
#define NUMPIXELS 16
#define SMOOTHING_RATIO 0.0
#define SAMPLING_NUM 30
#define HANAPIKU_RATIO_THRESHOLD 1.03
#define DELAY 200

Adafruit_NeoPixel pixels(NUMPIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);
int raw_value = 0;
float smoothing_value = 0;
float current_hanapiku_ratio = 1.0;
boolean is_hanapiku_on = false;
WiFiMulti wifiMulti;
HTTPClient http;
char buffer[255];
String authorization = "Bearer ";
boolean is_calibration = false;
int sampling_count = 0;
int sampling_values[10];
float base_value = 0.0;
boolean enable_post_line = false;
int num_of_hanapiku = 0;

void setup() {
  M5.begin();
  M5.Lcd.setRotation(1);
  Serial.begin(115200);
  pinMode(VOL_PIN, INPUT);

  #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
    clock_prescale_set(clock_div_1);
  #endif
  pixels.begin();
  led_off();

  wifiMulti.addAP(WIFI_SSID, WIFI_PASS);
  if ((wifiMulti.run() == WL_CONNECTED)) { // wait for WiFi connection.
    M5.Lcd.println("connect successful");
  } else {
    M5.Lcd.println("connect failed");
  }
  authorization += CHANNEL_ACCESS_TOKEN;
}

void lcd_init() {
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.fillScreen(BLACK);
}

void led_off() {
  pixels.clear();
  pixels.show();
}

void led_on() {
  if(num_of_hanapiku > 16){
    num_of_hanapiku = 0;
    led_off();
  }
  for (int i = 0; i < (num_of_hanapiku % 16) ; i++) {
    pixels.setPixelColor(i, pixels.Color(20, 20, 20));
    pixels.show();
  }
}

void post_line_message(String payload_text) {
  DynamicJsonDocument json_request_doc(200);
  JsonArray messages = json_request_doc.createNestedArray("messages");
  messages[0]["type"] = "text";
  messages[0]["text"] = "PIKUN";

  serializeJson(json_request_doc, Serial);
  Serial.println("");

  serializeJson(json_request_doc, buffer, sizeof(buffer));

  http.begin("https://api.line.me/v2/bot/message/broadcast");
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", authorization);

  int httpCode = http.POST((uint8_t*)buffer, strlen(buffer));  // start connection and send HTTP header.
  if (httpCode > 0) { // httpCode will be negative on error.
    Serial.printf("POST... code: %d\n", httpCode);
    if (httpCode == HTTP_CODE_OK) { // file found at server.
      Stream* resp = http.getStreamPtr();

      DynamicJsonDocument json_response(255);
      deserializeJson(json_response, *resp);

      serializeJson(json_response, Serial);
      Serial.println("");
    }
  } else {
    Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }
  http.end();
}

void update_sensor_value() {
  raw_value = analogRead(VOL_PIN);
  smoothing_value = SMOOTHING_RATIO * smoothing_value + (1 - SMOOTHING_RATIO) * (float)raw_value;
}

void print_sensor_value() {
  Serial.print("raw_value: ");
  Serial.print(raw_value);
  Serial.print(", ");

  Serial.print("smoothing_value: ");
  Serial.print(smoothing_value);
  Serial.print(", ");

  Serial.print("base_value: ");
  Serial.print(base_value);
  Serial.print(", ");

  Serial.print("current_hanapiku_ratio: ");
  Serial.println(current_hanapiku_ratio);
}

void handle_hanapiku_on() {
  is_hanapiku_on = true;
  num_of_hanapiku += 1;
  M5.Lcd.fillScreen(WHITE);
  led_on();
  if (enable_post_line) {
    post_line_message("HANAPIKU <ON>");
  }
}

void handle_hanapiku_off() {
  is_hanapiku_on = false;
//  led_off();
  lcd_init();
}

boolean is_hanapikuing_now() {
  current_hanapiku_ratio = smoothing_value / base_value;
  if (current_hanapiku_ratio > HANAPIKU_RATIO_THRESHOLD) {
    return true;
  } else {
    return false;
  }
}

void handle_sensor_value() {
  if (is_hanapikuing_now()) {
    // Serial.println("is_hanapikuing_now: ON");
    if (!is_hanapiku_on) {
      Serial.println("call handle <ON>");
      handle_hanapiku_on();
    }
  } else {
    // Serial.println("is_hanapikuing_now: OFF");
    if (is_hanapiku_on) {
      Serial.println("call handle <OFF>");
      handle_hanapiku_off();
    }
  }
}

void check_button() {
  M5.update();
  if ( M5.BtnA.pressedFor(1000) ) {
    is_calibration = true;
  }
  if ( M5.BtnB.wasPressed() ) {
    enable_post_line = !enable_post_line;
    M5.Lcd.print("enable_post_line: ");
    M5.Lcd.println(enable_post_line);
    delay(1000);
  }
}

void sensor_calibration() {
  if (sampling_count == 0) {
    M5.Lcd.println("calibration start");
  }

  if (sampling_count > SAMPLING_NUM) {
    int sampling_values_sum = 0;

    for (int i = 0; i < SAMPLING_NUM; i++) {
      sampling_values_sum += raw_value;
    }
    base_value = (float)sampling_values_sum / (float)SAMPLING_NUM;

    is_calibration = false;
    sampling_count = 0;
    M5.Lcd.println("calibration completed");
    M5.Lcd.print("base value: ");
    M5.Lcd.println(base_value);
    delay(2000);
    lcd_init();

    Serial.print("base_value: ");
    Serial.println(base_value);
  } else {
    sampling_values[sampling_count] = raw_value;

    is_calibration = true;
    sampling_count ++;
    M5.Lcd.drawCircle(random(M5.Lcd.width() - 1), random(M5.Lcd.height() - 1), random(M5.Lcd.width() - 1), random(0xfffe));
  }
}

void loop() {
  update_sensor_value();
  handle_sensor_value();
  print_sensor_value();

  check_button();
  if (is_calibration) {
    sensor_calibration();
  }

  delay(DELAY);
}
