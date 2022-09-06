#
#   .oooooo..o       .o.         .oooooo.    oooooooooooo
#  d8P'    `Y8      .888.       d8P'  `Y8b   `888'     `8
#  Y88bo.          .8"888.     888            888
#   `"Y8888o.     .8' `888.    888            888oooo8
#       `"Y88b   .88ooo8888.   888     ooooo  888    "
#  oo     .d8P  .8'     `888.  `88.    .88'   888       o
#  8""88888P'  o88o     o8888o  `Y8bood8P'   o888ooooood8
#

set(SAGE_THIRDPARTY_LIBCFG_HEADERS
    "cfg.hpp"
)

set(SAGE_THIRDPARTY_LIBCFG_SOURCES
    "cfg.cpp"
)

add_library(SageThirdPartyLibCfg OBJECT)

target_sources(SageThirdPartyLibCfg
    PRIVATE
        ${SAGE_THIRDPARTY_LIBCFG_HEADERS}
        ${SAGE_THIRDPARTY_LIBCFG_SOURCES}
)

target_link_libraries(SageThirdPartyLibCfg
    PRIVATE
        SageThirdPartyCommon
)

# Add to the list of third party targets
set(SAGE_THIRDPARTY_TARGETS "${SAGE_THIRDPARTY_TARGETS};SageThirdPartyLibCfg" CACHE INTERNAL "")
