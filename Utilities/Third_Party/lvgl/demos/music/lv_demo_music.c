/**
 * @file lv_demo_music.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_demo_music.h"

#if LV_USE_DEMO_MUSIC

#include "lv_demo_music_main.h"
#include "lv_demo_music_list.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
#if LV_DEMO_MUSIC_AUTO_PLAY
    static void auto_step_cb(lv_timer_t * timer);
#endif

 LV_IMG_DECLARE(img_home);
// LV_IMG_DECLARE(img_home_pressed);

/**********************
 *  STATIC VARIABLES
 **********************/
//static lv_obj_t * ctrl;
//static lv_obj_t * list;
static lv_obj_t * ctrl;
lv_obj_t * list;
extern lv_obj_t * music_cont;
lv_obj_t *imgbtn_home;

extern lv_obj_t* obj_main;
extern lv_obj_t* led_performance;
extern lv_obj_t* led_main;
extern lv_obj_t* tv_main;
extern uint16_t back_to_home;
static void home_event_cb(lv_event_t *e)
{
    back_to_home = 1;
    /* Stop playback and rewind to the first track while the page is hidden, so
     * the album slide/fade animations are never visible and the demo always
     * reopens in its initial state. */
    _lv_demo_music_reset();

    /* `ctrl` is the transparent full screen scroll container that holds the
     * player; `music_cont` is the player itself and `list` a sibling of `ctrl`
     * that shows through it. All of them have to go. */
    if(ctrl != NULL) lv_obj_add_flag(ctrl, LV_OBJ_FLAG_HIDDEN);
    if(music_cont != NULL) lv_obj_add_flag(music_cont, LV_OBJ_FLAG_HIDDEN);
    if(list != NULL) lv_obj_add_flag(list, LV_OBJ_FLAG_HIDDEN);
    if(imgbtn_home != NULL) lv_obj_add_flag(imgbtn_home, LV_OBJ_FLAG_HIDDEN);
//    lv_obj_del(tv);
//    lv_obj_clean(tv);
    lv_obj_clear_flag(obj_main, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(led_performance, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(led_main, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(tv_main, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(imgbtn_home, LV_OBJ_FLAG_CLICKABLE);
}

static const char * title_list[] = {
    "Waiting for true love",
    "Need a Better Future",
    "Vibrations",
    "Why now?",
    "Never Look Back",
    "It happened Yesterday",
    "Feeling so High",
    "Go Deeper",
    "Find You There",
    "Until the End",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
};

static const char * artist_list[] = {
    "The John Smith Band",
    "My True Name",
    "Robotics",
    "John Smith",
    "My True Name",
    "Robotics",
    "Robotics",
    "Unknown artist",
    "Unknown artist",
    "Unknown artist",
    "Unknown artist",
    "Unknown artist",
    "Unknown artist",
    "Unknown artist",
    "Unknown artist",
};

static const char * genre_list[] = {
    "Rock - 1997",
    "Drum'n bass - 2016",
    "Psy trance - 2020",
    "Metal - 2015",
    "Metal - 2015",
    "Metal - 2015",
    "Metal - 2015",
    "Metal - 2015",
    "Metal - 2015",
    "Metal - 2015",
    "Metal - 2015",
    "Metal - 2015",
    "Metal - 2015",
    "Metal - 2015",
};

static const uint32_t time_list[] = {
    1 * 60 + 14,
    2 * 60 + 26,
    1 * 60 + 54,
    2 * 60 + 24,
    2 * 60 + 37,
    3 * 60 + 33,
    1 * 60 + 56,
    3 * 60 + 31,
    2 * 60 + 20,
    2 * 60 + 19,
    2 * 60 + 20,
    2 * 60 + 19,
    2 * 60 + 20,
    2 * 60 + 19,
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_demo_music(unsigned char flag)
{
    if(0 == flag){
        lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0x343247), 0);

        list = _lv_demo_music_list_create(lv_scr_act());
        ctrl = _lv_demo_music_main_create(lv_scr_act(), flag);
        
        imgbtn_home = lv_imgbtn_create(ctrl);
        lv_imgbtn_set_src(imgbtn_home, LV_IMGBTN_STATE_PRESSED, &img_home, NULL, NULL);
        lv_imgbtn_set_src(imgbtn_home, LV_IMGBTN_STATE_RELEASED, &img_home, NULL, NULL);
        lv_obj_set_size(imgbtn_home, 30, 30);
        lv_obj_align(imgbtn_home, LV_ALIGN_TOP_RIGHT, -3, 3);
        lv_obj_add_event_cb(imgbtn_home, home_event_cb, LV_EVENT_CLICKED, NULL);
        lv_obj_add_flag(imgbtn_home, LV_OBJ_FLAG_CLICKABLE);

        lv_obj_add_flag(list, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ctrl, LV_OBJ_FLAG_HIDDEN);

#if LV_DEMO_MUSIC_AUTO_PLAY
    lv_timer_create(auto_step_cb, 1000, NULL);
#endif
    }else{
        /* Just show the player again. Deleting and rebuilding it is not safe:
         * lv_demo_music_main.c / _list.c keep a lot of static object pointers
         * plus running timers and animations that are not torn down with the
         * widgets, so the next timer tick would run on freed objects. */
        if(ctrl != NULL) {
            lv_obj_clear_flag(ctrl, LV_OBJ_FLAG_HIDDEN);
        }
        if(music_cont != NULL)  lv_obj_clear_flag(music_cont, LV_OBJ_FLAG_HIDDEN);
        if(list != NULL)        lv_obj_clear_flag(list, LV_OBJ_FLAG_HIDDEN);
        if(imgbtn_home != NULL) lv_obj_clear_flag(imgbtn_home, LV_OBJ_FLAG_HIDDEN);

        /* `ctrl` is the transparent scroll container returned by
         * _lv_demo_music_main_create(); scrolling it down slides the player away
         * and lets the track list show through. Its scroll position survives
         * hiding, so without this the demo reopens on the track list instead of
         * on the player page. */
        if(ctrl != NULL) {
            lv_obj_scroll_to_y(ctrl, 0, LV_ANIM_OFF);
        }

        /* With flag != 0 this does not rebuild anything: it restarts the second
         * counter and replays the intro (logo + fade in of the boxes). */
        _lv_demo_music_main_create(lv_scr_act(), flag);

        lv_obj_invalidate(lv_scr_act());
    }
}

const char * _lv_demo_music_get_title(uint32_t track_id)
{
    if(track_id >= sizeof(title_list) / sizeof(title_list[0])) return NULL;
    return title_list[track_id];
}

const char * _lv_demo_music_get_artist(uint32_t track_id)
{
    if(track_id >= sizeof(artist_list) / sizeof(artist_list[0])) return NULL;
    return artist_list[track_id];
}

const char * _lv_demo_music_get_genre(uint32_t track_id)
{
    if(track_id >= sizeof(genre_list) / sizeof(genre_list[0])) return NULL;
    return genre_list[track_id];
}

uint32_t _lv_demo_music_get_track_length(uint32_t track_id)
{
    if(track_id >= sizeof(time_list) / sizeof(time_list[0])) return 0;
    return time_list[track_id];
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#if LV_DEMO_MUSIC_AUTO_PLAY
static void auto_step_cb(lv_timer_t * t)
{
    LV_UNUSED(t);
    static uint32_t state = 0;

#if LV_DEMO_MUSIC_LARGE
    const lv_font_t * font_small = &lv_font_montserrat_22;
    const lv_font_t * font_large = &lv_font_montserrat_32;
#else
    const lv_font_t * font_small = &lv_font_montserrat_12;
    const lv_font_t * font_large = &lv_font_montserrat_16;
#endif

    switch(state) {
        case 5:
            _lv_demo_music_album_next(true);
            break;

        case 6:
            _lv_demo_music_album_next(true);
            break;
        case 7:
            _lv_demo_music_album_next(true);
            break;
        case 8:
            _lv_demo_music_play(0);
            break;
#if LV_DEMO_MUSIC_SQUARE || LV_DEMO_MUSIC_ROUND
        case 11:
            lv_obj_scroll_by(ctrl, 0, -LV_VER_RES, LV_ANIM_ON);
            break;
        case 13:
            lv_obj_scroll_by(ctrl, 0, -LV_VER_RES, LV_ANIM_ON);
            break;
#else
        case 12:
            lv_obj_scroll_by(ctrl, 0, -LV_VER_RES, LV_ANIM_ON);
            break;
#endif
        case 15:
            lv_obj_scroll_by(list, 0, -300, LV_ANIM_ON);
            break;
        case 16:
            lv_obj_scroll_by(list, 0, 300, LV_ANIM_ON);
            break;
        case 18:
            _lv_demo_music_play(1);
            break;
        case 19:
            lv_obj_scroll_by(ctrl, 0, LV_VER_RES, LV_ANIM_ON);
            break;
#if LV_DEMO_MUSIC_SQUARE || LV_DEMO_MUSIC_ROUND
        case 20:
            lv_obj_scroll_by(ctrl, 0, LV_VER_RES, LV_ANIM_ON);
            break;
#endif
        case 30:
            _lv_demo_music_play(2);
            break;
        case 40: {
                lv_obj_t * bg = lv_layer_top();
                lv_obj_set_style_bg_color(bg, lv_color_hex(0x6f8af6), 0);
                lv_obj_set_style_text_color(bg, lv_color_white(), 0);
                lv_obj_set_style_bg_opa(bg, LV_OPA_COVER, 0);
                lv_obj_fade_in(bg, 400, 0);
                lv_obj_t * dsc = lv_label_create(bg);
                lv_obj_set_style_text_font(dsc, font_small, 0);
                lv_label_set_text(dsc, "The average FPS is");
                lv_obj_align(dsc, LV_ALIGN_TOP_MID, 0, 90);

                lv_obj_t * num = lv_label_create(bg);
                lv_obj_set_style_text_font(num, font_large, 0);
#if LV_USE_PERF_MONITOR
                lv_label_set_text_fmt(num, "%d", lv_refr_get_fps_avg());
#endif
                lv_obj_align(num, LV_ALIGN_TOP_MID, 0, 120);

                lv_obj_t * attr = lv_label_create(bg);
                lv_obj_set_style_text_align(attr, LV_TEXT_ALIGN_CENTER, 0);
                lv_obj_set_style_text_font(attr, font_small, 0);
#if LV_DEMO_MUSIC_SQUARE || LV_DEMO_MUSIC_ROUND
                lv_label_set_text(attr, "Copyright 2020 LVGL Kft.\nwww.lvgl.io | lvgl@lvgl.io");
#else
                lv_label_set_text(attr, "Copyright 2020 LVGL Kft. | www.lvgl.io | lvgl@lvgl.io");
#endif
                lv_obj_align(attr, LV_ALIGN_BOTTOM_MID, 0, -10);
                break;
            }
        case 41:
            lv_scr_load(lv_obj_create(NULL));
            _lv_demo_music_pause();
            break;
    }
    state++;
}

#endif /*LV_DEMO_MUSIC_AUTO_PLAY*/

#endif /*LV_USE_DEMO_MUSIC*/
