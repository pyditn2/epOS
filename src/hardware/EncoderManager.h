#pragma once

class EncoderManager {
public:
    static void begin(int clk, int dt, int sw);
    static void update();

    static int getDelta();
    static bool wasPressed();

private:
    static int clkPin, dtPin, swPin;
    static int lastClk, lastSw;
    static int delta;
    static bool pressed;
    static unsigned long pressTime;
};
