/// ***************************************************************************
/// SPDX-License-Identifier: LicenseRef-TridentMSLA
/// SPDX-FileCopyrightText: 2025 Trident IoT, LLC <https://www.tridentiot.com>
/// ***************************************************************************

#include "unity.h"
#include "unity_print.h"
#include "sysfun.h"
#include "zpal_uart.h"
#include "uart_drv_mock.h"
#include "tr_uart_blocking_mock.h"
#include "sysctrl_mock.h"
#include "sysfun_mock.h"
#include "zpal_uart_gpio.h"
#include "status.h"
#include "SizeOf.h"
#include <string.h>
#include "tr_hal_gpio_mock.h"

void setUpSuite(void)
{

}

void tearDownSuite(void)
{

}

#define COMM_INT_TX_BUFFER_SIZE 100
uint8_t tx_data[COMM_INT_TX_BUFFER_SIZE];
#define COMM_INT_RX_BUFFER_SIZE 100
uint8_t rx_data[COMM_INT_RX_BUFFER_SIZE];

void receive_callback(zpal_uart_handle_t handle, size_t length)
{
  (void)handle;
  (void)length;
}

zpal_uart_config_t default_config = {
  .receive_callback = receive_callback
};
zpal_uart_handle_t handle;

void set_default_uart_configuration(void)
{
  // Make sure to set all bytes to zero so there will be no random data causing a failed comparison later.
  memset(&default_config, 0x00, sizeof(default_config));
  default_config.id = ZPAL_UART0;
  default_config.tx_buffer = tx_data;
  default_config.tx_buffer_len = COMM_INT_TX_BUFFER_SIZE;
  default_config.rx_buffer = rx_data;
  default_config.rx_buffer_len = COMM_INT_RX_BUFFER_SIZE;
  default_config.baud_rate = 115200;
  default_config.data_bits = 8;
  default_config.parity_bit = ZPAL_UART_NO_PARITY;
  default_config.stop_bits = ZPAL_UART_STOP_BITS_1;
  default_config.ptr = NULL;
  default_config.flags = 0;
}

void setUp(void)
{
  set_default_uart_configuration();

  handle = NULL;

  uart_drv_mock_Init();
}

void tearDown(void)
{
  uart_drv_mock_Verify();
  uart_drv_mock_Destroy();
}

typedef struct
{
  uint32_t input;
  uart_baudrate_t output;
}
io_baud_rate_t;

const io_baud_rate_t IO_BAUD_RATE[16] = {
  {2400,    UART_BAUDRATE_2400},
  {4800,    UART_BAUDRATE_4800},
  {9600,    UART_BAUDRATE_9600},
  {14400,   UART_BAUDRATE_14400},
  {19200,   UART_BAUDRATE_19200},
  {28800,   UART_BAUDRATE_28800},
  {38400,   UART_BAUDRATE_38400},
  {57600,   UART_BAUDRATE_57600},
  {76800,   UART_BAUDRATE_76800},
  {115200,  UART_BAUDRATE_115200},
  {230400,  UART_BAUDRATE_230400},
  {460800,  UART_BAUDRATE_460800},
  {921600,  UART_BAUDRATE_921600},
  {500000,  UART_BAUDRATE_500000},
  {1000000, UART_BAUDRATE_1000000},
  {2000000, UART_BAUDRATE_2000000},
};

uint32_t get_baud_rate_index(const uint32_t baud_rate)
{
  for (uint32_t i = 0; i < sizeof_array(IO_BAUD_RATE); i++)
  {
    if (baud_rate == IO_BAUD_RATE[i].input)
    {
      return i;
    }
  }
  return sizeof_array(IO_BAUD_RATE);
}

// This is uused to check correct UART id in uart_init_CALLBACK
uint32_t expected_uart_id;
uart_config_t expected_uart_config;

uint32_t uart_init_CALLBACK(uint32_t uart_id, uart_config_t const* p_config, uart_event_handler_t event_handler, int cmock_num_calls)
{
  TEST_ASSERT_EQUAL_UINT32(expected_uart_id, uart_id);

  TEST_ASSERT_NOT_NULL_MESSAGE(p_config->p_context, "p_context");
  TEST_ASSERT_EQUAL_MESSAGE(expected_uart_config.baudrate,            p_config->baudrate, "baudrate");
  TEST_ASSERT_EQUAL_MESSAGE(expected_uart_config.databits,            p_config->databits, "databits");
  TEST_ASSERT_EQUAL_MESSAGE(expected_uart_config.stopbit,             p_config->stopbit, "stopbits");
  TEST_ASSERT_EQUAL_MESSAGE(expected_uart_config.parity,              p_config->parity,"parity");
  TEST_ASSERT_EQUAL_MESSAGE(expected_uart_config.hwfc,                p_config->hwfc,"hwfc");
  TEST_ASSERT_EQUAL_MESSAGE(expected_uart_config.interrupt_priority,  p_config->interrupt_priority, "interrupt_priority");

  return STATUS_SUCCESS;
}

void test_zpal_uart_init_baud_rate(void)
{
  // Checked in uart_init_CALLBACK
  expected_uart_id = 0;

  Uart_Init_Stub(uart_init_CALLBACK); // Handle expectations in the callback.

  tr_hal_gpio_set_mode_IgnoreAndReturn(TR_HAL_SUCCESS);
  tr_hal_gpio_set_direction_IgnoreAndReturn(TR_HAL_SUCCESS);
  tr_hal_gpio_set_pull_mode_IgnoreAndReturn(TR_HAL_SUCCESS);

  Enter_Critical_Section_Ignore();
  Leave_Critical_Section_Ignore();
  Uart_Uninit_IgnoreAndReturn(STATUS_SUCCESS);

  for (uint32_t i = 0; i < 2000001; i++)
  {
    default_config.baud_rate = i;

    uint32_t index = get_baud_rate_index(i);
    uart_baudrate_t expected_baud_rate = ((index < sizeof_array(IO_BAUD_RATE)) ? IO_BAUD_RATE[index].output : UART_BAUDRATE_115200);
    //uart_baudrate_t expected_baud_rate = IO_BAUD_RATE[index].output;
    if (index < sizeof_array(IO_BAUD_RATE))
    {
      printf("%u %u\n", i, expected_baud_rate);
    }
    else
    {
      //printf("*");
    }

    memset(&expected_uart_config, 0, sizeof(expected_uart_config));
    expected_uart_config.p_context = NULL;
    expected_uart_config.baudrate = expected_baud_rate;
    expected_uart_config.databits = UART_DATA_BITS_8;
    expected_uart_config.stopbit = UART_STOPBIT_ONE;
    expected_uart_config.parity = UART_PARITY_NONE;
    expected_uart_config.hwfc = UART_HWFC_DISABLED;
    expected_uart_config.interrupt_priority = 3; // IRQ_PRIORITY_NORMAL

    Uart_Rx_ExpectAndReturn(expected_uart_id, NULL, 1, STATUS_SUCCESS);
    Uart_Rx_IgnoreArg_p_data();

    zpal_uart_init(&default_config, &handle);

    TEST_ASSERT_NOT_NULL(handle);
  }
  tr_hal_gpio_set_mode_StopIgnore();
  tr_hal_gpio_set_direction_StopIgnore();
  tr_hal_gpio_set_pull_mode_StopIgnore();

}

typedef struct
{
  uint8_t input;
  uart_databits_t output;
}
data_bits_map_t;

static const data_bits_map_t DATA_BITS_MAP[] = {
  {5, UART_DATA_BITS_5},
  {6, UART_DATA_BITS_6},
  {7, UART_DATA_BITS_7},
  {8, UART_DATA_BITS_8},
};

uint32_t get_data_bits_index(const uint8_t data_bits)
{
  for (uint32_t i = 0; i < sizeof_array(DATA_BITS_MAP); i++)
  {
    if (data_bits == DATA_BITS_MAP[i].input)
    {
      return i;
    }
  }
  return sizeof_array(DATA_BITS_MAP);
}

void test_zpal_uart_init_data_bits(void)
{
  // Checked in uart_init_CALLBACK
  expected_uart_id = 0;

  Uart_Init_Stub(uart_init_CALLBACK); // Handle expectations in the callback.

  Uart_Rx_IgnoreAndReturn(STATUS_SUCCESS);

  tr_hal_gpio_set_mode_IgnoreAndReturn(TR_HAL_SUCCESS);
  tr_hal_gpio_set_direction_IgnoreAndReturn(TR_HAL_SUCCESS);
  tr_hal_gpio_set_pull_mode_IgnoreAndReturn(TR_HAL_SUCCESS);
  Enter_Critical_Section_Ignore();
  Leave_Critical_Section_Ignore();
  Uart_Uninit_IgnoreAndReturn(STATUS_SUCCESS);

  for (uint8_t i = 0; i < UINT8_MAX; i++)
  {
    default_config.data_bits = i;

    uint32_t index = get_data_bits_index(i);
    uart_databits_t expected_data_bits = ((index < sizeof_array(DATA_BITS_MAP)) ? DATA_BITS_MAP[index].output : UART_DATA_BITS_8);

    memset(&expected_uart_config, 0, sizeof(expected_uart_config));
    expected_uart_config.p_context = NULL;
    expected_uart_config.baudrate = UART_BAUDRATE_115200;
    expected_uart_config.databits = expected_data_bits;
    expected_uart_config.stopbit = UART_STOPBIT_ONE;
    expected_uart_config.parity = UART_PARITY_NONE;
    expected_uart_config.hwfc = UART_HWFC_DISABLED;
    expected_uart_config.interrupt_priority = 3; // IRQ_PRIORITY_NORMAL

    zpal_uart_init(&default_config, &handle);
  }

  tr_hal_gpio_set_mode_StopIgnore();
  tr_hal_gpio_set_direction_StopIgnore();
  tr_hal_gpio_set_pull_mode_StopIgnore();
}

typedef struct
{
  zpal_uart_parity_bit_t input;
  uart_parity_t output;
}
parity_bit_map_t;

static const parity_bit_map_t PARITY_BIT_MAP[] = {
  {ZPAL_UART_NO_PARITY,   UART_PARITY_NONE},
  {ZPAL_UART_EVEN_PARITY, UART_PARITY_EVEN},
  {ZPAL_UART_ODD_PARITY,  UART_PARITY_ODD},
};

void test_zpal_uart_init_parity_bit(void)
{
  // Checked in uart_init_CALLBACK
  expected_uart_id = 0;

  Uart_Init_Stub(uart_init_CALLBACK); // Handle expectations in the callback.

  Uart_Rx_IgnoreAndReturn(STATUS_SUCCESS);

  tr_hal_gpio_set_mode_IgnoreAndReturn(TR_HAL_SUCCESS);
  tr_hal_gpio_set_direction_IgnoreAndReturn(TR_HAL_SUCCESS);
  tr_hal_gpio_set_pull_mode_IgnoreAndReturn(TR_HAL_SUCCESS);

  Enter_Critical_Section_Ignore();
  Leave_Critical_Section_Ignore();
  Uart_Uninit_IgnoreAndReturn(STATUS_SUCCESS);

  for (uint8_t i = 0; i < sizeof_array(PARITY_BIT_MAP); i++)
  {
    default_config.parity_bit = PARITY_BIT_MAP[i].input;

    memset(&expected_uart_config, 0, sizeof(expected_uart_config));
    expected_uart_config.p_context = NULL;
    expected_uart_config.baudrate = UART_BAUDRATE_115200;
    expected_uart_config.databits = UART_DATA_BITS_8;
    expected_uart_config.stopbit = UART_STOPBIT_ONE;
    expected_uart_config.parity = PARITY_BIT_MAP[i].output;
    expected_uart_config.hwfc = UART_HWFC_DISABLED;
    expected_uart_config.interrupt_priority = 3;

    zpal_uart_init(&default_config, &handle);
  }

  tr_hal_gpio_set_mode_StopIgnore();
  tr_hal_gpio_set_direction_StopIgnore();
  tr_hal_gpio_set_pull_mode_StopIgnore();
}

typedef struct
{
  zpal_uart_stop_bits_t input;
  uart_stopbit_t output;
}
stop_bit_map_t;

static const stop_bit_map_t STOP_BIT_MAP[] = {
  {ZPAL_UART_STOP_BITS_0P5, UART_STOPBIT_ONE},
  {ZPAL_UART_STOP_BITS_1,   UART_STOPBIT_ONE},
  {ZPAL_UART_STOP_BITS_1P5, UART_STOPBIT_ONE},
  {ZPAL_UART_STOP_BITS_2,   UART_STOPBIT_TWO},
};

void test_zpal_uart_init_stop_bit(void)
{
  // Checked in uart_init_CALLBACK
  expected_uart_id = 0;

  Uart_Init_Stub(uart_init_CALLBACK); // Handle expectations in the callback.

  Uart_Rx_IgnoreAndReturn(STATUS_SUCCESS);

  tr_hal_gpio_set_mode_IgnoreAndReturn(TR_HAL_SUCCESS);
  tr_hal_gpio_set_direction_IgnoreAndReturn(TR_HAL_SUCCESS);
  tr_hal_gpio_set_pull_mode_IgnoreAndReturn(TR_HAL_SUCCESS);
  Enter_Critical_Section_Ignore();
  Leave_Critical_Section_Ignore();
  Uart_Uninit_IgnoreAndReturn(STATUS_SUCCESS);

  for (uint8_t i = 0; i < sizeof_array(STOP_BIT_MAP); i++)
  {
    default_config.stop_bits = STOP_BIT_MAP[i].input;

    memset(&expected_uart_config, 0, sizeof(expected_uart_config));
    expected_uart_config.p_context = NULL;
    expected_uart_config.baudrate = UART_BAUDRATE_115200;
    expected_uart_config.databits = UART_DATA_BITS_8;
    expected_uart_config.stopbit = STOP_BIT_MAP[i].output;
    expected_uart_config.parity = UART_PARITY_NONE;
    expected_uart_config.hwfc = UART_HWFC_DISABLED;
    expected_uart_config.interrupt_priority = 3;

    zpal_uart_init(&default_config, &handle);
  }

  tr_hal_gpio_set_mode_StopIgnore();
  tr_hal_gpio_set_direction_StopIgnore();
  tr_hal_gpio_set_pull_mode_StopIgnore();
}

const zpal_uart_config_ext_t default_pins[3] = {{17,16},{28,29},{30,31}};
const zpal_uart_config_ext_t custom_pins[3] = {{1,2},{3,4},{5,6}};
const zpal_uart_config_ext_t custom_pins_flow_ctrl[3] = {{1,2,3,4},{5,6,7,8},{9,10,11,12}};

typedef struct
{
  uint32_t tx;
  uint32_t rx;
  uint32_t cts;
  uint32_t rts;
}
pin_mode_t;

static const pin_mode_t default_mode[3] = {
  {
    .tx  = TR_HAL_GPIO_MODE_UART_0_TX,
    .rx  = TR_HAL_GPIO_MODE_UART_0_RX,
    .cts = 0,
    .rts = 0
  },
  {
    .tx  = TR_HAL_GPIO_MODE_UART_1_TX,
    .rx  = TR_HAL_GPIO_MODE_UART_1_RX,
    .cts = TR_HAL_GPIO_MODE_UART_1_CTS,
    .rts = TR_HAL_GPIO_MODE_UART_1_RTSN
  },
  {
    .tx  = TR_HAL_GPIO_MODE_UART_2_TX,
    .rx  = TR_HAL_GPIO_MODE_UART_2_RX,
    .cts = TR_HAL_GPIO_MODE_UART_2_CTS,
    .rts = TR_HAL_GPIO_MODE_UART_2_RTSN
  }
};

void test_zpal_uart_init_pins(void)
{
  set_default_uart_configuration();

  Enter_Critical_Section_Ignore();
  Leave_Critical_Section_Ignore();
  Uart_Uninit_IgnoreAndReturn(STATUS_SUCCESS);

  Uart_Init_Stub(uart_init_CALLBACK); // Handle expectations in the callback.

  memset(&expected_uart_config, 0, sizeof(expected_uart_config));
  expected_uart_config.p_context = NULL;
  expected_uart_config.baudrate = UART_BAUDRATE_115200;
  expected_uart_config.databits = UART_DATA_BITS_8;
  expected_uart_config.stopbit = UART_STOPBIT_ONE;
  expected_uart_config.parity = UART_PARITY_NONE;
  expected_uart_config.hwfc = UART_HWFC_DISABLED;
  expected_uart_config.interrupt_priority = 3;

  default_config.ptr = NULL;
  tr_hal_gpio_pin_t _pin;
  // Loop through all UARTS and check default pins
  for (size_t i = 0; i < 3; i++)
  {
    // Set uart number
    default_config.id = i;
    // Checked in uart_init_CALLBACK
    expected_uart_id = i;

    Uart_Rx_IgnoreAndReturn(STATUS_SUCCESS);


    _pin.pin = default_pins[i].txd_pin;
    tr_hal_gpio_set_mode_ExpectAndReturn(_pin, default_mode[i].tx, TR_HAL_SUCCESS );
    tr_hal_gpio_set_pull_mode_ExpectAndReturn(_pin, TR_HAL_PULLOPT_PULL_NONE, TR_HAL_SUCCESS );
    tr_hal_gpio_set_direction_ExpectAndReturn(_pin, TR_HAL_GPIO_DIRECTION_OUTPUT, TR_HAL_SUCCESS );

    _pin.pin = default_pins[i].rxd_pin;
    tr_hal_gpio_set_mode_ExpectAndReturn(_pin, default_mode[i].rx, TR_HAL_SUCCESS );
    tr_hal_gpio_set_direction_ExpectAndReturn(_pin, TR_HAL_GPIO_DIRECTION_INPUT, TR_HAL_SUCCESS );

    zpal_uart_init(&default_config, &handle);
  }

  set_default_uart_configuration();

  // Test custom UART pins with flow ctrl
  for (size_t i = 0; i < 1; i++)
  {
    default_config.ptr = &custom_pins_flow_ctrl[i];
    // Set uart number
    default_config.id = i;
    // Checked in uart_init_CALLBACK
    expected_uart_id = i;

    Uart_Rx_IgnoreAndReturn(STATUS_SUCCESS);
    _pin.pin = custom_pins_flow_ctrl[i].txd_pin;
    tr_hal_gpio_set_mode_ExpectAndReturn(_pin, default_mode[i].tx, TR_HAL_SUCCESS );
    tr_hal_gpio_set_pull_mode_ExpectAndReturn(_pin, TR_HAL_PULLOPT_PULL_NONE, TR_HAL_SUCCESS );
    tr_hal_gpio_set_direction_ExpectAndReturn(_pin, TR_HAL_GPIO_DIRECTION_OUTPUT, TR_HAL_SUCCESS );

    _pin.pin = custom_pins_flow_ctrl[i].rxd_pin;
    tr_hal_gpio_set_mode_ExpectAndReturn(_pin, default_mode[i].rx, TR_HAL_SUCCESS );
    tr_hal_gpio_set_direction_ExpectAndReturn(_pin, TR_HAL_GPIO_DIRECTION_INPUT, TR_HAL_SUCCESS );

    _pin.pin = custom_pins_flow_ctrl[i].cts_pin;
    tr_hal_gpio_set_mode_ExpectAndReturn(_pin, default_mode[i].cts, TR_HAL_SUCCESS );
    tr_hal_gpio_set_direction_ExpectAndReturn(_pin, TR_HAL_GPIO_DIRECTION_INPUT, TR_HAL_SUCCESS );

    _pin.pin = custom_pins_flow_ctrl[i].rts_pin;
    tr_hal_gpio_set_mode_ExpectAndReturn(_pin, default_mode[i].rts, TR_HAL_SUCCESS );
    tr_hal_gpio_set_pull_mode_ExpectAndReturn(_pin, TR_HAL_PULLOPT_PULL_NONE, TR_HAL_SUCCESS );
    tr_hal_gpio_set_direction_ExpectAndReturn(_pin, TR_HAL_GPIO_DIRECTION_OUTPUT, TR_HAL_SUCCESS );

    zpal_uart_init(&default_config, &handle);
  }

  set_default_uart_configuration();

  // Test custom UART pins without flow ctrl
  for (size_t i = 0; i < 3; i++)
  {
    default_config.ptr = &custom_pins[i];
    // Set uart number
    default_config.id = i;
    // Checked in uart_init_CALLBACK
    expected_uart_id = i;

    Uart_Rx_IgnoreAndReturn(STATUS_SUCCESS);

    _pin.pin = custom_pins[i].txd_pin;
    tr_hal_gpio_set_mode_ExpectAndReturn(_pin, default_mode[i].tx, TR_HAL_SUCCESS );
    tr_hal_gpio_set_pull_mode_ExpectAndReturn(_pin, TR_HAL_PULLOPT_PULL_NONE, TR_HAL_SUCCESS );
    tr_hal_gpio_set_direction_ExpectAndReturn(_pin, TR_HAL_GPIO_DIRECTION_OUTPUT, TR_HAL_SUCCESS );

    _pin.pin = custom_pins[i].rxd_pin;
    tr_hal_gpio_set_mode_ExpectAndReturn(_pin, default_mode[i].rx, TR_HAL_SUCCESS );
    tr_hal_gpio_set_direction_ExpectAndReturn(_pin, TR_HAL_GPIO_DIRECTION_INPUT, TR_HAL_SUCCESS );

    zpal_uart_init(&default_config, &handle);
  }

}

void test_zpal_uart_init_fail(void)
{
  tr_hal_gpio_set_mode_IgnoreAndReturn(TR_HAL_SUCCESS);
  tr_hal_gpio_set_direction_IgnoreAndReturn(TR_HAL_SUCCESS);
  tr_hal_gpio_set_pull_mode_IgnoreAndReturn(TR_HAL_SUCCESS);
  Enter_Critical_Section_Ignore();
  Leave_Critical_Section_Ignore();
  Uart_Uninit_IgnoreAndReturn(STATUS_SUCCESS);

  Uart_Init_IgnoreAndReturn(STATUS_ERROR);
  zpal_status_t status = zpal_uart_init(&default_config, &handle);
  TEST_ASSERT_EQUAL(ZPAL_STATUS_FAIL, status);
  TEST_ASSERT_NULL(handle);
  tr_hal_gpio_set_mode_StopIgnore();
  tr_hal_gpio_set_direction_StopIgnore();
  tr_hal_gpio_set_pull_mode_StopIgnore();
}

void test_zpal_uart_enable_disable(void)
{
  tr_hal_gpio_set_mode_IgnoreAndReturn(TR_HAL_SUCCESS);
  tr_hal_gpio_set_direction_IgnoreAndReturn(TR_HAL_SUCCESS);
  tr_hal_gpio_set_pull_mode_IgnoreAndReturn(TR_HAL_SUCCESS);
  Enter_Critical_Section_Ignore();
  Leave_Critical_Section_Ignore();
  Uart_Uninit_IgnoreAndReturn(STATUS_SUCCESS);

  Uart_Init_IgnoreAndReturn(STATUS_SUCCESS);
  Uart_Rx_IgnoreAndReturn(STATUS_SUCCESS);
  zpal_status_t status = zpal_uart_init(&default_config, &handle);
  TEST_ASSERT_EQUAL(ZPAL_STATUS_OK, status);

  // Success
  uart_power_ExpectAndReturn(0, 1, STATUS_SUCCESS);
  status = zpal_uart_enable(handle);
  TEST_ASSERT_EQUAL(ZPAL_STATUS_OK, status);

  uart_power_ExpectAndReturn(0, 0, STATUS_SUCCESS);
  status = zpal_uart_disable(handle);
  TEST_ASSERT_EQUAL(ZPAL_STATUS_OK, status);

  // Failure
  uart_power_ExpectAndReturn(0, 1, STATUS_INVALID_PARAM);
  status = zpal_uart_enable(handle);
  TEST_ASSERT_EQUAL(ZPAL_STATUS_FAIL, status);

  uart_power_ExpectAndReturn(0, 0, STATUS_INVALID_PARAM);
  status = zpal_uart_disable(handle);
  TEST_ASSERT_EQUAL(ZPAL_STATUS_FAIL, status);

  tr_hal_gpio_set_mode_StopIgnore();
  tr_hal_gpio_set_direction_StopIgnore();
  tr_hal_gpio_set_pull_mode_StopIgnore();
}

uart_event_handler_t uart_event_handler;

uint32_t my_uart_init_stub(
  uint32_t uart_id,
  uart_config_t const* p_config,
  uart_event_handler_t event_handler,
  int cmock_num_calls)
{
  (void)uart_id;
  (void)p_config;
  (void)cmock_num_calls;

  // Save the event handler passed to uart_init() so we can invoke it later.
  uart_event_handler = event_handler;

  return STATUS_SUCCESS;
}

static uint32_t zpal_uart_transmit_done_num_calls = 0;

void zpal_uart_transmit_done(zpal_uart_handle_t handle_)
{
  TEST_ASSERT_EQUAL_PTR(handle, handle_);

  zpal_uart_transmit_done_num_calls++;
}

void test_zpal_uart_transmit(void)
{
  tr_hal_gpio_set_mode_IgnoreAndReturn(TR_HAL_SUCCESS);
  tr_hal_gpio_set_direction_IgnoreAndReturn(TR_HAL_SUCCESS);
  tr_hal_gpio_set_pull_mode_IgnoreAndReturn(TR_HAL_SUCCESS);
  Enter_Critical_Section_Ignore();
  Leave_Critical_Section_Ignore();
  Uart_Uninit_IgnoreAndReturn(STATUS_SUCCESS);

  zpal_uart_transmit_done_num_calls = 0;

  Uart_Init_Stub(my_uart_init_stub);
  Uart_Rx_IgnoreAndReturn(STATUS_SUCCESS);
  zpal_status_t status = zpal_uart_init(&default_config, &handle);
  TEST_ASSERT_EQUAL(ZPAL_STATUS_OK, status);

  uint8_t some_data[50];

  Uart_Tx_ExpectWithArrayAndReturn(0, some_data, sizeof(some_data), sizeof(some_data), STATUS_SUCCESS);
  status = zpal_uart_transmit(handle, some_data, sizeof(some_data), zpal_uart_transmit_done);
  TEST_ASSERT_EQUAL(ZPAL_STATUS_OK, status);

  // Invoke event handler with TX done event.
  uart_event_handler(UART_EVENT_TX_DONE, handle);

  TEST_ASSERT_EQUAL_UINT32(1, zpal_uart_transmit_done_num_calls);

  tr_hal_gpio_set_mode_StopIgnore();
  tr_hal_gpio_set_direction_StopIgnore();
  tr_hal_gpio_set_pull_mode_StopIgnore();
}

void test_zpal_uart_transmit_failure(void)
{
  tr_hal_gpio_set_mode_IgnoreAndReturn(TR_HAL_SUCCESS);
  tr_hal_gpio_set_direction_IgnoreAndReturn(TR_HAL_SUCCESS);
  tr_hal_gpio_set_pull_mode_IgnoreAndReturn(TR_HAL_SUCCESS);
  Uart_Uninit_IgnoreAndReturn(STATUS_SUCCESS);
  Uart_Init_IgnoreAndReturn(STATUS_SUCCESS);
  Uart_Rx_IgnoreAndReturn(STATUS_SUCCESS);

  zpal_uart_init(&default_config, &handle);

  uint8_t some_data[50];

  Uart_Tx_IgnoreAndReturn(STATUS_ERROR);
  zpal_status_t status = zpal_uart_transmit(handle, some_data, sizeof(some_data), zpal_uart_transmit_done);
  TEST_ASSERT_EQUAL(ZPAL_STATUS_FAIL, status);

  tr_hal_gpio_set_mode_StopIgnore();
  tr_hal_gpio_set_direction_StopIgnore();
  tr_hal_gpio_set_pull_mode_StopIgnore();
}

void test_zpal_uart_transmit_in_progress(void)
{
  tr_hal_gpio_set_mode_IgnoreAndReturn(TR_HAL_SUCCESS);
  tr_hal_gpio_set_direction_IgnoreAndReturn(TR_HAL_SUCCESS);
  tr_hal_gpio_set_pull_mode_IgnoreAndReturn(TR_HAL_SUCCESS);
  Uart_Uninit_IgnoreAndReturn(STATUS_SUCCESS);
  Uart_Init_IgnoreAndReturn(STATUS_SUCCESS);
  Uart_Rx_IgnoreAndReturn(STATUS_SUCCESS);

  zpal_uart_init(&default_config, &handle);

  tr_uart_trx_complete_ExpectAndReturn(0, false);
  bool retval = zpal_uart_transmit_in_progress(handle);
  TEST_ASSERT_TRUE(retval);

  tr_uart_trx_complete_ExpectAndReturn(0, true);
  retval = zpal_uart_transmit_in_progress(handle);
  TEST_ASSERT_FALSE(retval);

  tr_hal_gpio_set_mode_StopIgnore();
  tr_hal_gpio_set_direction_StopIgnore();
  tr_hal_gpio_set_pull_mode_StopIgnore();
}

uint32_t test_zpal_uart_init_blocking_uart_init_stub(
  uint32_t uart_id,
  uart_config_t const* p_config,
  uart_event_handler_t event_handler,
  int cmock_num_calls)
{
  (void)uart_id;
  (void)p_config;
  (void)cmock_num_calls;

  TEST_ASSERT_NULL(event_handler);

  return STATUS_SUCCESS;
}

void test_zpal_uart_init_blocking(void)
{
  tr_hal_gpio_set_mode_IgnoreAndReturn(TR_HAL_SUCCESS);
  tr_hal_gpio_set_direction_IgnoreAndReturn(TR_HAL_SUCCESS);
  tr_hal_gpio_set_pull_mode_IgnoreAndReturn(TR_HAL_SUCCESS);
  Enter_Critical_Section_Ignore();
  Leave_Critical_Section_Ignore();
  Uart_Uninit_IgnoreAndReturn(STATUS_SUCCESS);
  tr_uart_uninit_blocking_IgnoreAndReturn(STATUS_SUCCESS);

  Uart_Init_IgnoreAndReturn(STATUS_ERROR);

  default_config.flags = ZPAL_UART_CONFIG_FLAG_BLOCKING;

  tr_uart_init_blocking_Stub(test_zpal_uart_init_blocking_uart_init_stub);

  zpal_status_t status = zpal_uart_init(&default_config, &handle);
  TEST_ASSERT_EQUAL(ZPAL_STATUS_OK, status);
  TEST_ASSERT_NOT_NULL(handle);

  tr_hal_gpio_set_mode_StopIgnore();
  tr_hal_gpio_set_direction_StopIgnore();
  tr_hal_gpio_set_pull_mode_StopIgnore();
}

typedef struct
{
  zpal_uart_config_t config;
  zpal_uart_handle_t handle;
}
test_uart_t;

void test_zpal_uart_0_and_1_and_2(void)
{
  tr_hal_gpio_set_mode_IgnoreAndReturn(TR_HAL_SUCCESS);
  tr_hal_gpio_set_direction_IgnoreAndReturn(TR_HAL_SUCCESS);
  tr_hal_gpio_set_pull_mode_IgnoreAndReturn(TR_HAL_SUCCESS);
  Enter_Critical_Section_Ignore();
  Leave_Critical_Section_Ignore();
  Uart_Uninit_IgnoreAndReturn(STATUS_SUCCESS);
  tr_uart_uninit_blocking_IgnoreAndReturn(STATUS_SUCCESS);

  test_uart_t uarts[3];
  for (size_t i = 0; i < 3; i++)
  {
    uarts[i].config = default_config;
    uarts[i].config.id = i;
    uarts[i].config.flags = ZPAL_UART_CONFIG_FLAG_BLOCKING;

    tr_uart_init_blocking_ExpectAndReturn(i, NULL, NULL, STATUS_SUCCESS);
    tr_uart_init_blocking_IgnoreArg_p_config();
    tr_uart_init_blocking_IgnoreArg_event_handler();

    zpal_uart_init(&uarts[i].config, &uarts[i].handle);

    uart_power_ExpectAndReturn(i, 1, STATUS_SUCCESS);

    zpal_uart_enable(uarts[i].handle);
  }

  tr_hal_gpio_set_mode_StopIgnore();
  tr_hal_gpio_set_direction_StopIgnore();
  tr_hal_gpio_set_pull_mode_StopIgnore();

}
