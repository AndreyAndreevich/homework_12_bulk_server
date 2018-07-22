cmake_minimum_required(VERSION 3.2)

if($ENV{TRAVIS_BUILD_NUMBER})
        project(bulk_server VERSION 0.0.$ENV{TRAVIS_BUILD_NUMBER})
else()
        project(bulk_server VERSION 0.0.1)  
endif()

set(CMAKE_CXX_STANDARD 14)

add_subdirectory(src)
add_subdirectory(tests)

set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_PACKAGE_CONTACT l.a.r.p@yandex.ru)

include(CPack)

enable_testing()