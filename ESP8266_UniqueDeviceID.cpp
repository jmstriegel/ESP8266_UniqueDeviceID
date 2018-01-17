/*********************************************************************
An ESP8266 Arduino library for generating a non-volitile and unique
128-bit device identifier. The ID can be auto-generated the first
time a device is booted, and will be retained on subsequent boots.

Written by Jason Striegel.
BSD license. See license.txt for details.
**********************************************************************/

#include "ESP8266_UniqueDeviceID.h"


ESP8266_UniqueDeviceID::ESP8266_UniqueDeviceID(bool debug) {
  init(ESP8266_UNIQUEDEVICEID_DEFAULT_FLOAT_PIN, ESP8266_UNIQUEDEVICEID_DEFAULT_BLINK_PIN, debug);
}

ESP8266_UniqueDeviceID::ESP8266_UniqueDeviceID(int float_pin, int blink_pin, bool debug) {
  init(float_pin, blink_pin, debug);
}

void ESP8266_UniqueDeviceID::init(int float_pin, int blink_pin, bool debug) {
  Serial.println(String("Debug: ") + debug);
  this->float_pin = float_pin;
  this->blink_pin = blink_pin;
  this->debug = debug;

  clearID();
  clearRandom();

    
  SPIFFS.begin();
  pinMode(this->float_pin, INPUT);
  pinMode(this->blink_pin, OUTPUT);

  
  debugOut("ESP8266_UniqueDeviceID::init -- Looking for device ID");
  loadIDFromFile(ESP8266_UNIQUEDEVICEID_FILEPATH);
  

}

bool ESP8266_UniqueDeviceID::hasID() {
  return id_loaded;
}

bool ESP8266_UniqueDeviceID::getBytes(uint8_t bytes[16]) {
  if (!id_loaded) {
    return false;
  }
  for (int x=0; x<16; x++) {
    bytes[x] = unique_id[x];
  }
  return true;
}

void ESP8266_UniqueDeviceID::clearID() {
  id_loaded = false;
  for (int x=0; x<16; x++) {
    unique_id[x] = 0;
  }
}

void ESP8266_UniqueDeviceID::clearRandom() {
  random_data = 0;
  random_initialized = false;
}

bool ESP8266_UniqueDeviceID::initializeNewID() {
  debugOut("ESP8266_UniqueDeviceID::initializeNewID -- Creating new unique device ID");
  if (!random_initialized) {
    initRandom();
  }

  String idstr = "";
  for (int x=0; x<16; x++) {
    unique_id[x] = generateRandomByte();
    idstr += String(unique_id[x], HEX) + " ";
  }
  debugOut("ESP8266_UniqueDeviceID::initializeNewID -- New ID created: " + idstr);
  id_loaded = true;

  if (!saveIDToFile(ESP8266_UNIQUEDEVICEID_FILEPATH)) {
    id_loaded = false;
  }
  
  return id_loaded;
}

bool ESP8266_UniqueDeviceID::removeID() {
  int removed=false;
  debugOut("ESP8266_UniqueDeviceID::removeID -- Wiping device ID from storage");
  clearID();
  if (removed = SPIFFS.remove(ESP8266_UNIQUEDEVICEID_FILEPATH)) {
    debugOut("ESP8266_UniqueDeviceID::removeID -- ID file removed");
  } else {
    debugOut("ESP8266_UniqueDeviceID::removeID -- No ID file existed", 2);  
  }
  return removed;
}

bool ESP8266_UniqueDeviceID::initRandom() {
  debugOut("ESP8266_UniqueDeviceID::initRandom -- Initializing random data from float pin");
  for (int x=0; x<4; x++) {
    generateRandomByte();
  }
  random_initialized = true;
  return random_initialized;
}


uint8_t ESP8266_UniqueDeviceID::generateRandomByte() {
  uint8_t i = 0;
  uint8_t result = 0;
  int blinkon = 0;
  for (int x=0; x<ESP8266_UNIQUEDEVICEID_ROUNDS_PER_BYTE; x++) {
    i = analogRead(float_pin);
    random_data = (random_data << 1) | (random_data >> (32-1)); // rotate 1 bit
    random_data ^= i; // mix with random value from float pin
    delay(1); // delay required between successive pin reads

    if (x % 64 == 0) {
      blinkon = (blinkon + 1) % 2;
      digitalWrite(blink_pin, blinkon);
    }
  }
  result = random_data % 256;
  debugOut(String("ESP8266_UniqueDeviceID::generateRandomByte -- Byte: ") + result + " from mix: " + random_data);
  digitalWrite(blink_pin, HIGH);
  
  return result;
}

bool ESP8266_UniqueDeviceID::saveIDToFile(const char *path) {
  bool success = false;
  if (File id_file = SPIFFS.open(path, "w")) {
      id_file.write(unique_id, 16);
      id_file.close();
      debugOut("ESP8266_UniqueDeviceID::saveIDToFile -- Device ID saved");
      success = true;
  } else {
    debugOut("ESP8266_UniqueDeviceID::loadIDFromFile -- Error storing device ID", 3);
  }
  return success;
}

bool ESP8266_UniqueDeviceID::loadIDFromFile(const char *path) {
  digitalWrite(this->blink_pin, LOW);
  if (SPIFFS.exists(path)) {
    if (File id_file = SPIFFS.open(path, "r")) {
      int count = id_file.readBytes((char *)unique_id, 16);
      if (count == 16) {
        id_loaded = true;
        debugOut("ESP8266_UniqueDeviceID::loadIDFromFile -- Found existing ID");
      } else {
        id_loaded = false;
        debugOut("ESP8266_UniqueDeviceID::loadIDFromFile -- Bad ID file", 3);        
      }      
      id_file.close();
    }
  } else {
    debugOut("ESP8266_UniqueDeviceID::loadIDFromFile -- File not found", 2);
  }
  digitalWrite(this->blink_pin, HIGH);
  return id_loaded;
}


void ESP8266_UniqueDeviceID::debugOut(String output, int level) {
  if (debug == false) {
    return;
  }
  switch(level) {
    case 0:
      Serial.print("INFO: "); break;
    case 1:
      Serial.print("WARN: "); break;
    case 2:
      Serial.print("ERROR: "); break;
  }
  Serial.println(output);
}

