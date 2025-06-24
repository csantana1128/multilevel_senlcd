# SPDX-FileCopyrightText: 2025 Trident IoT, LLC <https://www.tridentiot.com>
# SPDX-License-Identifier: LicenseRef-TridentMSLA

#
# Generates a key pair (private and public) required for signing binaries.
#
# The function supports two configuration parameters:
# 1. ZWSDK_CONFIG_PRIVATE_SIGNING_KEY_PATH
# 2. ZWSDK_CONFIG_PUBLIC_SIGNING_KEY_PATH
#
function(zwsdk_generate_signing_keys)
  message(DEBUG "####################################")
  message(DEBUG "Invoke zwsdk_generate_signing_keys()")

  if(DEFINED ZWSDK_CONFIG_PRIVATE_SIGNING_KEY_PATH)
    if(NOT EXISTS ${ZWSDK_CONFIG_PRIVATE_SIGNING_KEY_PATH})
      message(FATAL_ERROR "Private key set by ZWSDK_CONFIG_PRIVATE_SIGNING_KEY_PATH not found!")
    endif()
    set(CUSTOM_SIGNING_KEY_PRIVATE_IS_SET "")
  endif()

  if(DEFINED ZWSDK_CONFIG_PUBLIC_SIGNING_KEY_PATH)
    if(NOT EXISTS ${ZWSDK_CONFIG_PUBLIC_SIGNING_KEY_PATH})
      message(FATAL_ERROR "Public key set by ZWSDK_CONFIG_PUBLIC_SIGNING_KEY_PATH not found!")
    endif()
    set(CUSTOM_SIGNING_KEY_PUBLIC_IS_SET "")
  endif()

  if(DEFINED CUSTOM_SIGNING_KEY_PRIVATE_IS_SET AND DEFINED CUSTOM_SIGNING_KEY_PUBLIC_IS_SET)
    # Custom keys exist. Use them.
    add_custom_target(signing_keys)
    set_target_properties(signing_keys
      PROPERTIES
        private_key_path ${ZWSDK_CONFIG_PRIVATE_SIGNING_KEY_PATH}
        public_key_path ${ZWSDK_CONFIG_PUBLIC_SIGNING_KEY_PATH}
    )
    message(STATUS "Using private key from ${ZWSDK_CONFIG_PRIVATE_SIGNING_KEY_PATH}")
    message(STATUS "Using public key from ${ZWSDK_CONFIG_PUBLIC_SIGNING_KEY_PATH}")

    get_filename_component(DIR ${ZWSDK_CONFIG_PUBLIC_SIGNING_KEY_PATH} DIRECTORY)
    get_filename_component(NAME_WE ${ZWSDK_CONFIG_PUBLIC_SIGNING_KEY_PATH} NAME_WE)  # Get filename without extension

    set(JSON_FILE "${DIR}/${NAME_WE}.json")  # Replace with new extension

  else()
    set(TEMP_KEYS_DIR             ${CMAKE_BINARY_DIR}/temp_keys)
    set(TEMP_PRIVATE_KEY_PATH     ${TEMP_KEYS_DIR}/temp_private_key.pem)
    set(TEMP_PUBLIC_KEY_DER_PATH  ${TEMP_KEYS_DIR}/temp_public_key.der)
    set(TEMP_PUBLIC_KEY_PATH      ${TEMP_KEYS_DIR}/temp_public_key.json)

    set (JSON_FILE ${TEMP_PUBLIC_KEY_PATH})

    if(EXISTS ${TEMP_PRIVATE_KEY_PATH} AND EXISTS ${TEMP_PUBLIC_KEY_PATH})
      message(DEBUG "Use existing temporary keys.")
    else()
      # If one of the keys exist make sure to clean up.
      file(REMOVE ${TEMP_PRIVATE_KEY_PATH} ${TEMP_PUBLIC_KEY_PATH})

      # Generate keys in default location.
      file(MAKE_DIRECTORY ${TEMP_KEYS_DIR})
      message(DEBUG "Generate temporary private key...")
      execute_process(
        COMMAND openssl ecparam -genkey -name prime256v1 -noout -out ${TEMP_PRIVATE_KEY_PATH}
      )
      message(DEBUG "Generate temporary public key...")
      execute_process(
        COMMAND openssl ec -in ${TEMP_PRIVATE_KEY_PATH} --outform DER  -pubout  -out ${TEMP_PUBLIC_KEY_DER_PATH}
      )
    endif()
    add_custom_target(signing_keys)
    set_target_properties(signing_keys
      PROPERTIES
        private_key_path ${TEMP_PRIVATE_KEY_PATH}
        public_key_path  ${TEMP_PUBLIC_KEY_DER_PATH}
    )
    message(STATUS "Using private key from ${TEMP_PRIVATE_KEY_PATH}")
    message(STATUS "Using public key from ${TEMP_PUBLIC_KEY_PATH}")
  endif()
   if(NOT EXISTS ${JSON_FILE})
    get_target_property(_public_key signing_keys public_key_path)
    message(DEBUG "Generates public key tokens JSON file ${JSON_FILE}...")
    set(PYTHON_COMMAND ${Python3_EXECUTABLE} ${ZW_SDK_ROOT}/tools/public_key_json.py -i ${_public_key} -o ${JSON_FILE})
    message(DEBUG "PYTHON_COMMAND: ${PYTHON_COMMAND}")
    execute_process(
      COMMAND ${PYTHON_COMMAND}
    )
  endif()
  message(DEBUG "####################################")
endfunction()
