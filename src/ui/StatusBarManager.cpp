#include "StatusBarManager.h"
#include "../os/HardwareService.h"
#include <cstdio>

lv_obj_t* StatusBarManager::topBar = nullptr;
lv_obj_t* StatusBarManager::rightBar = nullptr;
lv_obj_t* StatusBarManager::label_battery = nullptr;
lv_obj_t* StatusBarManager::label_files = nullptr;

lv_obj_t* StatusBarManager::anim_ball = nullptr; 
int StatusBarManager::anim_x = 10;    
int StatusBarManager::anim_direction = 1; 
static unsigned long last_battery_update = 0;

void StatusBarManager::init(lv_obj_t* parent) {
    const int top_bar_height = 28;
    const int right_bar_width = 54;

    // Top bar
    topBar = lv_obj_create(parent);
    lv_obj_set_size(topBar, lv_disp_get_hor_res(NULL), top_bar_height);
    lv_obj_align(topBar, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_style_bg_color(topBar, lv_color_hex(0x222244), 0);
    lv_obj_set_style_bg_opa(topBar, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(topBar, 0, 0);
    lv_obj_set_style_radius(topBar, 0, 0);
    lv_obj_set_style_pad_all(topBar, 0, 0);
    lv_obj_clear_flag(topBar, LV_OBJ_FLAG_SCROLLABLE);

    // Right bar
    rightBar = lv_obj_create(parent);
    lv_obj_set_size(rightBar, right_bar_width, lv_disp_get_ver_res(NULL) - top_bar_height);
    lv_obj_align(rightBar, LV_ALIGN_TOP_RIGHT, 0, top_bar_height);
    lv_obj_set_style_bg_color(rightBar, lv_color_hex(0x222244), 0);
    lv_obj_set_style_bg_opa(rightBar, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(rightBar, 0, 0);
    lv_obj_set_style_radius(rightBar, 0, 0);
    lv_obj_set_style_pad_all(rightBar, 0, 0);
    lv_obj_clear_flag(rightBar, LV_OBJ_FLAG_SCROLLABLE);

    // Battery label
    label_battery = lv_label_create(rightBar);
    lv_label_set_text(label_battery, "--%");
    lv_obj_set_style_text_color(label_battery, lv_color_white(), 0);
    lv_obj_align(label_battery, LV_ALIGN_TOP_MID, 0, 4);

    // File count label
    label_files = lv_label_create(rightBar);
    lv_label_set_text(label_files, "Files: ?");
    lv_obj_set_style_text_color(label_files, lv_color_white(), 0);
    lv_obj_align(label_files, LV_ALIGN_TOP_MID, 0, 20);

    // Moving ball
    anim_ball = lv_obj_create(rightBar);
    lv_obj_set_size(anim_ball, 16, 16);
    lv_obj_set_style_bg_color(anim_ball, lv_color_hex(0xFF0000), 0);
    lv_obj_set_style_bg_opa(anim_ball, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(anim_ball, 0, 0);
    lv_obj_clear_flag(anim_ball, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_align(anim_ball, LV_ALIGN_TOP_LEFT, anim_x, 40);
}


void StatusBarManager::setBatteryVoltage(float voltage) {
    char buf[16];
    snprintf(buf, sizeof(buf), "%.2f V", voltage);
    lv_label_set_text(label_battery, buf);
}


void StatusBarManager::setBattery(int percent) {
    char buf[16];
    snprintf(buf, sizeof(buf), "%d%%", percent);
    lv_label_set_text(label_battery, buf);
}

void StatusBarManager::setFileCount(int count) {
    char buf[32];
    snprintf(buf, sizeof(buf), "Files: %d", count);
    lv_label_set_text(label_files, buf);
}

void StatusBarManager::update(unsigned long now) {
    // Animation logic
    anim_x += anim_direction;
    if (anim_x > 40) anim_direction = -1;
    if (anim_x < 0) anim_direction = 1;
    lv_obj_set_x(anim_ball, anim_x);

    // Battery update every 1 second
    if (now - last_battery_update > 1000) {
        last_battery_update = now;
        float voltage = HardwareService::readBatteryVoltage();
        setBatteryVoltage(voltage);
    }
}

