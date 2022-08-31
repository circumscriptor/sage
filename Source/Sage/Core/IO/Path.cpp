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

#include <SDL2/SDL_filesystem.h>
#include <SDL2/SDL_rwops.h>
#include <Sage/Core/Console/Log.hpp>
#include <algorithm>

#ifndef SAGE_DEFAULT_BASE_PATH
    #define SAGE_DEFAULT_BASE_PATH ""
#endif

#ifndef SAGE_DEFAULT_USER_PATH
    #define SAGE_DEFAULT_USER_PATH ""
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

std::string Path::GetBasePath() {
    char*       basePath = SDL_GetPrefPath(SAGE_ORG_NAME, SAGE_APP_NAME);
    std::string path     = basePath == nullptr ? SAGE_DEFAULT_BASE_PATH : basePath;
    std::replace(path.begin(), path.end(), '\\', '/');
    SDL_free(basePath);
    return path;
}

std::string Path::GetUserPath() {
    char*       userPath = SDL_GetBasePath();
    std::string path     = userPath == nullptr ? SAGE_DEFAULT_USER_PATH : userPath;
    std::replace(path.begin(), path.end(), '\\', '/');
    SDL_free(userPath);
    return path;
}

std::string_view Path::Base() {
    static const std::string sBasePath = GetBasePath();
    return sBasePath;
}

std::string_view Path::User() {
    static const std::string sUserPath = GetUserPath();
    return sUserPath;
}

std::string_view Path::Log() {
    static const std::string sLogPath = fmt::format("{}{}", User(), SAGE_GLOBAL_LOG_FILE_NAME);
    return sLogPath;
}

std::string_view Path::Config() {
    static const std::string sConfigPath = fmt::format("{}{}", User(), SAGE_GLOBAL_CONFIG_FILE_NAME);
    return sConfigPath;
}

bool Path::IsFile(std::string_view path) {
    auto* RWops = SDL_RWFromFile(path.data(), "r");
    if (RWops == nullptr) {
        return false;
    }
    SDL_RWclose(RWops);
    return true;
}

} // namespace Sage::Core::IO
