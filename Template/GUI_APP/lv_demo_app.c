/**
 * @file lv_demo_app.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lvgl.h"
#include <stdio.h>
#include "lv_demos.h"
#include "gd32h73x_75x.h"
#include "lv_file_browse.h"
#include "lv_about.h"
#include "lv_port_disp.h"

/*********************
 *      DEFINES
 *********************/
LV_IMG_DECLARE(img_background);
LV_IMG_DECLARE(img_widget);
LV_IMG_DECLARE(img_music);
LV_IMG_DECLARE(img_speed);
LV_IMG_DECLARE(img_about);
LV_IMG_DECLARE(img_file);
LV_IMG_DECLARE(img_qrcode);
LV_IMG_DECLARE(img_home);
LV_IMG_DECLARE(img_home_pressed);
LV_IMG_DECLARE(img_gd_logo);
LV_IMG_DECLARE(img_picture);
LV_IMG_DECLARE(img_setting);
LV_IMG_DECLARE(img_temp);
LV_IMG_DECLARE(img_mem);

LV_FONT_DECLARE(lv_font_montserrat_8);
LV_FONT_DECLARE(lv_font_digital_20);
LV_FONT_DECLARE(lv_font_digital_30);

uint8_t g_music_flag = 0;

typedef struct {
    uint16_t year;                                                               /*!< RTC year value: 0x0 - 0x99(BCD format) */
    uint8_t month;                                                              /*!< RTC month value */
    uint8_t day;                                                               /*!< RTC date value: 0x1 - 0x31(BCD format) */
    uint8_t hour;                                                               /*!< RTC hour value */
    uint8_t minute;                                                             /*!< RTC minute value: 0x0 - 0x59(BCD format) */
    uint8_t second;                                                   /*!< RTC time notation */
} rtc_para_struct;


const uint8_t month_table[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
rtc_para_struct rtc_para;
uint32_t seccount;
lv_obj_t *font_label;
lv_obj_t *font_label_year;
lv_obj_t *calendar;

lv_obj_t *tv_main;       /* main page */
lv_obj_t *img;
lv_obj_t *obj_main;
lv_obj_t *obj_performance;
lv_obj_t *imgbtn_widget;
lv_obj_t *imgbtn_music;
lv_obj_t *imgbtn_speed;
lv_obj_t *imgbtn_about;
lv_obj_t *imgbtn_qrcode;
lv_obj_t *imgbtn_file;
lv_obj_t *imgbtn_picture;
lv_obj_t *imgbtn_setting;

lv_obj_t *cpu_chart;
lv_obj_t *fps_chart;
lv_chart_series_t *cpu_chart_series;
lv_chart_series_t *fps_chart_series;

lv_obj_t *led_main;
lv_obj_t *led_performance;

/* Button labels for language switching */
lv_obj_t *label_widget;
lv_obj_t *label_music;
lv_obj_t *label_speed;
lv_obj_t *label_picture;
lv_obj_t *label_about;
lv_obj_t *label_qrcode;
lv_obj_t *label_file;
lv_obj_t *label_setting;

uint32_t g_fps;
uint32_t g_cpu;
lv_mem_monitor_t g_mon;

/* Performance page sampling. Both charts must use the same point count,
 * otherwise the two curves cover a different time span. */
#define CHART_UPDATE_PERIOD 500
#define CHART_POINT_CNT     20      /* 20 * 500ms = 10s of history */
#define CHART_FPS_MAX       60      /* the panel is driven at 60Hz, nothing above that is possible */

static lv_timer_t *chart_timer;
static lv_timer_t *meter_timer;
static lv_timer_t *rtc_page_timer;
static bool return_home_pending;

extern lv_obj_t *music_cont;
extern lv_obj_t *list;
extern uint16_t back_to_home;

extern lv_obj_t * tv;

extern float g_temperature;
extern void update_main_page_language(void);
extern void update_status_labels(void);
extern void lv_demo_setting_reset_refs(void);
extern void lv_demo_qrcode(lv_obj_t *parent);
extern void lv_demo_setting(lv_obj_t * parent);
extern void lv_demo_picture(lv_obj_t * parent);
void lv_demo_app(void);

static void app_stop_page_timers(void)
{
    if(chart_timer != NULL) {
        lv_timer_del(chart_timer);
        chart_timer = NULL;
    }
    if(meter_timer != NULL) {
        lv_timer_del(meter_timer);
        meter_timer = NULL;
    }
    /* rtc_page_timer is deliberately kept alive so that the clock keeps
     * counting while benchmark/music owns the screen. Only drop the label
     * pointers, they are about to be deleted by lv_obj_clean(). */
    font_label = NULL;
    font_label_year = NULL;
}

static void return_home_async(void *data)
{
    LV_UNUSED(data);
    return_home_pending = false;
    app_stop_page_timers();
    /* The settings/about pages sit on the active screen too; their cached label
     * pointers must not survive the clean. */
    lv_demo_setting_reset_refs();
    lv_obj_clean(lv_scr_act());
    back_to_home = 1;
#if LV_USE_DEMO_MUSIC
    lv_demo_music(0);
#endif
    lv_demo_app();
}

void lv_demo_app_return_home(void)
{
    if(!return_home_pending) {
        return_home_pending = true;
        lv_async_call(return_home_async, NULL);
    }
}

/* Language switching function */
extern void update_main_page_language(void);
/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/
bool lv_img_bin_load(const char *path, uint8_t *buf, uint32_t expected_size)
{
if(!path || !buf || expected_size == 0) return false;

    lv_fs_file_t f;
    lv_fs_res_t res = lv_fs_open(&f, path, LV_FS_MODE_RD);
    if(res != LV_FS_RES_OK) return false;

    /* Get total file size first */
    uint32_t size_total = 0;
    res = lv_fs_seek(&f, 0, LV_FS_SEEK_END);
    if(res != LV_FS_RES_OK) { lv_fs_close(&f); return false; }

    res = lv_fs_tell(&f, &size_total);
    if(res != LV_FS_RES_OK) { lv_fs_close(&f); return false; }

    /* Move to payload start (skip 4 bytes header if present) */
    const uint32_t skip = 4;
    if(size_total < skip) { lv_fs_close(&f); return false; }
    uint32_t size = size_total - skip;

    res = lv_fs_seek(&f, (int32_t)skip, LV_FS_SEEK_SET);
    if(res != LV_FS_RES_OK) { lv_fs_close(&f); return false; }

    if(size > expected_size) {
        /* Buffer too small for file */
        lv_fs_close(&f);
        return false;
    }

    uint32_t read_total = 0;
    while(read_total < size) {
        uint32_t to_read = size - read_total;
        if(to_read > 1024) to_read = 1024; /* chunk read */
        uint32_t br = 0;
        res = lv_fs_read(&f, buf + read_total, to_read, &br);
        if(res != LV_FS_RES_OK) { lv_fs_close(&f); return false; }
        if(br == 0) break; /* EOF */
        read_total += br;
    }

    lv_fs_close(&f);
    return read_total == size;
}

static void widget_event_cb(lv_event_t *e)
{
    back_to_home = 0;
    lv_demo_widgets();
    lv_obj_clear_flag(tv, LV_OBJ_FLAG_HIDDEN);

    lv_obj_add_flag(led_main, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(led_performance, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(tv_main, LV_OBJ_FLAG_HIDDEN);
    
////    lv_obj_invalidate(obj_main);
}

static void music_event_cb(lv_event_t *e)
{
    LV_UNUSED(e);
    back_to_home = 0;
    lv_demo_music(1);
     
//    lv_obj_add_flag(obj_main, LV_OBJ_FLAG_HIDDEN);
//    lv_obj_add_flag(obj_performance, LV_OBJ_FLAG_HIDDEN);
////    lv_obj_invalidate(obj_main);
    lv_obj_add_flag(led_main, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(led_performance, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(tv_main, LV_OBJ_FLAG_HIDDEN);
}

static void speed_event_cb(lv_event_t *e)
{
    LV_UNUSED(e);
    back_to_home = 0;

    /* benchmark 会清空活动屏幕；先停止所有仍访问主页对象的 timer。 */
    app_stop_page_timers();
    lv_demo_benchmark_set_exit_cb(lv_demo_app_return_home);
    lv_demo_benchmark();
}

static void qrcode_event_cb(lv_event_t *e)
{
    lv_obj_add_flag(led_main, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(led_performance, LV_OBJ_FLAG_HIDDEN);
    lv_demo_qrcode(lv_event_get_user_data(e));
    lv_obj_add_flag(obj_performance, LV_OBJ_FLAG_HIDDEN);
}

static void setting_event_cb(lv_event_t *e)
{
    lv_demo_setting(lv_event_get_user_data(e));
    lv_obj_add_flag(obj_performance, LV_OBJ_FLAG_HIDDEN);
}

static void picture_event_cb(lv_event_t *e)
{
    lv_demo_picture(lv_event_get_user_data(e));
    lv_obj_add_flag(obj_performance, LV_OBJ_FLAG_HIDDEN);
}
 
static void close_event_cb(lv_event_t *e)
{
    back_to_home = 1;
    lv_obj_clear_state(imgbtn_music, LV_STATE_DISABLED);
    lv_obj_clear_state(imgbtn_speed, LV_STATE_DISABLED);
    lv_obj_clear_state(imgbtn_about, LV_STATE_DISABLED);
    lv_obj_clear_state(imgbtn_widget, LV_STATE_DISABLED);
//    lv_obj_add_flag(obj_about, LV_OBJ_FLAG_HIDDEN);
    lv_obj_del(lv_event_get_user_data(e));
}

/* true while music/widgets/benchmark owns the screen: the main page widgets are
 * hidden but still alive, and touching them from a timer makes lvgl re-run the
 * layout and call lv_obj_readjust_scroll() on the screen, which drags the
 * scroll position of the demo back to the top. */
static bool app_page_is_covered(void)
{
    return (tv_main == NULL) || lv_obj_has_flag(tv_main, LV_OBJ_FLAG_HIDDEN);
}

static void update_chart(lv_timer_t *timer)
{
    LV_UNUSED(timer);

    if(cpu_chart == NULL || fps_chart == NULL) return;
    if(app_page_is_covered()) return;

    g_cpu = 100U - lv_timer_get_idle();
    g_fps = lv_port_disp_get_fps();
    lv_chart_set_next_value(cpu_chart, cpu_chart_series, g_cpu > 100U ? 100 : (lv_coord_t)g_cpu);
    lv_chart_set_next_value(fps_chart, fps_chart_series,
                            g_fps > CHART_FPS_MAX ? CHART_FPS_MAX : (lv_coord_t)g_fps);
}

static void calendar_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_current_target(e);

    if(code == LV_EVENT_VALUE_CHANGED) {
        lv_calendar_date_t date;
        if(lv_calendar_get_pressed_date(obj, &date)) {

            rtc_para.year = date.year;
            rtc_para.month = date.month;
            rtc_para.day = date.day;
            lv_label_set_text_fmt(font_label_year, "#FFFFFF %d/%02d/%02d", rtc_para.year, rtc_para.month, rtc_para.day);

            lv_obj_del(calendar);
            calendar = NULL;
            lv_obj_clear_flag(lv_layer_top(), LV_OBJ_FLAG_CLICKABLE);
            lv_obj_set_style_bg_opa(lv_layer_top(), LV_OPA_TRANSP, 0);
        }

    }
}

static void year_label_evevt_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_current_target(e);

    if(code == LV_EVENT_CLICKED) {
        lv_obj_add_flag(lv_layer_top(), LV_OBJ_FLAG_CLICKABLE);
        calendar = lv_calendar_create(lv_layer_top());
        lv_obj_set_style_bg_opa(lv_layer_top(), LV_OPA_50, 0);
        lv_obj_set_style_bg_color(lv_layer_top(), lv_palette_main(LV_PALETTE_GREY), 0);

        lv_obj_set_size(calendar, SCALE_POS_X(400), SCALE_POS_Y(400));
        lv_obj_align_to(calendar, font_label_year, LV_ALIGN_LEFT_MID, SCALE_POS_X(0), SCALE_POS_Y(0));
        static lv_calendar_date_t highlighted_days[1];       /*Only its pointer will be saved so should be static*/
        highlighted_days[0].year = rtc_para.year;
        highlighted_days[0].month = rtc_para.month;
        highlighted_days[0].day = rtc_para.day;
        lv_calendar_set_highlighted_dates(calendar, highlighted_days, 1);

        lv_calendar_set_showed_date(calendar, rtc_para.year, rtc_para.month);
        lv_calendar_header_dropdown_create(calendar);
        lv_calendar_header_arrow_create(calendar);
        lv_obj_add_event_cb(calendar, calendar_event_cb, LV_EVENT_ALL, NULL);
    }
}



uint8_t is_leap_year(uint16_t year)
{
    if((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
        return 1;
    } else {
        return 0;
    }
}

uint32_t rtc_seccount_get(rtc_para_struct *rtc_initpara_struct)
{
    uint16_t t;
    uint32_t sec = 0;

    if(rtc_initpara_struct->year < 1970 || rtc_initpara_struct->year > 2099) {
        return 1;
    }

    for(t = 1970; t < rtc_initpara_struct->year; t++) {
        if(is_leap_year(t)) {
            sec += 31622400;
        } else {
            sec += 31536000;
        }
    }
    //rtc_initpara_struct->month -= 1;

    for(t = 0; t < rtc_initpara_struct->month - 1; t++) {
        sec += (uint32_t)month_table[t] * 86400;
        if(is_leap_year(rtc_initpara_struct->year) && t == 1) {
            sec += 86400;
        }
    }

    sec += (uint32_t)(rtc_initpara_struct->day - 1) * 86400;
    sec += (uint32_t)rtc_initpara_struct->hour * 3600;
    sec += (uint32_t)rtc_initpara_struct->minute * 60;
    sec += rtc_initpara_struct->second;
    //rtc_counter_set(sec);
    return sec;
}

void rtc_time_get(uint32_t timevar, rtc_para_struct *rtc_initpara_struct)
{
    static uint16_t daycnt = 0;
    uint32_t temp = 0;
    uint16_t temp1 = 0;
    temp = timevar / 86400;

    if(daycnt != temp) {
        daycnt = temp;
        temp1 = 1970;

        while(temp >= 365) {
            if(is_leap_year(temp1)) {
                if(temp >= 366) {
                    temp -= 366;
                } else {
                    break;
                }
            } else {
                temp -= 365;
            }
            temp1++;
        }

        rtc_initpara_struct->year = temp1;
        temp1 = 0;

        while(temp >= 28) {
            if(is_leap_year(rtc_initpara_struct->year) && temp1 == 1) {
                if(temp >= 29) {
                    temp -= 29;
                } else {
                    break;
                }
            } else {
                if(temp >= month_table[temp1]) {
                    temp -= month_table[temp1];
                } else {
                    break;
                }
            }
            temp1++;
        }
        rtc_initpara_struct->month = temp1 + 1;
        rtc_initpara_struct->day = temp + 1;
    }

    temp = timevar % 86400;
    rtc_initpara_struct->hour = temp / 3600;
    rtc_initpara_struct->minute = (temp % 3600) / 60;
    rtc_initpara_struct->second = (temp % 3600) % 60;
}

static void rtc_timer(lv_timer_t *timer)
{
    seccount = rtc_seccount_get(&rtc_para);
    seccount++;
    rtc_time_get(seccount, &rtc_para);

    /* The labels only exist while the home page is built; the timer itself
     * keeps running so no time is lost in benchmark/music. */
    if(font_label != NULL) {
        lv_label_set_text_fmt(font_label, "#FFFFFF %02d:%02d:%02d", rtc_para.hour, rtc_para.minute, rtc_para.second);
    }
    if(font_label_year != NULL) {
        lv_label_set_text_fmt(font_label_year, "#FFFFFF %d/%02d/%02d", rtc_para.year, rtc_para.month, rtc_para.day);
    }
}
static lv_obj_t *mem_meter;

static void mem_set_value(void *indic, int32_t v)
{
    lv_meter_set_indicator_end_value(mem_meter, indic, v);
}

static lv_obj_t *temp_mem_meter;
static lv_meter_indicator_t *indic_temp;
lv_obj_t * label_temp;

static void tv_main_event_cb(lv_event_t *e)
{
    if(lv_event_get_code(e) == LV_EVENT_VALUE_CHANGED) {
        if(0 == lv_tabview_get_tab_act(tv_main)) {
            lv_led_on(led_main);
            lv_led_off(led_performance);
            back_to_home = 1;
        } else if(1 == lv_tabview_get_tab_act(tv_main)) {
            lv_led_off(led_main);
            lv_led_on(led_performance);
        }
    }
}

//lv_event_code_t testtt;

static lv_meter_indicator_t * indic_total_size;
lv_obj_t * label_total_size;
static lv_meter_indicator_t * indic_mem_used;
lv_obj_t * label_mem_used;
static lv_meter_indicator_t * indic_frag;
lv_obj_t * label_frag;
//static void scroll_begin_event(lv_event_t * e)
//{
//    /*Disable the scroll animations. Triggered when a tab button is clicked */
////    if(lv_event_get_code(e) == LV_EVENT_SCROLL_BEGIN) {
//    testtt = lv_event_get_code(e);
//    if( LV_EVENT_DRAW_POST_END == lv_event_get_code(e))
//        back_to_home = 0;
////    }
//}

//lv_event_code_t testtt2;
//static void scroll_begin_event2(lv_event_t * e)
//{
//    /*Disable the scroll animations. Triggered when a tab button is clicked */
////    if(lv_event_get_code(e) == LV_EVENT_SCROLL_BEGIN) {
//    testtt2 = lv_event_get_code(e);
//    if( LV_EVENT_DRAW_POST_END == lv_event_get_code(e))
//        back_to_home = 1;
////    }
//}


static void update_meter_box(lv_timer_t *timer)
{
    LV_UNUSED(timer);

    if(mem_meter == NULL || temp_mem_meter == NULL) return;
    if(app_page_is_covered()) return;

    lv_mem_monitor(&g_mon);
    lv_meter_set_indicator_end_value(mem_meter, indic_total_size, 100);
    lv_meter_set_indicator_end_value(mem_meter, indic_mem_used, g_mon.used_pct);
    lv_meter_set_indicator_end_value(mem_meter, indic_frag, g_mon.frag_pct);

    /* Only refresh the values. update_main_page_language() re-applies fonts,
     * re-aligns all button labels and rebuilds the dropdown; running that from
     * a timer invalidates the whole layout every tick. */
    update_status_labels();

    lv_meter_set_indicator_value(temp_mem_meter, indic_temp, (int32_t)g_temperature);
}

static void create_meter_box(lv_obj_t * parent, const char * title, const char * text1, const char * text2,
                                   const char * text3)
{
    static lv_style_t style_title;
    lv_style_init(&style_title);


    static lv_style_t style_bullet;
    lv_style_init(&style_bullet);
    lv_style_set_border_width(&style_bullet, 0);
    lv_style_set_radius(&style_bullet, LV_RADIUS_CIRCLE);


    lv_obj_t * cont = lv_obj_create(parent);
    lv_obj_set_height(cont, LV_SIZE_CONTENT);
    lv_obj_set_width(cont, 140);
    lv_obj_set_flex_grow(cont, 1);
    lv_obj_set_pos(cont, SCALE_POS_X(165), 500);

    lv_obj_t * bullet1 = lv_obj_create(cont);
    lv_obj_set_size(bullet1, SCALE_POS_X(13), SCALE_POS_Y(13));
    lv_obj_remove_style(bullet1, NULL, LV_PART_SCROLLBAR);
    lv_obj_add_style(bullet1, &style_bullet, 0);
    lv_obj_set_style_bg_color(bullet1, lv_palette_main(LV_PALETTE_RED), 0);
    label_total_size = lv_label_create(cont);
    lv_label_set_text(label_total_size, text1);

    lv_obj_t * bullet2 = lv_obj_create(cont);
    lv_obj_set_size(bullet2, SCALE_POS_X(13), SCALE_POS_Y(13));
    lv_obj_remove_style(bullet2, NULL, LV_PART_SCROLLBAR);
    lv_obj_add_style(bullet2, &style_bullet, 0);
    lv_obj_set_style_bg_color(bullet2, lv_palette_main(LV_PALETTE_GREEN), 0);
    label_mem_used = lv_label_create(cont);
    lv_label_set_text(label_mem_used, text2);

    lv_obj_t * bullet3 = lv_obj_create(cont);
    lv_obj_set_size(bullet3, SCALE_POS_X(13), SCALE_POS_Y(13));
    lv_obj_remove_style(bullet3,  NULL, LV_PART_SCROLLBAR);
    lv_obj_add_style(bullet3, &style_bullet, 0);
    lv_obj_set_style_bg_color(bullet3, lv_palette_main(LV_PALETTE_BLUE), 0);
    label_frag = lv_label_create(cont);
    lv_label_set_text(label_frag, text3);

    {
        static lv_coord_t grid_col_dsc[] = {LV_GRID_CONTENT, LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
//        static lv_coord_t grid_row_dsc[] = {LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST};
        static lv_coord_t grid_row_dsc[] = {LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST};
        lv_obj_set_grid_dsc_array(cont, grid_col_dsc, grid_row_dsc);
//        lv_obj_set_grid_cell(title_label, LV_GRID_ALIGN_START, 0, 2, LV_GRID_ALIGN_START, 0, 1);
//        lv_obj_set_grid_cell(meter, LV_GRID_ALIGN_START, 0, 2, LV_GRID_ALIGN_START, 1, 1);
        lv_obj_set_grid_cell(bullet1, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 0, 1);
        lv_obj_set_grid_cell(bullet2, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 1, 1);
        lv_obj_set_grid_cell(bullet3, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 2, 1);
        lv_obj_set_grid_cell(label_total_size, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_START, 0, 1);
        lv_obj_set_grid_cell(label_mem_used, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_START, 1, 1);
        lv_obj_set_grid_cell(label_frag, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_START, 2, 1);
    }

}

static void create_temp_box(lv_obj_t * parent, const char * text1)
{
    static lv_style_t style_title;
    lv_style_init(&style_title);


    static lv_style_t style_bullet;
    lv_style_init(&style_bullet);
    lv_style_set_border_width(&style_bullet, 0);
    lv_style_set_radius(&style_bullet, LV_RADIUS_CIRCLE);


    lv_obj_t * cont = lv_obj_create(parent);
    lv_obj_set_height(cont, LV_SIZE_CONTENT);
    lv_obj_set_width(cont, 140);
    lv_obj_set_flex_grow(cont, 1);
    lv_obj_set_pos(cont, SCALE_POS_X(165+465), 500);

    lv_obj_t * bullet1 = lv_obj_create(cont);
    lv_obj_set_size(bullet1, SCALE_POS_X(13), SCALE_POS_Y(13));
    lv_obj_remove_style(bullet1, NULL, LV_PART_SCROLLBAR);
    lv_obj_add_style(bullet1, &style_bullet, 0);
    lv_obj_set_style_bg_color(bullet1, lv_palette_main(LV_PALETTE_GREY), 0);
    label_temp = lv_label_create(cont);
    lv_label_set_text(label_temp, text1);

    {
        static lv_coord_t grid_col_dsc[] = {LV_GRID_CONTENT, LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
//        static lv_coord_t grid_row_dsc[] = {LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST};
        static lv_coord_t grid_row_dsc[] = {LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST};
        lv_obj_set_grid_dsc_array(cont, grid_col_dsc, grid_row_dsc);
//        lv_obj_set_grid_cell(title_label, LV_GRID_ALIGN_START, 0, 2, LV_GRID_ALIGN_START, 0, 1);
//        lv_obj_set_grid_cell(meter, LV_GRID_ALIGN_START, 0, 2, LV_GRID_ALIGN_START, 1, 1);
        lv_obj_set_grid_cell(bullet1, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 0, 1);
        lv_obj_set_grid_cell(label_temp, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_START, 0, 1);
    }

}
/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_demo_app(void)
{
//    static lv_obj_t *tv_main;       /* main page */
    tv_main = lv_tabview_create(lv_scr_act(), LV_DIR_TOP, 0);
    lv_obj_add_event_cb(tv_main, tv_main_event_cb, LV_EVENT_VALUE_CHANGED, NULL);

    /*Create a LED and switch it OFF*/
    led_main  = lv_led_create(lv_scr_act());
    lv_obj_set_size(led_main, 10, 10);
    lv_obj_align(led_main, LV_ALIGN_CENTER, SCALE_POS_X(-30), SCALE_POS_Y(250));
    lv_led_on(led_main);
//    lv_obj_add_event_cb(led_main, scroll_begin_event2, LV_EVENT_ALL, NULL);

    /*Copy the previous LED and set a brightness*/
    led_performance  = lv_led_create(lv_scr_act());
    lv_obj_set_size(led_performance, 10, 10);
    lv_obj_align(led_performance, LV_ALIGN_CENTER, SCALE_POS_X(30), SCALE_POS_Y(250));
//    lv_led_set_color(led2, lv_palette_main(LV_PALETTE_LIGHT_GREEN));
    lv_led_off(led_performance);
//    lv_obj_add_event_cb(led_performance, scroll_begin_event, LV_EVENT_ALL, NULL);


    obj_main = lv_tabview_add_tab(tv_main, "Main page");
    obj_performance = lv_tabview_add_tab(tv_main, "Performance");

    lv_obj_set_width(obj_main, LV_HOR_RES_MAX);
    lv_obj_set_height(obj_main, LV_VER_RES_MAX);

    lv_obj_set_scrollbar_mode(obj_main, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_pad_top(obj_main, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_bottom(obj_main, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_left(obj_main, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_right(obj_main, 0, LV_PART_MAIN);
    lv_obj_set_style_border_width(obj_main, 0, LV_PART_MAIN);

    lv_obj_set_width(obj_performance, LV_HOR_RES_MAX);
    lv_obj_set_height(obj_performance, LV_VER_RES_MAX);

    lv_obj_clear_flag(obj_main, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_scrollbar_mode(obj_performance, LV_SCROLLBAR_MODE_OFF);
    /* the performance page is taller than the panel, but it must never scroll
     * horizontally - that fights with the tabview and ends up showing an empty
     * area next to the tabs */
    lv_obj_set_scroll_dir(obj_performance, LV_DIR_VER);
    lv_obj_set_style_pad_top(obj_performance, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_bottom(obj_performance, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_left(obj_performance, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_right(obj_performance, 0, LV_PART_MAIN);
    lv_obj_set_style_border_width(obj_performance, 0, LV_PART_MAIN);

    img = lv_img_create(obj_main);
    lv_img_set_src(img, &img_background);

    imgbtn_widget = lv_imgbtn_create(obj_main);
    lv_imgbtn_set_src(imgbtn_widget, LV_IMGBTN_STATE_PRESSED, &img_widget, NULL, NULL);
    lv_imgbtn_set_src(imgbtn_widget, LV_IMGBTN_STATE_RELEASED, &img_widget, NULL, NULL);
    APP_ICON_BTN_SIZE(imgbtn_widget);
    lv_obj_set_pos(imgbtn_widget, SCALE_POS_X(390), SCALE_POS_Y(207));

    imgbtn_music = lv_imgbtn_create(obj_main);
    lv_imgbtn_set_src(imgbtn_music, LV_IMGBTN_STATE_PRESSED, &img_music, NULL, NULL);
    lv_imgbtn_set_src(imgbtn_music, LV_IMGBTN_STATE_RELEASED, &img_music, NULL, NULL);
    APP_ICON_BTN_SIZE(imgbtn_music);
    lv_obj_set_pos(imgbtn_music, SCALE_POS_X(524), SCALE_POS_Y(207));

    imgbtn_speed = lv_imgbtn_create(obj_main);
    lv_imgbtn_set_src(imgbtn_speed, LV_IMGBTN_STATE_PRESSED, &img_speed, NULL, NULL);
    lv_imgbtn_set_src(imgbtn_speed, LV_IMGBTN_STATE_RELEASED, &img_speed, NULL, NULL);
    APP_ICON_BTN_SIZE(imgbtn_speed);
    lv_obj_set_pos(imgbtn_speed, SCALE_POS_X(657), SCALE_POS_Y(207));

    imgbtn_picture = lv_imgbtn_create(obj_main);
    lv_imgbtn_set_src(imgbtn_picture, LV_IMGBTN_STATE_PRESSED, &img_picture, NULL, NULL);
    lv_imgbtn_set_src(imgbtn_picture, LV_IMGBTN_STATE_RELEASED, &img_picture, NULL, NULL);
    APP_ICON_BTN_SIZE(imgbtn_picture);
    lv_obj_set_pos(imgbtn_picture, SCALE_POS_X(783), SCALE_POS_Y(207));

    imgbtn_about = lv_imgbtn_create(obj_main);
    lv_imgbtn_set_src(imgbtn_about, LV_IMGBTN_STATE_PRESSED, &img_about, NULL, NULL);
    lv_imgbtn_set_src(imgbtn_about, LV_IMGBTN_STATE_RELEASED, &img_about, NULL, NULL);
    APP_ICON_BTN_SIZE(imgbtn_about);
    lv_obj_set_pos(imgbtn_about, SCALE_POS_X(390), SCALE_POS_Y(350));

    imgbtn_qrcode = lv_imgbtn_create(obj_main);
    lv_imgbtn_set_src(imgbtn_qrcode, LV_IMGBTN_STATE_PRESSED, &img_qrcode, NULL, NULL);
    lv_imgbtn_set_src(imgbtn_qrcode, LV_IMGBTN_STATE_RELEASED, &img_qrcode, NULL, NULL);
    APP_ICON_BTN_SIZE(imgbtn_qrcode);
    lv_obj_set_pos(imgbtn_qrcode, SCALE_POS_X(524), SCALE_POS_Y(350));

    imgbtn_file = lv_imgbtn_create(obj_main);
    lv_imgbtn_set_src(imgbtn_file, LV_IMGBTN_STATE_PRESSED, &img_file, NULL, NULL);
    lv_imgbtn_set_src(imgbtn_file, LV_IMGBTN_STATE_RELEASED, &img_file, NULL, NULL);
    APP_ICON_BTN_SIZE(imgbtn_file);
    lv_obj_set_pos(imgbtn_file, SCALE_POS_X(657), SCALE_POS_Y(350));

    imgbtn_setting = lv_imgbtn_create(obj_main);
    lv_imgbtn_set_src(imgbtn_setting, LV_IMGBTN_STATE_PRESSED, &img_setting, NULL, NULL);
    lv_imgbtn_set_src(imgbtn_setting, LV_IMGBTN_STATE_RELEASED, &img_setting, NULL, NULL);
    APP_ICON_BTN_SIZE(imgbtn_setting);
    lv_obj_set_pos(imgbtn_setting, SCALE_POS_X(783), SCALE_POS_Y(350));

    // /* Add labels under each button */
    label_widget = lv_label_create(obj_main);
    lv_label_set_text(label_widget, "Widget");
    lv_obj_set_style_text_font(label_widget, &lv_font_montserrat_10, LV_STATE_DEFAULT);
    lv_obj_align_to(label_widget, imgbtn_widget, LV_ALIGN_OUT_BOTTOM_MID, APP_ICON_LABEL_OFS_X, APP_ICON_LABEL_OFS_Y);


    label_music = lv_label_create(obj_main);
    lv_label_set_text(label_music, "Music");
    lv_obj_set_style_text_font(label_music, &lv_font_montserrat_10, LV_STATE_DEFAULT);
    lv_obj_align_to(label_music, imgbtn_music, LV_ALIGN_OUT_BOTTOM_MID, APP_ICON_LABEL_OFS_X, APP_ICON_LABEL_OFS_Y);

    label_speed = lv_label_create(obj_main);
    lv_label_set_text(label_speed, "Benchmark");
    lv_obj_set_style_text_font(label_speed, &lv_font_montserrat_10, LV_STATE_DEFAULT);
    lv_obj_align_to(label_speed, imgbtn_speed, LV_ALIGN_OUT_BOTTOM_MID, APP_ICON_LABEL_OFS_X, APP_ICON_LABEL_OFS_Y);

    label_picture = lv_label_create(obj_main);
    lv_label_set_text(label_picture, "Picture");
    lv_obj_set_style_text_font(label_picture, &lv_font_montserrat_10, LV_STATE_DEFAULT);
    lv_obj_align_to(label_picture, imgbtn_picture, LV_ALIGN_OUT_BOTTOM_MID, APP_ICON_LABEL_OFS_X, APP_ICON_LABEL_OFS_Y);

    label_about = lv_label_create(obj_main);
    lv_label_set_text(label_about, "About");
    lv_obj_set_style_text_font(label_about, &lv_font_montserrat_10, LV_STATE_DEFAULT);
    lv_obj_align_to(label_about, imgbtn_about, LV_ALIGN_OUT_BOTTOM_MID, APP_ICON_LABEL_OFS_X, APP_ICON_LABEL_OFS_Y);

    label_qrcode = lv_label_create(obj_main);
    lv_label_set_text(label_qrcode, "QRCode");
    lv_obj_set_style_text_font(label_qrcode, &lv_font_montserrat_10, LV_STATE_DEFAULT);
    lv_obj_align_to(label_qrcode, imgbtn_qrcode, LV_ALIGN_OUT_BOTTOM_MID, APP_ICON_LABEL_OFS_X, APP_ICON_LABEL_OFS_Y);

    label_file = lv_label_create(obj_main);
    lv_label_set_text(label_file, "File");
    lv_obj_set_style_text_font(label_file, &lv_font_montserrat_10, LV_STATE_DEFAULT);
    lv_obj_align_to(label_file, imgbtn_file, LV_ALIGN_OUT_BOTTOM_MID, APP_ICON_LABEL_OFS_X, APP_ICON_LABEL_OFS_Y);

    label_setting = lv_label_create(obj_main);
    lv_label_set_text(label_setting, "Setting");
    lv_obj_set_style_text_font(label_setting, &lv_font_montserrat_10, LV_STATE_DEFAULT);
    lv_obj_align_to(label_setting, imgbtn_setting, LV_ALIGN_OUT_BOTTOM_MID, APP_ICON_LABEL_OFS_X, APP_ICON_LABEL_OFS_Y);

    //Write codes CPU usage chart
    cpu_chart = lv_chart_create(obj_performance);

    lv_obj_set_pos(cpu_chart, SCALE_POS_X(7 + 75), 19);
    lv_obj_set_size(cpu_chart, SCALE_POS_X(1004 - 75), 155);
    lv_chart_set_type(cpu_chart, LV_CHART_TYPE_LINE);
    lv_chart_set_range(cpu_chart, LV_CHART_AXIS_PRIMARY_Y, 0, 100);
    lv_chart_set_div_line_count(cpu_chart, 6, 12);
    cpu_chart_series = lv_chart_add_series(cpu_chart, lv_color_make(0x50, 0x0A, 0x88), LV_CHART_AXIS_PRIMARY_Y);
    lv_chart_set_axis_tick(cpu_chart, LV_CHART_AXIS_PRIMARY_Y, 10, 5, 6, 5, true, 30);
    lv_chart_set_point_count(cpu_chart, CHART_POINT_CNT);

    //Write codes FPS chart
    fps_chart = lv_chart_create(obj_performance);

    lv_obj_set_pos(fps_chart, SCALE_POS_X(7 + 75), 198);
    lv_obj_set_size(fps_chart, SCALE_POS_X(1004 - 75), 155);
    lv_chart_set_type(fps_chart, LV_CHART_TYPE_LINE);
    /* The series and the tick marks have to sit on the same axis, otherwise the
     * scale printed on the left has nothing to do with the drawn curve. */
    lv_chart_set_range(fps_chart, LV_CHART_AXIS_PRIMARY_Y, 0, CHART_FPS_MAX);
    lv_chart_set_div_line_count(fps_chart, 6, 12);
    fps_chart_series = lv_chart_add_series(fps_chart, lv_palette_main(LV_PALETTE_ORANGE), LV_CHART_AXIS_PRIMARY_Y);
    lv_chart_set_axis_tick(fps_chart, LV_CHART_AXIS_PRIMARY_Y, 10, 5, 6, 5, true, 30);
    /* Same number of points as the CPU chart so both cover the same time span. */
    lv_chart_set_point_count(fps_chart, CHART_POINT_CNT);

    //Write codes CPU usage label
    lv_obj_t *cpu_label = lv_label_create(obj_performance);
    lv_obj_set_style_text_font(cpu_label, LV_FONT_DEFAULT, LV_STATE_DEFAULT);
    lv_label_set_recolor(cpu_label, true);                      /*Enable re-coloring by commands in the text*/
    lv_label_set_text(cpu_label, "#F12F12 CPU usage(%)");

    lv_label_set_long_mode(cpu_label, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(cpu_label, SCALE_POS_X(21), 2);
    //Write codes FPS label
    lv_obj_t *fps_label = lv_label_create(obj_performance);
    lv_obj_set_style_text_font(fps_label, LV_FONT_DEFAULT, LV_STATE_DEFAULT);
    lv_label_set_recolor(fps_label, true);                      /*Enable re-coloring by commands in the text*/
    lv_label_set_text(fps_label, "#F12F12 FPS");

    lv_label_set_long_mode(fps_label, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(fps_label, SCALE_POS_X(21), 182);

    chart_timer = lv_timer_create(update_chart, CHART_UPDATE_PERIOD, NULL);

    /* 内存表盘 */
    mem_meter = lv_meter_create(obj_performance);
    lv_obj_set_pos(mem_meter, SCALE_POS_X(130), 357);
    lv_obj_set_size(mem_meter, SCALE_POS_X(300), SCALE_POS_X(300));

    /* Make tick lines closer to the dial rim: remove default padding/border so the meter content radius grows */
    lv_obj_set_style_pad_all(mem_meter, 10, LV_PART_MAIN);
    lv_obj_set_style_border_width(mem_meter, 0, LV_PART_MAIN);

    /*Remove the circle from the middle*/
    lv_obj_remove_style(mem_meter, NULL, LV_PART_INDICATOR);

    /*Add a scale first*/
    lv_meter_scale_t *mem_scale = lv_meter_add_scale(mem_meter);
    lv_meter_set_scale_ticks(mem_meter, mem_scale, 6, 2, 10, lv_palette_main(LV_PALETTE_GREY));
    lv_meter_set_scale_major_ticks(mem_meter, mem_scale, 1, 2, 10, lv_color_hex3(0xeee), 15);
    lv_meter_set_scale_range(mem_meter, mem_scale, 0, 100, 270, 90);

    /*Add a three arc indicator*/
    indic_total_size = lv_meter_add_arc(mem_meter, mem_scale, 5, lv_palette_main(LV_PALETTE_RED), 0);
    indic_mem_used = lv_meter_add_arc(mem_meter, mem_scale, 5, lv_palette_main(LV_PALETTE_GREEN), -5);
    indic_frag = lv_meter_add_arc(mem_meter, mem_scale, 5, lv_palette_main(LV_PALETTE_BLUE), -10);

    /*Create an animation to set the value*/
    lv_anim_t a_mem;
    lv_anim_init(&a_mem);
//    lv_anim_set_exec_cb(&a_mem, mem_set_value);
    lv_anim_set_values(&a_mem, 0, 100);
    lv_anim_set_repeat_delay(&a_mem, 100);
    lv_anim_set_playback_delay(&a_mem, 100);
    lv_anim_set_repeat_count(&a_mem, LV_ANIM_REPEAT_INFINITE);

    lv_anim_set_time(&a_mem, 2000);
    lv_anim_set_playback_time(&a_mem, 500);
    lv_anim_set_var(&a_mem, indic_total_size);
    lv_anim_start(&a_mem);

    lv_anim_set_time(&a_mem, 1000);
    lv_anim_set_playback_time(&a_mem, 1000);
    lv_anim_set_var(&a_mem, indic_mem_used);
    lv_anim_start(&a_mem);

    lv_anim_set_time(&a_mem, 1000);
    lv_anim_set_playback_time(&a_mem, 2000);
    lv_anim_set_var(&a_mem, indic_frag);
    lv_anim_start(&a_mem);

    lv_meter_set_indicator_end_value(mem_meter, indic_total_size, 100);
    lv_meter_set_indicator_end_value(mem_meter, indic_mem_used, 20);
    lv_meter_set_indicator_end_value(mem_meter, indic_frag, 80);

    /* 温度表盘 */
    temp_mem_meter = lv_meter_create(obj_performance);
    lv_obj_set_pos(temp_mem_meter, SCALE_POS_X(588), 357);
    lv_obj_set_size(temp_mem_meter, SCALE_POS_X(300), SCALE_POS_X(300));
    lv_obj_set_style_width(temp_mem_meter, 5, LV_PART_INDICATOR);
    lv_obj_set_style_height(temp_mem_meter, 5, LV_PART_INDICATOR);
    lv_obj_set_style_radius(temp_mem_meter, LV_RADIUS_CIRCLE, LV_PART_INDICATOR);  /* 保持圆形 */

    /* Make tick lines closer to the dial rim: remove default padding/border so the meter content radius grows */
    lv_obj_set_style_pad_all(temp_mem_meter, 10, LV_PART_MAIN);
    lv_obj_set_style_border_width(temp_mem_meter, 0, LV_PART_MAIN);

    /*Add a scale first*/
    lv_meter_scale_t *temp_scale = lv_meter_add_scale(temp_mem_meter);
    lv_meter_set_scale_ticks(temp_mem_meter, temp_scale, 31, 1, 6, lv_palette_main(LV_PALETTE_GREY));
    lv_meter_set_scale_major_ticks(temp_mem_meter, temp_scale, 5, 2, 15, lv_color_black(), 5);
    lv_meter_set_scale_range(temp_mem_meter, temp_scale, -40, 105, 270, 180 - 45);

    /*Add a blue arc to the start*/
    indic_temp = lv_meter_add_arc(temp_mem_meter, temp_scale, 1, lv_palette_main(LV_PALETTE_BLUE), 0);
    lv_meter_set_indicator_start_value(temp_mem_meter, indic_temp, -40);
    lv_meter_set_indicator_end_value(temp_mem_meter, indic_temp, 10);

    /*Make the tick lines blue at the start of the scale*/
    indic_temp = lv_meter_add_scale_lines(temp_mem_meter, temp_scale, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_GREY),
                                     true, 0);
    lv_meter_set_indicator_start_value(temp_mem_meter, indic_temp, -40);
    lv_meter_set_indicator_end_value(temp_mem_meter, indic_temp, 10);

    /*Add a red arc to the end*/
    indic_temp = lv_meter_add_arc(temp_mem_meter, temp_scale, 1, lv_palette_main(LV_PALETTE_RED), 0);
    lv_meter_set_indicator_start_value(temp_mem_meter, indic_temp, 50);
    lv_meter_set_indicator_end_value(temp_mem_meter, indic_temp, 105);

    /*Make the tick lines red at the end of the scale*/
    indic_temp = lv_meter_add_scale_lines(temp_mem_meter, temp_scale, lv_palette_main(LV_PALETTE_GREY), lv_palette_main(LV_PALETTE_RED), true, 0);
    lv_meter_set_indicator_start_value(temp_mem_meter, indic_temp, 50);
    lv_meter_set_indicator_end_value(temp_mem_meter, indic_temp, 105);

    /*Add a needle line indicator*/
    indic_temp = lv_meter_add_needle_line(temp_mem_meter, temp_scale, 2, lv_palette_main(LV_PALETTE_GREY), -12);

    ///////////////////////////////////////////
    lv_meter_scale_t * scale;
//    lv_meter_indicator_t * indic;
    create_meter_box(obj_performance, "Memory used", "Total size: 63%", "Memory used: 44%", "Amount of fragmentation: 58%");
    create_temp_box(obj_performance, "Temperature");
    meter_timer = lv_timer_create(update_meter_box, 500, NULL);

    /* button event add */
    lv_obj_add_event_cb(imgbtn_widget, widget_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(imgbtn_music, music_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(imgbtn_speed, speed_event_cb, LV_EVENT_RELEASED, NULL);
    lv_obj_add_event_cb(imgbtn_about, about_event_cb, LV_EVENT_CLICKED, obj_main);
    lv_obj_add_event_cb(imgbtn_qrcode, qrcode_event_cb, LV_EVENT_CLICKED, obj_main);
    lv_obj_add_event_cb(imgbtn_setting, setting_event_cb, LV_EVENT_CLICKED, obj_main);
    lv_obj_add_event_cb(imgbtn_picture, picture_event_cb, LV_EVENT_CLICKED, obj_main);
    lv_obj_add_event_cb(imgbtn_file, file_browse_event_cb, LV_EVENT_RELEASED, NULL);
    

    /* add main page timing label */
    font_label = lv_label_create(obj_main);
//    lv_obj_set_style_text_font(font_label, &lv_font_digital_35, LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(font_label, &lv_font_digital_30, LV_STATE_DEFAULT);
    lv_label_set_recolor(font_label, true);                      /*Enable re-coloring by commands in the text*/
    lv_label_set_text(font_label, "#FFFFFF 23:55");
    lv_obj_align(font_label, LV_ALIGN_LEFT_MID, SCALE_POS_X(50), SCALE_POS_Y(-40));

    font_label_year = lv_label_create(obj_main);
//    lv_obj_set_style_text_font(font_label, &lv_font_digital_35, LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(font_label_year, &lv_font_digital_20, LV_STATE_DEFAULT);
    lv_label_set_recolor(font_label_year, true);                      /*Enable re-coloring by commands in the text*/
    lv_label_set_text(font_label_year, "#FFFFFF 2024/7/4");
    lv_obj_align(font_label_year, LV_ALIGN_LEFT_MID, SCALE_POS_X(50), SCALE_POS_Y(40));
    lv_obj_add_flag(font_label_year, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(font_label_year, year_label_evevt_cb, LV_EVENT_ALL, 0);

    /* lv_demo_app() is called again every time we return from benchmark/music.
     * The clock timer is created only once and is never deleted afterwards, so
     * it keeps counting in the background and the time is not reset here. */
    if(rtc_page_timer == NULL) {
        rtc_para.year = 2026;
        rtc_para.month = 8;
        rtc_para.day = 1;
        rtc_para.hour = 10;
        rtc_para.minute = 00;
        rtc_para.second = 00;
        rtc_page_timer = lv_timer_create(rtc_timer, 1000, NULL);
    }
    /* Show the current time right away instead of the placeholder text. */
    lv_label_set_text_fmt(font_label, "#FFFFFF %02d:%02d:%02d", rtc_para.hour, rtc_para.minute, rtc_para.second);
    lv_label_set_text_fmt(font_label_year, "#FFFFFF %d/%02d/%02d", rtc_para.year, rtc_para.month, rtc_para.day);

    img = lv_img_create( obj_performance );
    lv_img_set_src(img, &img_temp);
    lv_obj_align_to(img, temp_mem_meter, LV_ALIGN_CENTER, SCALE_POS_X(0), SCALE_POS_Y(90));
    
    img = lv_img_create( obj_performance );
    lv_img_set_src(img, &img_mem);
    lv_obj_align_to(img, mem_meter, LV_ALIGN_CENTER, SCALE_POS_X(0), SCALE_POS_Y(0));
    // 1. 创建1个光标对象
    /* lv_indev_set_cursor() moves the cursor to the system layer, so it is not
     * removed by lv_obj_clean(lv_scr_act()). Creating it on every rebuild of
     * the page would leak one object per return-to-home and leave stale red
     * dots on screen, so create it only once. */
    static lv_obj_t * cursor = NULL;
    if(cursor == NULL) {
        cursor = lv_obj_create(lv_scr_act());
        lv_obj_set_size(cursor, SCALE_POS_X(20), SCALE_POS_Y(20));
        lv_obj_set_style_bg_color(cursor, lv_color_hex(0xFF0000), 0); // 红色
        lv_obj_set_style_radius(cursor, LV_RADIUS_CIRCLE, 0); // 圆形

        // 2. 获取触摸输入设备
        lv_indev_t * indev = lv_indev_get_next(NULL); // 获取第一个输入设备
        lv_indev_set_cursor(indev, cursor); // 绑定光标
    }

    /* The page is rebuilt with hard coded English texts and the Montserrat
     * font. Re-apply the selected language, otherwise returning from the
     * benchmark shows English on the main page and the periodic
     * update_status_labels() writes Chinese into Montserrat-only labels,
     * which renders as garbage on the performance page. */
    update_main_page_language();
}


#if SYS_configUSE_FAT32FS
    void lv_demo_app_fs_test(void)
    {

        lv_fs_res_t res;
        lv_fs_file_t f;
        uint8_t buf[100];
        uint8_t WriteBuffer[] = "1232This is FatFs working fine!\r\n";
        uint32_t read_num;
        uint32_t pos = 0;
        res = lv_fs_open(&f, "0:/测试.TXT",LV_FS_MODE_RD | LV_FS_MODE_WR);

        if(LV_FS_RES_OK == res){
            res = lv_fs_write(&f, WriteBuffer, sizeof(WriteBuffer), NULL);
            if(LV_FS_RES_OK == res){
                lv_fs_tell(&f, &pos);
                res = lv_fs_seek(&f, 0, LV_FS_SEEK_SET);
                lv_fs_tell(&f, &pos);
                res = lv_fs_read(&f, buf, 8, NULL);
            }
            lv_fs_tell(&f, &pos);
            lv_fs_close(&f);
            read_num = buf[0];
            read_num = buf[1];
            read_num = buf[2];
        }
    }
#endif
/**********************
 *   STATIC FUNCTIONS
 **********************/

