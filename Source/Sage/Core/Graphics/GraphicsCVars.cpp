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
/// @file GraphicsCVars.cpp
/// @brief ...
///
/// @copyright Copyright (c) 2022
///
///

#include "GraphicsCVars.hpp"

#include <Sage/Core/Console/Log.hpp>
#include <fmt/format.h>
#include <iterator>

namespace Sage::Core::Graphics {

using namespace Console;

void GraphicsCVars::Register(CVarManager& manager, CVar::Flags flags, const CVarManager* source) {
    if (&manager == source) {
        SAGE_LOG_DEBUG("Matching source and destination CVar managers, cannot register CVars");
        return;
    }

    bRetryRDInit = manager.RegisterBool("RetryRDInit",
                                        "retry render device initialization in case of failure",
                                        flags | CVar::InitOnly | CVar::RangeCheck,
                                        kDefRetryRDInit,
                                        source);

    iSyncInterval = manager.RegisterInt("SyncInterval",
                                        "synchronization (swap) interval",
                                        flags | CVar::RangeCheck,
                                        kDefSyncInterval,
                                        kMinSyncInterval,
                                        kMaxSyncInterval,
                                        source);

    iResolutionX = manager.RegisterInt("ResolutionX",
                                       "window resolution x-coord",
                                       flags | CVar::RangeCheck,
                                       kDefResolutionX,
                                       kMinResolutionX,
                                       kMaxResolutionX,
                                       source);

    iResolutionY = manager.RegisterInt("ResolutionY",
                                       "window resolution y-coord",
                                       flags | CVar::RangeCheck,
                                       kDefResolutionY,
                                       kMinResolutionY,
                                       kMaxResolutionY,
                                       source);

    eRenderDevice = manager.RegisterEnum("RenderDevice",
                                         "render device type",
                                         flags | CVar::RangeCheck,
                                         kDefRenderDevice,
                                         kRenderDeviceValues.data(),
                                         kRenderDeviceNames.data(),
                                         source);

    eValidationLevel = manager.RegisterEnum("ValidationLevel",
                                            "validation level",
                                            flags | CVar::InitOnly | CVar::RangeCheck,
                                            kDefValidationLevel,
                                            kValidationLevelValues.data(),
                                            kValidationLevelNames.data(),
                                            source);

    eFullScreenMode = manager.RegisterEnum("FullScreenMode",
                                           "full screen mode",
                                           flags | CVar::RangeCheck,
                                           kDefFullScreenMode,
                                           kFullScreenModeValues.data(),
                                           kFullScreenModeNames.data(),
                                           source);
}

} // namespace Sage::Core::Graphics
