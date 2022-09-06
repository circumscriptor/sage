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
# Add subdirectories
#

set(SAGE_THIRDPARTY_TARGETS "SageThirdParty" CACHE INTERNAL "")

add_subdirectory(Core)
add_subdirectory(ThirdParty)

#
# Add visibility properties to all relevant targets
#

set(_targets SageEngine ${SAGE_THIRDPARTY_TARGETS})

set_target_properties(${_targets}
    PROPERTIES
        C_VISIBILITY_PRESET hidden
        CXX_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN true
        POSITION_INDEPENDENT_CODE true
)

if(SAGE_IPO_SUPPORTED)
    set_target_properties(${_targets}
        PROPERTIES
            INTERPROCEDURAL_OPTIMIZATION true
    )
endif()

target_link_libraries(SageEngine
    PRIVATE
        SageThirdParty
)
