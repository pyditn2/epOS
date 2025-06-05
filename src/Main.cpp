#include <Arduino.h>
#include "lvgl.h"
#include "rm67162.h"
#include "pins_config.h"

#include "os/HardwareService.h"
#include "ui/StatusBarManager.h"

#define BATTERY_ADC_CHANNEL ADC1_CHANNEL_4
#define BATTERY_MIN_VOLTAGE 3.3
#define BATTERY_MAX_VOLTAGE 4.2

static lv_disp_draw_buf_t draw_buf;
static lv_color_t *buf;


void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
    uint32_t w = area->x2 - area->x1 + 1;
    uint32_t h = area->y2 - area->y1 + 1;
    lcd_PushColors(area->x1, area->y1, w, h, (uint16_t *)&color_p->full);
    lv_disp_flush_ready(disp);
}

void setup() {
    // Initialize the display hardware
    rm67162_init();
    lcd_setRotation(1);
    
    // Start LVGL
    lv_init();
    
    // Allocate buffer WITHOUT explicitly setting it to zeros
    buf = (lv_color_t *)ps_malloc(sizeof(lv_color_t) * LVGL_LCD_BUF_SIZE);
    assert(buf);
    
    lv_disp_draw_buf_init(&draw_buf, buf, NULL, LVGL_LCD_BUF_SIZE);
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = EXAMPLE_LCD_H_RES;
    disp_drv.ver_res = EXAMPLE_LCD_V_RES;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    // Set the screen background color directly using lv_scr_act()
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_black(), 0);
    
    // Use the existing screen instead of creating a new one
    lv_obj_t *screen = lv_scr_act();
    
    HardwareService::init();
    StatusBarManager::init(screen);
    
    // Create a black rectangle to cover the main area (not the status bars)
    lv_obj_t *black_area = lv_obj_create(screen);
    const int top_bar_height = 28;
    const int right_bar_width = 54;
    lv_obj_set_size(black_area, EXAMPLE_LCD_H_RES - right_bar_width, EXAMPLE_LCD_V_RES - top_bar_height);
    lv_obj_align(black_area, LV_ALIGN_TOP_LEFT, 0, top_bar_height);
    lv_obj_set_style_bg_color(black_area, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(black_area, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_border_width(black_area, 0, LV_PART_MAIN);
    lv_obj_move_background(black_area);
    
    // Process SD card and battery as before
    if (HardwareService::isSDMounted()) {
        if (HardwareService::tryLockSD()) {
            String files = HardwareService::listSD();
            int fileCount = std::count(files.begin(), files.end(), '\n');
            StatusBarManager::setFileCount(fileCount);
            HardwareService::unlockSD();
        }
    }

    float voltage = HardwareService::readBatteryVoltage();
    StatusBarManager::setBatteryVoltage(voltage);
}



void loop() {
    static unsigned long last_tick = 0;
    static unsigned long last_ui_update = 0;
    unsigned long now = millis();

    // Let LVGL tick (timekeeping) progress regardless
    if (now - last_tick >= 5) {
        lv_tick_inc(now - last_tick);
        last_tick = now;
    }

    // Update screen elements at 60 FPS
    if (now - last_ui_update >= 16) {
        last_ui_update = now;
        lv_timer_handler();
        StatusBarManager::update(now);
    }
    
    // Remove the periodic background refreshing - it might be causing issues
    // Also remove the lcd_fill(0, 0, EXAMPLE_LCD_H_RES, EXAMPLE_LCD_V_RES, 0x001F);
}

