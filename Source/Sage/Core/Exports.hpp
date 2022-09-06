//
//  .oooooo..o       .o.         .oooooo.    oooooooooooo
// d8P'    `Y8      .888.       d8P'  `Y8b   `888'     `8
// Y88bo.          .8"888.     888            888
//  `"Y8888o.     .8' `888.    888            888oooo8
//      `"Y88b   .88ooo8888.   888     ooooo  888    "
// oo     .d8P  .8'     `888.  `88.    .88'   888       o
// 8""88888P'  o88o     o8888o  `Y8bood8P'   o888ooooood8
//

///
/// @file Exports.hpp
/// @brief Export macros
///
/// @copyright Copyright (c) 2022
///
///

#pragma once

#if PLATFORM_WIN32 || PLATFORM_UNIVERSAL_WINDOWS
    #define SAGE_SYMBOL_EXPORT __declspec(dllexport)
    #define SAGE_SYMBOL_IMPORT __declspec(dllimport)
    #define SAGE_SYMBOL_HIDDEN
#else
    #define SAGE_SYMBOL_EXPORT __attribute__((visibility("default")))
    #define SAGE_SYMBOL_IMPORT __attribute__((visibility("default")))
    #define SAGE_SYMBOL_HIDDEN __attribute__((visibility("hidden")))
#endif

#ifdef SAGE_EXPORTING
    #define SAGE_EXPORT    SAGE_SYMBOL_EXPORT
    #define SAGE_NO_EXPORT SAGE_SYMBOL_HIDDEN
#else
    #define SAGE_EXPORT    SAGE_SYMBOL_IMPORT
    #define SAGE_NO_EXPORT SAGE_SYMBOL_HIDDEN
#endif

#if SAGE_EXPORT_API
    #define SGAPI SAGE_EXPORT
#else
    #define SGAPI
#endif
