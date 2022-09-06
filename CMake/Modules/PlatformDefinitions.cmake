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
    PLATFORM_WIN32
    PLATFORM_UNIVERSAL_WINDOWS
    PLATFORM_ANDROID
    PLATFORM_LINUX
    PLATFORM_MACOS
    PLATFORM_IOS
    PLATFORM_TVOS
    PLATFORM_EMSCRIPTEN
    D3D11_SUPPORTED
    D3D12_SUPPORTED
    GL_SUPPORTED
    GLES_SUPPORTED
    VULKAN_SUPPORTED
    METAL_SUPPORTED
    ARCHIVER_SUPPORTED
    ARCH_32BIT
    ARCH_64BIT
)

foreach(opt ${_platform_opts})
    set(${opt} FALSE CACHE INTERNAL "")
endforeach()

#
# Detect platform
#

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(PLATFORM_WIN32 TRUE CACHE INTERNAL "")
    set(ARCHIVER_SUPPORTED TRUE CACHE INTERNAL "")
    set(GL_SUPPORTED TRUE CACHE INTERNAL "")
    set(VULKAN_SUPPORTED TRUE CACHE INTERNAL "")
    set(D3D11_SUPPORTED TRUE CACHE INTERNAL "")
    if(CMAKE_SYSTEM_VERSION VERSION_GREATER_EQUAL "10.0")
        set(D3D12_SUPPORTED TRUE CACHE INTERNAL "")
    endif()
elseif(CMAKE_SYSTEM_NAME STREQUAL "WindowsStore")
    set(PLATFORM_UNIVERSAL_WINDOWS TRUE CACHE INTERNAL "")
    set(D3D11_SUPPORTED TRUE CACHE INTERNAL "")
    if(CMAKE_SYSTEM_VERSION VERSION_GREATER_EQUAL "10.0")
        set(D3D12_SUPPORTED TRUE CACHE INTERNAL "")
    endif()
elseif(CMAKE_SYSTEM_NAME STREQUAL "Android")
    set(PLATFORM_ANDROID TRUE CACHE INTERNAL "")
    set(GLES_SUPPORTED TRUE CACHE INTERNAL "")
    set(VULKAN_SUPPORTED TRUE CACHE INTERNAL "")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(PLATFORM_LINUX TRUE CACHE INTERNAL "")
    set(ARCHIVER_SUPPORTED TRUE CACHE INTERNAL "")
    set(GL_SUPPORTED TRUE CACHE INTERNAL "")
    set(VULKAN_SUPPORTED TRUE CACHE INTERNAL "")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(PLATFORM_MACOS TRUE CACHE INTERNAL "")
    set(ARCHIVER_SUPPORTED TRUE CACHE INTERNAL "")
    set(GL_SUPPORTED TRUE CACHE INTERNAL "")
    set(VULKAN_SUPPORTED TRUE CACHE INTERNAL "")
elseif(CMAKE_SYSTEM_NAME STREQUAL "iOS")
    set(GLES_SUPPORTED TRUE CACHE INTERNAL "")
    set(PLATFORM_IOS TRUE CACHE INTERNAL "")
elseif(CMAKE_SYSTEM_NAME STREQUAL "tvOS")
    set(PLATFORM_TVOS TRUE CACHE INTERNAL "")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
    set(PLATFORM_EMSCRIPTEN TRUE CACHE INTERNAL "")
    set(GLES_SUPPORTED TRUE CACHE INTERNAL "")
endif()

# TODO: Add MoltenVk
if(PLATFORM_MACOS OR PLATFORM_IOS OR PLATFORM_TVOS)
    set(PLATFORM_APPLE TRUE CACHE INTERNAL "")
endif()

#
# Detect architecture
#

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(ARCH 64 CACHE INTERNAL "64-bit architecture")
    set(ARCH_64BIT TRUE CACHE INTERNAL "")
else()
    set(ARCH 32 CACHE INTERNAL "32-bit architecture")
    set(ARCH_32BIT TRUE CACHE INTERNAL "")
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
