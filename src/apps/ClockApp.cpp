#include <cstdio>
#include "ClockApp.h"

static lv_obj_t* container = nullptr;
static lv_obj_t* label_time = nullptr;

void ClockApp::init(lv_obj_t* parent) {
    container = lv_obj_create(parent);
    lv_obj_set_size(container, lv_pct(100), lv_pct(100));
    lv_obj_set_style_bg_color(container, lv_color_black(), 0);
    lv_obj_clear_flag(container, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_border_width(container, 0, 0);

    label_time = lv_label_create(container);
    lv_label_set_text(label_time, "00:00");
    lv_obj_set_style_text_color(label_time, lv_color_white(), 0);
    lv_obj_set_style_text_font(label_time, &lv_font_montserrat_20, 0);
    lv_obj_center(label_time);
}

void ClockApp::update(unsigned long now) {
    // This will eventually be replaced with real-time data
    static int seconds = 0;
    static unsigned long last_update = 0;

    if (now - last_update > 1000) {
        last_update = now;
        seconds = (seconds + 1) % 60;
        char buf[8];
        snprintf(buf, sizeof(buf), "00:%02d", seconds);
        lv_label_set_text(label_time, buf);
    }
}

void ClockApp::destroy() {
    if (container) {
        lv_obj_del(container);
        container = nullptr;
        label_time = nullptr;
    }
}