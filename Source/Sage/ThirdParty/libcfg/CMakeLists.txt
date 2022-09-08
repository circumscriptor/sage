#
#   .oooooo..o       .o.         .oooooo.    oooooooooooo
#  d8P'    `Y8      .888.       d8P'  `Y8b   `888'     `8
#  Y88bo.          .8"888.     888            888
#   `"Y8888o.     .8' `888.    888            888oooo8
#       `"Y88b   .88ooo8888.   888     ooooo  888    "
#  oo     .d8P  .8'     `888.  `88.    .88'   888       o
#  8""88888P'  o88o     o8888o  `Y8bood8P'   o888ooooood8
#

add_library(SageThirdPartyLibCfg OBJECT)

target_sources(SageThirdPartyLibCfg
    PRIVATE
        "cfg.cpp"
        "cfg.hpp"
)

target_link_libraries(SageThirdPartyLibCfg
    PRIVATE
        SageThirdPartyCommon
)

# Add to the list of third party libraries
set(SAGE_THIRDPARTY_LIBRARIES "${SAGE_THIRDPARTY_LIBRARIES};SageThirdPartyLibCfg" CACHE INTERNAL "")
