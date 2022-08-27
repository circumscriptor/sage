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
/// @file Run.cpp
/// @brief ...
///
/// @copyright Copyright (c) 2022
///
///

#include "Run.hpp"

#include "Engine.hpp"

#include <SDL2/SDL.h>
#include <Sage/Console/Log.hpp>
#include <Sage/Console/VirtualConsole.hpp>
#include <Sage/Graphics/GraphicsCVars.hpp>
#include <Sage/IO/Path.hpp>

using namespace Sage;
using namespace Sage::Console;
using namespace Sage::Graphics;
using namespace Sage::IO;

namespace Sage {

constexpr int kExitSuccess = 0;
constexpr int kExitFailure = 1;

static bool PreRunOperations() {
    // Initialize SDL
#ifdef SDL_VIDEO_DRIVER_X11
    SDL_SetHint(SDL_HINT_VIDEODRIVER, "x11");
#endif

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        return false;
    }

    // Initialize logs
    Log::Initialize();

    // Initialize paths
    SAGE_LOG_DEBUG("Using base path: {}", Path::Base());
    SAGE_LOG_DEBUG("Using user path: {}", Path::User());

    // Initialize console
    if (!VirtualConsole::Initialize()) {
        return false;
    }

    // Register CVars
    GraphicsCVars::Register();

    // Load config
    VirtualConsole::LoadConfig();

    return true;
}

static void PostRunOperations() {
    // Shutdown SDL
    SDL_Quit();

    // Shutdown console
    VirtualConsole::Shutdown();

    // Shutdown logs
    Log::Shutdown();
}

} // namespace Sage

extern "C" int SageEngineRun(int /*argc*/, char** /*argv*/) {
    auto result = Sage::kExitFailure;

    if (Sage::PreRunOperations()) {
        SAGE_LOG_DEBUG("Pre-run operations complete");

        try {
            SAGE_LOG_DEBUG("Started running engine");
            Sage::Engine{}.Run();
            result = Sage::kExitSuccess;
            SAGE_LOG_DEBUG("Finished running engine");
        } catch (const std::exception& e) {
            SAGE_LOG_CRITICAL("Exception: {}, shutting down", e.what());
        } catch (...) {
            SAGE_LOG_CRITICAL("Unknown exception thrown, shutting down");
        }
    } else {
        SAGE_LOG_CRITICAL("Pre-run operations failed");
    }

    Sage::PostRunOperations();
    SAGE_LOG_DEBUG("Post-run operations complete");
    return result;
}
