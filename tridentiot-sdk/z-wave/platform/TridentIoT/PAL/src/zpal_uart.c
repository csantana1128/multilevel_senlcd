/// ****************************************************************************
/// SPDX-License-Identifier: LicenseRef-TridentMSLA
/// SPDX-FileCopyrightText: 2023 Trident IoT, LLC <https://www.tridentiot.com>
/// ***************************************************************************

#include "zpal_uart.h"
#include "zpal_uart_gpio.h"
#include "uart_drv.h"
#include "tr_uart_blocking.h"
#include "tr_platform.h"
#include "sysctrl.h"
#include "sysfun.h"
#include "status.h"
#include "Assert.h"
#include "tr_ring_buffer.h"
#include <string.h>
#include "tr_hal_gpio.h"

volatile uint8_t received_byte;
volatile bool is_data_ready = false;

typedef struct
{
  uint32_t id; // UART ID
  uart_config_t config; // UART config
  zpal_uart_config_t zpal_config;
  uint8_t rx_buf;
  tr_ring_buffer_t ring_buffer;
}
uart_t;

static uart_t uart[3] = {
  [0] = {
    .id = 0,
    .config = {
      .p_context = NULL,
      .stopbit = UART_STOPBIT_ONE,
      .hwfc = UART_HWFC_DISABLED,
      .interrupt_priority = IRQ_PRIORITY_NORMAL
    },
    .zpal_config = {0}
  },
  [1] = {
    .id = 1,
    .config = {
      .p_context = NULL,
      .stopbit = UART_STOPBIT_ONE,
      .hwfc = UART_HWFC_DISABLED,
      .interrupt_priority = IRQ_PRIORITY_NORMAL
    },
    .zpal_config = {0}
  },
  [2] = {
    .id = 2,
    .config = {
      .p_context = NULL,
      .stopbit = UART_STOPBIT_ONE,
      .hwfc = UART_HWFC_DISABLED,
      .interrupt_priority = IRQ_PRIORITY_NORMAL
    },
    .zpal_config = {0}
  }
};

static zpal_uart_transmit_done_t zpal_uart_transmit_done;

static void uart_event_handler(uint32_t event, void *p_context)
{
  uart_t * p_uart = (uart_t *)p_context;

  if (event & UART_EVENT_TX_DONE)
  {
    if (NULL != zpal_uart_transmit_done)
    {
      zpal_uart_transmit_done(p_uart);
    }
  }

  if (event & UART_EVENT_RX_DONE)
  {
    if (false == tr_ring_buffer_write(&p_uart->ring_buffer, p_uart->rx_buf))
    {
      // Buffer full, byte dropped - let the application handle situation.
    }

    // Data available.
    p_uart->zpal_config.receive_callback(p_uart, tr_ring_buffer_get_available(&p_uart->ring_buffer));

    uart_rx(p_uart->id, &p_uart->rx_buf, 1);
  }
#if 0
  if (event & UART_EVENT_RX_TIMEOUT)
  {
    // Received less bytes than expected.
    uart->zpal_config->receive_callback(uart, 1);
  }
#endif
}

static uart_baudrate_t zpal_to_rm_baud_rate(uint32_t zpal_baud_rate, bool uart_wakeup)
{
  if (uart_wakeup)
  {
    switch (zpal_baud_rate)
    {
    case 2400:    return UART_BAUDRATE_2400_LS;
    case 4800:    return UART_BAUDRATE_4800_LS;
    case 9600:    return UART_BAUDRATE_9600_LS;
    case 14400:   return UART_BAUDRATE_14400_LS;
    case 19200:   return UART_BAUDRATE_19200_LS;
    case 28800:   return UART_BAUDRATE_28800_LS;
    case 38400:   return UART_BAUDRATE_38400_LS;
    case 57600:   return UART_BAUDRATE_57600_LS;
    case 76800:   return UART_BAUDRATE_76800_LS;
    case 115200:  return UART_BAUDRATE_115200_LS;
    case 230400:  return UART_BAUDRATE_230400_LS;
    default:
      return UART_BAUDRATE_115200_LS;
    }
  }
  else
  {
    switch (zpal_baud_rate)
    {
    case 2400:    return UART_BAUDRATE_2400;
    case 4800:    return UART_BAUDRATE_4800;
    case 9600:    return UART_BAUDRATE_9600;
    case 14400:   return UART_BAUDRATE_14400;
    case 19200:   return UART_BAUDRATE_19200;
    case 28800:   return UART_BAUDRATE_28800;
    case 38400:   return UART_BAUDRATE_38400;
    case 57600:   return UART_BAUDRATE_57600;
    case 76800:   return UART_BAUDRATE_76800;
    case 115200:  return UART_BAUDRATE_115200;
    case 230400:  return UART_BAUDRATE_230400;
    case 460800:  return UART_BAUDRATE_460800;
    case 921600:  return UART_BAUDRATE_921600;
    case 500000:  return UART_BAUDRATE_500000;
    case 1000000: return UART_BAUDRATE_1000000;
    case 2000000: return UART_BAUDRATE_2000000;
    default:
      return UART_BAUDRATE_115200;
    }
  }
}

static uart_databits_t zpal_to_rm_data_bits(uint8_t zpal_data_bits)
{
  switch (zpal_data_bits)
  {
  case 5:    return UART_DATA_BITS_5;
  case 6:    return UART_DATA_BITS_6;
  case 7:    return UART_DATA_BITS_7;
  case 8:    return UART_DATA_BITS_8;
  default:
    return UART_DATA_BITS_8;
  }
}

static uart_parity_t zpal_to_rm_parity_bit(zpal_uart_parity_bit_t zpal_parity_bit)
{
  switch (zpal_parity_bit)
  {
  case ZPAL_UART_NO_PARITY:   return UART_PARITY_NONE;
  case ZPAL_UART_EVEN_PARITY: return UART_PARITY_EVEN;
  case ZPAL_UART_ODD_PARITY:  return UART_PARITY_ODD;
  default:
    return UART_PARITY_NONE;
  }
}

static uart_stopbit_t zpal_to_rm_stop_bits(zpal_uart_stop_bits_t zpal_stop_bits)
{
  switch (zpal_stop_bits)
  {
  case ZPAL_UART_STOP_BITS_0P5: return UART_STOPBIT_ONE;
  case ZPAL_UART_STOP_BITS_1:   return UART_STOPBIT_ONE;
  case ZPAL_UART_STOP_BITS_1P5: return UART_STOPBIT_ONE;
  case ZPAL_UART_STOP_BITS_2:   return UART_STOPBIT_TWO;
  default:
    return UART_STOPBIT_ONE;
  }
}

typedef struct
{
  uint32_t tx;
  uint32_t rx;
  uint32_t cts;
  uint32_t rts;
}
pin_mode_t;

static bool get_pin_mode(const zpal_uart_id_t id, pin_mode_t * const pin_mode)
{
  bool result = true;
  if (NULL == pin_mode)
  {
    ASSERT(false);
    return false;
  }
  switch (id)
  {
    case ZPAL_UART0:
      pin_mode->tx  = TR_HAL_GPIO_MODE_UART_0_TX;
      pin_mode->rx  = TR_HAL_GPIO_MODE_UART_0_RX;
      pin_mode->cts = 0; // Not supported by UART0.
      pin_mode->rts = 0; // Not supported by UART0.
    break;
    case ZPAL_UART1:
      pin_mode->tx  = TR_HAL_GPIO_MODE_UART_1_TX;
      pin_mode->rx  = TR_HAL_GPIO_MODE_UART_1_RX;
      pin_mode->cts = TR_HAL_GPIO_MODE_UART_1_CTS;
      pin_mode->rts = TR_HAL_GPIO_MODE_UART_1_RTSN;
    break;
    case ZPAL_UART2:
      pin_mode->tx  = TR_HAL_GPIO_MODE_UART_2_TX;
      pin_mode->rx  = TR_HAL_GPIO_MODE_UART_2_RX;
      pin_mode->cts = TR_HAL_GPIO_MODE_UART_2_CTS;
      pin_mode->rts = TR_HAL_GPIO_MODE_UART_2_RTSN;
    break;
    default:
      result = false;
      break;
  }
  return result;
}

zpal_status_t zpal_uart_init(zpal_uart_config_t const * const config_, zpal_uart_handle_t *handle)
{
  uint8_t tmp_txd_pin = 0;
  uint8_t tmp_rxd_pin = 0;
  uint8_t tmp_cts_pin = 0;
  uint8_t tmp_rts_pin = 0;
  const zpal_uart_config_ext_t *ext_uart_cfg = config_->ptr;
  bool enable_uart_wakeup = false;

  ASSERT(NULL != config_);

  if (NULL != uart[config_->id].config.p_context)
  {
    // Uart already initialized
    if (NULL == uart[config_->id].zpal_config.receive_callback)
    {
      // As initial uart init had no receive callback we can reinitialize with the new uart configuration
      if (config_->flags & ZPAL_UART_CONFIG_FLAG_BLOCKING)
      {
        tr_uart_uninit_blocking(config_->id);
      }
      else
      {
        uart_uninit(config_->id);
      }
    }
    else
    {
      // There can only be one receiver of data
      *handle = (zpal_uart_handle_t)&uart[config_->id]; // Pass handle to caller of zpal_uart_init().
      return ZPAL_STATUS_OK;
    }
  }
  enter_critical_section();
  // The config contains receive_callback() required by uart_event_handler().
  memcpy(&uart[config_->id].zpal_config, config_, sizeof(zpal_uart_config_t));
  zpal_uart_config_t * const config = &uart[config_->id].zpal_config;

  pin_mode_t pin_mode = {0};
  if (true != get_pin_mode(config->id, &pin_mode))
  {
    leave_critical_section();
    return ZPAL_STATUS_FAIL;
  }

  if (ext_uart_cfg == NULL)
  {
    tmp_cts_pin = 0;
    tmp_rts_pin = 0;

    // Set up default pins for UARTx
    switch (config->id)
    {
      case ZPAL_UART0:
      {
        tmp_txd_pin = 17;
        tmp_rxd_pin = 16;
      }
      break;

      case ZPAL_UART1:
      {
        tmp_txd_pin = 28;
        tmp_rxd_pin = 29;
      }
      break;

      case ZPAL_UART2:
      {
        tmp_txd_pin = 30;
        tmp_rxd_pin = 31;
      }
      break;

    default:
      break;
    }
  }
  else
  {
    // Custom set UART pins
    tmp_rxd_pin = ext_uart_cfg->rxd_pin;
    tmp_txd_pin = ext_uart_cfg->txd_pin;
    tmp_cts_pin = ext_uart_cfg->cts_pin;
    tmp_rts_pin = ext_uart_cfg->rts_pin;

    enable_uart_wakeup = ext_uart_cfg->uart_wakeup;
  }

  // Set internal UART configuration structure
  uart[config->id].config.p_context           = &uart[config->id]; // Required by uart_event_handler().
  uart[config->id].config.baudrate            = zpal_to_rm_baud_rate(config->baud_rate, enable_uart_wakeup);
  uart[config->id].config.databits            = zpal_to_rm_data_bits(config->data_bits);
  uart[config->id].config.stopbit             = zpal_to_rm_stop_bits(config->stop_bits);
  uart[config->id].config.parity              = zpal_to_rm_parity_bit(config->parity_bit);
  uart[config->id].config.hwfc                = UART_HWFC_DISABLED; // Hardcode to disabled.
  uart[config->id].config.interrupt_priority  = IRQ_PRIORITY_NORMAL; // Hardcode the interrupt priority.
  tr_hal_gpio_pin_t _pin;
  // Set up GPIO pins
  _pin.pin = tmp_txd_pin;
  tr_hal_gpio_set_mode(_pin, pin_mode.tx);
  tr_hal_gpio_set_pull_mode(_pin, TR_HAL_PULLOPT_PULL_NONE);
  tr_hal_gpio_set_direction(_pin, TR_HAL_GPIO_DIRECTION_OUTPUT);

  _pin.pin = tmp_rxd_pin;
  tr_hal_gpio_set_mode(_pin, pin_mode.rx);
  tr_hal_gpio_set_direction(_pin, TR_HAL_GPIO_DIRECTION_INPUT);

  if (tmp_cts_pin)
  {
    _pin.pin = tmp_cts_pin;
    tr_hal_gpio_set_mode(_pin, pin_mode.cts);
    tr_hal_gpio_set_direction(_pin, TR_HAL_GPIO_DIRECTION_INPUT);

  }
  if (tmp_rts_pin)
  {
    _pin.pin = tmp_rts_pin;
    tr_hal_gpio_set_mode(_pin, pin_mode.rts);
    tr_hal_gpio_set_pull_mode(_pin, TR_HAL_PULLOPT_PULL_NONE);
    tr_hal_gpio_set_direction(_pin, TR_HAL_GPIO_DIRECTION_OUTPUT);

  }

  // Set UART clock source
  if (enable_uart_wakeup)
  {
    Uart_Clock_Source(uart[config->id].id, UART_CLK_SRC_RCO1M);
  }

  // Initialize uart
  uart_event_handler_t event_handler = uart_event_handler;
  uint32_t retval;
  if (config->flags & ZPAL_UART_CONFIG_FLAG_BLOCKING)
  {
    event_handler = NULL;
    retval = tr_uart_init_blocking(uart[config->id].id, &uart[config->id].config, event_handler);
  }
  else
  {
    retval = uart_init(uart[config->id].id, &uart[config->id].config, event_handler);
  }

  if (STATUS_SUCCESS != retval)
  {
    leave_critical_section();
    return ZPAL_STATUS_FAIL;
  }

  *handle = (zpal_uart_handle_t)&uart[config->id]; // Pass handle to caller of zpal_uart_init().

  uart[config->id].ring_buffer.p_buffer    = config->rx_buffer;
  uart[config->id].ring_buffer.buffer_size = config->rx_buffer_len;
  tr_ring_buffer_init(&uart[config->id].ring_buffer);

  if ((config->flags & ZPAL_UART_CONFIG_FLAG_BLOCKING) == 0)
  {
    // Start receiving only in non-block mode. Otherwise this call will block while waiting for data.
    uart_rx(uart[config->id].id, &uart[config->id].rx_buf, 1);
  }

  leave_critical_section();
  return ZPAL_STATUS_OK;
}

zpal_status_t zpal_uart_enable(zpal_uart_handle_t handle)
{
  uart_t * p_uart = (uart_t *)handle;
  uint32_t retval = uart_power(p_uart->id, 1);
  if (STATUS_SUCCESS != retval)
  {
    return ZPAL_STATUS_FAIL;
  }
  return ZPAL_STATUS_OK;
}

zpal_status_t zpal_uart_disable(zpal_uart_handle_t handle)
{
  uart_t * p_uart = (uart_t *)handle;
  uint32_t retval = uart_power(p_uart->id, 0);
  if (STATUS_SUCCESS != retval)
  {
    return ZPAL_STATUS_FAIL;
  }
  return ZPAL_STATUS_OK;
}

zpal_status_t zpal_uart_transmit(zpal_uart_handle_t handle, const uint8_t *data, size_t length,
                                 zpal_uart_transmit_done_t tx_cb)
{
  uint32_t retval;
  uart_t * p_uart = (uart_t *)handle;
  enter_critical_section();
  zpal_uart_transmit_done = tx_cb;
  if (p_uart->zpal_config.flags  & ZPAL_UART_CONFIG_FLAG_BLOCKING)
  {
    retval = tr_uart_tx_blocking(p_uart->id, data, length);
  }
  else
  {
    retval = uart_tx(p_uart->id, data, length);
  }
  leave_critical_section();
  if (STATUS_SUCCESS != retval)
  {
    return ZPAL_STATUS_FAIL;
  }
  return ZPAL_STATUS_OK;
}

bool zpal_uart_transmit_in_progress(zpal_uart_handle_t handle)
{
  uart_t * p_uart = (uart_t *)handle;
#if defined(TR_PLATFORM_T32CZ20)
  return (tr_uart_trx_complete(p_uart->id) == false);
#endif
#if defined(TR_PLATFORM_ARM)
  return (uart_trx_complete(p_uart->id) == false);
#endif
}

size_t zpal_uart_get_available(zpal_uart_handle_t handle)
{
  size_t retval;
  uart_t * p_uart = (uart_t *)handle;
  enter_critical_section();
  retval = tr_ring_buffer_get_available(&p_uart->ring_buffer);
  leave_critical_section();
  return retval;
}

size_t zpal_uart_receive(zpal_uart_handle_t handle, uint8_t *data, size_t length)
{
  size_t retval;
  uart_t * p_uart = (uart_t *)handle;
  enter_critical_section();
  retval = tr_ring_buffer_read(&p_uart->ring_buffer, data, length);
  leave_critical_section();
  return retval;
}
