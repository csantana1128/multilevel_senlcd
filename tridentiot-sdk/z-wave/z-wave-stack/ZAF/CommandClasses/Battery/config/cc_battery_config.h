/**
 * @file
 * SPDX-License-Identifier: BSD-3-Clause
 * SPDX-FileCopyrightText: 2024 Z-Wave-Alliance <https://z-wavealliance.org/>
 * @note !!! THIS FILE IS AUTOGENERATED BY `generate_configurations.py` !!! DO NOT MODIFY !!!
 */

#ifndef _CC_BATTERY_CONFIG_H_
#define _CC_BATTERY_CONFIG_H_

/**
 * \defgroup configuration Configuration
 * Configuration
 *
 * \addtogroup configuration
 * @{
 */
/**
 * \defgroup command_class_battery_configuration Command Class Battery Configuration
 * Command Class Battery Configuration
 *
 * \addtogroup command_class_battery_configuration
 * @{
 */

/**
 * Battery reporting decrements <1..255:1>
 *
 */
#if !defined(CC_BATTERY_REPORTING_DECREMENTS)
#define CC_BATTERY_REPORTING_DECREMENTS  10
#endif /* !defined(CC_BATTERY_REPORTING_DECREMENTS) */

/**@}*/ /* \addtogroup command_class_battery_configuration */

/**@}*/ /* \addtogroup configuration */
#endif /* _CC_BATTERY_CONFIG_H_ */
