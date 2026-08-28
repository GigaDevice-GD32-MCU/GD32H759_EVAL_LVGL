/*!
    \file    main.c
    \brief   TLI_IPA demo

    \version 2023-01-20, V0.1.0, demo for GD32H73x_75x
*/

/*
    Copyright (c) 2023, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors
       may be used to endorse or promote products derived from this software without
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
OF SUCH DAMAGE.
*/

/* Lib*/
#include "gd32h73x_75x.h"
#include "gd32h759i_eval.h"
#include "gd32h759i_lcd_eval.h"
#include "gd32h73x_75x_rcu.h"
/* system Config*/
#include "sys_config.h"
/* bsp*/
#include "systick.h"
#include "exmc_sdram.h"
#include "bsp_adc.h"
#include "bsp_mpu.h"
/* lvgl*/
#if SYS_configUSE_LVGL
    #include "lvgl.h"
    #include "lv_port_disp.h"
    #include "lv_port_indev.h"
    #include "lv_demos.h"
#endif
/* FatFs*/
#if SYS_configUSE_FAT32FS
    #include "ff.h"
    /* Defined in Template/App/fatfs_file_browse.c, declared here so that main.c
       does not need the App folder on its include path. */
    extern FRESULT scanfiles_test(void);
#endif

/* FreeRTOS*/
#if SYS_configUSE_FREERTOS
    #include "taskmain.h"
#endif

uint16_t back_to_home = 1;
float g_temperature = 0;

static void cache_enable(void);
extern void lv_demo_app(void);
void RCU_PLLX_Config(void);

/* configure the ADC peripheral */
void adc_config(void);

/*!
    \brief      main program
    \param[in]  none
    \param[out] none
    \retval     none
*/


int main(void)
{
    /* configure  SDIO and EXMC peripherals clock;SDIO : 300Mhz,  EXMC: 166Mhz*/
    RCU_PLLX_Config();
    /* enable the CPU Cache */
    cache_enable();
    mpu_config();
    /* configure Systick and LCD */
    systick_config();
   /* ADC configuration */
    adc_config();

#if SYS_configUSE_LVGL
    lv_init();
    lv_port_disp_init();
    lv_port_indev_init();
#endif


 #if SYS_configUSE_FAT32FS
     lv_fs_fatfs_init();
     /* Mount the card once here, while no LVGL timer or RTOS task is running
        yet. sd_init() reconfigures PLL1 and takes a long time, doing it from
        inside a button callback is much less reliable. */
     scanfiles_test();
 #endif

 #if SYS_configUSE_LVGL
 #if LV_USE_DEMO_MUSIC
     lv_demo_music(0);
 #endif
     lv_demo_app();
 #endif

#if SYS_configUSE_WATCHDOG
    dbg_periph_enable(DBG_FWDGT_HOLD);
    /* enable write access to FWDGT_PSC and FWDGT_RLD registers.
       FWDGT counter clock: 40KHz(IRC40K) / 64 = 0.625 KHz.
       FWDGT timeout is set to 1s (the timeout may varies due to 
                                   IRC40K frequency dispersion). */
    fwdgt_config(0xFFF, FWDGT_PSC_DIV256);
    fwdgt_enable();
#endif

#if SYS_configUSE_FREERTOS
    taskmainInit();
#endif

    while(1){
 #if !SYS_configUSE_FREERTOS
         delay_1ms(5);
 #if SYS_configUSE_LVGL
         lv_task_handler();
 #endif

 #if SYS_configUSE_WATCHDOG
         fwdgt_counter_reload();
 #endif
       /* value convert */
       g_temperature = Get_temperature();
#endif
    }
}

/*!
    \brief      enable the CPU Cache
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void cache_enable(void)
{
    /* Enable I-Cache */
    SCB_EnableICache();

    /* Enable D-Cache */
    SCB_EnableDCache();
}

/*!
    \brief      Configure PLL as the clock source for SDIO and EXMC peripherals at all times.
    \param[in]  none
    \param[out] none
    \retval     none
*/
void RCU_PLLX_Config(void)
{
    /* PLL1R as EXMC source clock */
    rcu_pll_source_config(RCU_PLLSRC_HXTAL);
    rcu_pll1_config(HXTAL_VALUE/1000000, 332, 1, 1, 1);
    rcu_pll_clock_output_enable(RCU_PLL1R);
    rcu_osci_on(RCU_PLL1_CK);
    while(ERROR == rcu_osci_stab_wait(RCU_PLL1_CK)){
    }
    rcu_exmc_clock_config(RCU_EXMCSRC_PLL1R);
    
    /* SDIO clock  RCU_SDIO0SRC_PLL0Q = 300Mhz  */
    rcu_sdio_clock_config(IDX_SDIO0, RCU_SDIO0SRC_PLL0Q);
}

/*!
    \brief      Configure PLL as the clock source for SDIO and EXMC peripherals at all times.
    \param[in]  none
    \param[out] none
    \retval     none
*/

void __aeabi_assert(const char *ptr1, const char *ptr2, int val)
{
}
