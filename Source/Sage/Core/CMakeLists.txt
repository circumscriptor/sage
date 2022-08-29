#
#   .oooooo..o       .o.         .oooooo.    oooooooooooo
#  d8P'    `Y8      .888.       d8P'  `Y8b   `888'     `8
#  Y88bo.          .8"888.     888            888
#   `"Y8888o.     .8' `888.    888            888oooo8
#       `"Y88b   .88ooo8888.   888     ooooo  888    "
#  oo     .d8P  .8'     `888.  `88.    .88'   888       o
#  8""88888P'  o88o     o8888o  `Y8bood8P'   o888ooooood8
#

set(SAGE_CORE_HEADERS
    "BasicTypes.hpp"
    "ClassDefinitions.hpp"
    "Engine.hpp"
    "Exports.hpp"
    "Timer.hpp"
)

set(SAGE_CORE_SOURCES
    "Engine.cpp"
    "Timer.cpp"
)

target_sources(SageEngine
    PRIVATE
        ${SAGE_CORE_HEADERS}
        ${SAGE_CORE_SOURCES}
)

add_subdirectory(Console)
add_subdirectory(Graphics)
add_subdirectory(Input)
add_subdirectory(IO)
