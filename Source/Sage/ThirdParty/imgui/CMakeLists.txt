#
#   .oooooo..o       .o.         .oooooo.    oooooooooooo
#  d8P'    `Y8      .888.       d8P'  `Y8b   `888'     `8
#  Y88bo.          .8"888.     888            888
#   `"Y8888o.     .8' `888.    888            888oooo8
#       `"Y88b   .88ooo8888.   888     ooooo  888    "
#  oo     .d8P  .8'     `888.  `88.    .88'   888       o
#  8""88888P'  o88o     o8888o  `Y8bood8P'   o888ooooood8
#


set(SAGE_THIRDPARTY_IMGUI_HEADERS
    "imconfig.h"
    "imgui_internal.h"
    "imgui.h"
    "imstb_rectpack.h"
    "imstb_textedit.h"
    "imstb_truetype.h"
    "misc/cpp/imgui_stdlib.h"
)

set(SAGE_THIRDPARTY_IMGUI_SOURCES
    "imgui_draw.cpp"
    "imgui_tables.cpp"
    "imgui_widgets.cpp"
    "imgui.cpp"
    "imgui.cpp"
    "misc/cpp/imgui_stdlib.cpp"
)

add_library(SageThirdPartyImGui OBJECT)

target_sources(SageThirdPartyImGui
    PRIVATE
        ${SAGE_THIRDPARTY_IMGUI_HEADERS}
        ${SAGE_THIRDPARTY_IMGUI_SOURCES}
)

target_link_libraries(SageThirdPartyImGui
    PRIVATE
        SageThirdPartyCommon
)

# Add to the list of third party libraries
set(SAGE_THIRDPARTY_LIBRARIES "${SAGE_THIRDPARTY_LIBRARIES};SageThirdPartyImGui" CACHE INTERNAL "")
