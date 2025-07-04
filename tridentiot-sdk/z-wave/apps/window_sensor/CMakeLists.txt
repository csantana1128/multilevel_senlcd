# SPDX-FileCopyrightText: 2025 Trident IoT, LLC <https://www.tridentiot.com>
# SPDX-License-Identifier: LicenseRef-TridentMSLA

if(NOT DEFINED APP_NAME)
  set(APP_NAME "window_sensor")
endif()

set(ZW_INCLUDES
  ../../platform/apps/inc
  ${CONFIG_DIR}/zw_rf
  ${ZW_SDK_ROOT}/modules/api/
)

set(APP_SOURCES_REGION
  # Nothing here.
)

set(APP_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/app.c
  ${ZAF_PROTOCOL_CONFIG}/src/zaf_protocol_config.c
  ${ZW_SDK_ROOT}/platform/apps/src/adc_drv.c
)

set(ZW_DEFINITIONS
  ZAF_CONFIG_PRODUCT_ID=3
  ZAF_CONFIG_GENERIC_TYPE=GENERIC_TYPE_SENSOR_NOTIFICATION
  ZAF_CONFIG_SPECIFIC_TYPE=SPECIFIC_TYPE_NOTIFICATION_SENSOR
  ZAF_CONFIG_REQUEST_KEY_S2_UNAUTHENTICATED=1
  ZAF_CONFIG_REQUEST_KEY_S2_AUTHENTICATED=1
  ZAF_CONFIG_DEVICE_OPTION_MASK=APPLICATION_NODEINFO_NOT_LISTENING
  ZAF_CONFIG_APP_ROLE_TYPE=ZWAVEPLUS_INFO_REPORT_ROLE_TYPE_END_NODE_SLEEPING_REPORTING
  ZAF_CONFIG_INSTALLER_ICON_TYPE=ICON_TYPE_SPECIFIC_SENSOR_NOTIFICATION_ACCESS_CONTROL
  ZAF_CONFIG_USER_ICON_TYPE=ICON_TYPE_SPECIFIC_SENSOR_NOTIFICATION_ACCESS_CONTROL
  CC_ASSOCIATION_MAX_GROUPS_PER_ENDPOINT=2
  ZAF_TRANSPORT_CONFIG_QUEUE_SIZE=4
  ZAF_CONFIG_MANUFACTURER_ID=${ZW_CONFIG_MANUFACTURER_ID}
  APP_MAX_TX_POWER_LR=200
  APP_MAX_TX_POWER=0
  APP_MEASURED_0DBM_TX_POWER=0
)

set(ZW_LIBS
  ZWaveSlave
  ZAF
  ZAF_EventDistributor_soc
  ZAF_TSE
  CC_BasicController
  CC_Battery
  CC_Battery_nvm
  CC_Notification
  CC_WakeUp
  zaf_transport_layer
  AppsHw_platform
)

IF(CMAKE_BUILD_TYPE MATCHES Debug)
  list(APPEND ZW_LIBS ZAF_DEBUGPRINT)
ENDIF()

if(CMAKE_BUILD_TYPE MATCHES Release OR CMAKE_BUILD_TYPE MATCHES Debug)
  zw_create_app(
    "${APP_NAME}"
    "${APP_SOURCES}"
    "${APP_SOURCES_REGION}"
    "${ZW_INCLUDES}"
    "${ZW_LIBS}"
    "${ZW_DEFINITIONS}"
    "${FREQ_LIST}"
  )
endif()
