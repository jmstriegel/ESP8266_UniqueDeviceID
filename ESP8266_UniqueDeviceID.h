/*********************************************************************
An ESP8266 Arduino library for generating a non-volitile and unique
128-bit device identifier. The ID can be auto-generated the first
time a device is booted, and will be retained on subsequent boots.

Written by Jason Striegel.
BSD license. See license.txt for details.
**********************************************************************/

#ifndef _ESP8266_UniqueDeviceID_H_
#define _ESP8266_UniqueDeviceID_H_

#include <Arduino.h>
#include <FS.h>


#define ESP8266_UNIQUEDEVICEID_DEFAULT_FLOAT_PIN 0
#define ESP8266_UNIQUEDEVICEID_DEFAULT_BLINK_PIN 2
#define ESP8266_UNIQUEDEVICEID_ROUNDS_PER_BYTE 256
#define ESP8266_UNIQUEDEVICEID_FILEPATH "/unique_id_128"

class ESP8266_UniqueDeviceID {
  public:
    bool debug;
    
    ESP8266_UniqueDeviceID(bool debug=true);
    ESP8266_UniqueDeviceID(int float_pin, int blink_pin, bool debug=true);
    bool loadIDFromFile(const char *path);
    bool saveIDToFile(const char *path);
    bool hasID();
    bool initializeNewID();
    bool removeID();
    bool getBytes(uint8_t bytes[16]);
  
  private:
    bool id_loaded;
    bool random_initialized;
    uint8_t unique_id[16];
    uint32_t random_data;
    int float_pin;
    int blink_pin;

    void init(int float_pin, int blink_pin, bool debug=false);
    void debugOut(String output, int level=0);
    void clearID();
    void clearRandom();
    bool initRandom();
    uint8_t generateRandomByte();
};



#endif /*_ESP8266_UniqueDeviceID_H_*/
