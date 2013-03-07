#include <aJSON.h>

aJsonStream serial_stream(&Serial);
boolean started = false;
boolean debug = true;
void setup() {
  Serial.begin(9600);
  started = true;
}



void loop() {
  if (serial_stream.available()) {
    serial_stream.skip();
  }
  if (serial_stream.available()) {
    aJsonObject *msg = aJson.parse(&serial_stream);
    if (debug) {
      Serial.println("received:");
      sendJson(msg);
      Serial.println();
    }
    processMessage(msg);
    aJson.deleteItem(msg);
  }
}

void sendJson(aJsonObject *jsonObj) {
    char* json = aJson.print(jsonObj);
    Serial.print(json);
    free(json);
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
  if (!action) {
    aJsonObject *resp = createErrorResp(req, "Invalid action");
    sendJson(resp);
    aJson.deleteItem(resp);
    return;
  }
  
  char *actionValue = action->valuestring;
  if (debug) {
    Serial.print("actionValue:");
    Serial.println(actionValue);
  }
  aJsonObject *resp;
  if (!strcmp(actionValue,"status")) {// get arduino status
    resp = handleStatus(req);
  } else if (!strcmp(actionValue, "mode")) {// set pin mode
    resp = handleSetMode(req);
  } else if (!strcmp(actionValue, "digitalwrite")) {
    resp = handleDigitalWrite(req);
  } else if (!strcmp(actionValue, "digitalread")) {
    resp = handleDigitalRead(req);
  } else if (!strcmp(actionValue, "analogread")) {
    resp = handleAnalogRead(req);
  } else if (!strcmp(actionValue, "analogwrite")) {
    resp = handleAnalogWrite(req);
  } else {
    resp = createErrorResp(req, "Invalid action!");
  }
  
  sendJson(resp);
  if (debug) {
    Serial.println();
  }
  aJson.deleteItem(resp);
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
  return resp;
}

aJsonObject* createResp(aJsonObject *req, const char* result) {
  aJsonObject *requestId = aJson.getObjectItem(req, "id");
  aJsonObject *resp = aJson.createObject();
  aJson.addStringToObject(resp, "id", requestId->valuestring);
  aJson.addStringToObject(resp, "result", result);
  return resp;
}

/*
 * Set pin mode
 */
aJsonObject* handleSetMode(aJsonObject *req) {
  aJsonObject *payloadObj = aJson.getObjectItem(req, "payload");
  aJsonObject *pinObj = aJson.getObjectItem(payloadObj, "pin");
  char* pin  = pinObj->valuestring;
  if (debug) {
    Serial.print("pin:");
    Serial.println(pin);
  }
  int p = getPin(pin);
  if(p == -1) {
    aJsonObject *resp = createErrorResp(req, "Bad pin!");
    return resp;
  }
  aJsonObject *modeObj = aJson.getObjectItem(payloadObj, "mode");
  char* mode  = modeObj->valuestring;
  if (debug) {
    Serial.print("mode:");
    Serial.println(mode);
  }
  if (!strcmp(mode, "OUTPUT")) {
    pinMode(p, OUTPUT);
  } else if (!strcmp(mode, "INPUT")){
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
  if (debug) {
    Serial.print("pin:");
    Serial.println(p);
  }
  if(p == -1) {
    aJsonObject *resp = createErrorResp(req, "Bad pin!");
    return resp;
  }
  aJsonObject *valueObj = aJson.getObjectItem(payloadObj, "value");
  char* value  = valueObj->valuestring;
  if (debug) {
    Serial.print("value:");
    Serial.println(value);
  }
  if (!strcmp(value, "LOW")) {
    digitalWrite(p, LOW);
  } else if (!strcmp(value, "HIGH")){
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
  
  aJsonObject *valueObj = aJson.getObjectItem(payloadObj, "value");
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

