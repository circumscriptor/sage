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
/// @file Path.cpp
/// @brief ...
///
/// @copyright Copyright (c) 2022
///
///

#include "Path.hpp"

#include <SDL2/SDL.h>
#include <Sage/Core/Console/Log.hpp>
#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <string>
#include <system_error>

#ifndef SAGE_DEFAULT_BASE_PATH
    #define SAGE_DEFAULT_BASE_PATH "."
#endif

#ifndef SAGE_DEFAULT_USER_PATH
    #define SAGE_DEFAULT_USER_PATH "."
#endif

#ifndef SAGE_GLOBAL_LOG_FILE_NAME
    #define SAGE_GLOBAL_LOG_FILE_NAME "sage.log"
#endif

#ifndef SAGE_GLOBAL_CONFIG_FILE_NAME
    #define SAGE_GLOBAL_CONFIG_FILE_NAME "sage.cfg"
#endif

#ifndef SAGE_ORG_NAME
    #define SAGE_ORG_NAME "Sage"
#endif

#ifndef SAGE_APP_NAME
    #define SAGE_APP_NAME "Sage"
#endif

namespace Sage::Core::IO {

// NOTE: Using '/' instead of '\\', because some dependencies cannot handle '\\'

auto Path::GetBasePath() -> std::string {
    char*       basePath = SDL_GetBasePath();
    std::string path     = basePath == nullptr ? SAGE_DEFAULT_BASE_PATH : basePath;
    std::replace(path.begin(), path.end(), '\\', '/');
    SDL_free(basePath);
    return path;
}

auto Path::GetUserPath() -> std::string {
    char*       userPath = SDL_GetPrefPath(SAGE_ORG_NAME, SAGE_APP_NAME);
    std::string path     = userPath == nullptr ? SAGE_DEFAULT_USER_PATH : userPath;
    std::replace(path.begin(), path.end(), '\\', '/');
    SDL_free(userPath);
    return path;
}

auto Path::Base() -> std::string_view {
    static const std::string sBasePath = GetBasePath();
    return sBasePath;
}

auto Path::User() -> std::string_view {
    static const std::string sUserPath = GetUserPath();
    return sUserPath;
}

auto Path::Log() -> std::string_view {
    static const std::string sLogPath = fmt::format("{}/{}", User(), SAGE_GLOBAL_LOG_FILE_NAME);
    return sLogPath;
}

auto Path::Config() -> std::string_view {
    static const std::string sConfigPath = fmt::format("{}/{}", User(), SAGE_GLOBAL_CONFIG_FILE_NAME);
    return sConfigPath;
}

} // namespace Sage::Core::IO
