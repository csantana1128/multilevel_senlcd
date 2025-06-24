/**
 ******************************************************************************
 * @file    wdt.c
 * @author
 * @brief   wdt driver file
 ******************************************************************************
 * @attention
 * Copyright (c) 2024 Rafael Micro.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * This file is part of library_name.
 */

/**************************************************************************************************
 *    INCLUDES
 *************************************************************************************************/
#include "wdt.h"


/**************************************************************************************************
 *    LOCAL VARIABLES
 *************************************************************************************************/
static wdt_proc_cb   user_wdt_isr = NULL;

/**************************************************************************************************
 *    GLOBAL FUNCTIONS
 *************************************************************************************************/
/**
 * @brief watch dog start(rt58x)
 */
uint32_t Wdt_Start(wdt_config_mode_t wdt_mode, wdt_config_tick_t wdt_cfg_ticks, wdt_isr_handler_t wdt_handler)
{
    uint32_t status;

    wdt_config_mode_ex_t cfg_ex;

    cfg_ex.int_enable = wdt_mode.int_enable;
    cfg_ex.lock_enable = wdt_mode.lock_enable;
    cfg_ex.prescale = wdt_mode.prescale;
    cfg_ex.reset_enable = wdt_mode.reset_enable;
    cfg_ex.sp_prescale = 0;

    status = Wdt_Start_Ex(cfg_ex, wdt_cfg_ticks, (wdt_proc_cb)wdt_handler);

    return status;
}



/**
 * @brief  watch dog register call bck
 */
void Wdt_Register_Callback(wdt_proc_cb wdt_handler)
{
    user_wdt_isr = wdt_handler;

    return;
}
/**
 * @brief  watch dog timer start (rt584)
 */
uint32_t Wdt_Start_Ex(
    wdt_config_mode_ex_t wdt_mode,
    wdt_config_tick_t wdt_cfg_ticks,
    wdt_proc_cb wdt_handler)
{
    wdt_ctrl_t controller;
    controller.reg = 0;

    controller.bit.WDT_EN = 1;

    if (WDT->CONTROL.bit.LOCKOUT)
    {
        return STATUS_INVALID_REQUEST;    /*Lockmode can Not change anymore.*/
    }

    if (wdt_mode.int_enable)
    {
        /*interrupt mode should has interrupt ISR*/
        if (wdt_handler == NULL)
        {
            return STATUS_INVALID_PARAM;
        }

        if (wdt_cfg_ticks.int_ticks >= wdt_cfg_ticks.wdt_ticks)
        {
            return STATUS_INVALID_PARAM;    /*the setting is nonsense.*/
        }

        WDT->CLEAR = 1; /*clear interrupt REQ?*/

        Wdt_Register_Callback(wdt_handler);
        controller.bit.INT_EN = 1;

        WDT->INT_VALUE = wdt_cfg_ticks.int_ticks;

        NVIC_EnableIRQ(Wdt_IRQn);
    }
    else
    {
        /*No interrupt mode.*/
        user_wdt_isr = NULL;
        NVIC_DisableIRQ(Wdt_IRQn);
    }

    if (wdt_mode.reset_enable)
    {
        controller.bit.RESET_EN = 1;    /*Lock*/
    }

    if (wdt_mode.lock_enable)
    {
        controller.bit.LOCKOUT = 1;
    }

    WDT->PRESCALE = wdt_mode.sp_prescale;

    WDT->WIN_MIN = wdt_cfg_ticks.wdt_min_ticks;
    WDT->WIN_MAX = wdt_cfg_ticks.wdt_ticks;
    WDT->CONTROL.reg = controller.reg;

    return STATUS_SUCCESS;
}
/**
 * @brief  watch dog timer stop (rt584)
 */
uint32_t Wdt_Stop(void)
{
    if (WDT->CONTROL.bit.LOCKOUT)
    {
        return STATUS_INVALID_REQUEST;    /*Lockmode can Not change anymore.*/
    }

    WDT->CONTROL.bit.WDT_EN = 0;

    return STATUS_SUCCESS;
}

/**
 * @brief  watch dog timer interrupt handler
 */
void WDT_Handler(void)
{
    WDT->CLEAR = 1; /*clear interrupt REQ?*/
    if (user_wdt_isr)
    {
        user_wdt_isr();
    }
}



