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
/// @file SDL.cpp
/// @brief ...
///
/// @copyright Copyright (c) 2022
///
///

#include "SDL.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_filesystem.h>

#ifndef SAGE_ORG_NAME
    #define SAGE_ORG_NAME "Sage"
#endif

#ifndef SAGE_APP_NAME
    #define SAGE_APP_NAME "Sage"
#endif

namespace Sage::Core {

SDL::SDL() {
#ifdef SDL_VIDEO_DRIVER_X11
    SDL_SetHint(SDL_HINT_VIDEODRIVER, "x11");
#endif
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        throw std::exception("failed to initialize SDL");
    }
}

SDL::~SDL() {
    SDL_free(mBasePath);
    SDL_free(mUserPath);
    SDL_Quit();
}

const char* SDL::GetBasePath() {
    if (!mBasePathInitialized) {
        mBasePath = SDL_GetBasePath();
        if (mBasePath == nullptr) {
            SAGE_LOG_CRITICAL("Failed to retrieve base path. Error: {}", SDL_GetError());
        }
        mBasePathInitialized = true;
    }
    return mBasePath;
}

const char* SDL::GetUserPath() {
    if (!mUserPathInitialized) {
        mUserPath = SDL_GetPrefPath(SAGE_ORG_NAME, SAGE_APP_NAME);
        if (mUserPath == nullptr) {
            SAGE_LOG_CRITICAL("Failed to retrieve user path. Error: {}", SDL_GetError());
        }
        mUserPathInitialized = true;
    }
    return mUserPath;
}

bool SDL::FileExists(const char* path) {
    auto* RWops = SDL_RWFromFile(path, "r");
    if (RWops == nullptr) {
        return false;
    }
    SDL_RWclose(RWops);
    return true;
}

} // namespace Sage::Core
