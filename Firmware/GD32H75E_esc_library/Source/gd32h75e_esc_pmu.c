/*!
    \file    gd32h75e_esc_pmu.c
    \brief   ESC pmu driver basic configuration

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
#include "gd32h75e_esc_ospi.h"
#include "gd32h75e_esc_pmu.h"

#define BYTE_TEST_TIMEOUT           (0x000FFFFFU)

/*!
    \brief      read PMU ESC register value
    \param[in]  reg: PMU ESC register
                only one parameter can be selected which is shown as below:
      \arg        ESC_PMU_CTL0: ESC_PMU control register 0
      \arg        ESC_PMU_CTL1: ESC_PMU control register 1
      \arg        ESC_PMU_PDIREFVAL: ESC_PMU process data interface reference value register
    \param[out] none
    \retval     value of PMU ESC register: 0~0xFFFFFFFF
*/
static uint32_t pmu_esc_read(uint32_t reg)
{
    uint32_t temp_value = 0;
    ospi_read(reg, (uint8_t *)&temp_value, 4);
    return temp_value;
}

/*!
    \brief      write PMU ESC register value
    \param[in]  reg: PMU ESC register
                only one parameter can be selected which is shown as below:
      \arg        ESC_PMU_CTL0: ESC_PMU control register 0
      \arg        ESC_PMU_CTL1: ESC_PMU control register 1
      \arg        ESC_PMU_PDIREFVAL: ESC_PMU process data interface reference value register
    \param[in]  value: the value need write to PMU ESC register
    \param[out] none
    \retval     none
*/
static void pmu_esc_write(uint32_t reg, uint32_t value)
{
    ospi_write(reg, (uint8_t *)&value, 4);
}

/*!
    \brief      configure power management mode (API_ID(0x0001U))
    \param[in]  pm_mode:
                only one parameter can be selected which is shown as below:
      \arg        ESC_PMU_PMMOD0: power management mode 0
      \arg        ESC_PMU_PMMOD1: power management mode 1
      \arg        ESC_PMU_PMMOD2: power management mode 2
      \arg        ESC_PMU_PMMOD3: power management mode 3
    \param[out] none
    \retval     none
*/
void pmu_esc_power_mang_mode_config(uint32_t pm_mode)
{
    uint32_t temp_value = 0;

    temp_value = pmu_esc_read(ESC_PMU_CTL0);
    temp_value &= ~ESC_PMU_CTL0_PMMODCFG;
    temp_value |= (pm_mode & ESC_PMU_CTL0_PMMODCFG);
    pmu_esc_write(ESC_PMU_CTL0, temp_value);

    /* enable power management sleep mode */
    pmu_esc_write(ESC_PMU_CTL1, ESC_PMU_CTL1_PMSLPEN);
}

/*!
    \brief      configure PMU wake up mode (API_ID(0x0002U))
    \param[in]  wu_mode:
                only one parameter can be selected which is shown as below:
      \arg        ESC_PMU_WAKE_UP_MASTER: PMU wake up by master
      \arg        ESC_PMU_WAKE_UP_PME_MASTER: PMU wake up by PME or master
    \param[out] none
    \retval     none
*/
void pmu_esc_wake_up_mode_config(uint32_t wu_mode)
{
    uint32_t temp_value = pmu_esc_read(ESC_PMU_CTL0);

    if(ESC_PMU_WAKE_UP_MASTER == wu_mode) {
        temp_value &= ~ESC_PMU_CTL0_PMWUPCFG;
    } else if(ESC_PMU_WAKE_UP_PME_MASTER == wu_mode) {
        temp_value |= ESC_PMU_CTL0_PMWUPCFG;
    } else {
        /* illegal parameters */
    }
    pmu_esc_write(ESC_PMU_CTL0, temp_value);
}

/*!
    \brief      configure PMU LED work mode (API_ID(0x0003U))
    \param[in]  led_wm:
                only one parameter can be selected which is shown as below:
      \arg        ESC_PMU_LED_OPEN_DRAIN: the working mode of LEDs is open-drain / open-source
      \arg        ESC_PMU_LED_PUSH_PULL: the working mode of LEDs is push-pull
    \param[out] none
    \retval     none
*/
void pmu_esc_led_wm_config(uint32_t led_wm)
{
    uint32_t temp_value = pmu_esc_read(ESC_PMU_CTL0);

    if(ESC_PMU_LED_OPEN_DRAIN == led_wm) {
        temp_value &= ~ESC_PMU_CTL0_LEDMODCFG;
    } else if(ESC_PMU_LED_PUSH_PULL == led_wm) {
        temp_value |= ESC_PMU_CTL0_LEDMODCFG;
    } else {
        /* illegal parameters */
    }
    pmu_esc_write(ESC_PMU_CTL0, temp_value);
}

/*!
    \brief      configure PMU LED inactive state (API_ID(0x0004U))
    \param[in]  led_inact_stat: LED inactive state
                only one parameter can be selected which is shown as below:
      \arg        ESC_PMU_LED_INACT_STAT0: 0 is inactive state
      \arg        ESC_PMU_LED_INACT_STAT1: 1 is inactive state
    \param[out] none
    \retval     none
*/
void pmu_esc_led_inact_stat_config(uint32_t led_inact_stat)
{
    uint32_t temp_value = pmu_esc_read(ESC_PMU_CTL0);

    if(ESC_PMU_LED_INACT_STAT0 == led_inact_stat) {
        temp_value &= ~ESC_PMU_CTL0_LEDINACT;
    } else if(ESC_PMU_LED_INACT_STAT1 == led_inact_stat) {
        temp_value |= ESC_PMU_CTL0_LEDINACT;
    } else {
        /* illegal parameters */
    }
    pmu_esc_write(ESC_PMU_CTL0, temp_value);
}

/*!
    \brief      get device ready (API_ID(0x0008U))
    \param[in]  none
    \param[out] none
    \retval     FlagStatus: SET or RESET
*/
FlagStatus pmu_esc_ready_stat_get(void)
{
    FlagStatus reval = RESET;
    uint32_t temp_value = 0;

    temp_value = pmu_esc_read(ESC_PMU_CTL0);

    if(temp_value & ESC_PMU_CTL0_RDY) {
        reval = SET;
    } else {
        reval = RESET;
    }

    return reval;
}

/*!
    \brief      get energy detect / WoL port status (API_ID(0x0005U))
    \param[in]  edwol_port: energy detect / WoL port
                only one parameter can be selected which is shown as below:
      \arg        ESC_PMU_EDWOL_PORTA: energy detect / WoL portA
      \arg        ESC_PMU_EDWOL_PORTB: energy detect / WoL portB
    \param[out] none
    \retval     FlagStatus: SET or RESET
*/
FlagStatus pmu_esc_edwol_stat_get(uint32_t edwol_port)
{
    FlagStatus reval = RESET;
    uint32_t temp_value = 0;

    temp_value = pmu_esc_read(ESC_PMU_CTL0);

    switch(edwol_port) {
    /* energy detect / WoL portA */
    case ESC_PMU_EDWOL_PORTA:
        if(temp_value & ESC_PMU_CTL0_EDWOLASTAT) {
            reval = SET;
        } else {
            reval = RESET;
        }
        break;

    /* energy detect / WoL portB */
    case ESC_PMU_EDWOL_PORTB:
        if(temp_value & ESC_PMU_CTL0_EDWOLBSTAT) {
            reval = SET;
        } else {
            reval = RESET;
        }
        break;

    default:
        reval = RESET;
        break;
    }
    return reval;
}

/*!
    \brief      clear energy detect / WoL port status (API_ID(0x0006U))
    \param[in]  edwol_port: energy detect / WoL port
                only one parameter can be selected which is shown as below:
      \arg        ESC_PMU_EDWOL_PORTA: energy detect / WoL portA
      \arg        ESC_PMU_EDWOL_PORTB: energy detect / WoL portB
    \param[out] none
    \retval     none
*/
void pmu_esc_edwol_stat_clear(uint32_t edwol_port)
{
    uint32_t temp_value = 0;

    temp_value = pmu_esc_read(ESC_PMU_CTL0);

    switch(edwol_port) {
    /* energy detect / WoL portA */
    case ESC_PMU_EDWOL_PORTA:
        temp_value |= ESC_PMU_CTL0_EDWOLASTAT;
        pmu_esc_write(ESC_PMU_CTL0, temp_value);
        break;

    /* energy detect / WoL portB */
    case ESC_PMU_EDWOL_PORTB:
        temp_value |= ESC_PMU_CTL0_EDWOLBSTAT;
        pmu_esc_write(ESC_PMU_CTL0, temp_value);
        break;

    default:
        break;
    }
}

/*!
    \brief      configure PMU function (API_ID(0x0007U))
    \param[in]  fun_type: PMU fuction, refer to pmu_esc_fun_enum
                only one parameter can be selected which is shown as below:
      \arg        ESC_PMU_FUN_EDWOLA: PMU energy detect / WoL port A
      \arg        ESC_PMU_FUN_EDWOLB: PMU energy detect / WoL port B
      \arg        ESC_PMU_FUN_ECATCLK: PMU EtherCAT core clock
      \arg        ESC_PMU_FUN_LEDOUT: PMU LEDs output
    \param[in]  newvalue: ENABLE or DISABLE
    \param[out] none
    \retval     none
*/
void pmu_esc_fun_config(pmu_esc_fun_enum fun_type, ControlStatus newvalue)
{
    uint32_t temp_value = 0;

    temp_value = pmu_esc_read(ESC_PMU_CTL0);

    switch(fun_type) {
    case ESC_PMU_FUN_EDWOLA:

    /* PMU energy detect / WoL port B */
    case ESC_PMU_FUN_EDWOLB:
        if(ENABLE == newvalue) {
            temp_value |= fun_type;
        } else {
            temp_value &= ~fun_type;
        }
        break;

    case ESC_PMU_FUN_ECATCLK:

    /* PMU LEDs output */
    case ESC_PMU_FUN_LEDOUT:
        if(DISABLE == newvalue) {
            temp_value |= fun_type;
        } else {
            temp_value &= ~fun_type;
        }
        break;

    default:
        break;
    }

    pmu_esc_write(ESC_PMU_CTL0, temp_value);
}

/*!
    \brief      test PMU byte (API_ID(0x0009U))
    \note       This function includes timeout exit scenarios.
                Modify according to the user's actual usage scenarios.
    \param[in]  none
    \param[out] none
    \retval     FlagStatus: SET or RESET
*/
FlagStatus pmu_esc_byte_test(void)
{
    FlagStatus reval = RESET;
    uint32_t temp = 0, timeout_val = 0;

    /* read process data interface reference value register */
    do {
        temp = pmu_esc_read(ESC_PMU_PDIREFVAL);
        timeout_val++;
    } while((temp != 0x76543210) && (timeout_val < BYTE_TEST_TIMEOUT));

    /* set return value */
    if(timeout_val >= BYTE_TEST_TIMEOUT) {
        reval = RESET;
    } else {
        reval = SET;
    }
    return reval;
}

/*!
    \brief      enable power management sleep mode (API_ID(0x000AU))
    \param[in]  none
    \param[out] none
    \retval     none
*/
void pmu_esc_sleep_mode_enable(void)
{
    uint32_t temp_value = pmu_esc_read(ESC_PMU_CTL1);

    temp_value |= ESC_PMU_CTL1_PMSLPEN;
    pmu_esc_write(ESC_PMU_CTL1, temp_value);
}

/*!
    \brief      disable power management sleep mode (API_ID(0x000BU))
    \param[in]  none
    \param[out] none
    \retval     none
*/
void pmu_esc_sleep_mode_disable(void)
{
    uint32_t temp_value = pmu_esc_read(ESC_PMU_CTL1);

    temp_value &= ~ESC_PMU_CTL1_PMSLPEN;
    pmu_esc_write(ESC_PMU_CTL1, temp_value);
}
