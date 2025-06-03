#include "SDCardManager.h"
#include <SPI.h>

bool SDCardManager::mounted = false;

bool SDCardManager::begin(int csPin, int miso, int mosi, int sck) {
    SPI.begin(sck, miso, mosi, csPin);
    mounted = SD.begin(csPin);
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