#
#
# Project Sage
#
#

# TODO: Add platform subdirectories?

# Root directory
set(DEPS_ROOT_DIR "deps" CACHE PATH "Root directory for dependencies")

# Subdirectories
set(DEPS_INCLUDE_DIR "${DEPS_ROOT_DIR}/include")
set(DEPS_LIBRARY_DIR "${DEPS_ROOT_DIR}/lib"    )
set(DEPS_RUNTIME_DIR "${DEPS_ROOT_DIR}/bin"    )

#
# Helper macro
#
macro(_check_and_copy file)
    if(EXISTS "${DEPS_RUNTIME_DIR}/${file}")
        if(CMAKE_RUNTIME_OUTPUT_DIRECTORY)
            file(COPY_FILE "${DEPS_RUNTIME_DIR}/${file}" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${file}" ONLY_IF_DIFFERENT)
        endif()
    else()
        message(STATUS "FILE: ${DEPS_RUNTIME_DIR}/${file} NOT FOUND")
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

    if(arg_SHARED)
        set(imported_file_release "${CMAKE_SHARED_LIBRARY_PREFIX}${release_name}${CMAKE_SHARED_LIBRARY_SUFFIX}")
        set(imported_file_debug "${CMAKE_SHARED_LIBRARY_PREFIX}${debug_name}${CMAKE_SHARED_LIBRARY_SUFFIX}")

        _check_and_copy(${imported_file_release})

        if(NOT imported_file_release STREQUAL imported_file_debug)
            _check_and_copy(${imported_file_debug})
        endif()

        set_target_properties(${target_name}
            PROPERTIES
                IMPORTED_LOCATION_RELEASE "${DEPS_RUNTIME_DIR}/${imported_file_release}"
                IMPORTED_LOCATION_DEBUG "${DEPS_RUNTIME_DIR}/${imported_file_debug}"
        )

        if(WIN32)
            set_target_properties(${target_name}
                PROPERTIES
                    IMPORTED_IMPLIB_RELEASE "${DEPS_LIBRARY_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}${release_name}${CMAKE_STATIC_LIBRARY_SUFFIX}"
                    IMPORTED_IMPLIB_DEBUG "${DEPS_LIBRARY_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}${debug_name}${CMAKE_STATIC_LIBRARY_SUFFIX}"
            )
        endif()
    elseif(arg_STATIC)
        set_target_properties(${target_name}
            PROPERTIES
                IMPORTED_LOCATION_RELEASE "${DEPS_LIBRARY_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}${release_name}${CMAKE_STATIC_LIBRARY_SUFFIX}"
                IMPORTED_LOCATION_DEBUG "${DEPS_LIBRARY_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}${debug_name}${CMAKE_STATIC_LIBRARY_SUFFIX}"
        )
    endif()
endfunction()
