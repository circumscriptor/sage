#
#   .oooooo..o       .o.         .oooooo.    oooooooooooo
#  d8P'    `Y8      .888.       d8P'  `Y8b   `888'     `8
#  Y88bo.          .8"888.     888            888
#   `"Y8888o.     .8' `888.    888            888oooo8
#       `"Y88b   .88ooo8888.   888     ooooo  888    "
#  oo     .d8P  .8'     `888.  `88.    .88'   888       o
#  8""88888P'  o88o     o8888o  `Y8bood8P'   o888ooooood8
#

#
# Based on DiligentCore CMakeLists.txt
#

#
# Initialize options
#

set(_platform_opts 
    SAGE_PLATFORM_WIN32
    SAGE_PLATFORM_LINUX
    SAGE_PLATFORM_MACOS
    SAGE_PLATFORM_32BIT
    SAGE_PLATFORM_64BIT
)

foreach(opt ${_platform_opts})
    set(${opt} false CACHE INTERNAL "")
endforeach()

#
# Detect platform
#

if(UNIX AND NOT APPLE AND NOT WIN32)
    set(SAGE_PLATFORM_LINUX true CACHE INTERNAL "")
elseif(WIN32)
    set(SAGE_PLATFORM_WIN32 true CACHE INTERNAL "")
elseif(APPLE)
    set(SAGE_PLATFORM_MACOS true CACHE INTERNAL "")
else()
    message(FATAL_ERROR "Platform not supported")
endif()

#
# Detect architecture
#

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(SAGE_PLATFORM_64BIT true CACHE INTERNAL "")
else()
    set(SAGE_PLATFORM_32BIT true CACHE INTERNAL "")
endif()

#
# Add PlatformDefinitions target
#

add_library(PlatformDefinitions INTERFACE)

foreach(_opt ${_platform_opts})
    target_compile_definitions(PlatformDefinitions
        INTERFACE
            ${_opt}=$<BOOL:${${_opt}}>
    )
endforeach()
