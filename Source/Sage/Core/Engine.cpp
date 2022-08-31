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
#include <SDL2/SDL_timer.h>
#include <Sage/Core/IO/Path.hpp>
#include <Sage/Core/SDL.hpp>
#include <optional>

namespace Sage::Core {

using namespace Console;
using namespace Graphics;
using namespace IO;

Engine::Engine(IVirtualConsole& console) {
    console.RegisterPersistent(mGraphicsCVars);
}

Engine::~Engine() = default;

void Engine::Loop() {
    SDL_Event event;
    while (!mContexts.empty()) {
        while (SDL_PollEvent(&event) != 0) {
            for (auto& context : mContexts) {
                context.ProcessEvents(event);
            }
        }
    }

    auto checkContexts = std::find_if(mContexts.begin(), mContexts.end(), [](auto& context) -> bool {
        return context.ShouldDestroy();
    });

    for (auto& context : mContexts) {
        context.Update();
    }

    for (auto& context : mContexts) {
        context.Render();
    }
}

bool Engine::Run() {
    try {
        Sage::Core::SDL::Get();
    } catch (const std::exception& e) {
        return false;
    }

    // Initialize paths
    SAGE_LOG_DEBUG("Using base path: {}", Path::Base());
    SAGE_LOG_DEBUG("Using user path: {}", Path::User());

    std::optional<Engine> engineOpt = std::nullopt;

    try {
        SAGE_LOG_DEBUG("Starting running engine");
        engineOpt.emplace(IVirtualConsole::Get());
    } catch (const std::exception& e) {
        SAGE_LOG_CRITICAL("Exception: {}, shutting down", e.what());
        return false;
    } catch (...) {
        SAGE_LOG_CRITICAL("Unknown exception thrown, shutting down");
        return false;
    }

    auto& engine = engineOpt.value();
    while (!engine.mContexts.empty()) {
        engine.Loop();
    }
    SAGE_LOG_DEBUG("Finishing");
    return true;
}

} // namespace Sage::Core
