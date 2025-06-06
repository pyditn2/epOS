#pragma once

class EncoderManager {
public:
    static void begin(int clk, int dt, int sw);
    static void update();
    static int getDelta();
    static bool wasPressed();

private:
    static int clkPin;
    static int dtPin;
    static int swPin;
    static int lastClkState;
    static int lastSwState;
    static volatile int encoderValue;
    static volatile bool buttonPressed;
};
