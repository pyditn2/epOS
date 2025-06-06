#include "SDCardManager.h"
#include <SPI.h>
#include <SD.h>

// Use the HSPI bus for SD card operations
static SPIClass spiSD(HSPI);

bool SDCardManager::mounted = false;

bool SDCardManager::begin(int csPin, int miso, int mosi, int sck) {
    // Initialize HSPI bus with custom pins
    spiSD.begin(sck, miso, mosi, csPin);
    // Use this SPI instance for SD
    mounted = SD.begin(csPin, spiSD);
    return mounted;
}


bool SDCardManager::isMounted() {
    return mounted;
}

String SDCardManager::listRoot() {
    if (!mounted) return "SD not mounted";

    File root = SD.open("/");
    if (!root || !root.isDirectory()) return "Failed to open root";

    String result;
    File file = root.openNextFile();
    while (file) {
        result += file.name();
        result += "\n";
        file = root.openNextFile();
    }
    return result;
}

int SDCardManager::countFiles(const char* path) {
    if (!mounted) return 0;

    File dir = SD.open(path);
    if (!dir || !dir.isDirectory()) return 0;

    int count = 0;

    while (true) {
        File entry = dir.openNextFile();
        if (!entry) break;

        if (entry.isDirectory()) {
            count += countFiles(entry.path());
        } else {
            count++;
        }

        entry.close();
    }

    dir.close();
    return count;
}


// List files and folders in a specific directory
String SDCardManager::listDirectory(const char* path) {
    if (!mounted) return "SD not mounted";

    File dir = SD.open(path);
    if (!dir || !dir.isDirectory()) return "Invalid directory";

    String result;
    File entry = dir.openNextFile();
    while (entry) {
        if (entry.isDirectory()) {
            result += "[DIR] ";
        } else {
            result += "      ";
        }
        result += entry.name();
        result += "\n";
        entry.close();
        entry = dir.openNextFile();
    }
    return result;
}