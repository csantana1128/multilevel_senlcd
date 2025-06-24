/**
 ******************************************************************************
 * @file    gpio.c
 * @author
 * @brief   gpio driver file
 *
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
#include "gpio.h"

/**
 * @brief gpio_cb save callback and the p_context
 */
typedef struct
{
    gpio_proc_cb   gpio_handler;           /**< user application ISR handler. */
    void           *p_context;             /**< the context to callback */
} gpio_cb;

static gpio_cb user_isr[MAX_NUMBER_OF_PINS];

void Gpio_Cfg(uint32_t pin_number, gpio_pin_dir_t dir, gpio_pin_int_mode_t int_mode)
{
    uint32_t  MASK;

    assert_param(pin_number < MAX_NUMBER_OF_PINS);
    assert_param(dir < GPIO_PIN_DIR_INVALID);

    MASK = (1 << pin_number);

    if (dir == GPIO_PIN_DIR_INPUT)
    {
        GPIO->INPUT_EN = MASK;
        GPIO->ENABLE_INPUT = MASK;
    }
    else
    {
        GPIO->DISABLE_INPUT = MASK;
        GPIO->OUTPUT_EN = MASK;
    }

    user_isr[pin_number].gpio_handler = NULL;

    switch (int_mode)
    {
    case GPIO_PIN_INT_LEVEL_LOW:
        GPIO->NEGATIVE   = MASK;
        GPIO->LEVEL_TRIG = MASK;
        break;
    case GPIO_PIN_INT_LEVEL_HIGH:
        GPIO->POSTITIVE  = MASK;
        GPIO->LEVEL_TRIG = MASK;
        break;
    case GPIO_PIN_INT_EDGE_RISING:
        GPIO->POSTITIVE  = MASK;
        GPIO->EDGE_TRIG  = MASK;
        break;
    case GPIO_PIN_INT_EDGE_FALLING:
        GPIO->NEGATIVE   = MASK;
        GPIO->EDGE_TRIG  = MASK;
        break;
    case GPIO_PIN_INT_BOTH_EDGE:
        GPIO->BOTH_EDGE_EN = MASK;
        GPIO->EDGE_TRIG  = MASK;
        break;

    case GPIO_PIN_NOINT:
    default:
        GPIO->DISABLE_INT = MASK;
        break;
    }

    return;
}

void Gpio_Setup_Deep_Sleep_Io(uint8_t num, gpio_pin_wake_t level)
{
    uint32_t mask;

    mask = 1 << num;
    GPIO->SET_DS_EN |= mask;
    if ( level == GPIO_LEVEL_LOW )
    {
        GPIO->DIS_DS_INV |= mask;
    }
    else
    {
        GPIO->SET_DS_INV |= mask;
    }
}

void Gpio_Disable_Deep_Sleep_Io(uint8_t num)
{
    uint32_t mask;

    mask = 1 << num;
    GPIO->DIS_DS_EN |= mask;
}

void Gpio_Setup_Deep_Powerdown_Io(uint8_t num, gpio_pin_wake_t level)
{
    uint32_t mask;

    mask = 1 << num;

    DPD_CTRL->DPD_GPIO_EN |= mask;

    if ( level == GPIO_LEVEL_LOW )
    {
        DPD_CTRL->DPD_GPIO_INV &= ~mask;
    }
    else
    {
        DPD_CTRL->DPD_GPIO_INV |= mask;
    }
}

void Gpio_Disable_Deep_Powerdown_Io(uint8_t num)
{
    uint32_t mask;

    mask = 1 << num;

    DPD_CTRL->DPD_GPIO_EN &= ~mask;
}

void Gpio_Setup_Io_Schmitt(uint8_t num)
{
    uint32_t mask;

    mask = 1 << num;

    SYSCTRL->GPIO_EN_SCHMITT |= mask;
}

void Gpio_Disable_Io_Schmitt(uint8_t num)
{
    uint32_t mask;

    mask = 1 << num;

    SYSCTRL->GPIO_EN_SCHMITT &= ~mask;
}

void Gpio_Setup_Io_Filter(uint8_t num)
{
    uint32_t mask;

    mask = 1 << num;

    SYSCTRL->GPIO_EN_FILTER |= mask;
}

void Gpio_Disable_Io_Filter(uint8_t num)
{
    uint32_t mask;

    mask = 1 << num;

    SYSCTRL->GPIO_EN_FILTER &= ~mask;
}

void Gpio_Register_Callback(uint32_t pin, gpio_proc_cb  app_gpio_callback, void *param)
{
    user_isr[pin].gpio_handler = app_gpio_callback;
    user_isr[pin].p_context = param;

    NVIC_EnableIRQ(Gpio_IRQn);
    return;
}

void Gpio_Cancell_Callback(uint32_t pin)
{
    user_isr[pin].gpio_handler = NULL;
    user_isr[pin].p_context = NULL;

    return;
}

void Gpio_Cfg_Input_Parameters(uint32_t pin_number, gpio_pin_int_mode_t int_mode,
                               gpio_proc_cb app_gpio_callback, void *param, bool debounce_en)
{
    Gpio_Cfg_Input(pin_number, int_mode);
    Gpio_Register_Callback(pin_number, app_gpio_callback, param);
    if ( debounce_en )
    {
        Gpio_Debounce_Enable(pin_number);
    }

    if ( int_mode != GPIO_PIN_NOINT)
    {
        Gpio_Int_Enable(pin_number);
    }
}
/**
 * @ingroup GPIO_Driver
 * @brief GPIO interrupt
 * @details
 * @return
 */
void GPIO_Handler(void)
{
    uint32_t  irq_state;
    uint32_t  i = 0, Mask = 1;

    gpio_proc_cb   app_isr;

    irq_state = GPIO->INT_STATUS;

    for (i = 0; i < MAX_NUMBER_OF_PINS; i++, Mask <<= 1)
    {
        if (irq_state & Mask)
        {
            app_isr = user_isr[i].gpio_handler;

            /*clear Edgeinterrupt status..
             * if the interrupt source is level trigger, this clear
             * does NOT have change...
             */
            GPIO->EDGE_INT_CLR = Mask;

            if (app_isr != NULL)
            {
                app_isr(i, user_isr[i].p_context);
            }
        }
    }

    return;
}
