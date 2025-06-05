#pragma once
#include <Arduino.h>
#include <FS.h>

namespace HardwareService {
    void init();
    void update();

    // Encoder API
    int getEncoderDelta();
    bool wasEncoderPressed();

    // SD Card API
    bool isSDMounted();
    String listSD();
    File openFile(const char* path, const char* mode);
    bool tryLockSD();
    void unlockSD();
    float readBatteryVoltage();
}
