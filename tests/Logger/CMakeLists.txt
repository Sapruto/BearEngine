cmake_minimum_required(VERSION 3.10)
project(LoggerTests)

set(LOGGER_TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(ENGINE_ROOT    ${CMAKE_CURRENT_SOURCE_DIR}/../..)

add_executable(logger_tests
    ${LOGGER_TEST_DIR}/test_logger.cpp
)

set_target_properties(logger_tests PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

target_include_directories(logger_tests PRIVATE
    ${ENGINE_ROOT}/include
    ${LOGGER_TEST_DIR}
)

find_package(GTest QUIET)
if(GTest_FOUND)
    target_link_libraries(logger_tests PRIVATE
        GTest::GTest
        GTest::Main
    )
else()
    message(FATAL_ERROR "GTest not found")
endif()

add_test(NAME LoggerTests           COMMAND logger_tests)
add_test(NAME LoggerTests.Levels     COMMAND logger_tests --gtest_filter=LoggerLevels*)
add_test(NAME LoggerTests.Formatting COMMAND logger_tests --gtest_filter=LoggerFormatting*)
add_test(NAME LoggerTests.Settings   COMMAND logger_tests --gtest_filter=LoggerSettings*)