#include <Arduino.h>
#include "lvgl.h"
#include "rm67162.h"
#include "pins_config.h"

#include "os/HardwareService.h"
#include "ui/StatusBarManager.h"
#include "apps/ClockApp.h"

#define BATTERY_ADC_CHANNEL ADC1_CHANNEL_4
#define BATTERY_MIN_VOLTAGE 3.3
#define BATTERY_MAX_VOLTAGE 4.2

static lv_disp_draw_buf_t draw_buf;
static lv_color_t *buf;
static lv_obj_t* app_container = nullptr;

void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
    uint32_t w = area->x2 - area->x1 + 1;
    uint32_t h = area->y2 - area->y1 + 1;
    lcd_PushColors(area->x1, area->y1, w, h, (uint16_t *)&color_p->full);
    lv_disp_flush_ready(disp);
}

void setup() {
    // Initialize display
    rm67162_init();
    lcd_setRotation(3);

    // LVGL setup
    lv_init();
    buf = (lv_color_t *)ps_malloc(sizeof(lv_color_t) * LVGL_LCD_BUF_SIZE);
    assert(buf);

    lv_disp_draw_buf_init(&draw_buf, buf, nullptr, LVGL_LCD_BUF_SIZE);
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = EXAMPLE_LCD_H_RES;
    disp_drv.ver_res = EXAMPLE_LCD_V_RES;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    // Root screen setup
    lv_obj_t *screen = lv_scr_act();
    lv_obj_set_style_bg_color(screen, lv_color_black(), 0);

    // Initialize hardware and UI components
    HardwareService::init();
    StatusBarManager::init(screen);

    // Create the container for apps (excluding top bar and right bar)
    constexpr int top_bar_height = 28;
    constexpr int right_bar_width = 54;

    app_container = lv_obj_create(screen);
    lv_obj_clear_flag(app_container, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(app_container, EXAMPLE_LCD_H_RES - right_bar_width, EXAMPLE_LCD_V_RES - top_bar_height);
    lv_obj_set_pos(app_container, 0, top_bar_height);
    lv_obj_set_style_bg_color(app_container, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(app_container, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(app_container, 0, 0);
    lv_obj_set_style_pad_all(app_container, 0, 0);

    // Initialize the test clock app inside the app container
    ClockApp::init(app_container);

    // SD info and battery
    if (HardwareService::isSDMounted()) {
        if (HardwareService::tryLockSD()) {
            int fileCount = HardwareService::countSDFiles();
            StatusBarManager::setFileCount(fileCount);
            HardwareService::unlockSD();
        }
    }

    float voltage = HardwareService::readBatteryVoltage();
    StatusBarManager::setBatteryVoltage(voltage);
}

void loop() {
    HardwareService::update();
    static unsigned long last_tick = 0;
    static unsigned long last_ui_update = 0;
    unsigned long now = millis();

    if (now - last_tick >= 5) {
        lv_tick_inc(now - last_tick);
        last_tick = now;
    }

    if (now - last_ui_update >= 16) { // ~60 FPS
        last_ui_update = now;
        lv_timer_handler();
        StatusBarManager::update(now);
        ClockApp::update(now);
    }
}
