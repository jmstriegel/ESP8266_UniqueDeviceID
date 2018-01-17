/*********************************************************************
Example code for the ESP8266_UniqueDeviceID library.

This will generate a random 128-bit ID and save it to the device. On
subsequent device resets, the previously created ID will be loaded.

Written by Jason Striegel.
BSD license. See license.txt for details.
**********************************************************************/
#include "ESP8266_UniqueDeviceID.h"



// ESP8266_UniqueDeviceID manages loading, storage, and random
// generation of 128-bit unique device identifiers.
// This needs to be instantiated after Serial.begin() for debug output.
ESP8266_UniqueDeviceID deviceID; 


void setup() {

  Serial.begin(115200);
  delay(1000);
  Serial.println();

  // This initializes the device id from storage. If this code has
  // already run on the device, the ID will be loaded at this point
  // and hasID() will return true.
  deviceID = ESP8266_UniqueDeviceID();
  // disable debug output:
  // deviceID = ESP8266_UniqueDeviceID(false);
  // 
  // specify different float and blink pins:
  // deviceID = ESP8266_UniqueDeviceID(0, 2);

  // If the device has never booted with this code, it won't have
  // an ID yet. This will use the device's float pin (default: 0)
  // as a source of random entropy and generate a unique 128-bit ID.
  // When a new ID is being generated, the blink_pin (default: 2)
  // will blink brightly for several seconds, indicating that the
  // random number is being generated.
  if (!deviceID.hasID()) {
    deviceID.initializeNewID();
  }

  // The id can be retrieved as an array of 16 bytes
  uint8_t id[16];
  deviceID.getBytes(id);


  Serial.print("Received id: ");
  for (int x=0; x<16; x++) {
    Serial.print(String(id[x], HEX) + " ");
  }
  Serial.println();

  // Unless you call the removeID() method, this unique device
  // identifier will persist on the device when deployed in the field and
  // even if you flash another sketch to the device.
  //deviceID.removeID();
}

void loop() {


}

