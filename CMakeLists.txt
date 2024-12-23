enable_testing()

add_executable(test_runner
    src/gtest/test_Device.cpp
    src/gtest/test_ReedSolomonCodec.cpp
)
target_link_libraries(test_runner PRIVATE GTest::GTest GTest::Main)
