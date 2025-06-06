#pragma once
#include <Arduino.h>
#include <SD.h>

class SDCardManager {
public:
    static bool begin(int csPin, int miso, int mosi, int sck);
    static bool isMounted();
    static String listRoot();
    static int countFiles(const char* path = "/");               // Recursively count files
    static String listDirectory(const char* path = "/");         // List files and folders in a dir

private:
    static bool mounted;
};