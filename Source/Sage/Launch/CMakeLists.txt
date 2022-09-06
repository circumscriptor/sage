#
#   .oooooo..o       .o.         .oooooo.    oooooooooooo
#  d8P'    `Y8      .888.       d8P'  `Y8b   `888'     `8
#  Y88bo.          .8"888.     888            888
#   `"Y8888o.     .8' `888.    888            888oooo8
#       `"Y88b   .88ooo8888.   888     ooooo  888    "
#  oo     .d8P  .8'     `888.  `88.    .88'   888       o
#  8""88888P'  o88o     o8888o  `Y8bood8P'   o888ooooood8
#

add_executable(SageLaunch)

set(SAGE_LAUNCH_HEADERS
)

set(SAGE_LAUNCH_SOURCES
    "Main.cpp"
)

target_sources(SageLaunch
    PRIVATE
        ${SAGE_LAUNCH_HEADERS}
        ${SAGE_LAUNCH_SOURCES}
)

target_link_libraries(SageLaunch
    PRIVATE
        SageCommonPrivate
        SageCore

        SDL2::SDL2main
)

# Add to the list of executables
set(SAGE_EXECUTABLES "${SAGE_EXECUTABLES};SageLaunch" CACHE INTERNAL "")
