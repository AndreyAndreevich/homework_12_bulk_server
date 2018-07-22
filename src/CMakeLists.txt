cmake_minimum_required(VERSION 3.2)

file(GLOB SOURCE 
        ${CMAKE_CURRENT_SOURCE_DIR}/*.h
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
        )
        
add_library(${MY_BEST_LIB} SHARED ${SOURCE})
set_target_properties(${MY_BEST_LIB} PROPERTIES
        CXX_STANDARD_REQUIRED ON
        COMPILE_OPTIONS -Wpedantic -Wall -Wextra
        )

install(TARGETS ${MY_BEST_LIB} LIBRARY DESTINATION lib)