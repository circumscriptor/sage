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
/// @file GetNativeWindow.hpp
/// @brief ...
///
/// @copyright Copyright (c) 2022
///
///

#pragma once

#include <Sage/Core/Console/Log.hpp>

// Diligent
#include <NativeWindow.h>

// SDL
#include <SDL2/SDL_syswm.h>

namespace Sage::Core::Graphics::Internal {

using namespace Diligent;

static inline bool GetNativeWindow(SDL_Window* windowSDL, NativeWindow& nativeWindow) {
    SDL_SysWMinfo syswm{};
    SDL_VERSION(&syswm.version);
    if (SDL_GetWindowWMInfo(windowSDL, &syswm) == 0) {
        SAGE_LOG_ERROR("Failed to retrieve native window handle: {}", SDL_GetError());
        return false;
    }

#if PLATFORM_WIN32
    if (syswm.subsystem == SDL_SYSWM_WINDOWS) {
        nativeWindow.hWnd = syswm.info.win.window;
        return true;
    }
#elif PLATFORM_UNIVERSAL_WINDOWS
    if (syswm.subsystem == SDL_SYSWM_WINRT) {
        ABI::Windows::UI::Core::ICoreWindow* coreWindow;
        if (!FAILED(syswm.info.winrt.window->QueryInterface(&coreWindow)) {
            nativeWindow.pCoreWindow = coreWindow;
            return true;
		} 
        SAGE_LOG_ERROR("Failed to query CoreWindow interface");
        return false;
    }
#elif PLATFORM_ANDROID
    if (syswm.subsystem == SDL_SYSWM_ANDROID) {
        nativeWindow.pAWindow = syswm.info.android.window;
        return true;
    }
#elif PLATFORM_LINUX
    if (syswm.subsystem == SDL_SYSWM_X11) {
        nativeWindow.WindowId       = syswm.info.x11.window;
        nativeWindow.pDisplay       = syswm.info.x11.display;
        nativeWindow.pXCBConnection = nullptr;
        return true;
    }
#elif PLATFORM_MACOS
    // TODO: Convert syswm.info.cocoa.window from NSWindow to NSView
#elif PLATFORM_IOS || PLATFORM_TVOS
    // TODO: Convert syswm.info.uikit.window from UIWindow to CALayer
#elif PLATFORM_EMSCRIPTEN
    // TODO: ?
#endif
    SAGE_LOG_ERROR("Native window handle subsystem does not match platform");
    return false;
}

} // namespace Sage::Core::Graphics::Internal
