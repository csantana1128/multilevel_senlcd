/// ****************************************************************************
/// @file tr_hal_common.h
///
/// @brief This is the common include file for the Trident HAL
///
///
/// SPDX-License-Identifier: LicenseRef-TridentMSLA
/// SPDX-FileCopyrightText: 2025 Trident IoT, LLC <https://www.tridentiot.com>
/// ***************************************************************************

#ifndef TR_HAL_COMMON_H_
#define TR_HAL_COMMON_H_


/// ****************************************************************************
/// @defgroup tr_hal_common Error Codes
/// @ingroup tr_hal_api
/// @{
/// ****************************************************************************

/// ***************************************************************************
/// typedef for status codes for Trident HAL
/// ***************************************************************************
typedef enum
{
    // common success status
    TR_HAL_SUCCESS                     = 0,

    // common error codes 1-29
    TR_HAL_STATUS_NO_DATA_AVAILABLE    = 1,
    TR_HAL_STATUS_MORE_BYTES           = 2,
    TR_HAL_STATUS_DONE                 = 3,
    TR_HAL_ERROR_RECEIVE_FX_HANDLES_RX = 4,
    TR_HAL_ERROR_DMA_HANDLES_RX        = 5,
    TR_HAL_ERROR_DMA_HANDLES_TX        = 6,
    TR_HAL_ERROR_RAW_TX_BUFFER_MISSING = 7,
    TR_HAL_ERROR_TX_BUFFER_TOO_LONG    = 8,
    TR_HAL_ERROR_ALREADY_INITIALIZED   = 9,
    TR_HAL_ERROR_NOT_INITIALIZED       = 10,
    TR_HAL_TRANSMITTER_BUSY            = 11,
    TR_HAL_ERROR_DMA_NOT_ENABLED       = 12,
    TR_HAL_ERROR_DMA_RX_BUFFER_MISSING = 13,
    TR_HAL_ERROR_DMA_RX_BUFF_BAD_LEN   = 14,
    TR_HAL_ERROR_NULL_PARAMS           = 15,
    TR_HAL_ERROR_INT_PRI_TOO_LOW       = 16,
    TR_HAL_ERROR_INT_PRI_TOO_HIGH      = 17,
    TR_HAL_ERROR_UNSUPPORTED           = 18,
    TR_HAL_TRNG_BUSY                   = 19,
    TR_HAL_UNSUPPORTED_CLOCK           = 20,
    TR_HAL_UNSUPPORTED_POWER_MODE      = 21,
    TR_HAL_ERROR_INVALID_CLOCK         = 22,

    // GPIO driver error codes 30-49
    TR_HAL_ERROR_PIN_NOT_AVAILABLE     = 30,
    TR_HAL_ERROR_INVALID_PARAM         = 31,
    TR_HAL_ERROR_PIN_MUST_BE_INPUT     = 32,
    TR_HAL_ERROR_PIN_MUST_BE_OUTPUT    = 33,
    TR_HAL_ERROR_UNKNOWN               = 34,
    TR_HAL_ERROR_INVALID_DIRECTION     = 35,
    TR_HAL_ERROR_INVALID_PULL_MODE     = 36,
    TR_HAL_ERROR_INVALID_DRV_STR       = 37,
    TR_HAL_ERROR_INVALID_LEVEL         = 38,
    TR_HAL_ERROR_INVALID_INT_TRIGGER   = 39,

    // UART driver error codes 50-69
    TR_HAL_ERROR_INVALID_UART_ID       = 50,
    TR_HAL_ERROR_HWFC_NOT_VALID        = 52,
    TR_HAL_ERROR_BAD_PARAM             = 53,
    TR_HAL_ERROR_SET_PIN_FAILED        = 54,
    TR_HAL_ERROR_UART_NOT_POWERED      = 55,
    TR_HAL_ERROR_SEND_FAILED_CHIP_BUSY = 56,
    TR_HAL_ERROR_DMA_BUFFER_TOO_SMALL  = 57,
    TR_HAL_ERROR_INVALID_PINS          = 58,
    TR_HAL_ERROR_INVALID_BAUD_RATE     = 59,
    TR_HAL_ERROR_TRIGGER_BYTES_INVALID = 60,
    TR_HAL_DMA_BUFF_UNALIGNED_LENGTH   = 61,
    
    // TIMER driver error codes 70-79
    TR_HAL_INVALID_TIMER_ID            = 70,
    TR_HAL_TIMER_CALLBACK_NULL         = 71,
    TR_HAL_ERROR_BAD_PRESCALAR         = 72,
    TR_HAL_TIMER_NULL_SETTINGS         = 73,
    
    // SPI error codes 80-99
    TR_HAL_INVALID_SPI_ID              = 80,
    TR_HAL_SPI_INVALID_CLK_PIN         = 81,
    TR_HAL_SPI_INVALID_CS0_PIN         = 82,
    TR_HAL_SPI_INVALID_CS1_PIN         = 83,
    TR_HAL_SPI_INVALID_CS2_PIN         = 84,
    TR_HAL_SPI_INVALID_CS3_PIN         = 85,
    TR_HAL_SPI_INVALID_IO0_PIN         = 86,
    TR_HAL_SPI_INVALID_IO1_PIN         = 87,
    TR_HAL_SPI_INVALID_IO2_PIN         = 88,
    TR_HAL_SPI_INVALID_IO3_PIN         = 89,
    TR_HAL_SPI_UNSUPPORTED_MODE        = 90,
    TR_HAL_SPI_UNSUPPORTED_WMARK       = 91,
    TR_HAL_SPI_NULL_SETTINGS           = 92,
    TR_HAL_SPI_INVALID_CS_INDEX        = 93,
    
    // RTC error codes
    TR_HAL_RTC_INVALID_YEAR            = 100,
    TR_HAL_RTC_INVALID_MONTH           = 101,
    TR_HAL_RTC_INVALID_DAY             = 102,
    TR_HAL_RTC_INVALID_HOUR            = 103,
    TR_HAL_RTC_INVALID_MINUTE          = 104,
    TR_HAL_RTC_INVALID_SECOND          = 105,
    TR_HAL_RTC_INVALID_MILLISECOND     = 106,
    TR_HAL_RTC_INVALID_TIME_UNIT       = 107,
    TR_HAL_RTC_EVENT_CONFLICT          = 108,

    // WDOG error codes
    TR_HAL_WDOG_MIN_TIME_TOO_LARGE     = 110,
    TR_HAL_WDOG_INITIAL_TIME_TOO_SMALL = 111,
    TR_HAL_WDOG_INT_TIME_TOO_LARGE     = 112,
    TR_HAL_WDOG_NOT_INITIALIZED        = 113,
    TR_HAL_WDOG_STATE_AND_REG_OUT_OF_SYNC = 114,

    // I2C error codes
    TR_HAL_INVALID_I2C_ID              = 120,
    TR_HAL_I2C_NULL_SETTINGS           = 121,
    TR_HAL_I2C_WRITE_TIMEOUT_ERROR     = 122,
    TR_HAL_I2C_READ_TIMEOUT_ERROR      = 123,
    TR_HAL_I2C_WRITE_BYTES_TOO_LARGE   = 124,
    TR_HAL_I2C_READ_BYTES_TOO_LARGE    = 125,
    TR_HAL_I2C_BAD_ADDRESS             = 126,
    TR_HAL_I2C_LOST_ARB                = 127,
    TR_HAL_I2C_NO_BYTE_READ            = 128,
    TR_HAL_I2C_INVALID_SDA_PIN         = 129,
    TR_HAL_I2C_INVALID_SCL_PIN         = 130,

} tr_hal_status_t;


/// ****************************************************************************
/// @} // end of tr_hal_api
/// ****************************************************************************


#endif //TR_HAL_COMMON_H_
