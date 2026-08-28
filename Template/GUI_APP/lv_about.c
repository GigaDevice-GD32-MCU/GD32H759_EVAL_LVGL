/**
 * @file lv_about.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lvgl.h"
#include <stdio.h>
#include "lv_demos.h"
#include "gd32h73x_75x.h"

/*********************
 *      DEFINES
 *********************/
LV_IMG_DECLARE(wuxinghongqi_32);
LV_IMG_DECLARE(deguo_32);
LV_IMG_DECLARE(riben_32);
LV_IMG_DECLARE(meiguo_32);
LV_FONT_DECLARE(yahei_regular_10);
LV_FONT_DECLARE(japan_tanugo_reg_10);

lv_obj_t *main_label;
extern lv_obj_t *obj_performance;
/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/
static void cont_close_click_event_cb(lv_event_t *e)
{
    lv_obj_t *closebtn = lv_event_get_current_target(e);
    lv_obj_t *cont = lv_obj_get_parent(closebtn);
    lv_obj_clear_flag(obj_performance, LV_OBJ_FLAG_HIDDEN);

    lv_obj_del(cont);
}

static void img1_cn_cb(lv_event_t *e)
{
    lv_obj_t *closebtn = lv_event_get_current_target(e);
    lv_obj_clear_flag(closebtn, LV_OBJ_FLAG_HIDDEN);
    lv_label_set_text(main_label, "兆易创新科技集团股份有限公司是全球\n"
                      "领先的Fabless芯片供应商，公司的核心\n"
                      "产品线为存储器（Flash/DRAM）、32位\n"
                      "通用型MCU、智能人机交互传感器、电\n"
                      "源产品及整体解决方案，技术实力备受\n"
                      "行业认可。");
    lv_obj_set_style_text_font(main_label, &yahei_regular_10, 0);
    lv_obj_align(main_label, LV_ALIGN_CENTER, 5, -10);
}


static void img1_en_cb(lv_event_t *e)
{
    lv_obj_t *closebtn = lv_event_get_current_target(e);
    lv_obj_clear_flag(closebtn, LV_OBJ_FLAG_HIDDEN);
    lv_label_set_text(main_label, "Gigadevice is the world's leading Fabless\n"
                      "chip supplier. The core product line is \n"
                      "memory(Flash/DRAM), 32-bit general-\n"
                      "purpose MCU, intelligent human-\n"
                      "computer interaction sensor, power \n"
                      "products and overall solutions.\n");
    lv_obj_set_style_text_font(main_label, &yahei_regular_10, 0);
    lv_obj_align(main_label, LV_ALIGN_CENTER, 5, -5);
}


static void img1_jn_cb(lv_event_t *e)
{
    lv_obj_t *closebtn = lv_event_get_current_target(e);
    lv_obj_clear_flag(closebtn, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_style_text_font(main_label, &japan_tanugo_reg_10, 0);

    lv_label_set_text(main_label, "メガイーイノベーションテクノロジー株式会社\n"
                      "は世界をリードするFablessチップサプライヤ\n"
                      "ーであり、会社のコア製品ラインはメモリ（\n"
                      "Flash/DRAM）であり、32ビット汎用型MCU、\n"
                      "インテリジェントマンマシンインタラクショ\n"
                      "ンセンサー、電源製品及び全体ソリューショ\n"
                      "ンであり、技術力は業界に認められている。\n"
                     );
    lv_obj_align(main_label, LV_ALIGN_CENTER, 5, -5);
}


static void img1_gn_cb(lv_event_t *e)
{
    lv_obj_t *closebtn = lv_event_get_current_target(e);
    lv_obj_clear_flag(closebtn, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_style_text_font(main_label, &japan_tanugo_reg_10, 0);

    lv_label_set_text(main_label, "Gigadevice Innovation Technology Co.,\n"
                      "Ltd. ist ein führender globaler Anbi-\n"
                      "eter von Fabless-Chips. Die Kernprod-\n"
                      "uktlinie des Unternehmens ist Speich-\n"
                      "er(Flash/DRAM), 32-bit-Universal-MCU,\n"
                      "intelligente Mensch-Maschine-Interak-\n"
                      "tionssensoren, Leistungsp-rodukte und\n"
                      "Gesamtlösungen.\n"
                     );
    lv_obj_align(main_label, LV_ALIGN_CENTER, 5, -5);
}

void about_event_cb(lv_event_t *e)
{
    lv_obj_t *cont = lv_obj_create(lv_scr_act());
    lv_obj_set_size(cont, 480, 272);
    lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_align(cont, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t *close_btn = lv_btn_create(cont);
    lv_obj_set_ext_click_area(close_btn, LV_DPX(10));
    lv_obj_add_event_cb(close_btn, cont_close_click_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t *label = lv_label_create(close_btn);
    lv_label_set_text(label, LV_SYMBOL_CLOSE);
    const lv_font_t *font = lv_obj_get_style_text_font(close_btn, LV_PART_MAIN);
    lv_coord_t close_btn_size = lv_font_get_line_height(font) + LV_DPX(10);
    lv_obj_set_size(close_btn, close_btn_size, close_btn_size);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_align(close_btn, LV_ALIGN_TOP_RIGHT, 0, 0);

    /* 显示框 */
    lv_obj_t *cont_text = lv_obj_create(cont);
    lv_obj_set_size(cont_text, 265, 175);
    lv_obj_clear_flag(cont_text, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_align(cont_text, LV_ALIGN_CENTER, 0, -10);
    lv_obj_set_style_border_width(cont_text, 1, LV_PART_MAIN);
    lv_obj_set_style_bg_color(cont_text, lv_color_make(0xf6, 0xf5, 0xf3), LV_PART_MAIN);
    lv_obj_set_style_border_color(cont_text, lv_color_make(178, 178, 178), LV_PART_MAIN);
    lv_obj_set_style_radius(cont_text, 5, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(cont_text, 150, LV_PART_MAIN);

#if 1
    lv_obj_t *cont_lan = lv_obj_create(cont);
    //lv_obj_set_pos(cz_label, 100, 100);
    lv_obj_set_size(cont_lan, 240, 40);
    lv_obj_clear_flag(cont_lan, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_align(cont_lan, LV_ALIGN_BOTTOM_MID, 0, 12);
    lv_obj_set_style_border_width(cont_lan, 1, LV_PART_MAIN);
    lv_obj_set_style_radius(cont_lan, 5, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(cont_lan, 150, LV_PART_MAIN);

    /* imagebtn wuxinghongqi */
    lv_obj_t *img1_cn = lv_imgbtn_create(cont_lan);
    lv_imgbtn_set_src(img1_cn, LV_IMGBTN_STATE_RELEASED, &wuxinghongqi_32, NULL, NULL);
    lv_imgbtn_set_src(img1_cn, LV_IMGBTN_STATE_DISABLED, &wuxinghongqi_32, NULL, NULL);
    lv_obj_align(img1_cn, LV_ALIGN_BOTTOM_MID, -90, 12);
    lv_obj_set_size(img1_cn, 32, 21);

    /* imagebtn America */
    lv_obj_t *img1_en = lv_imgbtn_create(cont_lan);
    lv_imgbtn_set_src(img1_en, LV_IMGBTN_STATE_RELEASED, &meiguo_32, NULL, NULL);
    lv_imgbtn_set_src(img1_en, LV_IMGBTN_STATE_DISABLED, &meiguo_32, NULL, NULL);
    lv_obj_align(img1_en, LV_ALIGN_BOTTOM_MID, -30, 12);
    lv_obj_set_size(img1_en, 32, 21);

    /* imagebtn Japan */
    lv_obj_t *img1_jn = lv_imgbtn_create(cont_lan);
    lv_imgbtn_set_src(img1_jn, LV_IMGBTN_STATE_RELEASED, &riben_32, NULL, NULL);
    lv_imgbtn_set_src(img1_jn, LV_IMGBTN_STATE_DISABLED, &riben_32, NULL, NULL);
    lv_obj_align(img1_jn, LV_ALIGN_BOTTOM_MID, 30, 12);
    lv_obj_set_size(img1_jn, 32, 21);

    /* imagebtn Germany */
    lv_obj_t *img1_gn = lv_imgbtn_create(cont_lan);
    lv_imgbtn_set_src(img1_gn, LV_IMGBTN_STATE_RELEASED, &deguo_32, NULL, NULL);
    lv_imgbtn_set_src(img1_gn, LV_IMGBTN_STATE_DISABLED, &deguo_32, NULL, NULL);
    lv_obj_align(img1_gn, LV_ALIGN_BOTTOM_MID, 90, 12);
    lv_obj_set_size(img1_gn, 32, 21);

    lv_obj_add_event_cb(img1_cn, img1_cn_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(img1_en, img1_en_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(img1_jn, img1_jn_cb, LV_EVENT_RELEASED, NULL);
    lv_obj_add_event_cb(img1_gn, img1_gn_cb, LV_EVENT_CLICKED, NULL);
#endif
    /*Create the main label*/
    main_label = lv_label_create(cont);

    lv_label_set_text(main_label, "Gigadevice is the world's leading Fabless\n"
                      "chip supplier. The core product line is \n"
                      "memory(Flash/DRAM), 32-bit general-\n"
                      "purpose MCU, intelligent human-\n"
                      "computer interaction sensor, power \n"
                      "products and overall solutions.\n");

    lv_obj_set_style_text_color(main_label, lv_color_make(1, 77, 103), 0);
    lv_obj_set_style_text_font(main_label, &yahei_regular_10, 0);
    lv_obj_align(main_label, LV_ALIGN_CENTER, 5, -5);
    
    lv_obj_add_flag(obj_performance, LV_OBJ_FLAG_HIDDEN);
}
/**********************
 *   STATIC FUNCTIONS
 **********************/
