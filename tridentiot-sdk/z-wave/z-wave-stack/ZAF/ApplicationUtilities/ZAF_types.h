// SPDX-FileCopyrightText: Silicon Laboratories Inc. <https://www.silabs.com/>
//
// SPDX-License-Identifier: BSD-3-Clause

/**
 * @file
 * Contains a number of types commonly used by the ZAF.
 *
 * @warning To keep dependencies clean, this header file MUST NOT depend on any other header files
 *          than standard header files or Z-Wave API header files (located in ZWave/API).
 *
 * @copyright 2018 Silicon Laboratories Inc.
 */

#ifndef ZAF_APPLICATIONUTILITIES_ZAF_TYPES_H_
#define ZAF_APPLICATIONUTILITIES_ZAF_TYPES_H_

#include <ZW_security_api.h>
#include <ZW_classcmd.h>
#include <ZW_transport_api.h>

/**
 * @addtogroup ZAF
 * @{
 * @addtogroup TYPES
 * @{
 */

typedef struct _MULTICHAN_SOURCE_NODE_ID_
{
  node_id_t nodeId;          ///< The ID of the source node.
  /** uint8_t 1, bit 0-6 -
   * Can represent endpoints from 1-127 unless used as a bitmask where endpoint 1-7 is represented.*/
  uint8_t   endpoint   : 7;
  /** uint8_t 1, bit 7   - Reserved */
  uint8_t   res        : 1;
} MULTICHAN_SOURCE_NODE_ID;

typedef struct _MULTICHAN_DEST_NODE_ID_
{
  node_id_t nodeId;          ///< The ID of the destination node.
  /** uint8_t 1, bit 0-6 -
   * Can represent endpoints from 1-127 unless used as a bitmask where endpoint 1-7 is represented. */
  uint8_t   endpoint   : 7;
  /** uint8_t 1, bit 7   -
   * Interprets the endpoint fields as a bitmask addressing only the first 7 endpoints in destination. */
  uint8_t   BitAddress : 1;
} MULTICHAN_DEST_NODE_ID;

/**
 * For the implementation of 16bit NodeId for LR, while keeping backward compatibility, the FRAME-structrues
 * of Z-Wave classic where not copied onto ZAF structures but used with pointers, for this reason, this struct
 * was created to keep those part working.
 * KEEP THE USE OF THIS STRUCT TO BARE MINIMUM!
 */
typedef struct
{
  uint8_t   nodeId;         ///< The ID of the destination node.
  /** uint8_t 1, bit 0-6 -
   * Can represent endpoints from 1-127 unless used as a bitmask where endpoint 1-7 is represented. */
  uint8_t   endpoint   : 7;
  /** uint8_t 1, bit 7   -
   * Interprets the endpoint fields as a bitmask addressing only the first 7 endpoints in destination. */
  uint8_t   BitAddress : 1;
} MULTICHAN_DEST_NODE_ID_8bit;

/**
 * Properties of the received frame. Used mostly by command classes to prepare response to a command.
 */
typedef struct _RECEIVE_OPTIONS_TYPE_EX_
{
  uint8_t rxStatus;                       ///< Frame header info
  security_key_t securityKey;
  MULTICHAN_SOURCE_NODE_ID sourceNode;
  MULTICHAN_DEST_NODE_ID destNode;
  uint8_t bSupervisionActive       : 1;   ///< true if supervision is active
  uint8_t sessionId                : 6;   ///< Current sessionId
  uint8_t statusUpdate             : 1;   ///< Is statusUpdate enabled for current session
} RECEIVE_OPTIONS_TYPE_EX;

/**
 * Allocates space for Supervision when used to declare a buffer.
 */
typedef struct
{
  uint8_t multichanCmdEncap[4]; //4 = sizeof(ZW_MULTI_CHANNEL_CMD_ENCAP_V2_FRAME) - sizeof(ALL_EXCEPT_ENCAP)
  ZW_SUPERVISION_GET_FRAME supervisionGet;
  ZW_APPLICATION_TX_BUFFER appTxBuf;
} ZAF_TRANSPORT_TX_BUFFER;

/**
 * @enum e_cmd_handler_return_code_t
 */
typedef enum
{
  E_CMD_HANDLER_RETURN_CODE_FAIL,           ///< Not accepted or accepted but failed to execute. Command class returns FAIL
  E_CMD_HANDLER_RETURN_CODE_HANDLED,        ///< Accepted and executed by the command handler. Command class returns SUCCESS
  E_CMD_HANDLER_RETURN_CODE_WORKING,        ///< Accepted but is not fully executed. Command class returns WORKING
  E_CMD_HANDLER_RETURN_CODE_NOT_SUPPORTED,  ///< Command handler does not support this command. Command class returns NO_SUPPORT
  E_CMD_HANDLER_RETURN_CODE_NO_CHANGE,      ///< Accepted but device was already in final state, so no change was made. Command class returns SUCCESS
} e_cmd_handler_return_code_t;

/**
 * Status on incoming frame. Use same values as cc_supervision_status_t
 */
typedef enum
{
  RECEIVED_FRAME_STATUS_NO_SUPPORT = 0x00, ///< Frame not supported
  RECEIVED_FRAME_STATUS_WORKING = 0x01,    ///< Frame received successfully and timed change started
  RECEIVED_FRAME_STATUS_FAIL = 0x02,       ///< Could not handle incoming frame
  RECEIVED_FRAME_STATUS_CANCEL = 0x03,     ///< Don't care about status. CC or App will send the report
  RECEIVED_FRAME_STATUS_CC_NOT_FOUND = 0x04,  ///< CC handler was not found in CC handler map
  RECEIVED_FRAME_STATUS_SUCCESS = 0xFF     ///< Frame received successfully
} received_frame_status_t;

/**
 * Can be used for pairing a command class with a specific command in the command class.
 */
typedef struct
{
  uint8_t cmdClass; /**< Command class */
  uint8_t cmd;      /**< Command */
} ccc_pair_t;

/**
 * For backwards compatibility.
 */
typedef ccc_pair_t cc_group_t;
typedef cc_group_t CMD_CLASS_GRP;

/**
 * Can be used for pairing AGI profile identifiers.
 */
typedef struct
{
  uint8_t profile_MS; /**< AGI profile of type: ASSOCIATION_GROUP_INFO_REPORT_AGI_PROFILE_...*/
  uint8_t profile_LS; /**< AGI profile of type: ASSOCIATION_GROUP_INFO_REPORT_AGI_PROFILE_...*/
} agi_profile_t;

/**
 * For backwards compatibility.
 */
typedef agi_profile_t AGI_PROFILE;

/**
 * Callback status used on framework API for request/response-job
 */
typedef enum
{
  JOB_STATUS_SUCCESS = 0,     /**< Job has been started. */
  JOB_STATUS_BUSY,            /**< Job couldn't start.  */
  JOB_STATUS_NO_DESTINATIONS  /**< Job couldn't start because there is no destinations. */
} zaf_job_status_t;

/**
 * For backwards compatibility.
 */
typedef zaf_job_status_t job_status_t;
typedef job_status_t JOB_STATUS;

/**
 * Indicates whether all transmissions are done. Used by \ref TRANSMISSION_RESULT.
 */
typedef enum
{
  TRANSMISSION_RESULT_NOT_FINISHED, /**< Still transmitting. */
  TRANSMISSION_RESULT_FINISHED,     /**< Done transmitting to all nodes. */
  TRANSMISSION_RESULT_UNKNOWN       /**< Reserved for callbacks from the stack
                                         as the stack supplies no valid value
                                         for the finish field. */
} TRANSMISSION_RESULT_FINISH_STATUS;

/**
 * This struct defines the values which can be parsed to a callback function
 * upon an ended transmission regardless of the result.
 */
typedef struct
{
  node_id_t nodeId;  /**< The ID of the node to which the transmission has been done. */
  uint8_t   status;  /**< Status of the transmission. See ZW_transport_api.h. */
  /**
   * If transmission to several nodes, this flag is set if transmission for the last node has ended.
   */
  TRANSMISSION_RESULT_FINISH_STATUS isFinished;
} transmission_result_t;

/**
 * For backwards compatibility.
 */
typedef transmission_result_t TRANSMISSION_RESULT;

/**
 * @brief Defines a type for the generic command class handler.
 */
typedef void (*cc_handler_t)(void); // Generic handler

/**
 * @brief Defines a type for version 1 of the command class handler.
 */
typedef received_frame_status_t(*cc_handler_v1_t)(RECEIVE_OPTIONS_TYPE_EX *, ZW_APPLICATION_TX_BUFFER *, uint8_t);

/**
 * @brief Defines a type for version 2 of the command class handler.
 */
typedef received_frame_status_t(*cc_handler_v2_t)(
    RECEIVE_OPTIONS_TYPE_EX *,
    ZW_APPLICATION_TX_BUFFER *,
    uint8_t,
    ZW_APPLICATION_TX_BUFFER *,
    uint8_t *);

/**
 * @brief Defines the input parameters of a command class handler.
 */
typedef struct _cc_handler_input_t {
  RECEIVE_OPTIONS_TYPE_EX * rx_options;
  ZW_APPLICATION_TX_BUFFER * frame;
  uint8_t length;
} cc_handler_input_t;

/**
 * @brief Defines the output parameters of a command class handler.
 */
typedef struct _cc_handler_output_t {
  ZW_APPLICATION_TX_BUFFER * frame;
  uint8_t length;
  uint8_t duration;
} cc_handler_output_t;

/**
 * @brief Defines a type for version 3 of the command class handler.
 */
typedef received_frame_status_t(*cc_handler_v3_t)(
    cc_handler_input_t *,
    cc_handler_output_t *);

/**
 * Basic Set mapper function type.
 *
 * Use this type when defining a function for Basic Set mapping.
 * @param[out] p_frame The frame that must have command class and command replaced by the command
 *                     class that defines the Basic Set mapping function.
 */
typedef void(* basic_set_mapper_t)(ZW_APPLICATION_TX_BUFFER * p_frame);

/**
 * Basic Get mapper function type.
 *
 * Use this type when defining a function for Basic Get mapping.
 * @param[in]  endpoint        The endpoint that received the Basic Get command.
 * @param[out] p_current_value Pointer to variable where the current value can be written.
 * @param[out] p_target_value  Pointer to variable where the target value can be written.
 * @param[out] p_duration      Pointer to variable where the duration can be written.
 */
typedef void(* basic_get_mapper_t)(uint8_t endpoint, uint8_t * p_current_value, uint8_t * p_target_value, uint8_t * p_duration);

typedef void (*cc_init_function_t)(void); // Generic init function
typedef void (*cc_reset_function_t)(void); // Generic reset function

/**
 * A lifeline report function must take an array of CC pairs as input and return the number of
 * CC pairs being added to the array.
 */
typedef uint8_t(*lifeline_report_get_t)(cc_group_t * p_cc_pair);

/**
 * First generation of the CC handler item.
 */
typedef struct
{
  uint16_t CC;
  uint16_t version;
  cc_handler_t pHandler;
}
CC_handler_map_t;

/**
 * Second generation of the CC handler item.
 */
typedef struct
{
  uint32_t handler_api_version;
  uint16_t CC;
  uint16_t version;
  cc_handler_t handler;
}
CC_handler_map_v2_t;

/**
 * Third generation of the CC handler item.
 */
typedef struct
{
  uint32_t handler_api_version;
  uint16_t CC;
  uint16_t version;
  cc_handler_t handler;
  basic_set_mapper_t basic_set_mapper;
  basic_get_mapper_t basic_get_mapper;
  lifeline_report_get_t lifeline_report_get;
  uint32_t flags;
}
CC_handler_map_v3_t;

/**
 * Fourth generation of the CC handler item.
 */
typedef struct
{
  uint8_t handler_api_version;
  uint16_t CC;
  uint8_t version;
  cc_handler_t handler;
  basic_set_mapper_t basic_set_mapper;
  basic_get_mapper_t basic_get_mapper;
  lifeline_report_get_t lifeline_report_get;
  uint32_t flags;
  cc_init_function_t init;
  cc_reset_function_t reset;
}
CC_handler_map_v4_t;

/**
 * Defines a type for the latest available CC handle type.
 *
 * Users of the CC handle must use this type to avoid changes caused by a future handle version.
 */
typedef CC_handler_map_v4_t CC_handler_map_latest_t;

typedef void(*ZAF_TX_Callback_t)(transmission_result_t * pTxResult);

/**
 * Callback function triggered after completed transmission
 * @param txStatus Transmit complete codes
 * @param extendedTxStatus Extended Tx Status
 */
typedef void(*ZW_TX_Callback_t)(uint8_t txStatus, TX_STATUS_TYPE* extendedTxStatus);

/**
 * Callback function triggered after ZW_SendDataMulti_Bridge completed transmission
 * @param txStatus Transmit complete codes
 */
typedef void(*ZW_TX_Multi_Callback_t)(uint8_t txStatus);

/**
 * Generic Callback function type
 */
typedef void(*ZW_Void_Callback_t)(void);

#ifdef __APPLE__
#define HANDLER_V2_SECTION "__TEXT,__cc_handlers_v3"
extern CC_handler_map_v3_t __start__cc_handlers_v3 __asm("section$start$__TEXT$__cc_handlers_v3");
extern CC_handler_map_v3_t __stop__cc_handlers_v3 __asm("section$end$__TEXT$__cc_handlers_v3");
#else
#define HANDLER_SECTION "zw_cc_handlers_v3"
/**
 * This is the first of the registered app handlers
 */
extern const CC_handler_map_latest_t __start_zw_cc_handlers_v3;
#define cc_handlers_start __start_zw_cc_handlers_v3
/**
 * This marks the end of the handlers. The element
 * after the last element. This means that this element
 * is not valid.
 */
extern const CC_handler_map_latest_t __stop_zw_cc_handlers_v3;
#define cc_handlers_stop __stop_zw_cc_handlers_v3
#endif

/**
 * Registers a given command class with version, handler, etc.
 *
 * Every CC must register itself using the latest REGISTER_CC macro. Doing so will enable ZAF
 * to process certain parts without the need for additional handling in the application.
 * One example being the dispatching of command class frames to the correct command class.
 *
 * Using this macro will make the linker place a variable in a specific linker section which
 * effectively will create an array of registered command classes. ZAF uses this array to look
 * up different information about the supported command classes.
 *
 * Please see existing command classes for examples of usage.
 *
 * @remark Requires a CC handler matching @ref cc_handler_v3_t.
 *
 * @param[in] cc                  The command class number, e.g. COMMAND_CLASS_VERSION.
 * @param[in] version             The version of the command class the the implementation covers.
 * @param[in] handler             Address of the handler function.
 * @param[in] basic_set_mapper    Address of the Basic Set mapper function.
 * @param[in] basic_get_mapper    Address of the Basic Get mapper function.
 * @param[in] lifeline_report_cb  Pointer to a function that will set one or more command class /
 *                                command pairs.
 *                                Some command classes are required to report via Lifeline and each
 *                                Lifeline report callback will populate the list of command class /
 *                                command pairs for the Association Group Command List Report.
 *                                The list of mandatory command class / command pairs can be found
 *                                under "Lifeline Reports" in
 *                                https://sdomembers.z-wavealliance.org/wg/AWG/document/120.
 * @param[in] flags               Reserved for future use.
 * @param[in] init_cb             The CC init function to be invoked by ZAF_Init().
 * @param[in] reset_cb            The CC reset function to be invoked on factory reset.
 */
#define REGISTER_CC_V5(cc,version,handler,basic_set_mapper,basic_get_mapper, lifeline_report_cb, flags, init_cb, reset_cb) \
  static const CC_handler_map_latest_t thisHandler##cc __attribute__((aligned(4), __used__, __section__( HANDLER_SECTION ))) = {3,cc,version,(cc_handler_t)handler,basic_set_mapper,basic_get_mapper,lifeline_report_cb,flags,init_cb,reset_cb}; \
  void * dummy##cc


/**
 * Registers a given command class with version, handler, etc.
 *
 * Every CC must register itself using the latest REGISTER_CC macro. Doing so will enable ZAF
 * to process certain parts without the need for additional handling in the application.
 * One example being the dispatching of command class frames to the correct command class.
 *
 * Using this macro will make the linker place a variable in a specific linker section which
 * effectively will create an array of registered command classes. ZAF uses this array to look
 * up different information about the supported command classes.
 *
 * Please see existing command classes for examples of usage.
 *
 * @remark Requires a CC handler matching @ref cc_handler_v2_t.
 *
 * @param[in] cc                  The command class number, e.g. COMMAND_CLASS_VERSION.
 * @param[in] version             The version of the command class the the implementation covers.
 * @param[in] handler             Address of the handler function.
 * @param[in] basic_set_mapper    Address of the Basic Set mapper function.
 * @param[in] basic_get_mapper    Address of the Basic Get mapper function.
 * @param[in] lifeline_report_cb  Pointer to a function that will set one or more command class /
 *                                command pairs.
 *                                Some command classes are required to report via Lifeline and each
 *                                Lifeline report callback will populate the list of command class /
 *                                command pairs for the Association Group Command List Report.
 *                                The list of mandatory command class / command pairs can be found
 *                                under "Lifeline Reports" in
 *                                https://sdomembers.z-wavealliance.org/wg/AWG/document/120.
 * @param[in] flags               Reserved for future use.
 * @param[in] init_cb             The CC init function to be invoked by ZAF_Init().
 * @param[in] reset_cb            The CC reset function to be invoked on factory reset.
 */
#define REGISTER_CC_V4(cc,version,handler,basic_set_mapper,basic_get_mapper, lifeline_report_cb, flags, init_cb, reset_cb) \
  static const CC_handler_map_latest_t thisHandler##cc __attribute__((aligned(4), __used__, __section__( HANDLER_SECTION ))) = {2,cc,version,(cc_handler_t)handler,basic_set_mapper,basic_get_mapper,lifeline_report_cb,flags,init_cb,reset_cb}; \
  void * dummy##cc

/**
 * Registers a given command class with version, handler, etc.
 *
 * Every CC must register itself using the latest REGISTER_CC macro. Doing so will enable ZAF
 * to process certain parts without the need for additional handling in the application.
 * One example being the dispatching of command class frames to the correct command class.
 *
 * Using this macro will make the linker place a variable in a specific linker section which
 * effectively will create an array of registered command classes. ZAF uses this array to look
 * up different information about the supported command classes.
 *
 * Please see existing command classes for examples of usage.
 *
 * @remark Requires a CC handler matching @ref cc_handler_v2_t.
 *
 * @param[in] cc                  The command class number, e.g. COMMAND_CLASS_VERSION.
 * @param[in] version             The version of the command class the the implementation covers.
 * @param[in] handler             Address of the handler function.
 * @param[in] basic_set_mapper    Address of the Basic Set mapper function.
 * @param[in] basic_get_mapper    Address of the Basic Get mapper function.
 * @param[in] lifeline_report_cb  Pointer to a function that will set one or more command class /
 *                                command pairs.
 *                                Some command classes are required to report via Lifeline and each
 *                                Lifeline report callback will populate the list of command class /
 *                                command pairs for the Association Group Command List Report.
 *                                The list of mandatory command class / command pairs can be found
 *                                under "Lifeline Reports" in
 *                                https://sdomembers.z-wavealliance.org/wg/AWG/document/120.
 * @param[in] flags               Reserved for future use.
 */
#define REGISTER_CC_V3(cc,version,handler,basic_set_mapper,basic_get_mapper, lifeline_report_cb, flags) \
  static const CC_handler_map_latest_t thisHandler##cc __attribute__((aligned(4), __used__, __section__( HANDLER_SECTION ))) = {2,cc,version,(cc_handler_t)handler,basic_set_mapper,basic_get_mapper,lifeline_report_cb,flags,NULL,NULL}; \
  void * dummy##cc

/**
 * Registers a given command class with version, handler, etc.
 *
 * Requires a CC handler matching @ref cc_handler_v2_t.
 */
#define REGISTER_CC_V2(cc,version,handler)                                                                                                              \
  static const CC_handler_map_latest_t thisHandler##cc __attribute__((aligned(4), __used__, __section__( HANDLER_SECTION ))) = {2,cc,version,(cc_handler_t)handler,NULL,NULL,NULL,0,NULL,NULL}; \
  void * dummy##cc

/**
 * Registers a given command class with version, handler, etc.
 *
 * Requires a CC handler matching @ref cc_handler_t.
 */
#define REGISTER_CC(cc,version,handler)                                                                                                                 \
  static const CC_handler_map_latest_t thisHandler##cc __attribute__((aligned(4), __used__, __section__( HANDLER_SECTION ))) = {1,cc,version,(cc_handler_t)handler,NULL,NULL,NULL,0,NULL,NULL}; \
  void * dummy##cc

/**
 * Defines the first two elements for all CC configurations.
 */
typedef struct {
  RECEIVE_OPTIONS_TYPE_EX rxOpt;
  uint8_t endpoint;
} zaf_cc_config_t;

typedef struct {
  uint16_t command_class;
  /**
   * Pointer to first element in an array of configurations or to a single configuration element.
   */
  zaf_cc_config_t * p_cc_configuration;
} zaf_cc_config_entry_v1_t;

/**
 * Eases the upgrade to future versions of the CC config struct.
 */
typedef zaf_cc_config_entry_v1_t zaf_cc_config_entry_latest_t;

#ifdef __APPLE__
#define ZAF_CC_CONFIG_SECTION "__TEXT,__zaf_cc_config"
#else
#define ZAF_CC_CONFIG_SECTION "zw_zaf_cc_config"
#endif

/**
 * Registers a given command class configuration.
 *
 * @param[in] cc     The command class number, e.g. COMMAND_CLASS_VERSION.
 * @param[in] config The address of the corresponding CC configuration.
 * @param[in] index  A unique index for a set of configurations
 */
#define ZAF_CC_REGISTER_CONFIG(cc, config, index)                                                                                                                                 \
  static const zaf_cc_config_entry_latest_t cc_config##index##cc __attribute__((aligned(4), __used__, __section__(ZAF_CC_CONFIG_SECTION))) = { cc, (zaf_cc_config_t *) config }; \
  void * cc_register_config##index##cc

typedef struct {
  uint8_t list_size;
  uint8_t * cc_list;
} zaf_cc_list_t;

/**
 * @} // TYPES
 * @} // ZAF
 */

#endif /* ZAF_APPLICATIONUTILITIES_ZAF_TYPES_H_ */
