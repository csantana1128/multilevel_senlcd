add_definitions( -DUNIT_TEST )
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")

add_unity_test(NAME test_inclusion_joining_node_slave
  TEST_BASE
    test_inclusion_joining_node.c
  FILES
    ../s2_inclusion.c
    test_inclusion_joining_node.c
  LIBRARIES
    mock
    s2_mock
    s2_inclusion_mocks
    crypto_mocks
)

if(ENABLE_CONTROLLER)
  add_unity_test(NAME test_inclusion_including_node
    FILES
      ../s2_inclusion.c
      ../s2_inclusion_controller.c
      test_inclusion_including_node.c
    LIBRARIES
      mock
      s2_mock
      s2_inclusion_mocks
      crypto_mocks
  )
  set_target_properties(test_inclusion_including_node
    PROPERTIES
      COMPILE_DEFINITIONS "ZW_CONTROLLER"
  )

  add_unity_test(NAME test_inclusion_joining_node_controller
    TEST_BASE
    test_inclusion_joining_node.c
    FILES
      ../s2_inclusion.c
      ../s2_inclusion_controller.c
      test_inclusion_joining_node.c
    LIBRARIES
      mock
      s2_mock
      s2_inclusion_mocks
      crypto_mocks
  )
  set_target_properties(test_inclusion_joining_node_controller
    PROPERTIES
      COMPILE_DEFINITIONS "ZW_CONTROLLER"
  )

  add_unity_test(NAME test_inclusion
    FILES
      ../s2_inclusion.c
      ../s2_inclusion_controller.c
      test_inclusion.c
    LIBRARIES
      mock
      s2_mock
      s2_inclusion_mocks
      s2crypto
      aes
  )
  set_target_properties(test_inclusion
    PROPERTIES
      COMPILE_DEFINITIONS "ZW_CONTROLLER"
  )
endif(ENABLE_CONTROLLER)
