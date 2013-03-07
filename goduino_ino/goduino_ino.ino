#include <aJSON.h>

aJsonStream serial_stream(&Serial);
boolean started = false;
void setup() {
  Serial.begin(9600);
  started = true;
}


void loop() {
  if (serial_stream.available()) {
    /* Something real on input, let's take a look. */
    aJsonObject *msg = aJson.parse(&serial_stream);
    processMessage(msg);
    aJson.deleteItem(msg);
  }
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
void processMessage(aJsonObject *req) {
  aJsonObject *action = aJson.getObjectItem(req, "action");
  char *actionValue = action->valuestring;
  
  aJsonObject *resp;
  
  if (actionValue == "status") {// get arduino status
    resp = handleStatus(req);
  } else if (actionValue == "mode") {// set pin mode
    resp = handleSetMode(req);
  } else if (actionValue == "digitalwrite") {
    resp = handleDigitalWrite(req);
  } else if (actionValue == "digitalread") {
    resp = handleDigitalRead(req);
  } else if (actionValue == "analogread") {
    resp = handleAnalogRead(req);
  } else if (actionValue == "analogwrite") {
    resp = handleAnalogWrite(req);
  } else {
    aJsonObject *resp = createErrorResp(req, "Invalid action!");
  }
  
  if (resp) {
    aJson.print(resp, &serial_stream);
    aJson.deleteItem(resp);
  }
}

aJsonObject* handleStatus(aJsonObject *req) {
  aJsonObject *resp = createResp(req, "success");
  if (started) {
    aJson.addStringToObject(resp, "status", "started");
  } else {
    aJson.addStringToObject(resp, "status", "stop");
  }
  return resp;
}

aJsonObject* createErrorResp(aJsonObject *req, const char* message) {
  aJsonObject *resp = createResp(req, "error");
  aJson.addStringToObject(resp, "message", message);
}

aJsonObject* createResp(aJsonObject *req, const char* result) {
  aJsonObject *requestId = aJson.getObjectItem(req, "id");
  aJsonObject *resp = aJson.createObject();
  aJson.addStringToObject(resp, "id", requestId->valuestring);
  aJson.addStringToObject(resp, "result", result);
}

/*
 * Set pin mode
 */
aJsonObject* handleSetMode(aJsonObject *req) {
  aJsonObject *payloadObj = aJson.getObjectItem(req, "payload");
  aJsonObject *pinObj = aJson.getObjectItem(payloadObj, "pin");
  char* pin  = pinObj->valuestring;
  int p = getPin(pin);
  if(p == -1) {
    aJsonObject *resp = createErrorResp(req, "bad pin!");
    return resp;
  }
  aJsonObject *modeObj = aJson.getObjectItem(payloadObj, "mode");
  char* mode  = modeObj->valuestring;
  if (mode == "OUTPUT") {
    pinMode(p, OUTPUT);
  } else if (mode == "INPUT"){
    pinMode(p, INPUT);
  } else {
    aJsonObject *resp = createErrorResp(req, "Invalid mode, mode must be INPUT or OUTPUT");
    return resp;
  }
  
  aJsonObject *resp = createResp(req, "success");
  return resp;
}

aJsonObject* handleDigitalWrite(aJsonObject *req) {
  aJsonObject *payloadObj = aJson.getObjectItem(req, "payload");
  aJsonObject *pinObj = aJson.getObjectItem(payloadObj, "pin");
  char* pin  = pinObj->valuestring;
  int p = getPin(pin);
  if(p == -1) {
    aJsonObject *resp = createErrorResp(req, "Bad pin!");
    return resp;
  }
  aJsonObject *valueObj = aJson.getObjectItem(payloadObj, "mode");
  char* value  = valueObj->valuestring;
  if (value == "LOW") {
    digitalWrite(p, LOW);
  } else if (value == "HIGH"){
    digitalWrite(p, HIGH);
  } else {
    aJsonObject *resp = createErrorResp(req, "Invalid value, digital value must be LOW or HIGH");
    return resp;
  }
  aJsonObject *resp = createResp(req, "success");
  return resp;
}

aJsonObject* handleDigitalRead(aJsonObject *req) {
  aJsonObject *payloadObj = aJson.getObjectItem(req, "payload");
  aJsonObject *pinObj = aJson.getObjectItem(payloadObj, "pin");
  char* pin  = pinObj->valuestring;
  int p = getPin(pin);
  if(p == -1) {
    aJsonObject *resp = createErrorResp(req, "Bad pin!");
    return resp;
  }
  int val = digitalRead(p);

  aJsonObject *resp = createResp(req, "success");
  aJsonObject *respPayloadObj = aJson.createObject();
  aJson.addStringToObject(respPayloadObj,"pin", pin);
  if (val == HIGH) {
    aJson.addStringToObject(respPayloadObj,"value", "HIGH");
  } else {
    aJson.addStringToObject(respPayloadObj,"value", "LOW");
  }
  aJson.addItemToObject(resp, "payload", respPayloadObj);
  return resp;
}

aJsonObject* handleAnalogRead(aJsonObject *req) {
  aJsonObject *payloadObj = aJson.getObjectItem(req, "payload");
  aJsonObject *pinObj = aJson.getObjectItem(payloadObj, "pin");
  char* pin  = pinObj->valuestring;
  int p = getPin(pin);
  if(p == -1) {
    aJsonObject *resp = createErrorResp(req, "Bad pin!");
    return resp;
  }
  
  int val = analogRead(p);
  aJsonObject *resp = createResp(req, "success");
  aJsonObject *respPayloadObj = aJson.createObject();
  aJson.addStringToObject(respPayloadObj,"pin", pin);
  aJson.addNumberToObject(respPayloadObj,"value", val);
  aJson.addItemToObject(resp, "payload", respPayloadObj);
  return resp;
}

aJsonObject* handleAnalogWrite(aJsonObject *req) {
  aJsonObject *payloadObj = aJson.getObjectItem(req, "payload");
  aJsonObject *pinObj = aJson.getObjectItem(payloadObj, "pin");
  char* pin  = pinObj->valuestring;
  int p = getPin(pin);
  if(p == -1) {
    aJsonObject *resp = createErrorResp(req, "Bad pin!");
    return resp;
  }
  
  aJsonObject *valueObj = aJson.getObjectItem(payloadObj, "mode");
  int value  = valueObj->valueint;
  if (value < 0 || value > 255) {
    aJsonObject *resp = createErrorResp(req, "Invalid value, analog value must be 0~255!");
    return resp;
  }
  analogWrite(p,value);
  aJsonObject *resp = createResp(req, "success");
  return resp;
}


int getPin(char *pin) { //Converts to A0-A5, and returns -1 on error
  int ret = -1;
  if(pin[0] == 'A' || pin[0] == 'a') {
    switch(pin[1]) {
      case '0':  ret = A0; break;
      case '1':  ret = A1; break;
      case '2':  ret = A2; break;
      case '3':  ret = A3; break;
      case '4':  ret = A4; break;
      case '5':  ret = A5; break;
      default:             break;
    }
  } else {
    ret = atoi(pin);
    if(ret == 0 && (pin[0] != '0' || pin[1] != '0')) {
      ret = -1;
    }
  }
  return ret;
}

