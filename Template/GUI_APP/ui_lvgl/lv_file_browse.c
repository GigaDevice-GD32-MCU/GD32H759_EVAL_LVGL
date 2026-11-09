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

extern Folder_btnArray *p;
extern File_btnArray *q;
extern char file_name[FILE_NUM][NAME_LENGTH];//�ļ���
extern char folder_name[FOLDER_NUM][NAME_LENGTH];//�ļ�����
extern char mountsel[];
extern uint8_t file_cnt;
extern uint8_t folder_cnt;
extern uint8_t current_path[PATH_LENGTH];
extern lv_obj_t *obj_performance;
void setup_scr_file_browse(void);

extern void lv_demo_app(void);

char *pname;
/*********************
 *      DEFINES
 *********************/
LV_IMG_DECLARE(image_file_28);
LV_IMG_DECLARE(image_folder_28);
lv_obj_t *win;

static void storage_error_close_cb(lv_event_t *event)
{
    /* The message box must not be deleted from inside its own event handler,
     * otherwise LVGL keeps working on freed memory. */
    lv_msgbox_close_async(lv_event_get_current_target(event));
}

static void show_storage_error(const char *message)
{
    static const char *buttons[] = {"Close", ""};
    lv_obj_t *msgbox = lv_msgbox_create(NULL, "SD card", message, buttons, true);
    lv_obj_add_event_cb(msgbox, storage_error_close_cb, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_center(msgbox);
}

/* �ļ���������ؼ� */
static void filelist_btnback_handler(lv_event_t *event)
{
    lv_event_code_t code = lv_event_get_code(event);

    switch(code) {
    case LV_EVENT_CLICKED: {
        refersh_parameter();
        lv_obj_del(win);
        setup_scr_file_browse();
    }
    break;
    default:
        break;
    }
}

void filelist_btnevent_handler(lv_event_t *event)
{
    lv_event_code_t code = lv_event_get_code(event);
    lv_obj_t *obj = lv_event_get_target(event);
    switch(code) {
    case LV_EVENT_RELEASED: {
        char new_path[PATH_LENGTH];

        pname = (char *)lv_list_get_btn_text(lv_obj_get_parent(obj), obj);
        if(pname == NULL) return;
        /* Never use current_path as both source and destination of snprintf:
         * overlapping buffers are undefined behaviour and corrupt the path. */
        snprintf(new_path, sizeof(new_path), "%s/%s", (char *)(current_path), pname);

        if(mf_scan_file(new_path) != 0U) {
            /* Keep the previous directory listing usable. */
            mf_scan_file((char *)(current_path));
            show_storage_error("Unable to open directory");
            return;
        }
        snprintf((char *)(current_path), PATH_LENGTH, "%s", new_path);
        lv_obj_del(win);
        setup_scr_file_browse();
    }
    break;
    default:
        break;
    }
}

void file_browse_event_cb(lv_event_t *event)
{
    LV_UNUSED(event);
    if(scanfiles_test() != FR_OK) {
        show_storage_error("Unable to mount the SD card");
        return;
    }

    /* Always start from the volume root, a stale path from a previous session
     * would make the first f_opendir() fail. */
    snprintf((char *)(current_path), PATH_LENGTH, "%s", mountsel);

    lv_obj_add_flag(obj_performance, LV_OBJ_FLAG_HIDDEN);
#if 0
    /*Create a list*/
    list1 = lv_list_create(lv_scr_act());
    lv_obj_set_size(list1, 180, 220);
    lv_obj_center(list1);

    /*Add buttons to the list*/
    lv_obj_t *btn;

    lv_list_add_text(list1, "File");
    btn = lv_list_add_btn(list1, LV_SYMBOL_FILE, "New");
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, NULL);
    btn = lv_list_add_btn(list1, LV_SYMBOL_DIRECTORY, "Open");
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, NULL);
    btn = lv_list_add_btn(list1, LV_SYMBOL_SAVE, "Save");
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, NULL);
    btn = lv_list_add_btn(list1, LV_SYMBOL_CLOSE, "Delete");
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, NULL);
    btn = lv_list_add_btn(list1, LV_SYMBOL_EDIT, "Edit");
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, NULL);

    lv_list_add_text(list1, "Connectivity");
    btn = lv_list_add_btn(list1, LV_SYMBOL_BLUETOOTH, "Bluetooth");
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, NULL);
    btn = lv_list_add_btn(list1, LV_SYMBOL_GPS, "Navigation");
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, NULL);
    btn = lv_list_add_btn(list1, LV_SYMBOL_USB, "USB");
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, NULL);
    btn = lv_list_add_btn(list1, LV_SYMBOL_BATTERY_FULL, "Battery");
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, NULL);

    lv_list_add_text(list1, "Exit");
    btn = lv_list_add_btn(list1, LV_SYMBOL_OK, "Apply");
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, NULL);
    btn = lv_list_add_btn(list1, LV_SYMBOL_CLOSE, "Close");
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, NULL);
#endif
    if(mf_scan_file(mountsel) != 0U) {
        lv_obj_clear_flag(obj_performance, LV_OBJ_FLAG_HIDDEN);
        show_storage_error("Unable to read root directory");
        return;
    }
    setup_scr_file_browse();
#if 0
    /***************/
    p = (Folder_btnArray *)malloc(sizeof(Folder_btnArray) + sizeof(int) * folder_cnt);
    p->folderlen = folder_cnt;


    q = (File_btnArray *)malloc(sizeof(File_btnArray) + sizeof(int) * file_cnt);
    q->filelen = file_cnt;

    for(i = 0; i < p->folderlen; i++) {
        p->Folder_btn[i] = lv_list_add_btn(ui->file_browse_filelist, &_wenjianjia_20x20,
                                           folder_name[i]);
        lv_obj_set_event_cb(p->Folder_btn[i], filelist_btnevent_handler);
    }
    for(j = 0; j < q->filelen; j++) {
        q->File_btn[j] = lv_list_add_btn(ui->file_browse_filelist, &_wenjiankongxin_20x20, file_name[j]);
        lv_obj_set_event_cb(q->File_btn[j], file_btnevent_handler);
    }
    /***************/
#endif
}
//lv_obj_t * win;
lv_obj_t *back_btn;
static void win_close_click_event_cb(lv_event_t *event)
{
    lv_obj_clear_flag(obj_performance, LV_OBJ_FLAG_HIDDEN);
    lv_obj_del(win);

    strncpy((char *)(current_path), "0:", 3);
    strncpy((char *)(mountsel), "0:", 3);
    mf_scan_file(mountsel);
}

void events_init_file_browse(void)
{
    if(strcmp(mountsel, (char *)(current_path)) != 0) {
        lv_obj_add_event_cb(back_btn, filelist_btnback_handler, LV_EVENT_PRESSED, NULL);
    }
}

void setup_scr_file_browse(void)
{
    win = lv_win_create(lv_scr_act(), 40);
    lv_obj_set_size(win, 480, 272);
    lv_obj_set_style_radius(win, 8, LV_PART_MAIN);

    lv_obj_clear_flag(win, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_align(win, LV_ALIGN_CENTER, 0, 0);

    /* Make the window's content area vertically scrollable */
    lv_obj_t * cont = lv_win_get_content(win);
    lv_obj_set_scroll_dir(cont, LV_DIR_VER);
    lv_obj_set_scrollbar_mode(cont, LV_SCROLLBAR_MODE_AUTO);

    if(strcmp(mountsel, (char *)(current_path)) != 0) {
        back_btn = lv_win_add_btn(win, LV_SYMBOL_LEFT, 40);
        lv_obj_add_event_cb(back_btn, filelist_btnback_handler, LV_EVENT_CLICKED, NULL);
    }
    lv_win_add_title(win, "");
    lv_obj_t *close_btn = lv_win_add_btn(win, LV_SYMBOL_CLOSE, 60);
    lv_obj_set_ext_click_area(close_btn, LV_DPX(10));
    lv_obj_add_event_cb(close_btn, win_close_click_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t *label = lv_label_create(close_btn);
    lv_label_set_text(label, LV_SYMBOL_CLOSE);
    const lv_font_t *font = lv_obj_get_style_text_font(close_btn, LV_PART_MAIN);
    lv_coord_t close_btn_size = lv_font_get_line_height(font) + LV_DPX(10);
    lv_obj_set_size(close_btn, close_btn_size, close_btn_size);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_align(close_btn, LV_ALIGN_TOP_RIGHT, 0, 0);

    lv_obj_t *file_list = lv_list_create(cont);
    lv_obj_set_size(file_list, 460, LV_SIZE_CONTENT);
    lv_obj_align(file_list, LV_ALIGN_TOP_MID, 0, 0);

    static lv_style_t style_screen_list0_bg;
    lv_style_init(&style_screen_list0_bg);

    //Write style state: LV_STATE_DEFAULT for style_screen_list0_bg
    lv_style_set_radius(&style_screen_list0_bg, 3);
    lv_style_set_border_color(&style_screen_list0_bg, lv_color_make(0xe1, 0xe6, 0xee));
    lv_style_set_border_width(&style_screen_list0_bg, 1);
    lv_obj_add_style(file_list, &style_screen_list0_bg, LV_PART_MAIN);

    static lv_style_t style_screen_list0_scrollable;
    lv_style_init(&style_screen_list0_scrollable);

    //Write style state: LV_STATE_DEFAULT for style_screen_list0_scrollable
    lv_style_set_radius(&style_screen_list0_scrollable, 3);
    lv_style_set_bg_color(&style_screen_list0_scrollable, lv_color_make(0xff, 0xff, 0xff));
    lv_style_set_bg_grad_color(&style_screen_list0_scrollable, lv_color_make(0xff, 0xff, 0xff));
    lv_style_set_bg_grad_dir(&style_screen_list0_scrollable, LV_GRAD_DIR_VER);
    lv_style_set_bg_opa(&style_screen_list0_scrollable, 255);
    lv_obj_add_style(file_list, &style_screen_list0_scrollable, LV_PART_MAIN);

    //Write style LV_BTN_PART_MAIN for screen_list0
    static lv_style_t style_screen_list0_main_child;
    lv_style_init(&style_screen_list0_main_child);

    //Write style state: LV_STATE_DEFAULT for style_screen_list0_main_child
    lv_style_set_radius(&style_screen_list0_main_child, 3);
    lv_style_set_bg_color(&style_screen_list0_main_child, lv_color_make(0xff, 0xff, 0xff));
    lv_style_set_bg_grad_color(&style_screen_list0_main_child, lv_color_make(0xff, 0xff, 0xff));
    lv_style_set_bg_grad_dir(&style_screen_list0_main_child, LV_GRAD_DIR_VER);
    lv_style_set_bg_opa(&style_screen_list0_main_child, 255);
    lv_style_set_text_color(&style_screen_list0_main_child, lv_color_make(0x0D, 0x30, 0x55));

    /* Let the list grow with its items; scrolling happens on the win content */
    lv_obj_set_width(file_list, 460);
    lv_obj_set_height(file_list, LV_SIZE_CONTENT);
    p = (Folder_btnArray *)malloc(sizeof(Folder_btnArray) + sizeof(int) * folder_cnt);
    p->folderlen = folder_cnt;

    q = (File_btnArray *)malloc(sizeof(File_btnArray) + sizeof(int) * file_cnt);
    q->filelen = file_cnt;

    for(int i = 0; i < p->folderlen; i++) {
        p->Folder_btn[i] = lv_list_add_btn(file_list, &image_folder_28,
                                           folder_name[i]);
        lv_obj_add_event_cb(p->Folder_btn[i], filelist_btnevent_handler, LV_EVENT_RELEASED, NULL);
    }
    for(int j = 0; j < q->filelen; j++) {
        q->File_btn[j] = lv_list_add_btn(file_list, &image_file_28, file_name[j]);
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
