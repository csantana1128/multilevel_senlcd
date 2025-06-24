/// ****************************************************************************
/// @file zpal_misc_private.h
///
/// @brief Platform specific zpal functions
///
///
/// SPDX-License-Identifier: LicenseRef-TridentMSLA
/// SPDX-FileCopyrightText: 2025 Trident IoT, LLC <https://www.tridentiot.com>
/// ***************************************************************************
#ifndef ZPAL_MISC_PRIVATE_H_
#define ZPAL_MISC_PRIVATE_H_

/**
 * @addtogroup zpal
 * @brief
 * Z-Wave miscellaneous private zpal functions
 * @{
 * @addtogroup zpal-misc
 * @brief
 * Platform specific misc API
 * @{
 */


#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define RESTART_COUNTER_SIZE        4

/**
 * @brief Get the persisted restart counter.
 * This function gets the restart counter from retention RAM.

 * @return The number of restarts since the last call to zpal_clear_restarts()
 *
 */
uint32_t zpal_get_restarts(void);

/**
 * @brief Clear the persisted restart counter.
 * This function clears the restart counter in retention RAM.
 * The restart counter is never zero initialized so it is necessary to clear
 * it manually to get the number of restarts performed over a period of time.
 */
void zpal_clear_restarts(void);

/**
 * @brief Increase the persisted restart counter.
 * This function increases the restart counter in retention RAM.
 */
void zpal_increase_restarts(void);

/**
 * @brief Erase a block of FLASH
 * This function erases a block of FLASH. Minumum size is 4KB.
 */
void zpal_block_flash_erase(uint32_t flash_addr, uint32_t image_size);

#ifdef __cplusplus
}
#endif

#endif /* ZPAL_MISC_PRIVATE_H_ */
