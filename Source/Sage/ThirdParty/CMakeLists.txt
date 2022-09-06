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
# Add target with common flags for third party targets
#

add_library(SageThirdPartyCommon INTERFACE)

# ...

#
# Add subdirectories with third party targets
#

set(SAGE_THIRDPARTY_LIBRARIES "SageThirdParty" CACHE INTERNAL "")

add_subdirectory(imgui)
add_subdirectory(libcfg)

#
# Add combined third-party target
#

add_library(SageThirdParty STATIC
    $<TARGET_OBJECTS:SageThirdPartyImGui>
    $<TARGET_OBJECTS:SageThirdPartyLibCfg>
)

target_include_directories(SageThirdParty
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}"
)

# Add to the list of libraries
set(SAGE_LIBRARIES "${SAGE_LIBRARIES};${SAGE_THIRDPARTY_LIBRARIES}" CACHE INTERNAL "")
