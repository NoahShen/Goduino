#include <aJSON.h>

aJsonStream serial_stream(&Serial);

void setup()
{
  Serial.begin(9600);
}

/* Process message like: 
{
  "id": "LJjGrWk-187",
  "action": "mode",
  "payload": {
          "pin": "1",
          "mode": "OUTPUT"
     }
} 
*/
void processMessage(aJsonObject *msg)
{
  aJsonObject *action = aJson.getObjectItem(msg, "action");
  char *actionValue = action->valuestring;
  if (actionValue == "status") {// get arduino status
    Serial.println(actionValue);
  } else if (actionValue == "mode") {// set pin mode
    Serial.println(actionValue);
  } else if (actionValue == "digitalwrite") {
    Serial.println(actionValue);
  } else if (actionValue == "digitalread") {
    Serial.println(actionValue);
  } else if (actionValue == "analogread") {
    Serial.println(actionValue);
  } else if (actionValue == "analogwrite") {
    Serial.println(actionValue);
  } else if (actionValue == "analogreference") {
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
