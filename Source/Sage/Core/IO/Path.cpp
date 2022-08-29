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

#include <SDL2/SDL_error.h>
#include <SDL2/SDL_filesystem.h>
#include <Sage/Core/Console/Log.hpp>
#include <algorithm>

#ifndef SAGE_ORG_NAME
    #define SAGE_ORG_NAME "Sage"
#endif

#ifndef SAGE_APP_NAME
    #define SAGE_APP_NAME "Sage"
#endif

namespace Sage::Core::IO {

std::string Path::GetBasePath() {
    char* path = SDL_GetBasePath();
    if (path == nullptr) {
        SAGE_LOG_CRITICAL("Failed to retrieve base path, using relative path. Error: {}", SDL_GetError());
        return "./";
    }
    std::string sPath = path;
    std::replace(sPath.begin(), sPath.end(), '\\', '/');
    SDL_free(path);
    return sPath;
}

std::string Path::GetUserPath() {
    char* path = SDL_GetPrefPath(SAGE_ORG_NAME, SAGE_APP_NAME);
    if (path == nullptr) {
        SAGE_LOG_CRITICAL("Failed to retrieve user path, using relative path. Error: {}", SDL_GetError());
        return "./";
    }
    std::string sPath = path;
    std::replace(sPath.begin(), sPath.end(), '\\', '/');
    SDL_free(path);
    return sPath;
}

} // namespace Sage::Core::IO
