/// ****************************************************************************
/// @file tr_hal_wdog.c
///
/// @brief This contains the code for the Trident HAL Watchdog for T32CM11
///
///
/// SPDX-License-Identifier: LicenseRef-TridentMSLA
/// SPDX-FileCopyrightText: 2025 Trident IoT, LLC <https://www.tridentiot.com>
/// ***************************************************************************

#include "tr_hal_wdog.h"


// callback to the app to use for events
static tr_hal_wdog_event_callback_t g_wdog_callback = NULL;

// keep track of if the init has been run yet
static bool g_wdog_init_done = false;
static bool g_wdog_enabled = false;
static uint32_t g_control_reg = 0;


/// ***************************************************************************
/// tr_hal_wdog_get_register_address
/// ***************************************************************************
WDOG_REGISTERS_T* tr_hal_wdog_get_register_address(void)
{
    return WDOG_REGISTERS;
}


/// ***************************************************************************
/// check_init_params
/// ***************************************************************************
static tr_hal_status_t check_init_params(tr_hal_wdog_settings_t* wdog_settings)
{
    tr_hal_status_t status;

    // min time can't be larger than initial time
    if (wdog_settings->min_time_before_reset > wdog_settings->initial_value)
    {
        return TR_HAL_WDOG_MIN_TIME_TOO_LARGE;
    }
    
    // initial time needs to be larger than the minimum allowed
    if (wdog_settings->initial_value < TR_HAL_WDOG_MINIMUM_INITIAL_VALUE)
    {
        return TR_HAL_WDOG_INITIAL_TIME_TOO_SMALL;
    }

    // if interrupt enabled make sure int pri is within range and int time is < initial value
    if (wdog_settings->interrupt_enabled)
    {
        status = tr_hal_check_interrupt_priority(wdog_settings->interrupt_priority);
        if (status != TR_HAL_SUCCESS)
        {
            return status;
        }
        
        if (wdog_settings->interrupt_time_value >= wdog_settings->initial_value)
        {
            return TR_HAL_WDOG_INT_TIME_TOO_LARGE;
        }
    }

    return TR_HAL_SUCCESS;
}


/// ***************************************************************************
/// tr_hal_wdog_init
/// ***************************************************************************
tr_hal_status_t tr_hal_wdog_init(tr_hal_wdog_settings_t* wdog_settings)
{
    tr_hal_status_t status;
    WDOG_REGISTERS_T* wdog_reg = tr_hal_wdog_get_register_address();

    // settings can't be NULL
    if (wdog_settings == NULL)
    {
        return TR_HAL_ERROR_NULL_PARAMS;
    }

    // we disable the chip NOW, in case wd is running, and enable it at the end
    wdog_reg->control = TR_HAL_WDOG_CTRL_TIMER_DISABLED;

    // if we want to be disabled, then disable the INT and we are done
    if (!(wdog_settings->watchdog_enabled))
    {
        NVIC_DisableIRQ(Wdt_IRQn);
        return TR_HAL_SUCCESS;
    }

    // check init settings here
    status = check_init_params(wdog_settings);
    if (status != TR_HAL_SUCCESS)
    {
        return status;
    }
    
    // load the time into the initial_value register
    wdog_reg->initial_value = wdog_settings->initial_value;


    // **** load the control register (multiple steps) ****

    // 1. watchdog is enabled
    uint32_t control_settings = TR_HAL_WDOG_CTRL_TIMER_ENABLED;

    // 2. set clock prescalar value
    control_settings |= wdog_settings->clock_prescalar;

    // 3. see if reset should be enabled
    if (wdog_settings->reset_enabled)
    {
        control_settings |= TR_HAL_WDOG_CTRL_RESET_ENABLED;
    }

    // 4. see if interrupt should be enabled
    if (wdog_settings->interrupt_enabled)
    {
        control_settings |= TR_HAL_WDOG_CTRL_INTERRUPT_ENABLED;
        // set the time value that triggers the interrupt
        wdog_reg->interrupt_on_value = wdog_settings->interrupt_time_value;
        // clear any pending interrupt
        wdog_reg->interrupt_clear = TR_HAL_WDOG_CLEAR_INTERRUPT;
        // set the INT priority and enable interrupt
        NVIC_SetPriority(Wdt_IRQn, wdog_settings->interrupt_priority);
        NVIC_EnableIRQ(Wdt_IRQn);
    }

    // 5. see if lockout should be enabled
    if (wdog_settings->lockout_enabled)
    {
        control_settings |= TR_HAL_WDOG_CTRL_LOCKOUT;
    }

    // 6. wait untuil everything else is done and set before setting
    // the control register since that turns on the watchdog and
    // this could enable lockout (so things can't be set any more)

    // set the min time before reset register
    wdog_reg->min_time_before_reset = wdog_settings->min_time_before_reset;

    // if we are supposed to clear the reset counter, do that by writing to it
    if (wdog_settings->clear_reset_counter_on_init)
    {
        wdog_reg->reset_counter = TR_HAL_WDOG_CLEAR_RESET_COUNTER;
    }

    // if there is a callback remember it
    g_wdog_callback = wdog_settings->event_handler_fx;

    // now that all registers are set, set the control register which enables 
    // the watchdog
    wdog_reg->control = control_settings;

    g_control_reg = control_settings;
    g_wdog_init_done = true;
    g_wdog_enabled = true;

    return TR_HAL_SUCCESS;
}


/// ***************************************************************************
/// tr_hal_wdog_enable
///
/// this disables the watchdog timer completely by setting the timer enabled 
/// bit in the control register
/// ***************************************************************************
tr_hal_status_t tr_hal_wdog_enable(void)
{
    // make sure init has been done already
    if (!g_wdog_init_done)
    {
        return TR_HAL_WDOG_NOT_INITIALIZED;
    }

    // check if we are already enabled, if so we just return success
    if (g_wdog_enabled)
    {
        return TR_HAL_SUCCESS;
    }

    WDOG_REGISTERS_T* wdog_reg = tr_hal_wdog_get_register_address();

    // to enable we just set the control register to its previous (before disabled) 
    // value, which sets the enabled bit
    wdog_reg->control = g_control_reg;

    // set to enabled
    g_wdog_enabled = true;

    return TR_HAL_SUCCESS;
}


/// ***************************************************************************
/// tr_hal_wdog_disable
///
/// this enables the watchdog timer by setting the timer enabled bit in the 
/// control register
/// ***************************************************************************
tr_hal_status_t tr_hal_wdog_disable(void)
{
    // make sure init has been done already
    if (!g_wdog_init_done)
    {
        return TR_HAL_WDOG_NOT_INITIALIZED;
    }

    // check if we are already disabled, if so we just return success
    if (g_wdog_enabled == false)
    {
        return TR_HAL_SUCCESS;
    }

    WDOG_REGISTERS_T* wdog_reg = tr_hal_wdog_get_register_address();

    // to disable we need to set the control register, read what is there so it can be restored
    // and we also only want to change the one bit that controls wdog being enabled
    g_control_reg = wdog_reg->control;
    wdog_reg->control = g_control_reg & (~TR_HAL_WDOG_CTRL_TIMER_ENABLED);

    // set to disabled
    g_wdog_enabled = false;

    return TR_HAL_SUCCESS;
}


/// ***************************************************************************
/// tr_hal_is_wdog_enabled
///
/// this returns if the watchdog timer is enabled or disabled
/// we expect users to use the Trident HAL APIs. If, however, the users went
/// around the APIs and enabled/disabled the wdog timer directly by writing to
/// the register, this function will return the register value but an error
/// code. If the values match this function returns TR_HAL_SUCCESS
///
/// ***************************************************************************
tr_hal_status_t tr_hal_is_wdog_enabled(bool* is_enabled)
{
    // we *should* be able to just return the flag g_wdog_enabled, as long as
    // the user has used the Trident HAL APIs and not accessed the chip 
    // registers directly. So we double check that the register lines up
    // with the enabled flag. if they are out of sync then we return the 
    // register value for is_enabled and return a status that says they
    // are out of sync
    
    WDOG_REGISTERS_T* wdog_reg = tr_hal_wdog_get_register_address();

    // get the control register
    uint32_t control_register = wdog_reg->control;
    
    // check the enabled bit
    bool is_enabled_register_value = false;
    if ((control_register & TR_HAL_WDOG_CTRL_TIMER_ENABLED) > 0)
    {
        is_enabled_register_value = true;
    }
    
    // if the register and the saved status line up then we are good
    if ( g_wdog_enabled == is_enabled_register_value )
    {
        (*is_enabled) = g_wdog_enabled;
        return TR_HAL_SUCCESS;
    }

    // if the register and the saved status are different then
    // return the reg value but an error
    (*is_enabled) = is_enabled_register_value;
    return TR_HAL_WDOG_STATE_AND_REG_OUT_OF_SYNC;
}


/// ***************************************************************************
/// tr_hal_wdog_reset
/// ***************************************************************************
tr_hal_status_t tr_hal_wdog_reset(void)
{
    WDOG_REGISTERS_T* wdog_reg = tr_hal_wdog_get_register_address();

    // reset the watchdog timer by setting the special value
    wdog_reg->reset_watchdog = TR_HAL_WDOG_RESET_WATCHDOG_VALUE;

    return TR_HAL_SUCCESS;
}


/// ***************************************************************************
/// tr_hal_wdog_read_num_resets
/// ***************************************************************************
tr_hal_status_t tr_hal_wdog_read_num_resets(uint32_t* num_resets)
{
    if (num_resets == NULL) 
    {
        return TR_HAL_ERROR_NULL_PARAMS;
    }

    WDOG_REGISTERS_T* wdog_reg = tr_hal_wdog_get_register_address();

    (*num_resets) = wdog_reg->reset_counter;

    return TR_HAL_SUCCESS;
}

/// ***************************************************************************
/// tr_hal_wdog_read_curr_state
/// ***************************************************************************
tr_hal_status_t tr_hal_wdog_read_curr_state(uint32_t* initial_time,
                                            uint32_t* curr_time,
                                            uint32_t* int_time,
                                            uint32_t* min_time)
{
    if ( (initial_time == NULL) 
        || (curr_time == NULL)
        || (int_time == NULL)
        || (min_time == NULL)
       )
    {
        return TR_HAL_ERROR_NULL_PARAMS;
    }
    
    WDOG_REGISTERS_T* wdog_reg = tr_hal_wdog_get_register_address();
    
    (*initial_time) = wdog_reg->initial_value;
    (*curr_time)    = wdog_reg->current_value;
    (*int_time)     = wdog_reg->interrupt_on_value;
    (*min_time)     = wdog_reg->min_time_before_reset;

    return TR_HAL_SUCCESS;
}


/// ***************************************************************************
/// Watchdog INTERRUPT HANDLER
/// ***************************************************************************
void Wdt_Handler(void)
{
    WDOG_REGISTERS_T* wdog_reg = tr_hal_wdog_get_register_address();
    
    wdog_reg->interrupt_clear = TR_HAL_WDOG_CLEAR_INTERRUPT;
    
    uint32_t event_mask = TR_HAL_WDOG_EVENT_INT_TRIGGERED;
    
    if (g_wdog_callback != NULL)
    {
        g_wdog_callback(event_mask);
    }
}

