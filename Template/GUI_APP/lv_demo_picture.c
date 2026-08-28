#include "lvgl.h"

#if LV_USE_PNG && LV_USE_IMG && LV_BUILD_EXAMPLES

static lv_obj_t * win1;
static lv_obj_t * cont1;

static lv_obj_t * win_recutangle;
static lv_obj_t  *ta_pic;

static lv_obj_t * btn1;
static lv_obj_t * btn_png,* btn_gif,* btn_bmp,* btn_sjpg;
static lv_obj_t * pic_png, * pic_bmp,* pic_gif, * pic_sjpg;
static bool styles_initialized;
LV_IMG_DECLARE(img_sjpg);
LV_IMG_DECLARE(wave);
LV_IMG_DECLARE(img_bmp);
LV_IMG_DECLARE(img_png);
extern lv_obj_t *obj_performance;

static void lv_demo_png(void);
static void lv_demo_bmp(void);
static void lv_demo_gif(void);
static void lv_demo_sjpg(void);

static void hide_picture(lv_obj_t * picture)
{
    if(picture != NULL) {
        lv_obj_add_flag(picture, LV_OBJ_FLAG_HIDDEN);
    }
}

static void close_event_handler(lv_event_t * e)
{
    lv_obj_clear_flag(obj_performance, LV_OBJ_FLAG_HIDDEN);
    lv_obj_del(lv_event_get_user_data(e));

    win1 = NULL;
    cont1 = NULL;
    win_recutangle = NULL;
    ta_pic = NULL;
    btn1 = NULL;
    btn_png = NULL;
    btn_gif = NULL;
    btn_bmp = NULL;
    btn_sjpg = NULL;
    pic_png = NULL;
    pic_bmp = NULL;
    pic_gif = NULL;
    pic_sjpg = NULL;
}

static void btn_png_event_cb(lv_event_t *e)
{
    hide_picture(pic_bmp);
    hide_picture(pic_gif);
    hide_picture(pic_sjpg);
    lv_demo_png();
}

static void btn_bmp_event_cb(lv_event_t *e)
{
    hide_picture(pic_gif);
    hide_picture(pic_sjpg);
    hide_picture(pic_png);
    lv_demo_bmp();
}

static void btn_sjpg_event_cb(lv_event_t *e)
{
    hide_picture(pic_bmp);
    hide_picture(pic_gif);
    hide_picture(pic_png);
    lv_demo_sjpg();
}

static void btn_gif_event_cb(lv_event_t *e)
{
    hide_picture(pic_bmp);
    hide_picture(pic_sjpg);
    hide_picture(pic_png);
    lv_demo_gif();
}



/**
 * Decode bmp/sjpg/gif/png
 */

void lv_demo_picture(lv_obj_t * parent)
{
    /*Init the style for the default state*/
    static lv_style_t style;
    static lv_style_t style_pr;
    static lv_style_transition_dsc_t trans;
    static lv_style_prop_t props[] = {LV_STYLE_OUTLINE_WIDTH, LV_STYLE_OUTLINE_OPA, 0};

    if(win1 != NULL) {
        return;
    }

    if(!styles_initialized) {
        lv_style_init(&style);
        lv_style_set_radius(&style, 10);
        lv_style_set_bg_opa(&style, LV_OPA_100);
        lv_style_set_bg_color(&style, lv_palette_main(LV_PALETTE_BLUE));
        lv_style_set_bg_grad_color(&style, lv_palette_darken(LV_PALETTE_BLUE, 2));
        lv_style_set_bg_grad_dir(&style, LV_GRAD_DIR_VER);
        lv_style_set_border_opa(&style, LV_OPA_40);
        lv_style_set_border_width(&style, 2);
        lv_style_set_border_color(&style, lv_palette_main(LV_PALETTE_GREY));
        lv_style_set_shadow_width(&style, 6);
        lv_style_set_shadow_color(&style, lv_palette_main(LV_PALETTE_GREY));
        lv_style_set_shadow_ofs_y(&style, 6);
        lv_style_set_outline_opa(&style, LV_OPA_COVER);
        lv_style_set_outline_color(&style, lv_palette_main(LV_PALETTE_BLUE));
        lv_style_set_text_color(&style, lv_color_white());
        lv_style_set_pad_all(&style, 8);

        lv_style_init(&style_pr);
        lv_style_set_outline_width(&style_pr, 20);
        lv_style_set_outline_opa(&style_pr, LV_OPA_TRANSP);
        lv_style_set_translate_y(&style_pr, 5);
        lv_style_set_shadow_ofs_y(&style_pr, 3);
        lv_style_set_bg_color(&style_pr, lv_palette_darken(LV_PALETTE_BLUE, 2));
        lv_style_set_bg_grad_color(&style_pr, lv_palette_darken(LV_PALETTE_BLUE, 4));
        lv_style_transition_dsc_init(&trans, props, lv_anim_path_linear, 300, 0, NULL);
        lv_style_set_transition(&style_pr, &trans);
        styles_initialized = true;
    }

    win1 = lv_win_create(parent, 40);
    lv_obj_set_style_radius(win1, 8, LV_PART_MAIN);

    lv_obj_set_size(win1, 480, 272);
    lv_obj_set_align(win1, LV_ALIGN_CENTER);


    lv_win_add_title(win1, "Picture test");
    btn1 = lv_win_add_btn(win1, LV_SYMBOL_CLOSE, 40);
    lv_obj_add_event_cb(btn1, close_event_handler, LV_EVENT_CLICKED, win1);

    cont1 = lv_win_get_content(win1);  /*Content can be added here*/
    lv_obj_set_scrollbar_mode(cont1, LV_SCROLLBAR_MODE_OFF);

    win_recutangle = lv_obj_create(cont1);

    lv_obj_set_size(win_recutangle, 350, 225);
    lv_obj_align(win_recutangle, LV_ALIGN_CENTER,30,0);

    ta_pic = lv_textarea_create(cont1);
    lv_textarea_set_one_line(ta_pic, true);
    lv_textarea_set_max_length(ta_pic, 50);
    lv_obj_align(ta_pic, LV_ALIGN_BOTTOM_MID, 30, -2);
    lv_obj_set_content_width(ta_pic, 325);

    btn_png = lv_btn_create(cont1);
    lv_obj_remove_style_all(btn_png);                          /*Remove the style coming from the theme*/
    lv_obj_add_style(btn_png, &style, 0);
    lv_obj_add_style(btn_png, &style_pr, LV_STATE_PRESSED);
    lv_obj_set_size(btn_png, 60, 30);
    lv_obj_set_pos(btn_png, 0, 10);

    lv_obj_t * label = lv_label_create(btn_png);
    lv_label_set_text(label, "PNG");
    lv_obj_center(label);

    lv_obj_add_event_cb(btn_png, btn_png_event_cb, LV_EVENT_CLICKED, NULL);

    btn_gif = lv_btn_create(cont1);
    lv_obj_remove_style_all(btn_gif);                          /*Remove the style coming from the theme*/
    lv_obj_add_style(btn_gif, &style, 0);
    lv_obj_add_style(btn_gif, &style_pr, LV_STATE_PRESSED);
    lv_obj_set_size(btn_gif, 60, 30);
    lv_obj_set_pos(btn_gif, 0, 60);

    label = lv_label_create(btn_gif);
    lv_label_set_text(label, "GIF");
    lv_obj_center(label);

    lv_obj_add_event_cb(btn_gif, btn_gif_event_cb, LV_EVENT_CLICKED, NULL);

    btn_bmp = lv_btn_create(cont1);
    lv_obj_remove_style_all(btn_bmp);                          /*Remove the style coming from the theme*/
    lv_obj_add_style(btn_bmp, &style, 0);
    lv_obj_add_style(btn_bmp, &style_pr, LV_STATE_PRESSED);
    lv_obj_set_size(btn_bmp, 60, 30);
    lv_obj_set_pos(btn_bmp, 0, 110);

    label = lv_label_create(btn_bmp);
    lv_label_set_text(label, "BMP");
    lv_obj_center(label);

    lv_obj_add_event_cb(btn_bmp, btn_bmp_event_cb, LV_EVENT_CLICKED, NULL);

    btn_sjpg = lv_btn_create(cont1);
    lv_obj_remove_style_all(btn_sjpg);                          /*Remove the style coming from the theme*/
    lv_obj_add_style(btn_sjpg, &style, 0);
    lv_obj_add_style(btn_sjpg, &style_pr, LV_STATE_PRESSED);
    lv_obj_set_size(btn_sjpg, 60, 30);
    lv_obj_set_pos(btn_sjpg, 0, 160);

    label = lv_label_create(btn_sjpg);
    lv_label_set_text(label, "SJPG");
    lv_obj_center(label);

    lv_obj_add_event_cb(btn_sjpg, btn_sjpg_event_cb, LV_EVENT_CLICKED, NULL);
}


static void lv_demo_png(void)
{
    if(pic_png == NULL) {
        pic_png = lv_img_create(cont1);
        lv_img_set_src(pic_png, &img_png);
        lv_obj_align(pic_png, LV_ALIGN_CENTER, 30, -30);
    }
    else {
        lv_obj_clear_flag(pic_png, LV_OBJ_FLAG_HIDDEN);
    }

    lv_textarea_set_placeholder_text(ta_pic, "Built-in PNG image");
}

static void lv_demo_bmp(void)
{
    if(pic_bmp == NULL) {
        pic_bmp = lv_img_create(cont1);
        lv_img_set_src(pic_bmp, &img_bmp);
        lv_obj_align(pic_bmp, LV_ALIGN_CENTER, 30, -30);
    }
    else {
        lv_obj_clear_flag(pic_bmp, LV_OBJ_FLAG_HIDDEN);
    }

    lv_textarea_set_placeholder_text(ta_pic, "Built-in BMP image");
}

static void lv_demo_gif(void)
{
    lv_textarea_set_placeholder_text(ta_pic, "Built-in wave GIF");

    if(pic_gif == NULL) {
        pic_gif = lv_gif_create(cont1);
        lv_gif_set_src(pic_gif, &wave);
        lv_obj_align(pic_gif, LV_ALIGN_CENTER, 30, -30);
    }
    else {
        lv_obj_clear_flag(pic_gif, LV_OBJ_FLAG_HIDDEN);
        lv_gif_restart(pic_gif);
    }
}

static void lv_demo_sjpg(void)
{
    lv_textarea_set_placeholder_text(ta_pic, "Built-in SJPG image");

    if(pic_sjpg == NULL) {
        pic_sjpg = lv_img_create(cont1);
        lv_img_set_src(pic_sjpg, &img_sjpg);
        lv_obj_align(pic_sjpg, LV_ALIGN_CENTER, 30, -30);
    }
    else {
        lv_obj_clear_flag(pic_sjpg, LV_OBJ_FLAG_HIDDEN);
    }
}

#endif
