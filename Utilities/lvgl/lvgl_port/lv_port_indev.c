/**
 * @file lv_port_indev.c
 *
 */

/*Copy this file as "lv_port_indev.c" and set this value to "1" to enable content*/
#if 1

/*********************
 *      INCLUDES
 *********************/
#include "lv_port_indev.h"
#include "lvgl.h"

#include "bsp_ts_gt911.h"
#include "bsp_i2c_touch.h"
#include "lv_port_disp.h"
/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void touchpad_init(void);
static void touchpad_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);
static bool touchpad_is_pressed(void);
static void touchpad_get_xy(lv_coord_t * x, lv_coord_t * y);

/**********************
 *  STATIC VARIABLES
 **********************/
lv_indev_t * indev_touchpad;
extern uint8_t touch_buf[48]; 
extern uint8_t clear_flag;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_port_indev_init(void)
{
    static lv_indev_drv_t indev_drv;
    /*------------------
     * Touchpad
     * -----------------*/
    /*Initialize your touchpad if you have*/
    touchpad_init();

    /*Register a touchpad input device*/
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = touchpad_read;
    indev_touchpad = lv_indev_drv_register(&indev_drv);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/*------------------
 * Touchpad
 * -----------------*/

/*Initialize your touchpad*/
static void touchpad_init(void)
{
    /*Your code comes here*/
    gt911_init();
}

/* GT911 supports 5 touch points at most, `touch_buf` is only 48 bytes long */
#define GT911_MAX_POINT         5U
/* bit7 of register 0x814E: a new coordinate set is ready to be read.
 * When this bit is 0 the rest of the register (including the point counter)
 * is stale and must not be used. */
#define GT911_BUF_READY         0x80U
/* if gt911 stops reporting while a press is active, release it after this time */
#define GT911_PRESS_TIMEOUT     200U
/* a new press is only reported once two consecutive reads agree on roughly the
 * same coordinate. Real fingers stay down for many read periods, electrical
 * glitches only ever show up in a single frame. */
#define GT911_DEBOUNCE_DIST     40

/*Will be called by the library to read the touchpad*/
static void touchpad_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
    LV_UNUSED(indev_drv);

    static lv_coord_t last_x = 0;
    static lv_coord_t last_y = 0;
    static lv_indev_state_t last_state = LV_INDEV_STATE_REL;
    static uint32_t last_update = 0;
    /* pending (not yet confirmed) press candidate */
    static uint8_t  cand_valid = 0;
    static lv_coord_t cand_x = 0;
    static lv_coord_t cand_y = 0;

    if(gt911.enable == 0) {
        data->point.x = last_x;
        data->point.y = last_y;
        data->state = LV_INDEV_STATE_REL;
        return;
    }

    /* read gt911 status register; on a failed transfer simply keep the previous
     * state, never use whatever was left in touch_buf */
    if(SUCCESS == gt911_readreg(GT911_PRESSED_INFO_REG, touch_buf, 1)) {
        uint8_t status = touch_buf[0];

        /* only act when gt911 signals that fresh data is available */
        if(status & GT911_BUF_READY) {
            gt911.pressed_info = status;

            uint8_t cnt = status & 0x0FU;
            /* clamp: 8 * cnt must never exceed sizeof(touch_buf) (48) and i
             * must stay inside gt911.x/y/s[5] */
            if(cnt > GT911_MAX_POINT) {
                cnt = GT911_MAX_POINT;
            }

            lv_indev_state_t new_state = LV_INDEV_STATE_REL;

            if(cnt > 0U) {
                if(SUCCESS == gt911_readreg(GT911_COORDINATE_REG, touch_buf, 8U * cnt)) {
                    for(uint8_t i = 0; i < cnt; i++) {
                        gt911.x[i] = ((uint16_t)touch_buf[2 + i * 8] << 8) + touch_buf[1 + i * 8];
                        gt911.y[i] = ((uint16_t)touch_buf[4 + i * 8] << 8) + touch_buf[3 + i * 8];
                        gt911.s[i] = ((uint16_t)touch_buf[6 + i * 8] << 8) + touch_buf[5 + i * 8];
                    }

                    uint16_t x = gt911.x[0];
                    uint16_t y = gt911.y[0];
                    /* Drop out of range samples instead of clamping them:
                     * clamping would push corrupted readings onto the widget
                     * sitting in the bottom right corner. */
                    if((x < LV_HOR_RES_MAX) && (y < LV_VER_RES_MAX)) {
                        gt911.pressed = 2;
                        if(last_state == LV_INDEV_STATE_PR) {
                            /* already pressed: follow the finger without delay */
                            last_x = (lv_coord_t)x;
                            last_y = (lv_coord_t)y;
                            new_state = LV_INDEV_STATE_PR;
                        }
                        else if(cand_valid &&
                                (LV_ABS((lv_coord_t)x - cand_x) < GT911_DEBOUNCE_DIST) &&
                                (LV_ABS((lv_coord_t)y - cand_y) < GT911_DEBOUNCE_DIST)) {
                            /* second consistent sample: accept the press */
                            last_x = (lv_coord_t)x;
                            last_y = (lv_coord_t)y;
                            new_state = LV_INDEV_STATE_PR;
                        }
                        else {
                            /* first sample: remember it but stay released */
                            cand_valid = 1;
                            cand_x = (lv_coord_t)x;
                            cand_y = (lv_coord_t)y;
                        }
                    }
                }
                else {
                    /* coordinates could not be read, keep the previous state */
                    new_state = last_state;
                }
            }
            else {
                /* nothing on the panel: forget any pending candidate */
                cand_valid = 0;
            }

            if(new_state == LV_INDEV_STATE_REL) {
                gt911.pressed = 0;
            }
            last_state = new_state;
            last_update = lv_tick_get();

            /* clear the status register so gt911 can refresh the coordinates */
            (void)gt911_writereg(GT911_PRESSED_INFO_REG, &clear_flag, 1);
        }
    }

    /* safety net: a press must never stay latched if gt911 goes quiet */
    if((last_state == LV_INDEV_STATE_PR) && (lv_tick_elaps(last_update) > GT911_PRESS_TIMEOUT)) {
        last_state = LV_INDEV_STATE_REL;
    }

    data->point.x = last_x;
    data->point.y = last_y;
    data->state = last_state;
}

// /*Return true is the touchpad is pressed*/
// static bool touchpad_is_pressed(void)
// {
//     /*Your code comes here*/
//     if(is_touch){
//         is_touch = 0;
//         return true;
//     } else {
//         return false;
//     }
// }

// /*Get the x and y coordinates if the touchpad is pressed*/
// static void touchpad_get_xy(lv_coord_t * x, lv_coord_t * y)
// {
//     /*Your code comes here*/
//     (*x) = touch_x;
//     (*y) = touch_y;
// }

#endif
