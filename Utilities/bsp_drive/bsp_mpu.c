#include "bsp_MPU.h"

/*!
    \brief      configure the MPU
    \param[in]  none
    \param[out] none
    \retval     none
*/
void mpu_config(void)
{
    mpu_region_init_struct mpu_init_struct;
    mpu_region_struct_para_init(&mpu_init_struct);

    /* disable the MPU */
    ARM_MPU_Disable();
    ARM_MPU_SetRegion(0, 0);

    /* configure the MPU attributes for Reserved, no access */
    mpu_init_struct.region_base_address  = 0x0;
    mpu_init_struct.region_size          = MPU_REGION_SIZE_4GB;
    mpu_init_struct.access_permission    = MPU_AP_NO_ACCESS;
    mpu_init_struct.access_bufferable    = MPU_ACCESS_NON_BUFFERABLE;
    mpu_init_struct.access_cacheable     = MPU_ACCESS_NON_CACHEABLE;
    mpu_init_struct.access_shareable     = MPU_ACCESS_SHAREABLE;
    mpu_init_struct.region_number        = MPU_REGION_NUMBER0;
    mpu_init_struct.subregion_disable    = 0x87;
    mpu_init_struct.instruction_exec     = MPU_INSTRUCTION_EXEC_NOT_PERMIT;
    mpu_init_struct.tex_type             = MPU_TEX_TYPE0;
    mpu_region_config(&mpu_init_struct);
    mpu_region_enable();

    /* configure the MPU attributes for SRAM */
    mpu_init_struct.region_base_address  = 0x24000000;
    mpu_init_struct.region_size          = MPU_REGION_SIZE_1MB;
    mpu_init_struct.access_permission    = MPU_AP_FULL_ACCESS;
    mpu_init_struct.access_bufferable    = MPU_ACCESS_NON_BUFFERABLE;
    mpu_init_struct.access_cacheable     = MPU_ACCESS_CACHEABLE;
    mpu_init_struct.access_shareable     = MPU_ACCESS_NON_SHAREABLE;
    mpu_init_struct.region_number        = MPU_REGION_NUMBER1;
    mpu_init_struct.subregion_disable    = MPU_SUBREGION_ENABLE;
    mpu_init_struct.instruction_exec     = MPU_INSTRUCTION_EXEC_PERMIT;
    mpu_init_struct.tex_type             = MPU_TEX_TYPE0;
    mpu_region_config(&mpu_init_struct);
    mpu_region_enable();

    /* The SDRAM region stays covered by the no access region 0 until mpu_sdram_enable():
     * the Cortex-M7 may read Normal memory speculatively, and a read of the SDRAM before
     * the EXMC is initialized never completes and stalls the whole bus matrix (the
     * debugger can't connect any more either). */

    /* enable the MPU */
    ARM_MPU_Enable(MPU_MODE_PRIV_DEFAULT);
}

/*!
    \brief      make the SDRAM (0xC0000000, 32MB) accessible, call it only after
                exmc_synchronous_dynamic_ram_init()
    \param[in]  none
    \param[out] none
    \retval     none
*/
void mpu_sdram_enable(void)
{
    mpu_region_init_struct mpu_init_struct;
    mpu_region_struct_para_init(&mpu_init_struct);

    ARM_MPU_Disable();

    mpu_init_struct.region_base_address = 0xC0000000;
    mpu_init_struct.region_size         = MPU_REGION_SIZE_32MB;
    mpu_init_struct.access_permission   = MPU_AP_FULL_ACCESS;
#if (SDRAM_CACHE_POLICY == SDRAM_CACHE_WT)
    mpu_init_struct.access_bufferable   = MPU_ACCESS_NON_BUFFERABLE;
    mpu_init_struct.tex_type            = MPU_TEX_TYPE0;
#elif (SDRAM_CACHE_POLICY == SDRAM_CACHE_WB)
    mpu_init_struct.access_bufferable   = MPU_ACCESS_BUFFERABLE;
    mpu_init_struct.tex_type            = MPU_TEX_TYPE0;
#elif (SDRAM_CACHE_POLICY == SDRAM_CACHE_WBWA)
    mpu_init_struct.access_bufferable   = MPU_ACCESS_BUFFERABLE;
    mpu_init_struct.tex_type            = MPU_TEX_TYPE1;
#else
#error "SDRAM_CACHE_POLICY must be SDRAM_CACHE_WT, SDRAM_CACHE_WB or SDRAM_CACHE_WBWA"
#endif
    mpu_init_struct.access_cacheable    = MPU_ACCESS_CACHEABLE;
    mpu_init_struct.access_shareable    = MPU_ACCESS_NON_SHAREABLE;
    mpu_init_struct.region_number       = MPU_REGION_NUMBER2;
    mpu_init_struct.subregion_disable   = MPU_SUBREGION_ENABLE;
    mpu_init_struct.instruction_exec    = MPU_INSTRUCTION_EXEC_NOT_PERMIT;
    mpu_region_config(&mpu_init_struct);
    mpu_region_enable();

    /* enable the MPU */
    ARM_MPU_Enable(MPU_MODE_PRIV_DEFAULT);
}