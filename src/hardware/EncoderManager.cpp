#include "EncoderManager.h"
#include <Arduino.h>

// Define static member variables
int EncoderManager::clkPin = -1;
int EncoderManager::dtPin = -1;
int EncoderManager::swPin = -1;
int EncoderManager::lastClkState = HIGH;
int EncoderManager::lastSwState = HIGH;
volatile int EncoderManager::encoderValue = 0;
volatile bool EncoderManager::buttonPressed = false;

void EncoderManager::begin(int clk, int dt, int sw) {
    clkPin = clk;
    dtPin = dt;
    swPin = sw;
    
    // Enable pullups for stable readings
    pinMode(clkPin, INPUT_PULLUP);
    pinMode(dtPin, INPUT_PULLUP);
    pinMode(swPin, INPUT_PULLUP);
    
    // Initialize states
    lastClkState = digitalRead(clkPin);
    lastSwState = digitalRead(swPin);
}

void EncoderManager::update() {
    // Read current states
    int clkState = digitalRead(clkPin);
    int dtState = digitalRead(dtPin);
    int swState = digitalRead(swPin);
    
    // Check for rotation
    if (clkState != lastClkState) {
        if (dtState != clkState) {
            encoderValue++;
        } else {
            encoderValue--;
        }
        lastClkState = clkState;
        delay(5); // Debounce
    }
    
    // Check for button press - only set buttonPressed on transition
    if (swState == LOW && lastSwState == HIGH) {
        buttonPressed = true;
    }
    
    // Just update the last state without clearing the flag
    lastSwState = swState;
}

int EncoderManager::getDelta() {
    static int lastValue = encoderValue;
    int delta = encoderValue - lastValue;
    lastValue = encoderValue;
    return delta;
}

bool EncoderManager::wasPressed() {
    bool pressed = buttonPressed;
    buttonPressed = false; // Clear the flag after reading
    return pressed;
}