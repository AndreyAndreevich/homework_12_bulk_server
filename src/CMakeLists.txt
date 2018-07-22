cmake_minimum_required(VERSION 3.2)

set(SERVER ${PROJECT_NAME})

include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/bulk
        ${CMAKE_CURRENT_SOURCE_DIR}/server
        ${Boost_INCLUDE_DIR}
        )

file(GLOB SOURCE 
        ${CMAKE_CURRENT_SOURCE_DIR}/bulk/*.h
        ${CMAKE_CURRENT_SOURCE_DIR}/bulk/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/server/*.h
        ${CMAKE_CURRENT_SOURCE_DIR}/server/*.cpp
        )
        
add_executable(${SERVER} ${SOURCE})
set_target_properties(${SERVER} PROPERTIES
        CXX_STANDARD_REQUIRED ON
        COMPILE_DEFINITIONS BOOST_ASIO_ENABLE_HANDLER_TRACKING
        COMPILE_OPTIONS -Wpedantic -Wall -Wextra
        )

target_link_libraries(${SERVER}
        ${Boost_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT}
        ${ASYNC_LIB}
        )

install(TARGETS ${SERVER} RUNTIME DESTINATION bin)