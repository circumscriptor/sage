#
#   .oooooo..o       .o.         .oooooo.    oooooooooooo
#  d8P'    `Y8      .888.       d8P'  `Y8b   `888'     `8
#  Y88bo.          .8"888.     888            888
#   `"Y8888o.     .8' `888.    888            888oooo8
#       `"Y88b   .88ooo8888.   888     ooooo  888    "
#  oo     .d8P  .8'     `888.  `88.    .88'   888       o
#  8""88888P'  o88o     o8888o  `Y8bood8P'   o888ooooood8
#

# TODO: Add platform subdirectories?
#       e.g. ${DEPS_ROOT_DIR}/lib/windows/32/...
#            ${DEPS_ROOT_DIR}/lib/linux/64/...

#
# Root directory
#
set(DEPS_ROOT_DIR "deps" CACHE PATH "Root directory for dependencies")

#
# Subdirectories
#
set(DEPS_INCLUDE_DIR "${DEPS_ROOT_DIR}/include")
set(DEPS_LIBRARY_DIR "${DEPS_ROOT_DIR}/lib"    )
set(DEPS_RUNTIME_DIR "${DEPS_ROOT_DIR}/bin"    )

#
# Helper macro for copying shared libraries
#
# If CMAKE_RUNTIME_OUTPUT_DIRECTORY_<CONFIG> is not set, the file will not be copied.
# Supported configurations: Release, Debug
#
macro(_check_and_copy_shared release_file debug_file)
    if(EXISTS "${DEPS_RUNTIME_DIR}/${release_file}")
        if(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE)
            file(COPY_FILE "${DEPS_RUNTIME_DIR}/${release_file}" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}/${release_file}" ONLY_IF_DIFFERENT)
        endif()
    else()
        message(STATUS "FILE: ${DEPS_RUNTIME_DIR}/${release_file} NOT FOUND")
    endif()
    if(NOT release_file STREQUAL debug_file)
        if(EXISTS "${DEPS_RUNTIME_DIR}/${debug_file}")
            if(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG)
                file(COPY_FILE "${DEPS_RUNTIME_DIR}/${debug_file}" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}/${debug_file}" ONLY_IF_DIFFERENT)
            endif()
        else()
            message(STATUS "FILE: ${DEPS_RUNTIME_DIR}/${debug_file} NOT FOUND")
        endif()
    endif()
endmacro()

#
# Function for adding imported targets
#
# add_imported_target(<target_name> ... [STATIC | SHARED | DYNAMIC | INTERFACE]
#   [BASE_NAME <base_name>]
#   [RELEASE_SUFFIX <release_suffix>]
#   [DEBUG_SUFFIX <debug_suffix>]
#   [ALIAS <alias>]
#   [INCLUDE_DIRS dirs...]
# )
#
# Adds imported target called <target_name> of STATIC, SHARED or INTERFACE type.
# Other options:
#
#   BASE_NAME
#       Base name of the imported file (without suffix), if not specified then <target_name> is used.
#
#   RELEASE_SUFFIX
#       Suffix of the imported file in release build.
#
#   DEBUG_SUFFIX
#       Suffix of the imported file in debug build.
#
#   ALIAS
#       If specified, alias target named <alias> is created.
#
#   INCLUDE_DIRS
#       Include directories relative to root directory of imported targets (DEPS_ROOT_DIR)
#
# Note:
#   Each imported target has DEPS_INCLUDE_DIR added to its INTERFACE_INCLUDE_DIRECTORIES property.
#   If CMAKE_RUNTIME_OUTPUT_DIRECTORY is set, runtime files will be copied to the specified directory.
#
function(add_imported_target target_name)
    cmake_parse_arguments(PARSE_ARGV 1 arg
        "STATIC;SHARED;INTERFACE"
        "BASE_NAME;RELEASE_SUFFIX;DEBUG_SUFFIX;ALIAS"
        "INCLUDE_DIRS"
    )

    if(arg_STATIC AND NOT (arg_SHARED OR arg_INTERFACE))
        add_library(${target_name} STATIC IMPORTED)
    elseif(arg_SHARED AND NOT (arg_STATIC OR arg_INTERFACE))
        add_library(${target_name} SHARED IMPORTED)
    elseif(arg_INTERFACE AND NOT (arg_STATIC OR arg_SHARED))
        add_library(${target_name} INTERFACE IMPORTED)
    else()
        message(FATAL_ERROR "Invalid target type, use one of these: SHARED, STATIC, INTERFACE")
    endif()

    if(arg_ALIAS)
        add_library(${arg_ALIAS} ALIAS ${target_name})
    endif()

    if(arg_BASE_NAME)
        set(base_name "${arg_BASE_NAME}")
    else()
        set(base_name "${target_name}")
    endif()

    if(arg_RELEASE_SUFFIX)
        set(release_name "${base_name}${arg_RELEASE_SUFFIX}")
    else()
        set(release_name "${base_name}")
    endif()

    if(arg_DEBUG_SUFFIX)
        set(debug_name "${base_name}${arg_DEBUG_SUFFIX}")
    else()
        set(debug_name "${base_name}")
    endif()

    set(include_dirs "${DEPS_INCLUDE_DIR}")
    if(arg_INCLUDE_DIRS)
        foreach(dir ${arg_INCLUDE_DIRS})
            list(APPEND include_dirs "${DEPS_INCLUDE_DIR}/${dir}")
        endforeach()
    endif()
    set_property(TARGET ${target_name} PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${include_dirs}")

    set(imported_shared_debug   "${CMAKE_SHARED_LIBRARY_PREFIX}${debug_name}${CMAKE_SHARED_LIBRARY_SUFFIX}")
    set(imported_static_debug   "${CMAKE_STATIC_LIBRARY_PREFIX}${debug_name}${CMAKE_STATIC_LIBRARY_SUFFIX}")
    set(imported_shared_release "${CMAKE_SHARED_LIBRARY_PREFIX}${release_name}${CMAKE_SHARED_LIBRARY_SUFFIX}")
    set(imported_static_release "${CMAKE_STATIC_LIBRARY_PREFIX}${release_name}${CMAKE_STATIC_LIBRARY_SUFFIX}")

    if(arg_SHARED)
        _check_and_copy_shared(${imported_shared_release} ${imported_shared_debug})

        set_target_properties(${target_name}
            PROPERTIES
                IMPORTED_LOCATION_RELEASE "${DEPS_RUNTIME_DIR}/${imported_shared_release}"
                IMPORTED_LOCATION_DEBUG "${DEPS_RUNTIME_DIR}/${imported_shared_debug}"
        )

        if(WIN32)
            set_target_properties(${target_name}
                PROPERTIES
                    IMPORTED_IMPLIB_RELEASE "${DEPS_LIBRARY_DIR}/${imported_static_release}"
                    IMPORTED_IMPLIB_DEBUG "${DEPS_LIBRARY_DIR}/${imported_static_debug}"
            )
        endif()
    elseif(arg_STATIC)
        set_target_properties(${target_name}
            PROPERTIES
                IMPORTED_LOCATION_RELEASE "${DEPS_LIBRARY_DIR}/${imported_static_release}"
                IMPORTED_LOCATION_DEBUG "${DEPS_LIBRARY_DIR}/${imported_static_debug}"
        )
    endif()
endfunction()

#
# Create the output directories if they do not exist
#

if(NOT EXISTS "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}")
    file(MAKE_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}")
endif()

if(NOT EXISTS "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}")
    file(MAKE_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}")
endif()
