/// ***************************************************************************
///
/// @file tr_uart_blocking.h
///
/// SPDX-License-Identifier: LicenseRef-TridentMSLA
/// SPDX-FileCopyrightText: 2025 Trident IoT, LLC <https://www.tridentiot.com>
/// ***************************************************************************

/**
* @defgroup UART_group UART
* @ingroup peripheral_group
* @{
* @brief  Define UART definitions, structures, and functions
*/
#ifndef _TR_UART_BLOCKING_H__
#define _TR_UART_BLOCKING_H__

#ifdef __cplusplus
extern "C"
{
#endif

/**************************************************************************************************
 *    INCLUDES
 *************************************************************************************************/
#include <stdbool.h>
#include <stdint.h>
#include <uart_drv.h>
/***********************************************************************************************************************
 *    CONSTANTS AND DEFINES
 ***********************************************************************************************************************/

/**
 * @brief Function for initializing the UART driver.
 *
 *  This function configures and enables UART.
 *  Call this function will auto power-on the uart device.
 *
 * @param[in] uart_id       identifier for uart device, 0 for uart0, 1 for uart1
 * @param[in] p_config      Pointer to the structure with initial configuration.
 * @param[in] event_handler Event handler provided by the user. If not provided driver works in
 *                          blocking mode.
 *
 * @retval    STATUS_SUCCESS           If initialization was successful.
 * @retval    STATUS_INVALID_REQUEST   If driver is already initialized.
 * @retval    STATUS_INVALID_PARAM     Wrong parameter, for example uart_id > maximum real uart device number
 *                                      or p_config is NULL.
 */
uint32_t tr_uart_init_blocking(uint32_t uart_id, uart_config_t const *p_config, uart_event_handler_t  event_handler);



/**
* @brief Function for uninitializing the UART driver.
*
*   Call this function will also turn off the clock of the uart device
* If the uart device has wakeup feature, please don't call this function when sleep
*
* @param[in] uart_id       identifier for uart device
*
* @retval    STATUS_SUCCESS           If uninitialization was successful.
* @retval    STATUS_INVALID_PARAM     Wrong parameter, uart_id is invalid number.
*
*/
uint32_t tr_uart_uninit_blocking(uint32_t uart_id);

/**
 * @brief Function for sending data over UART.
 *
 * If an event handler was provided in uart_init() call, this function
 * returns immediately and the handler is called when the transfer is done.
 * Otherwise, the transfer is performed in blocking mode, i.e. this function
 * returns when the transfer is finished.
 *
 *   During the operation it is not allowed to call this function or any other data
 * transfer function again in non-blocking mode. Also the data buffers must stay
 * allocated and the contents of data must not be modified.
 *
 *   Status of the transmitter can be monitored by calling the function
 * "uart_status_get" and checking the tx_busy flag which indicates if transmission
 * is still in progress.
 *
 * @param[in] identifier for uart device,
 * @param[in] p_data     Pointer to data.
 * @param[in] length     Number of bytes to send. Maximum possible length is 4095
 *
 * @retval    STATUS_SUCCESS           If transmit was successful. For non-block mode,
 *                                      data is transmiting.
 * @retval    STATUS_INVALID_PARAM     Wrong parameter, like uart_id is invalid number
 *                                      length size is too large or zero.
 *
 * @retval    STATUS_NO_INIT           uart device did NOT configure yet.
 *
 * @retval    STATUS_EBUSY             uart device is already in previous transmitting.
 *
 *
 */
uint32_t  tr_uart_tx_blocking(uint32_t uart_id, uint8_t const *p_data, uint32_t length);

/**
 * @brief Function for checking if UART is currently transmitting or receiving.
 *
 * @param[in]  uart_id  identifier for uart device.
 *
 * @retval true  If UART is transmitting and receiving completed.
 * @retval false If UART is transmitting and receiving incomplete.
 */
bool tr_uart_trx_complete(uint32_t uart_id);
#ifdef __cplusplus
}
#endif

#endif /* End of _UART_DRV_NON_BLOCKING_H__*/
/** @} */ /* End of Peripheral Group */
