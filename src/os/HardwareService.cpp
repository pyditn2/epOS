#include "HardwareService.h"
#include "../hardware/EncoderManager.h"
#include "../hardware/SDCardManager.h"
#include <SPI.h>

namespace {
    constexpr int ENCODER_CLK = 2;
    constexpr int ENCODER_DT  = 1;
    constexpr int ENCODER_SW  = 3;

    constexpr int SD_CS   = 10;
    constexpr int SD_MISO = 13;
    constexpr int SD_MOSI = 11;
    constexpr int SD_SCK  = 12;

    volatile bool sdLocked = false;
}

void HardwareService::init() {
    EncoderManager::begin(ENCODER_CLK, ENCODER_DT, ENCODER_SW);
    SDCardManager::begin(SD_CS, SD_MISO, SD_MOSI, SD_SCK);
}

void HardwareService::update() {
    EncoderManager::update();
}

int HardwareService::getEncoderDelta() {
    return EncoderManager::getDelta();
}

bool HardwareService::wasEncoderPressed() {
    return EncoderManager::wasPressed();
}

bool HardwareService::isSDMounted() {
    return SDCardManager::isMounted();
}

String HardwareService::listSD() {
    if (!sdLocked) return "ERROR: SD not locked\n";
    return SDCardManager::listRoot();
}

File HardwareService::openFile(const char* path, const char* mode) {
    if (!sdLocked) return File();
    return SD.open(path, mode);
}

bool HardwareService::tryLockSD() {
    if (sdLocked) return false;
    sdLocked = true;
    return true;
}

void HardwareService::unlockSD() {
    sdLocked = false;
}

int HardwareService::countSDFiles() {
    if (!sdLocked) return 0;
    return SDCardManager::countFiles("/");
}

String HardwareService::listDirectory(const char* path) {
    if (!sdLocked) return "ERROR: SD not locked\n";
    return SDCardManager::listDirectory(path);
}

float HardwareService::readBatteryVoltage() {
    int raw = analogRead(4);
    return (raw / 4095.0f) * 3.3f * 2.0f;
}