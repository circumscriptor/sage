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
# Sage::Engine
#

add_library(SageEngine SHARED)
add_library(Sage::Engine ALIAS SageEngine)

target_include_directories(SageEngine
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_compile_definitions(SageEngine
    PRIVATE
        _CRT_SECURE_NO_WARNINGS 
        SAGE_LOG_SOURCE=1
    PUBLIC
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,STATIC_LIBRARY>:SAGE_BUILD_STATIC>
)

target_link_libraries(SageEngine
    PRIVATE
        PlatformDefinitions
        Diligent::Headers
        SDL2::SDL2
        EnTT::EnTT
        Edyn::Edyn
        fmt::fmt
        spdlog::spdlog
)

target_compile_features(SageEngine
    PUBLIC
        cxx_std_17
)

add_subdirectory(Sage)

#
# Sage::Launch
#

add_executable(SageLaunch "Launch.cpp")
add_executable(Sage::Launch ALIAS SageLaunch)

target_link_libraries(SageLaunch
    PRIVATE
        PlatformDefinitions
        Sage::Engine 
        SDL2::SDL2main
)
