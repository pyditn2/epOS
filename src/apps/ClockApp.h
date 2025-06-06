#pragma once
#include "lvgl.h"

class ClockApp {
public:
    static void init(lv_obj_t* parent);   // Create and attach UI to parent
    static void update(unsigned long now); // Called every frame or second
    static void destroy();                // Remove elements when switching apps
};