/**
 * @file
 * SPDX-License-Identifier: BSD-3-Clause
 * SPDX-FileCopyrightText: 2024 Z-Wave-Alliance <https://z-wavealliance.org/>
 * @note !!! THIS FILE IS AUTOGENERATED BY `generate_configurations.py` !!! DO NOT MODIFY !!!
 */

#ifndef _ZW_CONFIG_RF_H_
#define _ZW_CONFIG_RF_H_

/**
 * \defgroup configuration Configuration
 * Configuration
 *
 * \addtogroup configuration
 * @{
 */
/**
 * \defgroup max_tx_power_lr Max Tx Power Configuration
 * Max Tx Power Configuration
 *
 * \addtogroup max_tx_power_lr
 * @{
 */

/**
 * The maximum allowed Tx power in deci dBm for Z-Wave Long Range network <-200..200:1>
 *
 * The maximum allowed Tx power in deci dBm for Z-Wave Long Range network
 */
#if !defined(APP_MAX_TX_POWER_LR)
#define APP_MAX_TX_POWER_LR  140
#endif /* !defined(APP_MAX_TX_POWER_LR) */

/**
 * The maximum allowed Tx power in deci dBm for Z-Wave network <-200..200:1>
 *
 * The maximum allowed Tx power in deci dBm for Z-Wave network
 */
#if !defined(APP_MAX_TX_POWER)
#define APP_MAX_TX_POWER  0
#endif /* !defined(APP_MAX_TX_POWER) */

/**
 * The deci dBmoutput measured at a PA setting of 0dBm <-50..50:1>
 *
 * The deci dBm output measured at a PA setting of 0dBm
 */
#if !defined(APP_MEASURED_0DBM_TX_POWER)
#define APP_MEASURED_0DBM_TX_POWER  0
#endif /* !defined(APP_MEASURED_0DBM_TX_POWER) */

/**
 * Enable Radio Debug
 *
 * Enable Radio Debug
 */
#if !defined(ENABLE_RADIO_DEBUG)
#define ENABLE_RADIO_DEBUG  0
#endif /* !defined(ENABLE_RADIO_DEBUG) */

/**@}*/ /* \addtogroup max_tx_power_lr */

/**@}*/ /* \addtogroup configuration */
#endif /* _ZW_CONFIG_RF_H_ */
