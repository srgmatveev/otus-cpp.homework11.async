cmake_minimum_required(VERSION 3.9)
set(VERSION_MAJOR 0)
set(VERSION_MINOR 0)
set(VERSION_PATCH 1)
if($ENV{TRAVIS_BUILD_NUMBER})
    set(VERSION_PATCH $ENV{TRAVIS_BUILD_NUMBER})
endif()
set(MY_PROJECT_NAME async)
project(${MY_PROJECT_NAME})
find_package(Threads REQUIRED)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(-Wpedantic -Wall -Wextra)

add_library(${MY_PROJECT_NAME} SHARED async.cpp async.h
bulk.cpp bulk_storage.cpp bulk_observer.cpp metrics.cpp)


#add_executable(${MY_PROJECT_NAME}_cli main.cpp)

#target_link_libraries(${MY_PROJECT_NAME}_cli ${MY_PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})

set(BUILD_TEST OFF)
if(${BUILD_TEST})
enable_testing()
include_directories(./)
add_subdirectory(tests)
endif()

#install(TARGETS ${MY_PROJECT_NAME}_cli RUNTIME DESTINATION bin)
install(TARGETS ${MY_PROJECT_NAME} LIBRARY DESTINATION lib)
set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_VERSION_MAJOR "${VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${VERSION_PATCH}")
set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE amd64)
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${MY_PROJECT_NAME}")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Sergio Matveev aka Feiler")
set(CPACK_PACKAGE_CONTACT srgmatveev@gmail.com)

include (CPack)