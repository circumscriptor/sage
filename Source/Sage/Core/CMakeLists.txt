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
# Add SageCore library
#

add_library(SageCore STATIC)
conan_target_link_libraries(SageCore)

target_link_libraries(SageCore
    SageCommonPublic
    SageCommonPrivate
)

#
# Add sources
#

target_sources(SageCore
    PRIVATE
        "BasicTypes.hpp"
        "ClassDefinitions.hpp"
        "Exports.cpp"
        "Exports.hpp"
        "Timer.hpp"
)

#
# Add subdirectories
#

add_subdirectory(Console)
add_subdirectory(Graphics)
add_subdirectory(Input)
add_subdirectory(IO)

# Add to the list of libraries
set(SAGE_LIBRARIES "${SAGE_LIBRARIES};SageCore" CACHE INTERNAL "")
