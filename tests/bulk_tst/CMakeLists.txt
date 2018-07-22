cmake_minimum_required(VERSION 3.2)

set(TEST_NAME bulk_test)

include_directories(
        ${CMAKE_SOURCE_DIR}/src/bulk
        ${Boost_INCLUDE_DIR}
        )

file(GLOB SOURCE 
        ${CMAKE_SOURCE_DIR}/src/bulk/*.h
        ${CMAKE_SOURCE_DIR}/src/bulk/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
        )

add_executable(${TEST_NAME} ${SOURCE})

set_target_properties(${TEST_NAME} PROPERTIES
        COMPILE_DEFINITIONS BOOST_TEST_DYN_LINK 
        CXX_STANDARD_REQUIRED ON   
        COMPILE_OPTIONS -Wpedantic -Wall -Wextra
        )

target_link_libraries(${TEST_NAME}
        ${Boost_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT}
        )

add_test(${TEST_NAME} ${TEST_NAME})