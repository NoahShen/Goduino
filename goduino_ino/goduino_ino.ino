#include <aJSON.h>

unsigned long last_print = 0;
aJsonStream serial_stream(&Serial);

void setup()
{
  Serial.begin(9600);
}

/* Process message like: { "pwm": { "8": 0, "9": 128 } } */
void processMessage(aJsonObject *msg)
{
  aJsonObject *action = aJson.getObjectItem(msg, "action");
  char *actionValue = action->valuestring;
  if (actionValue == "status") {
    Serial.println(actionValue);
  } else if (actionValue == "mode") {
    Serial.println(actionValue);
  } 
}

void loop()
{
  if (serial_stream.available()) {
    /* Something real on input, let's take a look. */
    aJsonObject *msg = aJson.parse(&serial_stream);
    processMessage(msg);
    aJson.deleteItem(msg);
  }
}
