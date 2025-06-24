
cmake_minimum_required(VERSION 3.23.5)

project(multilevel_senlcd
        VERSION 1.0.0
        LANGUAGES C ASM)

set(ZWSDK_CONFIG_REGION REGION_US_LR)

set(APP_NAME ${CMAKE_PROJECT_NAME})
set(MULTILEVEL_SENLCD_TARGET_NAME_TEMPLATE "{{name}}_{{hardware}}.elf")

add_subdirectory(tridentiot-sdk/z-wave)
add_subdirectory(hardware)
add_subdirectory(app)
