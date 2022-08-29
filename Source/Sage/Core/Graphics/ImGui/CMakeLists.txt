#
#   .oooooo..o       .o.         .oooooo.    oooooooooooo
#  d8P'    `Y8      .888.       d8P'  `Y8b   `888'     `8
#  Y88bo.          .8"888.     888            888
#   `"Y8888o.     .8' `888.    888            888oooo8
#       `"Y88b   .88ooo8888.   888     ooooo  888    "
#  oo     .d8P  .8'     `888.  `88.    .88'   888       o
#  8""88888P'  o88o     o8888o  `Y8bood8P'   o888ooooood8
#

set(SAGE_GRAPHICS_IMGUI_HEADERS
    "ImGuiContext.hpp"
    "ImGuiRenderer.hpp"
    "ThirdParty/imconfig.h"
    "ThirdParty/imgui_internal.h"
    "ThirdParty/imgui.h"
    "ThirdParty/imstb_rectpack.h"
    "ThirdParty/imstb_textedit.h"
    "ThirdParty/imstb_truetype.h"
    "ThirdParty/misc/"
    )
    
set(SAGE_GRAPHICS_IMGUI_SOURCES
    "ImGuiContext.cpp"
    "ImGuiRenderer.cpp"
    "ThirdParty/imgui_draw.cpp"
    "ThirdParty/imgui_tables.cpp"
    "ThirdParty/imgui_widgets.cpp"
    "ThirdParty/imgui.cpp"
    "ThirdParty/imgui.cpp"
)

target_sources(SageEngine
    PRIVATE
        ${SAGE_GRAPHICS_IMGUI_HEADERS}
        ${SAGE_GRAPHICS_IMGUI_SOURCES}
)
