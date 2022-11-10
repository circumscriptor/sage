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
# Add common targets
#

add_library(SageCommonPublic INTERFACE)
add_library(SageCommonPrivate INTERFACE)

target_include_directories(SageCommonPublic
    INTERFACE
        "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_compile_definitions(SageCommonPrivate
    INTERFACE
        _CRT_SECURE_NO_WARNINGS 
        SAGE_EXPORTING
        SAGE_LOG_SOURCE=$<BOOL:${SAGE_LOG_SOURCE}>
)

target_compile_definitions(SageCommonPublic
    INTERFACE
        SAGE_EXPORT_API=$<BOOL:${SAGE_EXPORT_API}>
)

target_compile_features(SageCommonPublic
    INTERFACE
        cxx_std_20
)

target_link_libraries(SageCommonPrivate
    INTERFACE
        SageCommonPublic
        PlatformDefinitions
)

#
# Add subdirectories
#

add_subdirectory(Sage)
