# SPDX-FileCopyrightText: 2025 Trident IoT, LLC <https://www.tridentiot.com>
# SPDX-License-Identifier: LicenseRef-TridentMSLA

IF( CMAKE_BUILD_TYPE MATCHES Release OR CMAKE_BUILD_TYPE MATCHES Debug )

  set(ELF_TARGET "zw_radio_cli.elf")
  add_executable(${ELF_TARGET}
  radio_cli_startup.c
  radio_cli_app.c
  cli_uart_interface.c
  radio_cli_commands.c
  ../../z-wave-stack/Components/DebugPrint/DebugPrint.c
  ../../z-wave-stack/Components/Assert/Assert_zw.c
  ../../z-wave-stack/Components/EventDistributor/EventDistributor.c
  ../../ThirdParty/embedded-cli/lib/src/embedded_cli.c
  ../../platform/apps/src/firmware_properties.c
  )

  target_compile_definitions(${ELF_TARGET}
    PUBLIC
      -DTR_PLATFORM_${PLATFORM_VARIANT}
      -DGIT_HASH_ID="${GIT_HASH}"
      -DZAF_CONFIG_MANUFACTURER_ID=${ZW_CONFIG_MANUFACTURER_ID}
  )

  target_include_directories(${ELF_TARGET} PRIVATE
    .
    ../../z-wave-stack/Components/DebugPrint
    ../../z-wave-stack/Components/Assert
    ../../z-wave-stack/Components/EventDistributor
    ../../ThirdParty/embedded-cli/lib/include/
    ../../modules/api/
    ../../platform/apps/inc/
    ../../platform/TridentIoT/inc/)

  target_link_libraries(${ELF_TARGET} PRIVATE zpal_${PLATFORM_VARIANT})

  IF(CMAKE_BUILD_TYPE MATCHES Debug)
    list(APPEND ZW_LIBS ZAF_DEBUGPRINT)
  ENDIF()

  set_target_properties(${ELF_TARGET} PROPERTIES
    LINK_FLAGS "-Xlinker -Map=${ELF_TARGET}.map"
  )

  string(REPLACE ".elf" ""  ELF_NAME ${ELF_TARGET})
  to_hex(${ELF_NAME}.elf ${ELF_NAME}.hex)
  zwsdk_generate_fw_update_image(${ELF_NAME})

ENDIF()
