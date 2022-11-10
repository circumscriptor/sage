cmake_minimum_required(VERSION 3.20)

#
#   .oooooo..o       .o.         .oooooo.    oooooooooooo
#  d8P'    `Y8      .888.       d8P'  `Y8b   `888'     `8
#  Y88bo.          .8"888.     888            888
#   `"Y8888o.     .8' `888.    888            888oooo8
#       `"Y88b   .88ooo8888.   888     ooooo  888    "
#  oo     .d8P  .8'     `888.  `88.    .88'   888       o
#  8""88888P'  o88o     o8888o  `Y8bood8P'   o888ooooood8
#

project(Sage)

#
# Set supported configuration types
#

set(CMAKE_CONFIGURATION_TYPES "Release;Debug" CACHE INTERNAL "Sage is build only with Release and Debug configs")

#
# Set output directories
#

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}" CACHE INTERNAL "")

if(MSVC AND NOT Clang)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug" CACHE INTERNAL "")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release" CACHE INTERNAL "")
else()
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}" CACHE INTERNAL "")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}" CACHE INTERNAL "")
endif()

#
# Include modules
#

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/CMake/Modules")

include(PlatformDefinitions)
include(CheckIPOSupported)
include(CheckPIESupported)

#
# Check IPO/LTO support
#

check_ipo_supported(
    RESULT _ipo_supported 
    OUTPUT _ipo_supported_output
    LANGUAGES C CXX
)

if(_ipo_supported)
    set(SAGE_IPO_SUPPORTED TRUE CACHE INTERNAL "")
    message(STATUS "Building with IPO/LTO")
else()
    set(SAGE_IPO_SUPPORTED FALSE CACHE INTERNAL "")
    message(STATUS "${_ipo_supported_output}")
endif()

#
# Check PIE support
#

check_pie_supported(
    OUTPUT_VARIABLE _pie_supported_output
    LANGUAGES CXX
)

if(CMAKE_CXX_LINK_PIE_SUPPORTED)
    set(SAGE_PIE_SUPPORTED TRUE CACHE INTERNAL "")
    message(STATUS "Building with PIE")
else()
    set(SAGE_PIE_SUPPORTED FALSE CACHE INTERNAL "")
    message(STATUS "${_pie_supported_output}")
endif()

#
# Import dependencies
#

include(${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

#
#
# Add source
#
#

add_subdirectory(Source)
