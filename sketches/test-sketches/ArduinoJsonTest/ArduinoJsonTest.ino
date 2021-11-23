#include <M5StickC.h>
#include <ArduinoJson.h>

void setup() {
  M5.begin();

  // Initialize Serial port
  Serial.begin(115200);
  while (!Serial) continue;

  // Allocate the JSON document
  //
  // Inside the brackets, 200 is the RAM allocated to this document.
  // Don't forget to change this value to match your requirement.
  // Use arduinojson.org/v6/assistant to compute the capacity.
  StaticJsonDocument<200> doc;

  // Add an array.
  //
  JsonArray messages = doc.createNestedArray("messages");
  messages[0]["type"] = "text";
  messages[0]["text"] = "Hello HANAPIKU!";
//  messages[1]["type"] = "text";
//  messages[1]["text"] = "Hello HANAPIKU!!!!!";

  // Generate the minified JSON and send it to the Serial port.
  //
  serializeJson(doc, Serial);

  // Start a new line
  Serial.println();

  // Generate the prettified JSON and send it to the Serial port.
  //
  serializeJsonPretty(doc, Serial);
}

void loop() {
  // not used in this example
}
