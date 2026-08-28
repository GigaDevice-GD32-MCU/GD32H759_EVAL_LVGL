/**
 * @file lv_demos.h
 *
 */

#ifndef LV_DEMOS_H
#define LV_DEMOS_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lvgl.h"

#if LV_USE_DEMO_WIDGETS
#include "widgets/lv_demo_widgets.h"
#endif

#if LV_USE_DEMO_BENCHMARK
#include "benchmark/lv_demo_benchmark.h"
#endif

#if LV_USE_DEMO_STRESS
#include "stress/lv_demo_stress.h"
#endif

#if LV_USE_DEMO_KEYPAD_AND_ENCODER
#include "keypad_encoder/lv_demo_keypad_encoder.h"
#endif

#if LV_USE_DEMO_MUSIC
#include "music/lv_demo_music.h"
#endif

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
/**********************
 * GLOBAL PROTOTYPES
 **********************/
#define SCALE_X ((float)480/1024)
#define SCALE_Y ((float)272/600)
#define SCALE_POS_X(x) ((lv_coord_t)((x) * SCALE_X))
#define SCALE_POS_Y(y) ((lv_coord_t)((y) * SCALE_Y))

/* Main page icon buttons.
 * The icons are 35x35. lv_imgbtn only sizes itself from the image passed as
 * `src_mid`; this project passes the icon as `src_left`, so refr_img() bails out
 * and the buttons kept lv_obj_class's default size of LV_DPI_DEF x LV_DPI_DEF
 * (130x130). The columns are only 63px apart and the rows 65px, so every touch
 * area overlapped its neighbours and the topmost button (Setting, created last)
 * won most of the taps. */
#define APP_ICON_SIZE           35
/* extra touch margin on each side -> 47x47 hit area, still 16px clear of the
 * neighbouring column */
#define APP_ICON_TOUCH_PAD      6
#define APP_ICON_BTN_SIZE(btn)                                  \
    do {                                                        \
        lv_obj_set_size((btn), APP_ICON_SIZE, APP_ICON_SIZE);   \
        lv_obj_set_ext_click_area((btn), APP_ICON_TOUCH_PAD);   \
    } while(0)
/* Label offsets from LV_ALIGN_OUT_BOTTOM_MID of the icon button. They used to be
 * (-48, -90) to compensate for the accidental 130x130 button size. */
#define APP_ICON_LABEL_OFS_X    0
#define APP_ICON_LABEL_OFS_Y    5

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_DEMO_H*/
