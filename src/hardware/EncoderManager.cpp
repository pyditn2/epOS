#include "EncoderManager.h"
#include <Arduino.h>

int EncoderManager::clkPin, EncoderManager::dtPin, EncoderManager::swPin;
int EncoderManager::lastClk = HIGH, EncoderManager::lastSw = HIGH;
int EncoderManager::delta = 0;
bool EncoderManager::pressed = false;
unsigned long EncoderManager::pressTime = 0;

void EncoderManager::begin(int clk, int dt, int sw) {
    clkPin = clk; dtPin = dt; swPin = sw;
    pinMode(clkPin, INPUT_PULLUP);
    pinMode(dtPin, INPUT_PULLUP);
    pinMode(swPin, INPUT_PULLUP);
    lastClk = digitalRead(clkPin);
    lastSw = digitalRead(swPin);
}

void EncoderManager::update() {
    int clk = digitalRead(clkPin);
    int dt = digitalRead(dtPin);
    int sw = digitalRead(swPin);

    if (clk != lastClk) {
        if (dt != clk) delta++;
        else delta--;
        lastClk = clk;
    }

    if (sw == LOW && lastSw == HIGH) {
        pressed = true;
        pressTime = millis();
    }
    lastSw = sw;

    if (pressed && millis() - pressTime > 300) {
        pressed = false;
    }
}

int EncoderManager::getDelta() {
    int d = delta;
    delta = 0;
    return d;
}

bool EncoderManager::wasPressed() {
    if (pressed) {
        pressed = false;
        return true;
    }
    return false;
}