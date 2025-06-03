#pragma once
#include <Arduino.h>
#include <SD.h>

class SDCardManager {
public:
    static bool begin(int csPin, int miso, int mosi, int sck);
    static bool isMounted();
    static String listRoot();

private:
    static bool mounted;
};