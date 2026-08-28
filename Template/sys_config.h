#ifndef SYS_CONFIG_H
#define SYS_CONFIG_H

/* ========== System Component Configuration ========== */

/* FreeRTOS real-time operating system: 1=Enable, 0=Disable */
#define SYS_configUSE_FREERTOS			0

/* FAT32 file system support: 1=Enable, 0=Disable */
#define SYS_configUSE_FAT32FS			1

/* ========== GUI Library Selection (Mutually Exclusive) ========== */
/* Note: Only ONE GUI library can be enabled at a time */
#define SYS_configUSE_LVGL				1   
#define SYS_configUSE_EMWIN				0  
/* Check for mutual exclusion */
#if (SYS_configUSE_LVGL == 1) && (SYS_configUSE_EMWIN == 1)
    #error "Cannot enable both LVGL and EMWIN at the same time! Set one to 0."
#endif

/* ========== System Protection Configuration ========== */

/* Independent watchdog (FWDGT): 1=Enable, 0=Disable */
/* Note: Watchdog must be fed regularly to prevent system reset */
#define SYS_configUSE_WATCHDOG          0


#endif