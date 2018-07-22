cmake_minimum_required(VERSION 3.2)

set(SERVER ${PROJECT_NAME})

file(GLOB SOURCE 
        ${CMAKE_CURRENT_SOURCE_DIR}/*.h
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
        )
        
add_executable(${SERVER} ${SOURCE})
set_target_properties(${ASYNC_LIB} PROPERTIES
        CXX_STANDARD_REQUIRED ON
        COMPILE_OPTIONS -Wpedantic -Wall -Wextra
        )

install(TARGETS ${SERVER} RUNTIME DESTINATION bin)