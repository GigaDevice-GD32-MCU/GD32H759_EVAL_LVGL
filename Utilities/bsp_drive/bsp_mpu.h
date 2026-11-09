#ifndef __BSP_MPU_H
#define __BSP_MPU_H

#include "gd32h73x_75x.h"

/* D-Cache policy of the SDRAM region (0xC0000000, 32MB)
 * 0: write-through, no write-allocate        (TEX=0 C=1 B=0)
 * 1: write-back, no write-allocate           (TEX=0 C=1 B=1)
 * 2: write-back, read and write-allocate     (TEX=1 C=1 B=1)
 * With 1 and 2 the SDRAM may be older than the D-Cache: clean the D-Cache before
 * a DMA / IPA / TLI reads SDRAM, invalidate it after they write SDRAM. */
#define SDRAM_CACHE_WT          0
#define SDRAM_CACHE_WB          1
#define SDRAM_CACHE_WBWA        2

#ifndef SDRAM_CACHE_POLICY
#define SDRAM_CACHE_POLICY      SDRAM_CACHE_WT
#endif

void mpu_config(void);
void mpu_sdram_enable(void);
#endif