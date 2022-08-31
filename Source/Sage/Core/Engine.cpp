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
/// @file Engine.cpp
/// @brief ...
///
/// @copyright Copyright (c) 2022
///
///

#include "Engine.hpp"

#include <GraphicsTypes.h>
#include <RmlUi/Core.h>
#include <SDL2/SDL.h>
#include <Sage/Core/Console/Log.hpp>
#include <Sage/Core/IO/Path.hpp>
#include <iostream>
#include <optional>

namespace Sage::Core {

using namespace Console;
using namespace Graphics;
using namespace IO;

Engine::Engine(std::shared_ptr<IVirtualConsole> console) : mConsole{std::move(console)} {
    mConsole->RegisterPersistent(mGraphicsCVars);

    // Sync
    mConsole->SyncWithFile(IVirtualConsole::kReload);

    // Insert primary context
    mContexts.emplace_back(mConsole);
}

Engine::~Engine() = default;

void Engine::DestroyNextStoppedContext() {
    auto contextToBeDestroyed = std::find_if(mContexts.begin(), mContexts.end(), [](auto& context) -> bool {
        return context.ShouldDestroy();
    });

    if (contextToBeDestroyed != mContexts.end()) {
        mContexts.erase(contextToBeDestroyed);
    }
}

void Engine::Loop() {
    SDL_Event event;

    while (!mContexts.empty()) {
        while (SDL_PollEvent(&event) != 0) {
            for (auto& context : mContexts) {
                context.ProcessEvents(event);
            }
        }

        DestroyNextStoppedContext();

        for (auto& context : mContexts) {
            context.Update();
        }

        for (auto& context : mContexts) {
            context.Render();
        }
    }
}

bool Engine::Run() {
    // Initialize SDL
#ifdef SDL_VIDEO_DRIVER_X11
    SDL_SetHint(SDL_HINT_VIDEODRIVER, "x11");
#endif
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        std::cout << "ERROR: " << SDL_GetError() << '\n';
        return false;
    }

    // Initialize paths
    SAGE_LOG_DEBUG("Using base path: {}", Path::Base());
    SAGE_LOG_DEBUG("Using user path: {}", Path::User());

    // Create virtual console
    auto console = IVirtualConsole::CreateInstance();

    std::optional<Engine> engine = std::nullopt;

    try {
        SAGE_LOG_DEBUG("Starting running engine");
        engine.emplace(console);
    } catch (const std::exception& e) {
        SAGE_LOG_CRITICAL("Exception: {}, shutting down", e.what());
        return false;
    } catch (...) {
        SAGE_LOG_CRITICAL("Unknown exception thrown, shutting down");
        return false;
    }

    engine->Loop();
    SAGE_LOG_DEBUG("Finishing");

    // Close SDL
    SDL_Quit();
    return true;
}

} // namespace Sage::Core
