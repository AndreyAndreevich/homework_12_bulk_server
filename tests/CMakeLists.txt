cmake_minimum_required(VERSION 3.2)

find_package(Boost COMPONENTS unit_test_framework REQUIRED)
find_package(Threads REQUIRED QUIET)

set(TEST_NAME async_test)
set(MT_TEST_NAME mt_test)

include_directories(
        ${CMAKE_SOURCE_DIR}/src
        ${Boost_INCLUDE_DIR}
        )

set(TESTS 
        ${CMAKE_CURRENT_SOURCE_DIR}/parser_test.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/writers_test.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/handler_test.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/bulk_test.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/storage_test.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/event_loop_test.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/async_test.cpp
        )

set(MT_TESTS ${CMAKE_CURRENT_SOURCE_DIR}/multi_thread_test.cpp)

add_executable(${TEST_NAME} ${TESTS})
add_executable(${MT_TEST_NAME} ${MT_TESTS})

set_target_properties(${TEST_NAME} ${MT_TEST_NAME} PROPERTIES
        COMPILE_DEFINITIONS BOOST_TEST_DYN_LINK 
        CXX_STANDARD_REQUIRED ON   
        COMPILE_OPTIONS -Wpedantic -Wall -Wextra
        )

target_link_libraries(${TEST_NAME}
        ${Boost_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT}
        ${MY_BEST_LIB}
        )

target_link_libraries(${MT_TEST_NAME}
        ${Boost_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT}
        ${MY_BEST_LIB}
        )

add_test(${TEST_NAME} ${TEST_NAME})
add_test(${MT_TEST_NAME} ${MT_TEST_NAME})