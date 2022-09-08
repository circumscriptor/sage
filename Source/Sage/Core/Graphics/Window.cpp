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
/// @file Window.cpp
/// @brief ...
///
/// @copyright Copyright (c) 2022
///

#include "Window.hpp"

#include <SDL2/SDL_error.h>
#include <SDL2/SDL_video.h>
#include <Sage/Core/Console/Log.hpp>
#include <Sage/Core/Graphics/Internal/GetNativeWindow.hpp>

#ifndef SAGE_WINDOW_TITLE
    #define SAGE_WINDOW_TITLE "Sage"
#endif

#ifndef SAGE_DEFAULT_WINDOW_WIDTH
    #define SAGE_DEFAULT_WINDOW_WIDTH 1280
#endif

#ifndef SAGE_DEFAULT_WINDOW_HEIGHT
    #define SAGE_DEFAULT_WINDOW_HEIGHT 720
#endif

namespace Sage::Core::Graphics {

using namespace Internal;

Window::Window(const CreateInfo& createInfo) {
    UInt32 flags = SDL_WINDOW_SHOWN;
    UInt32 xPos  = SDL_WINDOWPOS_CENTERED_DISPLAY(createInfo.display);
    UInt32 yPos  = SDL_WINDOWPOS_CENTERED_DISPLAY(createInfo.display);
    UInt32 xRes  = createInfo.width;
    UInt32 yRes  = createInfo.height;

    if (xRes == 0 || yRes == 0) {
        xRes = SAGE_DEFAULT_WINDOW_WIDTH;
        yRes = SAGE_DEFAULT_WINDOW_HEIGHT;
    }

    switch (createInfo.mode) {
        case kWindowed:
            break;
        case kWindowedBorderless:
            flags |= UInt32(SDL_WINDOW_BORDERLESS);
            break;
        case kFullScreen:
            flags |= UInt32(SDL_WINDOW_FULLSCREEN);
            break;
        case kFullScreenDesktop:
            flags |= UInt32(SDL_WINDOW_FULLSCREEN_DESKTOP);
            break;
        case kFullScreenBorderless:
            flags |= UInt32(SDL_WINDOW_BORDERLESS);
            if (SDL_DisplayMode mode; SDL_GetDesktopDisplayMode(int(createInfo.display), &mode) == 0) {
                xRes = mode.w;
                yRes = mode.h;
            } else {
                SAGE_LOG_WARN("Cannot receive display mode. Error: {}", SDL_GetError());
            }
            break;
    }

    mWindow = SDL_CreateWindow(SAGE_WINDOW_TITLE, int(xPos), int(yPos), int(xRes), int(yRes), flags);
    if (mWindow == nullptr) {
        SAGE_LOG_ERROR("Failed to create window. Error: {}", SDL_GetError());
        throw std::exception("failed to create window");
    }

    if (!GetNativeWindow(mWindow, mNativeWindow)) {
        throw std::exception("failed to get native handle");
    }
}

Window::~Window() {
    SDL_DestroyWindow(mWindow);
}

} // namespace Sage::Core::Graphics
