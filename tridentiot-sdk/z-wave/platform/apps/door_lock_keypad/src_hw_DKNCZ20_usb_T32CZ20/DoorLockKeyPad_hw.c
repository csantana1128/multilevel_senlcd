/// ****************************************************************************
/// @file DoorLockKeypad_hw.c
///
/// @brief Door Lock Keypad Hardware setup for the DKNCZ20 board with UART on USB
///
///
/// SPDX-License-Identifier: LicenseRef-TridentMSLA
/// SPDX-FileCopyrightText: 2025 Trident IoT, LLC <https://www.tridentiot.com>
/// ***************************************************************************

#include <apps_hw.h>
#include <DebugPrint.h>
#include "CC_BinarySwitch.h"
#include "SizeOf.h"
#include "events.h"
#include <inttypes.h>
#ifdef TR_CLI_ENABLED
#include "tr_cli.h"
#endif
#include <zaf_event_distributor_soc.h>
#include "tr_board_DKNCZ20.h"
#include "zpal_uart.h"
#include "zpal_misc.h"
#include "zpal_misc_private.h"
#include "setup_common_cli.h"
#include "MfgTokens.h"
#include "zpal_uart_gpio.h"
#include "ZAF_Common_interface.h"
#include "tr_hal_gpio.h"

/*defines what gpios ids are used*/
const uint8_t PB_LEARN_MODE = (uint8_t)TR_BOARD_BTN_LEARN_MODE;

/*Defines if the gpio support low power mode or nots*/
const  low_power_wakeup_cfg_t PB_LEARN_MODE_LP    = LOW_POWER_WAKEUP_GPIO4;

/*Defines the on state for the gpios*/
const uint8_t PB_LEARN_MODE_ON  = 0;

static gpio_info_t m_gpio_info[] = {
  GPIO_INFO()
};

static const gpio_config_t m_gpio_config[] = {
  GPIO_CONFIG(PB_LEARN_MODE, PB_LEARN_MODE_LP, PB_LEARN_MODE_ON, EVENT_SYSTEM_LEARNMODE_TOGGLE, EVENT_SYSTEM_EMPTY, EVENT_SYSTEM_RESET, EVENT_SYSTEM_EMPTY)
};

#define BUTTONS_SIZE    sizeof_array(m_gpio_config)

/*Setup for debug and remote CLI uart*/

static const zpal_uart_config_ext_t ZPAL_UART_CONFIG_GPIO = {
  .txd_pin = TR_BOARD_UART0_TX,
  .rxd_pin = TR_BOARD_UART0_RX,
  .cts_pin = 0, // Not used.
  .rts_pin = 0, // Not used.
  .uart_wakeup = true
};

static zpal_uart_config_t UART_CONFIG =
{
  .id = ZPAL_UART1, // Use UART1 because UART2 is disabled when the Door Lock sleeps.
  .baud_rate = 115200,
  .data_bits = 8,
  .parity_bit = ZPAL_UART_NO_PARITY,
  .stop_bits = ZPAL_UART_STOP_BITS_1,
  .receive_callback = NULL,
  .ptr = &ZPAL_UART_CONFIG_GPIO,
  .flags = 0
};

zpal_debug_config_t debug_port_cfg = &UART_CONFIG;

#ifdef TR_CLI_ENABLED

static int cli_cmd_app_battery(int  argc, char *argv[]);

// Application specific CLI commands
TR_CLI_COMMAND_TABLE(app_specific_commands) =
{
  { "battery", cli_cmd_app_battery,   "Toggle switch state"                                },
  TR_CLI_COMMAND_TABLE_END
};

static int cli_cmd_app_battery(__attribute__((unused)) int  argc,__attribute__((unused))  char *argv[])
{
  zaf_event_distributor_enqueue_app_event_from_isr(EVENT_APP_BATTERY_REPORT);
  return 0;
}

#endif // #ifdef TR_CLI_ENABLED

void app_hw_init(void)
{
  apps_hw_init(m_gpio_config, m_gpio_info, BUTTONS_SIZE);

#ifdef TR_CLI_ENABLED
  // Enable wake up from UART0 upon reception of some data.
  Lpm_Enable_Low_Power_Wakeup(LOW_POWER_WAKEUP_UART0_DATA);

  setup_cli(&UART_CONFIG);
#endif
}

uint8_t board_indicator_gpio_get( void)
{
  return TR_BOARD_LED_LEARN_MODE;
}

uint8_t board_indicator_led_off_gpio_state(void)
{
  return TR_HAL_GPIO_LEVEL_HIGH;
}
