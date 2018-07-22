cmake_minimum_required(VERSION 3.2)

set_property(GLOBAL PROPERTY ASYNC_LIB async)

file(GLOB SOURCE 
        ${CMAKE_CURRENT_SOURCE_DIR}/*.h
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
        )
        
add_library(${ASYNC_LIB} SHARED ${SOURCE})
set_target_properties(${ASYNC_LIB} PROPERTIES
        CXX_STANDARD_REQUIRED ON
        COMPILE_OPTIONS -Wpedantic -Wall -Wextra
        )

install(TARGETS ${ASYNC_LIB} LIBRARY DESTINATION lib)