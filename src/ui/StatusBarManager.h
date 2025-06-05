#pragma once

#include <lvgl.h>

class StatusBarManager {
public:
    static void init(lv_obj_t* parent);
    static void update(unsigned long now);
    static void setBattery(int percent);
    static void setBatteryVoltage(float voltage);
    static void setFileCount(int count);

private:
    static lv_obj_t* topBar;
    static lv_obj_t* rightBar;
    static lv_obj_t* label_battery;
    static lv_obj_t* label_files;

    static lv_obj_t* anim_ball;
    static int anim_x;
    static int anim_direction;
};
