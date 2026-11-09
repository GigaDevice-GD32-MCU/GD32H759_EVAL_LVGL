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
#define BYTE_PER_PIXEL (LV_COLOR_FORMAT_GET_SIZE(LV_COLOR_FORMAT_RGB565)) /* 2 for RGB565 */

#define FLUSH_BY_IPA   LV_FLUSH_USE_IPA

/* With LV_USE_DRAW_IPA the draw unit shares the IPA with the flush copy: it waits for the
 * flush transfer through lv_draw_ipa_wait_external_idle() and polls its own transfers
 * (LV_OS_NONE). The interrupt driven (RTOS) draw unit would need the IRQ to be shared too. */
#if LV_USE_DRAW_IPA && FLUSH_BY_IPA && defined(LV_USE_DRAW_IPA_INTERRUPT) && LV_USE_DRAW_IPA_INTERRUPT
    #error "LV_FLUSH_USE_IPA 1 can't be used with LV_USE_DRAW_IPA_INTERRUPT 1"
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void disp_init(void);
static void disp_flush(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map);

/**********************
 *  STATIC VARIABLES
 **********************/
/* TLI buf */
__ALIGNED(32) uint8_t buf_disp[LV_HOR_RES_MAX * LV_VER_RES_MAX * BYTE_PER_PIXEL];
/* LVGL buf: LV_BUF_LINES lines each */
#define LV_BUF_SIZE (LV_HOR_RES_MAX * LV_BUF_LINES * BYTE_PER_PIXEL)
__ALIGNED(32) uint8_t buf_2_1[LV_BUF_SIZE];
__ALIGNED(32) uint8_t buf_2_2[LV_BUF_SIZE];

#if FLUSH_BY_IPA
volatile uint8_t g_ipa_flag = 0;
static lv_display_t * volatile g_disp_drv;
#endif

volatile bool disp_flush_enabled = true;
static volatile uint32_t completed_frame_count;

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

    /*------------------------------------
     * Create a display and set a flush_cb
     * -----------------------------------*/
    lv_display_t * disp = lv_display_create(LV_HOR_RES_MAX, LV_VER_RES_MAX);
    lv_display_set_color_format(disp, LV_COLOR_FORMAT_RGB565);
    lv_display_set_flush_cb(disp, disp_flush);

    /*-----------------------------
     * Create a buffer for drawing
     *----------------------------*/
    lv_display_set_buffers(disp, buf_2_1, buf_2_2, sizeof(buf_2_1), LV_DISPLAY_RENDER_MODE_PARTIAL);
#if FLUSH_BY_IPA
    nvic_irq_enable(IPA_IRQn, 2, 0);
#endif
}

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

/**********************
 *   STATIC FUNCTIONS
 **********************/

/*Initialize your display and the required peripherals.*/
static void disp_init(void)
{
#if FLUSH_BY_IPA
    /* the IPA copies the rendered areas into the TLI frame */
    rcu_periph_clock_enable(RCU_IPA);
#endif
    /* initilize the LCD and layers */
    lcd_init();
    lcd_layer_init(LCD_LAYER_BACKGROUND, (uint32_t)buf_disp, LV_HOR_RES_MAX, LV_VER_RES_MAX, LAYER_PPF_RGB565);
    lcd_layer_enable(LCD_LAYER_BACKGROUND);
    lcd_layer_set(LCD_LAYER_BACKGROUND, (uint32_t)buf_disp);
    lcd_transparency_set(255);
    tli_enable();
}

/*Flush the content of the internal buffer the specific area on the display.
 *`px_map` contains the rendered image as raw pixel map and it should be copied to `area` on the display.
 *You can use DMA or any hardware acceleration to do this operation in the background but
 *'lv_display_flush_ready()' has to be called when it's finished.*/
static void disp_flush(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map)
{
#if !FLUSH_BY_IPA
    /* CPU copy into the TLI frame. The IPA draw unit invalidates the D-Cache lines of
     * what it rendered, so px_map can be read directly; the frame lines written here
     * are cleaned so that the TLI sees them. */
    const int32_t w_bytes = (area->x2 - area->x1 + 1) * BYTE_PER_PIXEL;
    const uint8_t * src = px_map;
    uint8_t * dst = buf_disp + BYTE_PER_PIXEL * (LV_HOR_RES_MAX * area->y1 + area->x1);
    int32_t y;

    for(y = area->y1; y <= area->y2; y++) {
        lv_memcpy(dst, src, w_bytes);
        src += w_bytes;
        dst += LV_HOR_RES_MAX * BYTE_PER_PIXEL;
    }
#if __CORTEX_M >= 0x07
    {
        uint32_t start = ((uint32_t)buf_disp + BYTE_PER_PIXEL * LV_HOR_RES_MAX * area->y1) & ~31U;
        uint32_t end = (uint32_t)buf_disp + BYTE_PER_PIXEL * LV_HOR_RES_MAX * (area->y2 + 1);
        SCB_CleanDCache_by_Addr((uint32_t *)start, (int32_t)(end - start));
    }
#endif
    if(lv_display_flush_is_last(disp)) {
        completed_frame_count++;
    }
    lv_display_flush_ready(disp);
#else
    uint16_t offline;
    offline = LV_HOR_RES_MAX - (area->x2 - area->x1 + 1);

#if __CORTEX_M >= 0x07
    /* AXI SRAM is write-through (bsp_mpu.c): only wait for the stores to complete */
    __DSB();
#endif

    if(lv_display_flush_is_last(disp)) {
        completed_frame_count++;
    }

    while(1 == g_ipa_flag) {
    }

    IPA_CTL = 0x0;
    /* clear the flags left by the previous transfer */
    IPA_INTC = 0x3FU;
    IPA_FMADDR = (uint32_t)px_map;
    IPA_DMADDR = (uint32_t)buf_disp + BYTE_PER_PIXEL * (LV_HOR_RES_MAX * area->y1 + area->x1);
    IPA_FLOFF = 0;
    IPA_DLOFF = offline;
    IPA_FPCTL = FOREGROUND_PPF_RGB565;
    IPA_DPCTL = (IPA_DPCTL & ~0x07U) | (uint32_t)IPA_DPF_RGB565;
    IPA_IMS = (uint32_t)((area->x2 - area->x1 + 1) << 16) | (uint16_t)(area->y2 - area->y1 + 1);

    g_disp_drv = disp;
    /* enable IPA interrupt */
    g_ipa_flag = 1;
    IPA_CTL |= IPA_CTL_FTFIE | IPA_CTL_TAEIE | IPA_CTL_WCFIE;
    IPA_CTL |= IPA_CTL_TEN;
    /* With two buffers LVGL renders into the other buffer meanwhile; the next
     * flush waits for this transfer through g_ipa_flag. */
    lv_display_flush_ready(disp);
#endif
}

#if FLUSH_BY_IPA

#if LV_USE_DRAW_IPA
/* Called by the IPA draw unit before it programs the IPA: let the flush copy finish */
void lv_draw_ipa_wait_external_idle(void)
{
    while(1 == g_ipa_flag) {
    }
}
#endif

/*!
    \brief      this function handles IPA exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void IPA_IRQHandler(void)
{
    /* the copy ends with FTF, or with TAE / WCF and no FTF; disable and clear all three so
     * that none of them stays pending or enabled for the next transfer */
    uint32_t flags = IPA_INTF & (IPA_INTF_TAEIF | IPA_INTF_FTFIF | IPA_INTF_WCFIF);

    if(0U == flags) {
        return;
    }
    IPA_CTL &= ~(IPA_CTL_FTFIE | IPA_CTL_TAEIE | IPA_CTL_WCFIE);
    IPA_INTC = flags;

    if(1U == g_ipa_flag) {
        g_ipa_flag = 0U;
        /* IMPORTANT!!!
         * Inform the graphics library that you are ready with the flushing*/
        lv_display_flush_ready(g_disp_drv);
    }
}

#endif

#else /*Enable this file at the top*/

/*This dummy typedef exists purely to silence -Wpedantic.*/
typedef int keep_pedantic_happy;
#endif
