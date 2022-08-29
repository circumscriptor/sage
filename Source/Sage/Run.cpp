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

#include <SDL2/SDL.h>
#include <Sage/Core/Console/Log.hpp>
#include <Sage/Core/Console/VirtualConsole.hpp>
#include <Sage/Core/Engine.hpp>
#include <Sage/Core/Graphics/GraphicsCVars.hpp>
#include <Sage/Core/IO/Path.hpp>
#include <Sage/Core/SDL.hpp>

using namespace Sage::Core;
using namespace Sage::Core::Console;
using namespace Sage::Core::Graphics;
using namespace Sage::Core::IO;

namespace Sage::Core {

constexpr int kExitSuccess = 0;
constexpr int kExitFailure = 1;

static bool PreRunOperations() {
    try {
        Sage::Core::SDL::Get();
    } catch (const std::exception& e) {
        return false;
    }

    // Initialize paths
    SAGE_LOG_DEBUG("Using base path: {}", Path::Base());
    SAGE_LOG_DEBUG("Using user path: {}", Path::User());

    // Register CVars
    GraphicsCVars::Register();

    // Load config
    IVirtualConsole::Get().SyncWithFile();

    return true;
}

} // namespace Sage::Core

extern "C" int SageEngineRun(int /*argc*/, char** /*argv*/) {
    auto result = Sage::Core::kExitFailure;

    if (Sage::Core::PreRunOperations()) {
        SAGE_LOG_DEBUG("Pre-run operations complete");

        try {
            SAGE_LOG_DEBUG("Started running engine");
            Sage::Core::Engine{}.Run();
            result = Sage::Core::kExitSuccess;
            SAGE_LOG_DEBUG("Finished running engine");
        } catch (const std::exception& e) {
            SAGE_LOG_CRITICAL("Exception: {}, shutting down", e.what());
        } catch (...) {
            SAGE_LOG_CRITICAL("Unknown exception thrown, shutting down");
        }
    } else {
        SAGE_LOG_CRITICAL("Pre-run operations failed");
    }

    SAGE_LOG_DEBUG("Finished");
    return result;
}
