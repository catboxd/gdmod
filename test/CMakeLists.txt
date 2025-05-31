cmake_minimum_required(VERSION 3.21)    
# cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if ("${CMAKE_SYSTEM_NAME}" STREQUAL "iOS" OR IOS)
    set(CMAKE_OSX_ARCHITECTURES "arm64")
else()
    set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
endif()
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
project(test VERSION 1.0.0)

# Add all source files inside src (recursively)
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp)

# Set up the mod binary
add_library(${PROJECT_NAME} SHARED ${SOURCES})



if (NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR "Unable to find Geode SDK! Please define GEODE_SDK environment variable to point to Geode")
else()
    message(STATUS "Found Geode: $ENV{GEODE_SDK}")
endif()

add_subdirectory($ENV{GEODE_SDK} ${CMAKE_CURRENT_BINARY_DIR}/geode)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    add_compile_definitions(WIN32_LEAN_AND_MEAN=1)
    target_link_libraries(${PROJECT_NAME} Crypt32)
endif()

set(USE_ZLIB OFF)
if (NOT APPLE)
    set(USE_MBED_TLS ON)
endif()
set(USE_OPEN_SSL OFF)
SET(CMAKE_C_SIMULATE_ID "Clang")
SET(CMAKE_C_SIMULATE_ID "Clang")
set(ENABLE_PROGRAMS OFF)
set(ENABLE_TESTING OFF)
set(MBEDTLS_AS_SUBPROJECT OFF)
set(DISABLE_PACKAGE_CONFIG_AND_INSTALL OFF)
set(MBEDTLS_FATAL_WARNINGS OFF)

CPMAddPackage("gh:TechStudent10/mbedtls#fc79213")
set(MBEDTLS_INCLUDE_DIRS "${mbedtls_SOURCE_DIR}/include")
set(MBEDTLS_LIBRARY "mbedtls")
set(MBEDX509_LIBRARY "mbedx509")
set(MBEDCRYPTO_LIBRARY "mbedcrypto")


CPMAddPackage("gh:machinezone/IXWebSocket#dc8807e")
install(TARGETS mbedtls mbedx509 mbedcrypto EXPORT mbedtls-targets)
target_include_directories(${PROJECT_NAME} PRIVATE ${ixwebsocket_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} ixwebsocket)
# Set up dependencies, resources, and link Geode.
setup_geode_mod(${PROJECT_NAME})
