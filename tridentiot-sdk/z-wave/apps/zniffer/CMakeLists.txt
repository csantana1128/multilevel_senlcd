# SPDX-FileCopyrightText: 2025 Trident IoT, LLC <https://www.tridentiot.com>
# SPDX-License-Identifier: LicenseRef-TridentMSLA

IF( CMAKE_BUILD_TYPE MATCHES Release OR CMAKE_BUILD_TYPE MATCHES Debug )

  set(ELF_TARGET "zw_zniffer.elf")
  add_executable(${ELF_TARGET}
  zniffer_startup.c
  zniffer_app.c
  comm_interface.c
  ../../z-wave-stack/Components/DebugPrint/DebugPrint.c
  ../../z-wave-stack/Components/EventDistributor/EventDistributor.c
  ../../z-wave-stack/Components/Assert/Assert_zw.c
  ../../platform/apps/src/firmware_properties.c
  )

  target_include_directories(${ELF_TARGET} PRIVATE
    .
    ../../z-wave-stack/Components/DebugPrint
    ../../z-wave-stack/Components/EventDistributor
    ../../z-wave-stack/Components/Assert
    ../../platform/apps/inc
    ../../platform/TridentIoT/inc)

  target_link_libraries(${ELF_TARGET} PUBLIC zpal_${PLATFORM_VARIANT})

  target_compile_definitions(${ELF_TARGET}
    PRIVATE
      -DTR_PLATFORM_${PLATFORM_VARIANT}
      -DZAF_CONFIG_MANUFACTURER_ID=${ZW_CONFIG_MANUFACTURER_ID}
  )

  string(REPLACE ".elf" ""  ELF_NAME ${ELF_TARGET})
  to_hex(${ELF_NAME}.elf ${ELF_NAME}.hex)
  zwsdk_generate_fw_update_image(${ELF_NAME})

ENDIF()
