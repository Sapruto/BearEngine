cmake_minimum_required(VERSION 3.10)
project(MathTests)

set(MATH_TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(math_tests
    ${MATH_TEST_DIR}/test_math.cpp
)

target_include_directories(math_tests PRIVATE
    ${ENGINE_INCLUDE_DIR}
    ${ENGINE_ROOT}/src
    ${MATH_TEST_DIR}
)

target_link_libraries(math_tests
    BearEngine
    ${OPENGL_LIBRARIES}
)

add_test(
    NAME MathTests
    COMMAND math_tests
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

add_test(
    NAME MathTests.Vector3
    COMMAND math_tests --gtest_filter=Vector3*
)

add_test(
    NAME MathTests.Quaternion
    COMMAND math_tests --gtest_filter=Quaternion*
)

add_test(
    NAME MathTests.Matrix
    COMMAND math_tests --gtest_filter=Matrix*
)

find_package(GTest QUIET)
if(GTest_FOUND)
    target_link_libraries(math_tests
        GTest::GTest
        GTest::Main
    )
endif()