#include "driver/adc.h"

// ADC config for battery measurement
#define BATTERY_ADC_CHANNEL ADC1_CHANNEL_4  // GPIO 4
#define BATTERY_MIN_VOLTAGE 3.3
#define BATTERY_MAX_VOLTAGE 4.2

#include <Arduino.h>
#include "lvgl.h"
#include "rm67162.h"

#ifndef BOARD_HAS_PSRAM
#error "PSRAM is required. Please enable OPI PSRAM in your settings."
#endif

static lv_disp_draw_buf_t draw_buf;
static lv_color_t *buf;

// UI Elements
static lv_obj_t *root;
static lv_obj_t *top_bar;
static lv_obj_t *status_bar;
static lv_obj_t *app_container;

// Status Widgets
static lv_obj_t *label_clock;
static lv_obj_t *label_battery;

// Mock Time
static unsigned long last_update = 0;
static int mock_hours = 14;
static int mock_minutes = 0;
static int mock_seconds = 0;

// Flush callback for LVGL
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
    uint32_t w = area->x2 - area->x1 + 1;
    uint32_t h = area->y2 - area->y1 + 1;
    lcd_PushColors(area->x1, area->y1, w, h, (uint16_t *)&color_p->full);
    lv_disp_flush_ready(disp);
}

void update_clock() {
    mock_seconds++;
    if (mock_seconds >= 60) {
        mock_seconds = 0;
        mock_minutes++;
        if (mock_minutes >= 60) {
            mock_minutes = 0;
            mock_hours = (mock_hours + 1) % 24;
        }
    }
    char time_str[9];
    snprintf(time_str, sizeof(time_str), "%02d:%02d", mock_hours, mock_minutes);
    lv_label_set_text(label_clock, time_str);
}

// Layout Initialization
void create_layout() {
    root = lv_scr_act();
    lv_obj_set_flex_flow(root, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(root, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);

    // Top decorative bar
    top_bar = lv_obj_create(root);
    lv_obj_set_size(top_bar, LV_PCT(100), 15);
    lv_obj_set_style_bg_color(top_bar, lv_color_hex(0xFFD700), 0);
    lv_obj_clear_flag(top_bar, LV_OBJ_FLAG_SCROLLABLE);

    // Horizontal container: [app | status]
    lv_obj_t *content_row = lv_obj_create(root);
    lv_obj_set_size(content_row, LV_PCT(100), LV_PCT(100));
    lv_obj_set_flex_flow(content_row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_grow(content_row, 1);
    lv_obj_set_style_pad_all(content_row, 0, 0);
    lv_obj_clear_flag(content_row, LV_OBJ_FLAG_SCROLLABLE);

    // App container
    app_container = lv_obj_create(content_row);
    lv_obj_set_flex_grow(app_container, 3);
    lv_obj_set_style_bg_color(app_container, lv_color_hex(0x000000), 0);

    // Right vertical status bar
    status_bar = lv_obj_create(content_row);
    lv_obj_set_size(status_bar, 60, LV_PCT(100));
    lv_obj_set_flex_flow(status_bar, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_bg_color(status_bar, lv_color_hex(0xFFD700), 0);
    lv_obj_set_style_pad_all(status_bar, 4, 0);
    lv_obj_clear_flag(status_bar, LV_OBJ_FLAG_SCROLLABLE);

    // Status bar content
    label_clock = lv_label_create(status_bar);
    lv_label_set_text(label_clock, "14:00");

    label_battery = lv_label_create(status_bar);
    lv_label_set_text(label_battery, "89%");
}

// A simple app
void app_clock(lv_obj_t *parent) {
    lv_obj_t *label = lv_label_create(parent);
    lv_label_set_text(label, "This is the Clock App");
    lv_obj_set_style_text_color(label, lv_color_hex(0xFF55FF), 0);
    lv_obj_center(label);
}

float readBatteryVoltage() {
    int raw = adc1_get_raw(BATTERY_ADC_CHANNEL);
    return (raw / 4095.0f) * 3.3f * 2.0f;  // 2.0x due to voltage divider
}

int batteryPercentFromVoltage(float voltage) {
    int percent = (int)((voltage - BATTERY_MIN_VOLTAGE) / (BATTERY_MAX_VOLTAGE - BATTERY_MIN_VOLTAGE) * 100.0f);
    if (percent < 0) percent = 0;
    if (percent > 100) percent = 100;
    return percent;
}

void update_battery() {
    float voltage = readBatteryVoltage();
    int percent = batteryPercentFromVoltage(voltage);

    Serial.print("ADC voltage: ");
    Serial.println(voltage);
    Serial.print("Battery %: ");
    Serial.println(percent);

    char buf[16];
    snprintf(buf, sizeof(buf), "%d%%", percent);
    lv_label_set_text(label_battery, buf);
}


void setup() {
    Serial.begin(115200);

    pinMode(PIN_LED, OUTPUT);
    digitalWrite(PIN_LED, HIGH);

    rm67162_init();
    lcd_setRotation(3); // 180 degrees

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
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(BATTERY_ADC_CHANNEL, ADC_ATTEN_DB_11);

    create_layout();
    app_clock(app_container);
}

void loop() {
    lv_timer_handler();
    delay(5);

    if (millis() - last_update > 1000) {
        last_update = millis();
        update_clock();
        update_battery();
        Serial.print("Battery voltage: ");
        Serial.println(readBatteryVoltage());

    }
}