/*!
    \file    gd32h75e_esc_pmu.h
    \brief   definitions for the ESC PMU

    \version 2026-07-30, V1.7.0, firmware for GD32H75E
*/

/*
    Copyright (c) 2026, GigaDevice Semiconductor Inc.

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

#include "gd32h73x_75x.h"

#ifndef GD32H75E_ESC_PMU_H
#define GD32H75E_ESC_PMU_H

/* ESC_PMU definitions */
#define ESC_PMU                        (uint32_t)0x00003700U                        /*!< ESC_PMU base address */

/* registers definitions */
#define ESC_PMU_CTL0                   (uint32_t)((ESC_PMU) + 0x00000000U)          /*!< ESC_PMU control register 0 */
#define ESC_PMU_CTL1                   (uint32_t)((ESC_PMU) + 0x00000004U)          /*!< ESC_PMU control register 1 */
#define ESC_PMU_PDIREFVAL              (uint32_t)((ESC_PMU) + 0x0000001CU)          /*!< ESC_PMU process data interface reference value register */

/* bits definitions */
/* ESC_PMU_CTL0 */
#define ESC_PMU_CTL0_RDY               BIT(0)                                       /*!< device ready bit */
#define ESC_PMU_CTL0_EDWOLAEN          BIT(14)                                      /*!< energy detect / WoL port A enable bit */
#define ESC_PMU_CTL0_EDWOLBEN          BIT(15)                                      /*!< energy detect / WoL port B enable bit */
#define ESC_PMU_CTL0_EDWOLASTAT        BIT(16)                                      /*!< energy detect / WoL port A status bit */
#define ESC_PMU_CTL0_EDWOLBSTAT        BIT(17)                                      /*!< energy detect / WoL port B status bit */
#define ESC_PMU_CTL0_ECATCLKDIS        BIT(21)                                      /*!< EtherCAT core clock disable bit */
#define ESC_PMU_CTL0_LEDINACT          BIT(24)                                      /*!< push-pull mode LEDs inactive state configuration bit (take effect only when LEDOUTDIS is set) */
#define ESC_PMU_CTL0_LEDMODCFG         BIT(25)                                      /*!< LEDs working mode configuration bit (take effect only when LEDOUTDIS is set) */
#define ESC_PMU_CTL0_LEDOUTDIS         BIT(26)                                      /*!< LEDs output disabled bit */
#define ESC_PMU_CTL0_PMWUPCFG          BIT(27)                                      /*!< power management wake up mode configuration bit */
#define ESC_PMU_CTL0_PMMODCFG          BITS(30, 31)                                 /*!< power management mode configuration bits */

/* ESC_PMU_CTL1 */
#define ESC_PMU_CTL1_PMSLPEN           BIT(0)                                       /*!< power management sleep mode enable bit */

/* ESC_PMU_PDIREFVAL */
#define ESC_PMU_PDIREFVAL_PDIVAL       BITS(0, 31)                                  /*!< process data interface reference value bits */

/* PMU function definition */
typedef enum {
    ESC_PMU_FUN_EDWOLA                  = ESC_PMU_CTL0_EDWOLAEN,                    /*!< PMU energy detect / WoL port A */
    ESC_PMU_FUN_EDWOLB                  = ESC_PMU_CTL0_EDWOLBEN,                    /*!< PMU energy detect / WoL port B */
    ESC_PMU_FUN_ECATCLK                 = ESC_PMU_CTL0_ECATCLKDIS,                  /*!< PMU EtherCAT core clock */
    ESC_PMU_FUN_LEDOUT                  = ESC_PMU_CTL0_LEDOUTDIS                    /*!< PMU LEDs output */
} pmu_esc_fun_enum;

/* PMU command constants definitions */
/* power management mode */
#define ESC_PMU_CTL0_PMMODCFG_VAL(regval)  (BITS(30, 31) & ((uint32_t)(regval) << 30U))
#define ESC_PMU_PMMOD0                     ESC_PMU_CTL0_PMMODCFG_VAL(0)             /*!< power management mode 0 */
#define ESC_PMU_PMMOD1                     ESC_PMU_CTL0_PMMODCFG_VAL(1)             /*!< power management mode 1 */
#define ESC_PMU_PMMOD2                     ESC_PMU_CTL0_PMMODCFG_VAL(2)             /*!< power management mode 2 */
#define ESC_PMU_PMMOD3                     ESC_PMU_CTL0_PMMODCFG_VAL(3)             /*!< power management mode 3 */

/* PMU wake up mode */
#define ESC_PMU_WAKE_UP_MASTER             ((uint32_t)0x00000000U)                  /*!< PMU wake up by master */
#define ESC_PMU_WAKE_UP_PME_MASTER         ((uint32_t)0x00000001U)                  /*!< PMU wake up by PME or master */

/* PMU LED work mode */
#define ESC_PMU_LED_OPEN_DRAIN             ((uint32_t)0x00000000U)                  /*!< the working mode of LEDs is open-drain / open-source */
#define ESC_PMU_LED_PUSH_PULL              ((uint32_t)0x00000001U)                  /*!< the working mode of LEDs is push-pull */

/* PMU LED inactive state */
#define ESC_PMU_LED_INACT_STAT0            ((uint32_t)0x00000000U)                  /*!< 0 is inactive state */
#define ESC_PMU_LED_INACT_STAT1            ((uint32_t)0x00000001U)                  /*!< 1 is inactive state */

/* PMU energy detect / WoL port */
#define ESC_PMU_EDWOL_PORTA                ((uint32_t)0x00000000U)                  /*!< energy detect / WoL port A */
#define ESC_PMU_EDWOL_PORTB                ((uint32_t)0x00000001U)                  /*!< energy detect / WoL port B */

/* function declarations */
/* configure power management mode */
void pmu_esc_power_mang_mode_config(uint32_t pm_mode);
/* configure PMU wake up mode */
void pmu_esc_wake_up_mode_config(uint32_t wu_mode);
/* configure PMU wake up mode */
void pmu_esc_led_wm_config(uint32_t led_wm);
/* configure PMU LED inactive state */
void pmu_esc_led_inact_stat_config(uint32_t led_inact_stat);
/* get device ready */
FlagStatus pmu_esc_ready_stat_get(void);
/* get energy detect / WoL port status */
FlagStatus pmu_esc_edwol_stat_get(uint32_t edwol_port);
/* clear energy detect / WoL port status */
void pmu_esc_edwol_stat_clear(uint32_t edwol_port);
/* configure PMU function */
void pmu_esc_fun_config(pmu_esc_fun_enum fun_type, ControlStatus newvalue);
/* test PMU byte */
FlagStatus pmu_esc_byte_test(void);
/* enable power management sleep mode */
void pmu_esc_sleep_mode_enable(void);
/* disable power management sleep mode */
void pmu_esc_sleep_mode_disable(void);

#endif /* GD32H75E_ESC_PMU_H */
