#include "lvgl.h"
#if LV_USE_QRCODE && LV_BUILD_EXAMPLES

static lv_obj_t * win;
static lv_obj_t * cont;
static lv_obj_t * qr;
extern lv_obj_t *obj_performance;
extern lv_obj_t *led_main;
extern lv_obj_t *led_performance;

static void close_event_handler(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_target(e);
//    LV_LOG_USER("Button %d clicked", (int)lv_obj_get_index(obj));
    lv_obj_clear_flag(led_main, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(led_performance, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(obj_performance, LV_OBJ_FLAG_HIDDEN);
    lv_obj_del(lv_event_get_user_data(e));
}

static void gen_event_handler(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_user_data(e);
    const char *txt = lv_textarea_get_text(obj);
    lv_qrcode_update(qr, txt, strlen(txt));
    lv_obj_clear_flag(qr, LV_OBJ_FLAG_HIDDEN);
}

static void ta_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * ta = lv_event_get_target(e);
    lv_obj_t * kb = lv_event_get_user_data(e);
    if(code == LV_EVENT_FOCUSED) {
        if(lv_indev_get_type(lv_indev_get_act()) != LV_INDEV_TYPE_KEYPAD) {
            lv_keyboard_set_textarea(kb, ta);
            lv_obj_set_style_max_height(kb, LV_HOR_RES , 0);
            lv_obj_update_layout(win);   /*Be sure the sizes are recalculated*/
            lv_obj_clear_flag(kb, LV_OBJ_FLAG_HIDDEN);
            lv_obj_scroll_to_view_recursive(ta, LV_ANIM_OFF);
            lv_obj_add_flag(qr, LV_OBJ_FLAG_HIDDEN);
        }

    }
    else if(code == LV_EVENT_DEFOCUSED) {
        lv_keyboard_set_textarea(kb, NULL);
        lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
        lv_indev_reset(NULL, ta);
        lv_obj_add_flag(qr, LV_OBJ_FLAG_HIDDEN);
    }
    else if(code == LV_EVENT_READY || code == LV_EVENT_CANCEL) {
        lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_state(ta, LV_STATE_FOCUSED);
        lv_indev_reset(NULL, ta);   /*To forget the last clicked object to make it focusable again*/
        lv_obj_add_flag(qr, LV_OBJ_FLAG_HIDDEN);
    }
}

/**
 * Create a QR Code
 */
void lv_demo_qrcode(lv_obj_t * parent)
{
    win = lv_win_create(parent, 40);
    lv_obj_t * btn;
    lv_obj_set_size(win, 480, 272);
    lv_obj_set_align(win, LV_ALIGN_CENTER);
    lv_obj_set_style_radius(win, 8, LV_PART_MAIN);

    lv_win_add_title(win, "QR Code test");
    btn = lv_win_add_btn(win, LV_SYMBOL_CLOSE, 40);
    lv_obj_add_event_cb(btn, close_event_handler, LV_EVENT_CLICKED, win);

    cont = lv_win_get_content(win);  /*Content can be added here*/
    lv_obj_set_scrollbar_mode(cont, LV_SCROLLBAR_MODE_OFF);

    lv_obj_t  *ta = lv_textarea_create(cont);
    lv_textarea_set_one_line(ta, true);
    lv_textarea_set_max_length(ta, 50);
    lv_textarea_set_placeholder_text(ta, "https://www.gd32mcu.com");
    lv_obj_align(ta, LV_ALIGN_CENTER, -60, -90);

    lv_obj_t *keyboard = lv_keyboard_create(cont);
    lv_obj_add_flag(keyboard, LV_OBJ_FLAG_HIDDEN);

    lv_obj_add_event_cb(ta, ta_event_cb, LV_EVENT_ALL, keyboard);

    lv_obj_t * label_gen;
    lv_obj_t * btn_gen = lv_btn_create(cont);
    lv_obj_add_event_cb(btn_gen, gen_event_handler, LV_EVENT_CLICKED, ta);
    lv_obj_align(btn_gen, LV_ALIGN_CENTER, 135, -90);
    label_gen = lv_label_create(btn_gen);
    lv_label_set_text(label_gen, "Generate");
    lv_obj_center(label_gen);

    lv_color_t bg_color = lv_palette_lighten(LV_PALETTE_LIGHT_BLUE, 5);
    lv_color_t fg_color = lv_palette_darken(LV_PALETTE_BLUE, 4);

    qr = lv_qrcode_create(cont, 120, fg_color, bg_color);

    /*Set data*/
    const char * data = "https://www.gd32mcu.com";
    lv_qrcode_update(qr, data, strlen(data));
    lv_obj_align(qr, LV_ALIGN_CENTER, 0, 30);

    /*Add a border with bg_color*/
    lv_obj_set_style_border_color(qr, bg_color, 0);
    lv_obj_set_style_border_width(qr, 5, 0);
}

#endif
