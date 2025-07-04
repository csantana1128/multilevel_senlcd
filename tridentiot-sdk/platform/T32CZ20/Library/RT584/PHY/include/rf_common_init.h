/**
 * @file rf_common_init.h
 * @author
 * @date
 * @brief Brief single line description use for indexing
 *
 * More detailed description can go here
 *
 * @copyright (C) Rafael Microelectronics. Taiwan
 *
 * @see http://
 */

#ifndef _RF_COMMON_INIT_H__
#define _RF_COMMON_INIT_H__

/**************************************************************************************************
 *    CONSTANTS AND DEFINES
 *************************************************************************************************/

#define RF_CAL_OFF                  (0x00)
#define RF_CAL_PWR_ON_MODE          (0x01)
#define RF_CAL_MP_MODE              (0x02)
#define RF_CAL_FT_MODE              (0x04)
#ifndef RF_CAL_TYPE
#if (CHIP_VERSION == RT58X_MPB)
#if ((RF_MCU_CHIP_MODEL == RF_MCU_CHIP_569M0) && (RF_MCU_CHIP_VER == RF_MCU_CHIP_VER_B))
#define RF_CAL_TYPE                 (RF_CAL_OFF)
#else
#define RF_CAL_TYPE                 (RF_CAL_PWR_ON_MODE)
#endif
#else
#define RF_CAL_TYPE                 (RF_CAL_PWR_ON_MODE)
#endif
#endif

#ifndef RF_FW_INCLUDE_PCI
#define RF_FW_INCLUDE_PCI           (TRUE)
#endif

#ifndef RF_FW_INCLUDE_BLE
#define RF_FW_INCLUDE_BLE           (FALSE)
#endif

#ifndef RF_FW_INCLUDE_MULTI_2P4G
#define RF_FW_INCLUDE_MULTI_2P4G    (FALSE)
#endif

#ifndef RF_FW_INCLUDE_MAC_ACC
#define RF_FW_INCLUDE_MAC_ACC       (FALSE)
#endif

#ifndef RF_FW_INCLUDE_INTERNAL_TEST
#define RF_FW_INCLUDE_INTERNAL_TEST (FALSE)
#endif

#ifndef RF_TX_POWER_COMP
#define RF_TX_POWER_COMP            (FALSE)
#endif

/**************************************************************************************************
*    TYPEDEFS
*************************************************************************************************/
typedef uint8_t RF_FW_LOAD_SELECT;
#define RF_FW_LOAD_SELECT_RUCI_CMD                ((RF_FW_LOAD_SELECT)0x01)
#define RF_FW_LOAD_SELECT_BLE_CONTROLLER          ((RF_FW_LOAD_SELECT)0x02)
#define RF_FW_LOAD_SELECT_MULTI_PROTCOL_2P4G      ((RF_FW_LOAD_SELECT)0x10)
#define RF_FW_LOAD_SELECT_MX_MAC_ACCELARATOR      ((RF_FW_LOAD_SELECT)0x20)
#define RF_FW_LOAD_SELECT_INTERNAL_TEST           ((RF_FW_LOAD_SELECT)0x40)
#define RF_FW_LOAD_SELECT_RFK                     ((RF_FW_LOAD_SELECT)0x80)
#define RF_FW_LOAD_SELECT_UNSUPPORTED_CMD         ((RF_FW_LOAD_SELECT)0xE0)
#define RF_FW_LOAD_SELECT_NONE                    ((RF_FW_LOAD_SELECT)0xFE)
#define RF_FW_LOAD_SELECT_FAIL                    ((RF_FW_LOAD_SELECT)0xFF)

typedef uint8_t RF_CAL_MODE;
#define RF_CAL_MP                                   ((RF_CAL_MODE)0x01)
#define RF_CAL_STARTUP                              ((RF_CAL_MODE)0x02)
#define RF_CAL_AON                                  ((RF_CAL_MODE)0x03)

typedef uint8_t RF_BAND;
#define RF_BAND_2P4G                                ((RF_BAND)0x00)
#define RF_BAND_SUB1G0                              ((RF_BAND)0x01)
#define RF_BAND_SUB1G1                              ((RF_BAND)0x02)
#define RF_BAND_SUB1G2                              ((RF_BAND)0x03)
#define RF_BAND_SUB1G3                              ((RF_BAND)0x04)
#define RF_BAND_MAX                                 ((RF_BAND)0x05)

typedef uint8_t RF_BAND_SUPP_TYPE;
#define RF_BAND_SUPP(x)                             ((RF_BAND_SUPP_TYPE)(1 << x))


/**************************************************************************************************
 *    Global Prototypes
 *************************************************************************************************/
bool rf_common_tx_pwr_comp_set(int8_t offset, uint8_t poly_gain, uint8_t pa_pw_pre, uint8_t modemType);
bool rf_common_tx_pwr_ch_comp_set(int8_t offset0, int8_t offset1, int8_t offset2, int8_t offset3, int8_t modemType);
bool rf_common_init_by_fw(RF_FW_LOAD_SELECT fw_select, COMM_SUBSYSTEM_ISR_t isr_func);
bool rf_common_init_fw_preload(RF_FW_LOAD_SELECT fw_select, COMM_SUBSYSTEM_ISR_t isr_func);

#endif  //_RF_COMMON_INIT_H__


