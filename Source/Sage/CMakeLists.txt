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

set(SAGE_LIBRARIES "" CACHE INTERNAL "")
set(SAGE_EXECUTABLES "" CACHE INTERNAL "")

add_subdirectory(Core)
add_subdirectory(Launch)
add_subdirectory(ThirdParty)

#
# Add visibility properties to all relevant targets
#

set_target_properties(${SAGE_LIBRARIES} ${SAGE_EXECUTABLES}
    PROPERTIES
        C_VISIBILITY_PRESET hidden
        CXX_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN true
)

if(SAGE_PIE_SUPPORTED)
    set_target_properties(${SAGE_LIBRARIES} ${SAGE_EXECUTABLES}
        PROPERTIES
            POSITION_INDEPENDENT_CODE true
    )
else()
    set_target_properties(${SAGE_LIBRARIES}
        PROPERTIES
            POSITION_INDEPENDENT_CODE true
    )
endif()

if(SAGE_IPO_SUPPORTED)
    set_target_properties(${SAGE_LIBRARIES} ${SAGE_EXECUTABLES}
        PROPERTIES
            INTERPROCEDURAL_OPTIMIZATION true
    )
endif()

