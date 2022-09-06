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

if(MSVC)
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

include(ImportedTarget)
include(PlatformDefinitions)
include(CheckIPOSupported)

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
endif()

#
# Import dependencies
#

# Import SDL2
add_imported_target(SDL2 SHARED
    ALIAS SDL2::SDL2
    BASE_NAME "SDL2"
    DEBUG_SUFFIX "d"
)

# Import SDL2main
add_imported_target(SDL2main STATIC
    ALIAS SDL2::SDL2main
    BASE_NAME "SDL2main"
    DEBUG_SUFFIX "d"
)

target_link_libraries(SDL2main INTERFACE SDL2::SDL2)

# Import EnTT
add_imported_target(EnTT INTERFACE
    ALIAS EnTT::EnTT
)

# Import Edyn
add_imported_target(Edyn SHARED
    ALIAS Edyn::Edyn
    BASE_NAME "Edyn"
    DEBUG_SUFFIX "_d"
)


# Import DiligentCore headers
add_imported_target(Diligent-Headers INTERFACE
    ALIAS Diligent::Headers
    INCLUDE_DIRS
        "Diligent/Common/interface"
        "Diligent/Platforms/interface"
        "Diligent/Primitives/interface"
        "Diligent/Graphics/GraphicsAccessories/interface"
        "Diligent/Graphics/GraphicsEngine/interface"
        "Diligent/Graphics/GraphicsEngineD3D11/interface"
        "Diligent/Graphics/GraphicsEngineD3D12/interface"
        "Diligent/Graphics/GraphicsEngineD3DBase/interface"
        "Diligent/Graphics/GraphicsEngineOpenGL/interface"
        "Diligent/Graphics/GraphicsEngineVulkan/interface"
        "Diligent/Graphics/GraphicsTools/interface"
        "Diligent/Graphics/HLSL2GLSLConverterLib/interface"
        "Diligent/Graphics/Archiver/interface"
)

# Import DiligentCore OpenGL graphics engine
if(GL_SUPPORTED OR GLES_SUPPORTED)
    add_imported_target(Diligent-GraphicsEngineOpenGL SHARED
        BASE_NAME "GraphicsEngineOpenGL"
        RELEASE_SUFFIX "_${ARCH}r"
        DEBUG_SUFFIX "_${ARCH}d"
    )
endif()

# Import DiligentCore Vulkan graphics engine
if(VULKAN_SUPPORTED)
    add_imported_target(Diligent-GraphicsEngineVulkan SHARED
        BASE_NAME "GraphicsEngineVk"
        RELEASE_SUFFIX "_${ARCH}r"
        DEBUG_SUFFIX "_${ARCH}d"
    )
endif()

# Import DiligentCore Directx 11 graphics engine
if(D3D11_SUPPORTED)
    add_imported_target(Diligent-GraphicsEngineD3D11 SHARED
        BASE_NAME "GraphicsEngineD3D11"
        RELEASE_SUFFIX "_${ARCH}r"
        DEBUG_SUFFIX "_${ARCH}d"
    )
endif()

# Import DiligentCore Directx 12 graphics engine
if(D3D12_SUPPORTED)
    add_imported_target(Diligent-GraphicsEngineD3D12 SHARED
        BASE_NAME "GraphicsEngineD3D12"
        RELEASE_SUFFIX "_${ARCH}r"
        DEBUG_SUFFIX "_${ARCH}d"
    )
endif()

# Import FreeType
add_imported_target(FreeType SHARED
    ALIAS FreeType::FreeType
    BASE_NAME "freetype"
    DEBUG_SUFFIX "d"
)

# Import RmlUi
add_imported_target(RmlCore SHARED
    ALIAS RmlUi::Core
    BASE_NAME "RmlCore"
    DEBUG_SUFFIX "d"
)

# Import fmt
add_imported_target(fmt SHARED
    ALIAS fmt::fmt
    BASE_NAME "fmt"
    DEBUG_SUFFIX "d"
)

# Import spdlog
add_imported_target(spdlog SHARED
    ALIAS spdlog::spdlog
    BASE_NAME "spdlog"
    DEBUG_SUFFIX "d"
)

target_compile_definitions(spdlog
    INTERFACE
        SPDLOG_SHARED_LIB
        SPDLOG_COMPILED_LIB
        SPDLOG_FMT_EXTERNAL
)

#
#
# Add source
#
#

add_subdirectory(Source)
