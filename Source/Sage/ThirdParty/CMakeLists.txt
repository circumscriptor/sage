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
    "libcfg/cfg.hpp"
)

set(SAGE_THIRDPARTY_LIBCFG_SOURCES
    "libcfg/cfg.cpp"
)

set(SAGE_THIRDPARTY_IMGUI_HEADERS
    "imgui/imconfig.h"
    "imgui/imgui_internal.h"
    "imgui/imgui.h"
    "imgui/imstb_rectpack.h"
    "imgui/imstb_textedit.h"
    "imgui/imstb_truetype.h"
    "imgui/misc/cpp/imgui_stdlib.h"
)

set(SAGE_THIRDPARTY_IMGUI_SOURCES
    "imgui/imgui_draw.cpp"
    "imgui/imgui_tables.cpp"
    "imgui/imgui_widgets.cpp"
    "imgui/imgui.cpp"
    "imgui/imgui.cpp"
    "imgui/misc/cpp/imgui_stdlib.cpp"
)

target_sources(SageEngine
    PRIVATE
        ${SAGE_THIRDPARTY_LIBCFG_HEADERS}
        ${SAGE_THIRDPARTY_LIBCFG_SOURCES}
        ${SAGE_THIRDPARTY_IMGUI_HEADERS}
        ${SAGE_THIRDPARTY_IMGUI_SOURCES}
)

# Include third party directory
target_include_directories(SageEngine
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}"
)
