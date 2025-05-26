#include <Arduino.h>
#include "lvgl.h"
#include "rm67162.h"

#ifndef BOARD_HAS_PSRAM
#error "PSRAM is required. Please enable OPI PSRAM in your settings."
#endif

// Updated GPIO pins for rotary encoder
#define ENCODER_CLK 2   // GPIO2
#define ENCODER_DT  1   // GPIO1
#define ENCODER_SW  3   // GPIO3

#define BATTERY_ADC_CHANNEL ADC1_CHANNEL_4  // GPIO4
#define BATTERY_MIN_VOLTAGE 3.3
#define BATTERY_MAX_VOLTAGE 4.2

static lv_disp_draw_buf_t draw_buf;
static lv_color_t *buf;

static lv_obj_t *label_counter;
static lv_obj_t *anim_rect;
static lv_obj_t *label_battery;

int counter = 0;
int last_clk_state = HIGH;
int last_sw_state = HIGH;

int anim_x = 10;
int anim_direction = 1;

bool pressed_displayed = false;
unsigned long press_display_time = 0;


unsigned long last_battery_update = 0;

void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
    uint32_t w = area->x2 - area->x1 + 1;
    uint32_t h = area->y2 - area->y1 + 1;
    lcd_PushColors(area->x1, area->y1, w, h, (uint16_t *)&color_p->full);
    lv_disp_flush_ready(disp);
}

float readBatteryVoltage() {
    int raw = analogRead(4);  // GPIO4
    return (raw / 4095.0f) * 3.3f * 2.0f;
}

int batteryPercentFromVoltage(float voltage) {
    int percent = (int)((voltage - BATTERY_MIN_VOLTAGE) / (BATTERY_MAX_VOLTAGE - BATTERY_MIN_VOLTAGE) * 100.0f);
    return constrain(percent, 0, 100);
}

void update_battery() {
    float voltage = readBatteryVoltage();

    char raw[32];
    snprintf(raw, sizeof(raw), "%.2f V raw", voltage);
    lv_label_set_text(label_battery, raw);
}

void setup() {
    // Enable pull-ups for stable reading
    pinMode(ENCODER_CLK, INPUT_PULLUP);
    pinMode(ENCODER_DT, INPUT_PULLUP);
    pinMode(ENCODER_SW, INPUT_PULLUP);

    rm67162_init();
    lcd_setRotation(3);

    lv_init();
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

    lv_obj_t *screen = lv_scr_act();
    lv_obj_set_style_bg_color(screen, lv_color_black(), 0);

    label_counter = lv_label_create(screen);
    lv_label_set_text_fmt(label_counter, "%d", counter);
    lv_obj_set_style_text_color(label_counter, lv_color_white(), 0);  // ✅ Make it visible
    lv_obj_center(label_counter);

    anim_rect = lv_obj_create(screen);
    lv_obj_set_size(anim_rect, 20, 20);
    lv_obj_set_style_bg_color(anim_rect, lv_color_hex(0xFF0000), 0);
    lv_obj_set_style_radius(anim_rect, 4, 0);
    lv_obj_align(anim_rect, LV_ALIGN_TOP_LEFT, anim_x, 10);

    label_battery = lv_label_create(screen);
    lv_obj_set_style_text_color(label_battery, lv_color_hex(0x00FF00), 0);
    lv_obj_align(label_battery, LV_ALIGN_TOP_RIGHT, -8, 4);
    lv_label_set_text(label_battery, "--%");
}

void update_animation() {
    anim_x += anim_direction;
    if (anim_x > 280) anim_direction = -1;
    if (anim_x < 10) anim_direction = 1;

    lv_obj_set_x(anim_rect, anim_x);
}

void update_encoder() {
    int clk = digitalRead(ENCODER_CLK);
    int dt  = digitalRead(ENCODER_DT);
    int sw  = digitalRead(ENCODER_SW);

    if (clk != last_clk_state) {
        if (dt != clk) {
            counter++;
        } else {
            counter--;
        }
        lv_label_set_text_fmt(label_counter, "%d", counter);
        last_clk_state = clk;
        delay(5);
    }

    if (sw == LOW && last_sw_state == HIGH) {
    lv_label_set_text(label_counter, "Pressed!");
    pressed_displayed = true;
    press_display_time = millis();
}
last_sw_state = sw;

// Handle restoring counter text after 300ms
if (pressed_displayed && millis() - press_display_time > 300) {
    lv_label_set_text_fmt(label_counter, "%d", counter);
    pressed_displayed = false;
}

}

void loop() {
    static unsigned long last_tick = 0;
    unsigned long now = millis();

    if (now - last_tick >= 5) {
        lv_tick_inc(now - last_tick);
        last_tick = now;
    }

    lv_timer_handler();
    update_animation();
    update_encoder();

    if (now - last_battery_update > 1000) {
        last_battery_update = now;
        update_battery();
    }
}
