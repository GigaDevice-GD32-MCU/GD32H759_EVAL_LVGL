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
#include "lvgl.h"

typedef struct _lv_demo_args lv_demo_args_t;

#if LV_USE_DEMO_BENCHMARK
#include "benchmark/lv_demo_benchmark.h"
#endif

#if LV_USE_DEMO_KEYPAD_AND_ENCODER
#include "keypad_encoder/lv_demo_keypad_encoder.h"
#endif

#if LV_USE_DEMO_MUSIC
#include "music/lv_demo_music.h"
#endif

#if LV_USE_DEMO_STRESS
#include "stress/lv_demo_stress.h"
#endif

#if LV_USE_DEMO_WIDGETS
#include "widgets/lv_demo_widgets.h"
#endif

#if LV_USE_DEMO_VECTOR_GRAPHIC && LV_USE_VECTOR_GRAPHIC
#include "vector_graphic/lv_demo_vector_graphic.h"
#endif

#if LV_USE_DEMO_RENDER
#include "render/lv_demo_render.h"
#endif

#if LV_USE_DEMO_GLTF
#include "gltf/lv_demo_gltf.h"
#endif

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
struct _lv_demo_args {
    lv_obj_t * parent;
};

/**********************
 * GD32 DEMO APP HELPERS
 **********************/
/* The app layout was designed for 1024x600, scale it to the 480x272 panel */
#define SCALE_X   ((float)480/1024)
#define SCALE_Y   ((float)272/600)
#define SCALE_POS_X(x) ((int32_t)((x) * SCALE_X))
#define SCALE_POS_Y(y) ((int32_t)((y) * SCALE_Y))

/* Main page icon buttons.
 * The icons are 35x35. The columns are only 63px apart and the rows 65px, so
 * the buttons get an explicit size; otherwise every touch area overlaps its
 * neighbours and the topmost button wins most of the taps. */
#define APP_ICON_SIZE           35
/* extra touch margin on each side -> 47x47 hit area, still 16px clear of the
 * neighbouring column */
#define APP_ICON_TOUCH_PAD      6
#define APP_ICON_BTN_SIZE(btn)                                  \
    do {                                                        \
        lv_obj_set_size((btn), APP_ICON_SIZE, APP_ICON_SIZE);   \
        lv_obj_set_ext_click_area((btn), APP_ICON_TOUCH_PAD);   \
    } while(0)
/* Label offsets from LV_ALIGN_OUT_BOTTOM_MID of the icon button. */
#define APP_ICON_LABEL_OFS_X    0
#define APP_ICON_LABEL_OFS_Y    5

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize the lv_demo_args_t structure with default values.
 * @param args Pointer to the lv_demo_args_t structure to be initialized.
 */
void lv_demo_args_init(lv_demo_args_t * args);

/**
 * Call lv_demo_xxx.
 * @param   info the information which contains demo name and parameters
 *               needs by lv_demo_xxx.
 * @size    size of information.
 */
bool lv_demos_create(char * info[], int size);

/**
 * Show help for lv_demos.
 */
void lv_demos_show_help(void);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_DEMOS_H*/
