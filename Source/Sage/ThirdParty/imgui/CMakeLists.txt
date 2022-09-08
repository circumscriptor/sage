#
#   .oooooo..o       .o.         .oooooo.    oooooooooooo
#  d8P'    `Y8      .888.       d8P'  `Y8b   `888'     `8
#  Y88bo.          .8"888.     888            888
#   `"Y8888o.     .8' `888.    888            888oooo8
#       `"Y88b   .88ooo8888.   888     ooooo  888    "
#  oo     .d8P  .8'     `888.  `88.    .88'   888       o
#  8""88888P'  o88o     o8888o  `Y8bood8P'   o888ooooood8
#

add_library(SageThirdPartyImGui OBJECT)

target_sources(SageThirdPartyImGui
    PRIVATE
        "imconfig.h"
        "imgui_draw.cpp"
        "imgui_internal.h"
        "imgui_tables.cpp"
        "imgui_widgets.cpp"
        "imgui.cpp"
        "imgui.cpp"
        "imgui.h"
        "imstb_rectpack.h"
        "imstb_textedit.h"
        "imstb_truetype.h"
        "misc/cpp/imgui_stdlib.cpp"
        "misc/cpp/imgui_stdlib.h"
)

target_link_libraries(SageThirdPartyImGui
    PRIVATE
        SageThirdPartyCommon
)

# Add to the list of third party libraries
set(SAGE_THIRDPARTY_LIBRARIES "${SAGE_THIRDPARTY_LIBRARIES};SageThirdPartyImGui" CACHE INTERNAL "")
