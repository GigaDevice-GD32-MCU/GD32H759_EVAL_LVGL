#include "lvgl.h"
#include <stdint.h>
#include "lv_demos.h"

extern void lcd_backlight_set_percent(uint8_t percent);
#if LV_USE_MENU && LV_USE_MSGBOX && LV_BUILD_EXAMPLES

/* Language selection enum */
typedef enum {
    LANG_ENGLISH = 0,
    LANG_CHINESE = 1
} language_t;

/* Current language setting */
static language_t current_language = LANG_ENGLISH;

/* Declare Chinese font - 20px to match English font size */
LV_FONT_DECLARE(lv_customer_font_SourceHanSerifSC_Regular_10);

static const lv_font_t * get_ui_font(void);

/* String tables for multi-language support - Main page buttons */
static const char str_widget_cn[] = {0xE5,0xB0,0x8F,0xE9,0x83,0xA8,0xE4,0xBB,0xB6,0x00};
static const char str_music_cn[] = {0xE9,0x9F,0xB3,0xE4,0xB9,0x90,0x00};
static const char str_benchmark_cn[] = {0xE5,0x9F,0xBA,0xE5,0x87,0x86,0xE6,0xB5,0x8B,0xE8,0xAF,0x95,0x00};
static const char str_picture_cn[] = {0xE5,0x9B,0xBE,0xE7,0x89,0x87,0x00};
static const char str_about_cn[] = {0xE5,0x85,0xB3,0xE4,0xBA,0x8E,0x00};
static const char str_qrcode_cn[] = {0xE4,0xBA,0x8C,0xE7,0xBB,0xB4,0xE7,0xA0,0x81,0x00};
static const char str_file_cn[] = {0xE6,0x96,0x87,0xE4,0xBB,0xB6,0x00};
static const char str_setting_cn[] = {0xE8,0xAE,0xBE,0xE7,0xBD,0xAE,0x00};
static const char* str_widget[] = {"Widget", str_widget_cn};
static const char* str_music[] = {"Music", str_music_cn};
static const char* str_benchmark[] = {"Benchmark", str_benchmark_cn};
static const char* str_picture[] = {"Picture", str_picture_cn};
static const char* str_about[] = {"About", str_about_cn};
static const char* str_qrcode[] = {"QRCode", str_qrcode_cn};
static const char* str_file[] = {"File", str_file_cn};
static const char* str_setting[] = {"Setting", str_setting_cn};

/* String tables for Settings menu */
static const char str_settings_title_cn[] = {0xE8,0xAE,0xBE,0xE7,0xBD,0xAE,0x00};
static const char str_time_cn[] = {0xE6,0x97,0xB6,0xE9,0x97,0xB4,0x00};
static const char str_language_cn[] = {0xE8,0xAF,0xAD,0xE8,0xA8,0x80,0x00};
static const char str_display_cn[] = {0xE6,0x98,0xBE,0xE7,0xA4,0xBA,0x00};
static const char str_others_cn[] = {0xE5,0x85,0xB6,0xE4,0xBB,0x96,0x00};
static const char str_about_menu_cn[] = {0xE5,0x85,0xB3,0xE4,0xBA,0x8E,0x00};
static const char str_mcu_info_cn[] = {0x4D,0x43,0x55,0xE4,0xBF,0xA1,0xE6,0x81,0xAF,0x00};
static const char str_lvgl_info_cn[] = {0x4C,0x56,0x47,0x4C,0xE4,0xBF,0xA1,0xE6,0x81,0xAF,0x00};
static const char str_velocity_cn[] = {0xE9,0x80,0x9F,0xE5,0xBA,0xA6,0x00};
static const char str_acceleration_cn[] = {0xE5,0x8A,0xA0,0xE9,0x80,0x9F,0xE5,0xBA,0xA6,0x00};
static const char str_weight_limit_cn[] = {0xE9,0x87,0x8D,0xE9,0x87,0x8F,0xE9,0x99,0x90,0xE5,0x88,0xB6,0x00};
static const char str_brightness_cn[] = {0xE4,0xBA,0xAE,0xE5,0xBA,0xA6,0x00};
/* 制造商: 兆易创新\r\nMCU型号: GD32H759IM\r\nFlash大小: 3840KB\r\nRAM大小: 1024KB\r\n固件版本: 1.0.0 */
static const char str_mcu_details_cn[] = {0xE5,0x88,0xB6,0xE9,0x80,0xA0,0xE5,0x95,0x86,0x3A,0x20,0xE5,0x85,0x86,0xE6,0x98,0x93,0xE5,0x88,0x9B,0xE6,0x96,0xB0,0x0D,0x0A,0x4D,0x43,0x55,0xE5,0x9E,0x8B,0xE5,0x8F,0xB7,0x3A,0x20,0x47,0x44,0x33,0x32,0x48,0x37,0x35,0x39,0x49,0x4D,0x0D,0x0A,0x46,0x6C,0x61,0x73,0x68,0xE5,0xA4,0xA7,0xE5,0xB0,0x8F,0x3A,0x20,0x33,0x38,0x34,0x30,0x4B,0x42,0x0D,0x0A,0x52,0x41,0x4D,0xE5,0xA4,0xA7,0xE5,0xB0,0x8F,0x3A,0x20,0x31,0x30,0x32,0x34,0x4B,0x42,0x0D,0x0A,0xE5,0x9B,0xBA,0xE4,0xBB,0xB6,0xE7,0x89,0x88,0xE6,0x9C,0xAC,0x3A,0x20,0x31,0x2E,0x30,0x2E,0x30,0x00};
static const char str_lvgl_version_cn[] = {0x4C,0x56,0x47,0x4C,0xE7,0x89,0x88,0xE6,0x9C,0xAC,0x3A,0x20,0x56,0x38,0x2E,0x33,0x2E,0x31,0x31,0x00};
static const char str_total_size_cn[] = {0xE6,0x80,0xBB,0xE5,0xA4,0xA7,0xE5,0xB0,0x8F,0x00};
static const char str_mem_used_cn[] = {0xE5,0xB7,0xB2,0xE7,0x94,0xA8,0xE5,0x86,0x85,0xE5,0xAD,0x98,0x00};
static const char str_frag_cn[] = {0xE7,0xA2,0x8E,0xE7,0x89,0x87,0xE5,0x8C,0x96,0xE7,0xA8,0x8B,0xE5,0xBA,0xA6,0x00};
static const char str_temperature_cn[] = {0xE6,0xB8,0xA9,0xE5,0xBA,0xA6,0x00};
static const char str_degrees_cn[] = {0xE6,0x91,0x84,0xE6,0xB0,0x8F,0xE5,0xBA,0xA6,0x00};
static const char str_dropdown_options_cn[] = {0xE8,0x8B,0xB1,0xE8,0xAF,0xAD,0x0A,0xE4,0xB8,0xAD,0xE6,0x96,0x87,0x00};
static const char* str_settings_title[] = {"Setting", str_settings_title_cn};
static const char* str_time[] = {"Time", str_time_cn};
static const char* str_language[] = {"Language", str_language_cn};
static const char* str_display[] = {"Display", str_display_cn};
static const char* str_others[] = {"Others", str_others_cn};
static const char* str_about_menu[] = {"About", str_about_menu_cn};
static const char* str_mcu_info[] = {"MCU information", str_mcu_info_cn};
static const char* str_lvgl_info[] = {"LVGL information", str_lvgl_info_cn};
static const char* str_velocity[] = {"Velocity", str_velocity_cn};
static const char* str_acceleration[] = {"Acceleration", str_acceleration_cn};
static const char* str_weight_limit[] = {"Weight limit", str_weight_limit_cn};
static const char* str_brightness[] = {"Brightness", str_brightness_cn};
static const char* str_mcu_details[] = {
    "Manufacturer: GigaDevice\r\nMCU CPU: GD32H759IM\r\nFlash Size: 3840KB\r\nRAM Size: 1024KB\r\nFW Version: 1.0.0",
    str_mcu_details_cn
};
static const char* str_lvgl_version[] = {"LVGL Version: V8.3.11", str_lvgl_version_cn};

static const char* str_total_size[] = {"Total size", str_total_size_cn};
static const char* str_mem_used[] = {"Memory used", str_mem_used_cn};
static const char* str_frag[] = {"Amount of fragmentation", str_frag_cn};
static const char* str_temperature[] = {"Temperature", str_temperature_cn};
static const char* str_degrees[] = {"degrees centigrade", str_degrees_cn};
static const char* str_dropdown_options[] = {"English\nChinese", str_dropdown_options_cn};

enum {
    LV_MENU_ITEM_BUILDER_VARIANT_1,
    LV_MENU_ITEM_BUILDER_VARIANT_2
};
typedef uint8_t lv_menu_builder_variant_t;

static lv_obj_t * menu;
static lv_obj_t * dropdown_widget = NULL;
static lv_obj_t * brightness_slider = NULL;

/* Static references for settings page labels to enable real-time language updates */
static lv_obj_t * label_time_menu = NULL;
static lv_obj_t * label_language_menu = NULL;
static lv_obj_t * label_display_menu = NULL;
static lv_obj_t * label_others_header = NULL;
static lv_obj_t * label_about_menu = NULL;
static lv_obj_t * label_velocity = NULL;
static lv_obj_t * label_acceleration = NULL;
static lv_obj_t * label_weight = NULL;
static lv_obj_t * label_brightness = NULL;
static lv_obj_t * label_language_item = NULL;
static lv_obj_t * label_mcu_info = NULL;
static lv_obj_t * label_lvgl_info = NULL;
static lv_obj_t * label_mcu_details = NULL;
static lv_obj_t * label_lvgl_version = NULL;

extern lv_obj_t * led_main;
extern lv_obj_t* led_performance;
extern lv_obj_t *obj_performance;

/* External references to main page labels */
extern lv_obj_t *label_widget;
extern lv_obj_t *label_music;
extern lv_obj_t *label_speed;
extern lv_obj_t *label_picture;
extern lv_obj_t *label_about;
extern lv_obj_t *label_qrcode;
extern lv_obj_t *label_file;
extern lv_obj_t *label_setting;

extern lv_obj_t *imgbtn_widget;
extern lv_obj_t *imgbtn_music;
extern lv_obj_t *imgbtn_speed;
extern lv_obj_t *imgbtn_about;
extern lv_obj_t *imgbtn_qrcode;
extern lv_obj_t *imgbtn_file;
extern lv_obj_t *imgbtn_picture;
extern lv_obj_t *imgbtn_setting;

/* External references to performance page labels */
extern lv_obj_t *label_total_size;
extern lv_obj_t *label_mem_used;
extern lv_obj_t *label_frag;
extern lv_obj_t *label_temp;

/* Update performance page labels */
extern lv_mem_monitor_t g_mon;
extern float g_temperature;

static void back_event_handler(lv_event_t * e);
static void switch_handler(lv_event_t * e);
static void language_dropdown_event_handler(lv_event_t * e);
static void brightness_slider_event_handler(lv_event_t * e);
lv_obj_t * root_page;

static lv_obj_t * find_first_label_child(lv_obj_t * parent);
static void set_dropdown_internal_label_font(lv_obj_t * dropdown, const lv_font_t * font);
static void set_dropdown_full_font(lv_obj_t * dropdown, const lv_font_t * font);

static const lv_font_t * get_ui_font(void)
{
    /* Use a CN font with Montserrat fallback so LV_SYMBOL_* (arrow) won't garble */
    static lv_font_t cn_font_with_fallback;
    static bool inited = false;

    if(!inited) {
        cn_font_with_fallback = lv_customer_font_SourceHanSerifSC_Regular_10;
        cn_font_with_fallback.fallback = &lv_font_montserrat_10;
        inited = true;
    }

    return (current_language == LANG_CHINESE) ? (const lv_font_t *)&cn_font_with_fallback : &lv_font_montserrat_10;
}

static lv_obj_t * create_text(lv_obj_t * parent, const char * icon, const char * txt,
                              lv_menu_builder_variant_t builder_variant);
static lv_obj_t * create_slider(lv_obj_t * parent,
                                const char * icon, const char * txt, int32_t min, int32_t max, int32_t val);
static lv_obj_t * create_switch(lv_obj_t * parent,
                                const char * icon, const char * txt, bool chk);
static lv_obj_t * create_dropdown(lv_obj_t * parent, const char * icon, const char * txt, const char * options);

/* Function to update main page language */
void update_main_page_language(void)
{
    /* Select font based on language:
     * English: lv_font_montserrat_20 (20px)
     * Chinese: lv_font_msyh_20_cn (20px) - Same size for consistent appearance
     */
    
    const lv_font_t * font = get_ui_font();
    
    if(label_widget != NULL) {
        lv_label_set_text(label_widget, str_widget[current_language]);
        lv_obj_set_style_text_font(label_widget, font, LV_STATE_DEFAULT);
        lv_obj_align_to(label_widget, imgbtn_widget, LV_ALIGN_OUT_BOTTOM_MID, APP_ICON_LABEL_OFS_X, APP_ICON_LABEL_OFS_Y);
    }
    if(label_music != NULL) {
        lv_label_set_text(label_music, str_music[current_language]);
        lv_obj_set_style_text_font(label_music, font, LV_STATE_DEFAULT);
        lv_obj_align_to(label_music, imgbtn_music, LV_ALIGN_OUT_BOTTOM_MID, APP_ICON_LABEL_OFS_X, APP_ICON_LABEL_OFS_Y);
    }
    if(label_speed != NULL) {
        lv_label_set_text(label_speed, str_benchmark[current_language]);
        lv_obj_set_style_text_font(label_speed, font, LV_STATE_DEFAULT);
        lv_obj_align_to(label_speed, imgbtn_speed, LV_ALIGN_OUT_BOTTOM_MID, APP_ICON_LABEL_OFS_X, APP_ICON_LABEL_OFS_Y);
    }
    if(label_picture != NULL) {
        lv_label_set_text(label_picture, str_picture[current_language]);
        lv_obj_set_style_text_font(label_picture, font, LV_STATE_DEFAULT);
        lv_obj_align_to(label_picture, imgbtn_picture, LV_ALIGN_OUT_BOTTOM_MID, APP_ICON_LABEL_OFS_X, APP_ICON_LABEL_OFS_Y);
    }
    if(label_about != NULL) {
        lv_label_set_text(label_about, str_about[current_language]);
        lv_obj_set_style_text_font(label_about, font, LV_STATE_DEFAULT);
        lv_obj_align_to(label_about, imgbtn_about, LV_ALIGN_OUT_BOTTOM_MID, APP_ICON_LABEL_OFS_X, APP_ICON_LABEL_OFS_Y);
    }
    if(label_qrcode != NULL) {
        lv_label_set_text(label_qrcode, str_qrcode[current_language]);
        lv_obj_set_style_text_font(label_qrcode, font, LV_STATE_DEFAULT);
        lv_obj_align_to(label_qrcode, imgbtn_qrcode, LV_ALIGN_OUT_BOTTOM_MID, APP_ICON_LABEL_OFS_X, APP_ICON_LABEL_OFS_Y);
    }
    if(label_file != NULL) {
        lv_label_set_text(label_file, str_file[current_language]);
        lv_obj_set_style_text_font(label_file, font, LV_STATE_DEFAULT);
        lv_obj_align_to(label_file, imgbtn_file, LV_ALIGN_OUT_BOTTOM_MID, APP_ICON_LABEL_OFS_X, APP_ICON_LABEL_OFS_Y);
    }
    if(label_setting != NULL) {
        lv_label_set_text(label_setting, str_setting[current_language]);
        lv_obj_set_style_text_font(label_setting, font, LV_STATE_DEFAULT);
        lv_obj_align_to(label_setting, imgbtn_setting, LV_ALIGN_OUT_BOTTOM_MID, APP_ICON_LABEL_OFS_X, APP_ICON_LABEL_OFS_Y);
    }
   
    
    if(label_total_size != NULL) {
        lv_obj_set_style_text_font(label_total_size, font, LV_STATE_DEFAULT);
        lv_label_set_text_fmt(label_total_size, "%s: %"LV_PRId32" KB", str_total_size[current_language], g_mon.total_size/1024);
    }
    if(label_mem_used != NULL) {
        uint32_t used_size = g_mon.total_size - g_mon.free_size;
        uint32_t used_kb = used_size / 1024;
        lv_obj_set_style_text_font(label_mem_used, font, LV_STATE_DEFAULT);
        lv_label_set_text_fmt(label_mem_used, "%s: %"LV_PRIu32" KB", str_mem_used[current_language], used_kb);
    }
    if(label_frag != NULL) {
        lv_obj_set_style_text_font(label_frag, font, LV_STATE_DEFAULT);
        lv_label_set_text_fmt(label_frag, "%s: %"LV_PRIu32"%%", str_frag[current_language], g_mon.frag_pct);
    }
    if(label_temp != NULL) {
        lv_obj_set_style_text_font(label_temp, font, LV_STATE_DEFAULT);
        lv_label_set_text_fmt(label_temp, "%s: %"LV_PRId32" %s", str_temperature[current_language], (uint32_t)g_temperature, str_degrees[current_language]);
    }

    /* Update settings page labels (if settings page is currently active) */
    if(label_time_menu != NULL) {
        lv_obj_set_style_text_font(label_time_menu, font, LV_STATE_DEFAULT);
        lv_label_set_text(label_time_menu, str_time[current_language]);
    }
    if(label_language_menu != NULL) {
        lv_obj_set_style_text_font(label_language_menu, font, LV_STATE_DEFAULT);
        lv_label_set_text(label_language_menu, str_language[current_language]);
    }
    if(label_display_menu != NULL) {
        lv_obj_set_style_text_font(label_display_menu, font, LV_STATE_DEFAULT);
        lv_label_set_text(label_display_menu, str_display[current_language]);
    }
    if(label_others_header != NULL) {
        lv_obj_set_style_text_font(label_others_header, font, LV_STATE_DEFAULT);
        lv_label_set_text(label_others_header, str_others[current_language]);
    }
    if(label_about_menu != NULL) {
        lv_obj_set_style_text_font(label_about_menu, font, LV_STATE_DEFAULT);
        lv_label_set_text(label_about_menu, str_about_menu[current_language]);
    }
    if(label_velocity != NULL) {
        lv_obj_set_style_text_font(label_velocity, font, LV_STATE_DEFAULT);
        lv_label_set_text(label_velocity, str_velocity[current_language]);
    }
    if(label_acceleration != NULL) {
        lv_obj_set_style_text_font(label_acceleration, font, LV_STATE_DEFAULT);
        lv_label_set_text(label_acceleration, str_acceleration[current_language]);
    }
    if(label_weight != NULL) {
        lv_obj_set_style_text_font(label_weight, font, LV_STATE_DEFAULT);
        lv_label_set_text(label_weight, str_weight_limit[current_language]);
    }
    if(label_brightness != NULL) {
        lv_obj_set_style_text_font(label_brightness, font, LV_STATE_DEFAULT);
        lv_label_set_text(label_brightness, str_brightness[current_language]);
    }
    if(label_language_item != NULL) {
        lv_obj_set_style_text_font(label_language_item, font, LV_STATE_DEFAULT);
        lv_label_set_text(label_language_item, str_language[current_language]);
    }

    /* Update the About sub-pages so they switch language immediately */
    if(label_mcu_info != NULL) {
        lv_obj_set_style_text_font(label_mcu_info, font, LV_STATE_DEFAULT);
        lv_label_set_text(label_mcu_info, str_mcu_info[current_language]);
    }
    if(label_lvgl_info != NULL) {
        lv_obj_set_style_text_font(label_lvgl_info, font, LV_STATE_DEFAULT);
        lv_label_set_text(label_lvgl_info, str_lvgl_info[current_language]);
    }
    if(label_mcu_details != NULL) {
        lv_obj_set_style_text_font(label_mcu_details, font, LV_STATE_DEFAULT);
        lv_label_set_text(label_mcu_details, str_mcu_details[current_language]);
    }
    if(label_lvgl_version != NULL) {
        lv_obj_set_style_text_font(label_lvgl_version, font, LV_STATE_DEFAULT);
        lv_label_set_text(label_lvgl_version, str_lvgl_version[current_language]);
    }

    /* Update dropdown options/font without breaking arrow symbol */
    if(dropdown_widget != NULL) {
        uint16_t selected = lv_dropdown_get_selected(dropdown_widget);
        lv_dropdown_set_options(dropdown_widget, str_dropdown_options[current_language]);
        lv_dropdown_set_selected(dropdown_widget, selected);
        set_dropdown_full_font(dropdown_widget, font);
    }
}

/* Lightweight periodic refresh: only the four labels whose *value* changes.
 * update_main_page_language() must NOT be called from a timer - it re-applies
 * fonts, re-aligns every button label and rebuilds the dropdown options, which
 * invalidates the whole layout on every call. */
void update_status_labels(void)
{
    if(label_total_size != NULL) {
        lv_label_set_text_fmt(label_total_size, "%s: %"LV_PRId32" KB", str_total_size[current_language],
                              g_mon.total_size / 1024);
    }
    if(label_mem_used != NULL) {
        uint32_t used_kb = (g_mon.total_size - g_mon.free_size) / 1024;
        lv_label_set_text_fmt(label_mem_used, "%s: %"LV_PRIu32" KB", str_mem_used[current_language], used_kb);
    }
    if(label_frag != NULL) {
        lv_label_set_text_fmt(label_frag, "%s: %"LV_PRIu32"%%", str_frag[current_language], g_mon.frag_pct);
    }
    if(label_temp != NULL) {
        lv_label_set_text_fmt(label_temp, "%s: %"LV_PRId32" %s", str_temperature[current_language],
                              (uint32_t)g_temperature, str_degrees[current_language]);
    }
}

/* The settings/about pages live on the active screen. When the screen is wiped
 * (e.g. the benchmark demo takes it over) every cached pointer below becomes
 * dangling, so drop them before update_main_page_language() runs again. */
void lv_demo_setting_reset_refs(void)
{
    menu = NULL;
    root_page = NULL;
    dropdown_widget = NULL;
    brightness_slider = NULL;

    label_time_menu = NULL;
    label_language_menu = NULL;
    label_display_menu = NULL;
    label_others_header = NULL;
    label_about_menu = NULL;
    label_velocity = NULL;
    label_acceleration = NULL;
    label_weight = NULL;
    label_brightness = NULL;
    label_language_item = NULL;
    label_mcu_info = NULL;
    label_lvgl_info = NULL;
    label_mcu_details = NULL;
    label_lvgl_version = NULL;
}

/* Language dropdown event handler */
static void language_dropdown_event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    
    if(code == LV_EVENT_VALUE_CHANGED) {
        uint16_t selected = lv_dropdown_get_selected(obj);
        
        if(selected == 0) {
            current_language = LANG_ENGLISH;
        } else if(selected == 1) {
            current_language = LANG_CHINESE;
        }
        
        /* Update all text on main page */
        update_main_page_language();
    }
}

static void brightness_slider_event_handler(lv_event_t * e)
{
    if(lv_event_get_code(e) != LV_EVENT_VALUE_CHANGED) return;

    lv_obj_t * slider = lv_event_get_target(e);
    int32_t value = lv_slider_get_value(slider);

    /* Slider range is 30..100 in this UI; map directly to PWM duty percent. */
    if(value < 0) value = 0;
    if(value > 100) value = 100;
    lcd_backlight_set_percent((uint8_t)value);
}

void lv_demo_setting(lv_obj_t * parent)
{
    lv_obj_add_flag(led_main, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(led_performance, LV_OBJ_FLAG_HIDDEN);
    menu = lv_menu_create(parent);

    lv_color_t bg_color = lv_obj_get_style_bg_color(menu, 0);
    if(lv_color_brightness(bg_color) > 127) {
        lv_obj_set_style_bg_color(menu, lv_color_darken(lv_obj_get_style_bg_color(menu, 0), 10), 0);
    }
    else {
        lv_obj_set_style_bg_color(menu, lv_color_darken(lv_obj_get_style_bg_color(menu, 0), 50), 0);
    }
    lv_menu_set_mode_root_back_btn(menu, LV_MENU_ROOT_BACK_BTN_ENABLED);
    lv_obj_add_event_cb(menu, back_event_handler, LV_EVENT_CLICKED, menu);
    lv_obj_set_size(menu, lv_disp_get_hor_res(NULL), lv_disp_get_ver_res(NULL));
    lv_obj_center(menu);

    lv_obj_t * cont;
    lv_obj_t * section;

    /*Create sub pages*/
    lv_obj_t * sub_time_page = lv_menu_page_create(menu, NULL);
    lv_obj_set_style_pad_hor(sub_time_page, lv_obj_get_style_pad_left(lv_menu_get_main_header(menu), 0), 0);
    lv_menu_separator_create(sub_time_page);
    section = lv_menu_section_create(sub_time_page);
    cont = create_slider(section, LV_SYMBOL_SETTINGS, str_velocity[current_language], 0, 150, 120);
    label_velocity = find_first_label_child(cont);
    cont = create_slider(section, LV_SYMBOL_SETTINGS, str_acceleration[current_language], 0, 150, 50);
    label_acceleration = find_first_label_child(cont);
    cont = create_slider(section, LV_SYMBOL_SETTINGS, str_weight_limit[current_language], 0, 150, 80);
    label_weight = find_first_label_child(cont);

    lv_obj_t * sub_display_page = lv_menu_page_create(menu, NULL);
    lv_obj_set_style_pad_hor(sub_display_page, lv_obj_get_style_pad_left(lv_menu_get_main_header(menu), 0), 0);
    lv_menu_separator_create(sub_display_page);
    section = lv_menu_section_create(sub_display_page);

    cont = create_slider(section, LV_SYMBOL_SETTINGS, str_brightness[current_language], 30, 100, 99);
    label_brightness = find_first_label_child(cont);

    /* The slider is created as the last child of the container returned by create_slider() */
    brightness_slider = lv_obj_get_child(cont, -1);
    if(brightness_slider != NULL) {
        lv_obj_add_event_cb(brightness_slider, brightness_slider_event_handler, LV_EVENT_VALUE_CHANGED, NULL);
        lcd_backlight_set_percent((uint8_t)lv_slider_get_value(brightness_slider));
    }

    lv_obj_t * sub_language_page = lv_menu_page_create(menu, NULL);
    lv_obj_set_style_pad_hor(sub_language_page, lv_obj_get_style_pad_left(lv_menu_get_main_header(menu), 0), 0);
    lv_menu_separator_create(sub_language_page);
    section = lv_menu_section_create(sub_language_page);
    // lv_obj_t* lv_ddlist1 = lv_dropdown_create(section);
    // // 使用 set_options 一次性设置所有选项（用 \n 分隔）
    // lv_dropdown_set_options(lv_ddlist1, "English\nChinese");
    // lv_dropdown_set_selected(lv_ddlist1, 0);
    lv_obj_t * language_dropdown_cont = create_dropdown(section, LV_SYMBOL_SETTINGS, str_language[current_language], str_dropdown_options[current_language]);
    label_language_item = find_first_label_child(language_dropdown_cont);
    
    /* Get the actual dropdown widget and add event callback */
    dropdown_widget = lv_obj_get_child(language_dropdown_cont, -1); /* Get last child (dropdown) */
    
    /* Set dropdown to current language selection */
    lv_dropdown_set_selected(dropdown_widget, current_language);
    
    /* Add event callback for value changes */
    lv_obj_add_event_cb(dropdown_widget, language_dropdown_event_handler, LV_EVENT_VALUE_CHANGED, NULL);
    // 设置下拉列表宽度以适应内容
    // lv_obj_set_width(lv_ddlist1, LV_PCT(100));
    

    lv_obj_t * sub_software_info_page = lv_menu_page_create(menu, NULL);
    lv_obj_set_style_pad_hor(sub_software_info_page, lv_obj_get_style_pad_left(lv_menu_get_main_header(menu), 0), 0);
    section = lv_menu_section_create(sub_software_info_page);
    cont = create_text(section, NULL, str_mcu_details[current_language], LV_MENU_ITEM_BUILDER_VARIANT_1);
    label_mcu_details = find_first_label_child(cont);

    lv_obj_t * sub_legal_info_page = lv_menu_page_create(menu, NULL);
    lv_obj_set_style_pad_hor(sub_legal_info_page, lv_obj_get_style_pad_left(lv_menu_get_main_header(menu), 0), 0);
    section = lv_menu_section_create(sub_legal_info_page);
    cont = create_text(section, NULL, str_lvgl_version[current_language], LV_MENU_ITEM_BUILDER_VARIANT_1);
    label_lvgl_version = find_first_label_child(cont);

    lv_obj_t * sub_about_page = lv_menu_page_create(menu, NULL);
    lv_obj_set_style_pad_hor(sub_about_page, lv_obj_get_style_pad_left(lv_menu_get_main_header(menu), 0), 0);
    lv_menu_separator_create(sub_about_page);
    section = lv_menu_section_create(sub_about_page);
    cont = create_text(section, NULL, str_mcu_info[current_language], LV_MENU_ITEM_BUILDER_VARIANT_1);
    label_mcu_info = find_first_label_child(cont);
    lv_menu_set_load_page_event(menu, cont, sub_software_info_page);
    cont = create_text(section, NULL, str_lvgl_info[current_language], LV_MENU_ITEM_BUILDER_VARIANT_1);
    label_lvgl_info = find_first_label_child(cont);
    lv_menu_set_load_page_event(menu, cont, sub_legal_info_page);

    /*Create a root page*/
    root_page = lv_menu_page_create(menu, "");
    lv_obj_set_style_pad_hor(root_page, lv_obj_get_style_pad_left(lv_menu_get_main_header(menu), 0), 0);
    section = lv_menu_section_create(root_page);

    cont = create_text(section, LV_SYMBOL_SETTINGS, str_time[current_language], LV_MENU_ITEM_BUILDER_VARIANT_1);
    label_time_menu = find_first_label_child(cont);
    lv_menu_set_load_page_event(menu, cont, sub_time_page);

    cont = create_text(section, LV_SYMBOL_SETTINGS, str_language[current_language], LV_MENU_ITEM_BUILDER_VARIANT_1);
    label_language_menu = find_first_label_child(cont);
    lv_menu_set_load_page_event(menu, cont, sub_language_page);

    cont = create_text(section, LV_SYMBOL_SETTINGS, str_display[current_language], LV_MENU_ITEM_BUILDER_VARIANT_1);
    label_display_menu = find_first_label_child(cont);
    lv_menu_set_load_page_event(menu, cont, sub_display_page);

    cont = create_text(root_page, NULL, str_others[current_language], LV_MENU_ITEM_BUILDER_VARIANT_1);
    label_others_header = find_first_label_child(cont);
    section = lv_menu_section_create(root_page);
    cont = create_text(section, LV_SYMBOL_BELL, str_about_menu[current_language], LV_MENU_ITEM_BUILDER_VARIANT_1);
    label_about_menu = find_first_label_child(cont);
    lv_menu_set_load_page_event(menu, cont, sub_about_page);

    lv_menu_set_sidebar_page(menu, root_page);

    lv_event_send(lv_obj_get_child(lv_obj_get_child(lv_menu_get_cur_sidebar_page(menu), 0), 0), LV_EVENT_CLICKED, NULL);
}

static void back_event_handler(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_target(e);
    lv_obj_t * menu_to_delete = lv_event_get_user_data(e);

    if(lv_menu_back_btn_is_root(menu_to_delete, obj)) {
        lv_obj_clear_flag(led_main, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(led_performance, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(obj_performance, LV_OBJ_FLAG_HIDDEN);
        
        /* Clear all static references before deleting menu to avoid dangling pointers */
        dropdown_widget = NULL;
        brightness_slider = NULL;
        label_time_menu = NULL;
        label_language_menu = NULL;
        label_display_menu = NULL;
        label_others_header = NULL;
        label_about_menu = NULL;
        label_velocity = NULL;
        label_acceleration = NULL;
        label_weight = NULL;
        label_brightness = NULL;
        label_language_item = NULL;
        label_mcu_info = NULL;
        label_lvgl_info = NULL;
        label_mcu_details = NULL;
        label_lvgl_version = NULL;
        menu = NULL;
        root_page = NULL;
        
        lv_obj_del(menu_to_delete);
    }
}

static lv_obj_t * find_first_label_child(lv_obj_t * parent)
{
    if(parent == NULL) return NULL;
    uint32_t child_cnt = lv_obj_get_child_cnt(parent);
    for(uint32_t i = 0; i < child_cnt; i++) {
        lv_obj_t * child = lv_obj_get_child(parent, i);
        if(lv_obj_check_type(child, &lv_label_class)) {
            return child;
        }
    }
    return NULL;
}

static void set_dropdown_internal_label_font(lv_obj_t * dropdown, const lv_font_t * font)
{
    if(dropdown == NULL || font == NULL) return;
    uint32_t child_cnt = lv_obj_get_child_cnt(dropdown);
    for(uint32_t i = 0; i < child_cnt; i++) {
        lv_obj_t * child = lv_obj_get_child(dropdown, i);
        if(lv_obj_check_type(child, &lv_label_class)) {
            lv_obj_set_style_text_font(child, font, LV_STATE_DEFAULT);
        }
    }
}

static void set_dropdown_full_font(lv_obj_t * dropdown, const lv_font_t * font)
{
    if(dropdown == NULL || font == NULL) return;

    /* 1) Apply to dropdown object itself (selected text is styled here in some themes) */
    lv_obj_set_style_text_font(dropdown, font, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(dropdown, font, LV_PART_SELECTED | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(dropdown, font, LV_PART_ITEMS | LV_STATE_DEFAULT);

    /* 2) Apply to internal label(s) (keeps arrow OK via fallback) */
    set_dropdown_internal_label_font(dropdown, font);

    /* 3) Apply to the popup list (btnmatrix items) */
    lv_obj_t * list = lv_dropdown_get_list(dropdown);
    if(list != NULL) {
        lv_obj_set_style_text_font(list, font, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(list, font, LV_PART_ITEMS | LV_STATE_DEFAULT);

        uint32_t cnt = lv_obj_get_child_cnt(list);
        for(uint32_t i = 0; i < cnt; i++) {
            lv_obj_t * c = lv_obj_get_child(list, i);
            lv_obj_set_style_text_font(c, font, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(c, font, LV_PART_ITEMS | LV_STATE_DEFAULT);
        }
    }
}

static void switch_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * menu = lv_event_get_user_data(e);
    lv_obj_t * obj = lv_event_get_target(e);
    if(code == LV_EVENT_VALUE_CHANGED) {
        if(lv_obj_has_state(obj, LV_STATE_CHECKED)) {
            lv_menu_set_page(menu, NULL);
            lv_menu_set_sidebar_page(menu, root_page);
            lv_event_send(lv_obj_get_child(lv_obj_get_child(lv_menu_get_cur_sidebar_page(menu), 0), 0), LV_EVENT_CLICKED, NULL);
        }
        else {
            lv_menu_set_sidebar_page(menu, NULL);
            lv_menu_clear_history(menu); /* Clear history because we will be showing the root page later */
            lv_menu_set_page(menu, root_page);
        }
    }
}

static lv_obj_t * create_text(lv_obj_t * parent, const char * icon, const char * txt,
                              lv_menu_builder_variant_t builder_variant)
{
    lv_obj_t * obj = lv_menu_cont_create(parent);

    lv_obj_t * img = NULL;
    lv_obj_t * label = NULL;

    if(icon) {
        img = lv_img_create(obj);
        lv_img_set_src(img, icon);
    }

    if(txt) {
        label = lv_label_create(obj);
        lv_label_set_text(label, txt);
        lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR);
        lv_obj_set_flex_grow(label, 1);
        
        /* Set font based on current language */
        const lv_font_t * font = get_ui_font();
        lv_obj_set_style_text_font(label, font, LV_STATE_DEFAULT);
    }

    if(builder_variant == LV_MENU_ITEM_BUILDER_VARIANT_2 && icon && txt) {
        lv_obj_add_flag(img, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);
        lv_obj_swap(img, label);
    }

    return obj;
}

static lv_obj_t * create_slider(lv_obj_t * parent, const char * icon, const char * txt, int32_t min, int32_t max,
                                int32_t val)
{
    lv_obj_t * obj = create_text(parent, icon, txt, LV_MENU_ITEM_BUILDER_VARIANT_2);

    /* Set font for the label based on current language */
    const lv_font_t * font = get_ui_font();
    
    /* Find the label child and set its font */
    uint32_t child_cnt = lv_obj_get_child_cnt(obj);
    for(uint32_t i = 0; i < child_cnt; i++) {
        lv_obj_t * child = lv_obj_get_child(obj, i);
        if(lv_obj_check_type(child, &lv_label_class)) {
            lv_obj_set_style_text_font(child, font, LV_STATE_DEFAULT);
            break;
        }
    }

    lv_obj_t * slider = lv_slider_create(obj);
    lv_obj_set_flex_grow(slider, 1);
    lv_slider_set_range(slider, min, max);
    lv_slider_set_value(slider, val, LV_ANIM_OFF);

    if(icon == NULL) {
        lv_obj_add_flag(slider, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);
    }

    return obj;
}

static lv_obj_t * create_switch(lv_obj_t * parent, const char * icon, const char * txt, bool chk)
{
    // const lv_font_t * font = (current_language == LANG_CHINESE) ? &lv_customer_font_SourceHanSerifSC_Regular_20 : &lv_font_montserrat_20;
    // lv_obj_set_style_text_font(txt, font, LV_STATE_DEFAULT);
    lv_obj_t * obj = create_text(parent, icon, txt, LV_MENU_ITEM_BUILDER_VARIANT_1);

    lv_obj_t * sw = lv_switch_create(obj);
    lv_obj_add_state(sw, chk ? LV_STATE_CHECKED : 0);

    return obj;
}

static lv_obj_t * create_dropdown(lv_obj_t * parent, const char * icon, const char * txt, const char * options)
{
    lv_obj_t * obj = create_text(parent, icon, txt, LV_MENU_ITEM_BUILDER_VARIANT_2);

    lv_obj_t * dropdown = lv_dropdown_create(obj);
    lv_obj_set_flex_grow(dropdown, 1);
    lv_dropdown_set_options(dropdown, options);
    
    /* Set fonts for selected text + options list; arrow stays correct via fallback */
    const lv_font_t * font = get_ui_font();
    set_dropdown_full_font(dropdown, font);
    
    if(icon == NULL) {
        lv_obj_add_flag(dropdown, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);
    }

    return obj;
}

#endif
