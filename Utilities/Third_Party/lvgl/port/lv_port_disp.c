/**
 * @file lv_port_disp.c
 *
 */

/*Copy this file as "lv_port_disp.c" and set this value to "1" to enable content*/
#if 1

/*********************
 *      INCLUDES
 *********************/
#include "lv_port_disp.h"
#include <stdbool.h>

#include "gd32h73x_75x.h"
#include "gd32h759i_lcd_eval.h"

/*********************
 *      DEFINES
 *********************/
#define LV_BUF_TYPE    2 /* 1 one buf 2 two buf 3 pingpong*/
#define USE_IPA_INT    1

/**********************
 *      TYPEDEFS
 **********************/
//extern uint16_t back_to_home;
/**********************
 *  STATIC PROTOTYPES
 **********************/
static void disp_init(void);
static void disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);
//static void gpu_fill(lv_disp_drv_t * disp_drv, lv_color_t * dest_buf, lv_coord_t dest_width,
//        const lv_area_t * fill_area, lv_color_t color);
static void disp_clean_dcache(lv_disp_drv_t * disp_drv);
/**********************
 *  STATIC VARIABLES
 **********************/
#if(LV_BUF_TYPE == 1)
/* TLI buf */
lv_color_t buf_disp[LV_HOR_RES_MAX * LV_VER_RES_MAX] __attribute__((section("SDRAM"))); 
/* LVGL buf */
#define LV_BUF_SIZE (LV_HOR_RES_MAX* 256/sizeof(lv_color_t))
lv_color_t buf_2_1[LV_BUF_SIZE] __attribute__((section("ADDSRAM")));
#elif(LV_BUF_TYPE==2)
/* TLI buf */
__ALIGNED(32) lv_color_t buf_disp[LV_HOR_RES_MAX * LV_VER_RES_MAX];
/* LVGL buf */
#define LV_BUF_SIZE (LV_HOR_RES_MAX * 300/sizeof(lv_color_t))
__ALIGNED(32) lv_color_t buf_2_1[LV_BUF_SIZE] ;
__ALIGNED(32) lv_color_t buf_2_2[LV_BUF_SIZE] ;
#else
/* LVGL buf  TLI buf */
#define LV_BUF_SIZE (LV_HOR_RES_MAX * LV_VER_RES_MAX)
static lv_color_t buf_2_1[LV_HOR_RES_MAX * LV_VER_RES_MAX] __attribute__((section("SDRAM")));
static lv_color_t buf_2_2[LV_HOR_RES_MAX * LV_VER_RES_MAX] __attribute__((section("SDRAM")));
#endif

#if (USE_IPA_INT == 1)
volatile uint8_t g_ipa_flag = 0;
static lv_disp_drv_t * g_disp_drv;
#endif
/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_port_disp_init(void)
{
    /*-------------------------
     * Initialize your display
     * -----------------------*/
    disp_init();

    /*-----------------------------
     * Create a buffer for drawing
     *----------------------------*/
    static lv_disp_draw_buf_t draw_buf_dsc;
#if(LV_BUF_TYPE==1)
    lv_disp_draw_buf_init(&draw_buf_dsc, buf_2_1, NULL, LV_BUF_SIZE);   /*Initialize the display buffer*/
#else
    lv_disp_draw_buf_init(&draw_buf_dsc, buf_2_1, buf_2_2, LV_BUF_SIZE);   /*Initialize the display buffer*/
#endif
    /*-----------------------------------
     * Register the display in LVGL
     *----------------------------------*/
    static lv_disp_drv_t disp_drv;                         /*Descriptor of a display driver*/
    lv_disp_drv_init(&disp_drv);                    /*Basic initialization*/

    /*Set up the functions to access to your display*/

    /*Set the resolution of the display*/
    disp_drv.hor_res = LV_HOR_RES_MAX;
    disp_drv.ver_res = LV_VER_RES_MAX;

    /*Used to copy the buffer's content to the display*/
    disp_drv.flush_cb = disp_flush;

    /*Set a display buffer*/
    disp_drv.draw_buf = &draw_buf_dsc;

    /*Required for Example 3)*/
#if(LV_BUF_TYPE==3)
    disp_drv.full_refresh = 1;
    //disp_drv.direct_mode = 1;
#endif /* LV_BUF_TYPE==3 */
    disp_drv.clean_dcache_cb = &disp_clean_dcache;

    /* Fill a memory array with a color if you have GPU.
     * Note that, in lv_conf.h you can enable GPUs that has built-in support in LVGL.
     * But if you have a different GPU you can use with this callback.*/
    //disp_drv.gpu_fill_cb = gpu_fill;

    /*Finally register the driver*/
    lv_disp_drv_register(&disp_drv);
    
#if(LV_BUF_TYPE == 3)
    nvic_irq_enable(TLI_IRQn, 2, 0);
#else
    nvic_irq_enable(IPA_IRQn, 2, 0);
#endif /* LV_BUF_TYPE==3 */
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/*Initialize your display and the required peripherals.*/
static void disp_init(void)
{
    /*You code here*/
    /* ipa init */
    rcu_periph_clock_enable(RCU_IPA);
    /* initilize the LCD and layers */
    lcd_init();
#if(LV_BUF_TYPE==3)
    lcd_layer_init(LCD_LAYER_BACKGROUND, (uint32_t)buf_2_1, LV_HOR_RES_MAX, LV_VER_RES_MAX, LAYER_PPF_RGB565);
#else
    lcd_layer_init(LCD_LAYER_BACKGROUND, (uint32_t)buf_disp, LV_HOR_RES_MAX, LV_VER_RES_MAX, LAYER_PPF_RGB565);
#endif
    lcd_layer_enable(LCD_LAYER_BACKGROUND);
    lcd_layer_set(LCD_LAYER_BACKGROUND,(uint32_t)buf_disp);
    lcd_transparency_set(255);
//    lcd_clear(LCD_COLOR_BLACK);
    tli_enable();
}

volatile bool disp_flush_enabled = true;
static volatile uint32_t completed_frame_count;

uint32_t lv_port_disp_get_fps(void)
{
    static uint32_t last_tick;
    static uint32_t last_frame_count;
    static uint32_t last_fps;
    uint32_t now = lv_tick_get();
    uint32_t elapsed = now - last_tick;

    /* Called again too soon: too few frames in the window to divide by, so the
     * result would be heavily quantised. Report the previous value instead of 0. */
    if(elapsed < 250U) return last_fps;

    uint32_t frames = completed_frame_count - last_frame_count;
    last_tick = now;
    last_frame_count = completed_frame_count;

    /* Nobody asked for a while, e.g. the performance page was hidden while the
     * music demo was running. This window spans seconds of unrelated activity,
     * so its average is meaningless. Drop the sample and start a fresh window. */
    if(elapsed > 1500U) return last_fps;

    last_fps = (frames * 1000U) / elapsed;
    return last_fps;
}

/* Enable updating the screen (the flushing process) when disp_flush() is called by LVGL
 */
void disp_enable_update(void)
{
    disp_flush_enabled = true;
}

/* Disable updating the screen (the flushing process) when disp_flush() is called by LVGL
 */
void disp_disable_update(void)
{
    disp_flush_enabled = false;
}

/* A callback for cleaning any caches related to the display
 */
static void disp_clean_dcache(lv_disp_drv_t * disp_drv)
{
#if __CORTEX_M >= 0x07
    SCB_CleanInvalidateDCache();
#endif
}

/*Flush the content of the internal buffer the specific area on the display
 *You can use DMA or any hardware acceleration to do this operation in the background but
 *'lv_disp_flush_ready()' has to be called when finished.*/
static void disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
//    disp_clean_dcache(disp_drv);
    /*The most simple case (but also the slowest) to put all pixels to the screen one-by-one*/
#if (USE_IPA_INT == 0)
    uint32_t time_out;
#endif
    uint16_t offline;
    offline = LV_HOR_RES_MAX - (area->x2 - area->x1 + 1);

    if(lv_disp_flush_is_last(disp_drv)) {
        completed_frame_count++;
    }
    
    while(1 == g_ipa_flag){
    }
    
    IPA_CTL = 0x0;
    IPA_FMADDR = (uint32_t)color_p;
    IPA_DMADDR = (uint32_t)buf_disp + 2*(LV_HOR_RES_MAX*area->y1+area->x1);
    IPA_FLOFF = 0;
    IPA_DLOFF = offline;
    IPA_FPCTL = FOREGROUND_PPF_RGB565;
    IPA_IMS = (uint32_t)((area->x2 - area->x1 + 1) << 16) | (uint16_t)(area->y2 - area->y1 + 1);
    
#if (USE_IPA_INT == 1)
    g_disp_drv = disp_drv;
    /* enable IPA interrupt */
    g_ipa_flag = 1;
    IPA_CTL |= IPA_CTL_FTFIE | IPA_CTL_TAEIE | IPA_CTL_WCFIE;
    IPA_CTL |= IPA_CTL_TEN;
    lv_disp_flush_ready(g_disp_drv);
#else
    IPA_CTL |= IPA_CTL_TEN;
    /* Wait until transfer is done */
    while (IPA_CTL & IPA_CTL_TEN)
    {
        if(time_out++ >= 0XFFFFFFFF) break;
    }

    /* IMPORTANT!!!
     * Inform the graphics library that you are ready with the flushing*/
    lv_disp_flush_ready(disp_drv);
#endif

}

#if (USE_IPA_INT == 1)

/*!
    \brief      this function handles IPA exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void IPA_IRQHandler(void)
{
    if(RESET != ipa_interrupt_flag_get(IPA_INT_FLAG_FTF)){
        if(0U != (IPA_CTL & IPA_CTL_FTFIE)){
            IPA_CTL &= (uint32_t)(~IPA_CTL_FTFIE);
            ipa_interrupt_flag_clear(IPA_INT_FLAG_FTF);
            
            if(1U == g_ipa_flag){
                g_ipa_flag = 0U;
                /* IMPORTANT!!!
                 * Inform the graphics library that you are ready with the flushing*/
                lv_disp_flush_ready(g_disp_drv);
            }
        }
    }
}

#endif

/*OPTIONAL: GPU INTERFACE*/


#else /*Enable this file at the top*/

/*This dummy typedef exists purely to silence -Wpedantic.*/
typedef int keep_pedantic_happy;
#endif
