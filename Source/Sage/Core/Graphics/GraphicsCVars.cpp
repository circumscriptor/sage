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

#include <fmt/format.h>
#include <iterator>

namespace Sage::Core::Graphics {

using namespace Console;

void GraphicsCVars::Register() {
    SAGE_REGISTER_CVAR_BOOL("RetryRDInit",
                            "retry render device initialization in case of failure",
                            CVar::Persistent | CVar::InitOnly | CVar::RangeCheck,
                            true);

    SAGE_REGISTER_CVAR_INT("SyncInterval",
                           "synchronization (swap) interval",
                           CVar::Persistent | CVar::RangeCheck,
                           1,
                           kMinSyncInterval,
                           kMaxSyncInterval);

    SAGE_REGISTER_CVAR_INT("ResolutionX",
                           "window resolution x-coord",
                           CVar::Persistent | CVar::RangeCheck,
                           0,
                           kMinResolutionX,
                           kMaxResolutionX);

    SAGE_REGISTER_CVAR_INT("ResolutionY",
                           "window resolution y-coord",
                           CVar::Persistent | CVar::RangeCheck,
                           0,
                           kMinResolutionY,
                           kMaxResolutionY);

    SAGE_REGISTER_CVAR_ENUM("RenderDevice",
                            "render device type",
                            CVar::Persistent | CVar::RangeCheck,
                            kRDValues[0],
                            kRDValues.data(),
                            kRDNames.data());

    SAGE_REGISTER_CVAR_ENUM("ValidationLevel",
                            "validation level",
                            CVar::Persistent | CVar::InitOnly | CVar::RangeCheck,
                            kVLValues[0],
                            kVLValues.data(),
                            kVLNames.data());

    SAGE_REGISTER_CVAR_ENUM("FullScreenMode",
                            "full screen mode",
                            CVar::Persistent | CVar::RangeCheck,
                            kFSMValues[0],
                            kFSMValues.data(),
                            kFSMNames.data());
}

void GraphicsCVars::RegisterVolatileCollection(GraphicsCVarsCollection& collection, std::string_view base) {
    std::string name;
    name.reserve(base.length() + 20);

    // Sync interval
    {
        fmt::format_to(std::back_inserter(name), "{}.{}", base, "SyncInterval");
        auto cvar                = SAGE_GET_CVAR("SyncInterval");
        collection.iSyncInterval = SAGE_REGISTER_CVAR_INT(name.c_str(),
                                                          cvar.GetDescription(),
                                                          CVar::Volatile | CVar::RangeCheck,
                                                          cvar.GetInt(),
                                                          kMinSyncInterval,
                                                          kMaxSyncInterval);
        name.resize(0);
    }

    // Resolution X
    {
        fmt::format_to(std::back_inserter(name), "{}.{}", base, "ResolutionX");
        auto cvar               = SAGE_GET_CVAR("ResolutionX");
        collection.iResolutionX = SAGE_REGISTER_CVAR_INT(name.c_str(),
                                                         cvar.GetDescription(),
                                                         CVar::Volatile | CVar::RangeCheck,
                                                         cvar.GetInt(),
                                                         kMinResolutionX,
                                                         kMaxResolutionX);
        name.resize(0);
    }

    // Resolution Y
    {
        fmt::format_to(std::back_inserter(name), "{}.{}", base, "ResolutionY");
        auto cvar               = SAGE_GET_CVAR("ResolutionY");
        collection.iResolutionY = SAGE_REGISTER_CVAR_INT(name.c_str(),
                                                         cvar.GetDescription(),
                                                         CVar::Volatile | CVar::RangeCheck,
                                                         cvar.GetInt(),
                                                         kMinResolutionY,
                                                         kMaxResolutionY);
        name.resize(0);
    }

    // Render device
    {
        fmt::format_to(std::back_inserter(name), "{}.{}", base, "RenderDevice");
        auto cvar                = SAGE_GET_CVAR("RenderDevice");
        collection.eRenderDevice = SAGE_REGISTER_CVAR_ENUM(name.c_str(),
                                                           cvar.GetDescription(),
                                                           CVar::Volatile | CVar::RangeCheck,
                                                           cvar.GetInt(),
                                                           kRDValues.data(),
                                                           kRDNames.data());
        name.resize(0);
    }

    // Validation level
    {
        fmt::format_to(std::back_inserter(name), "{}.{}", base, "ValidationLevel");
        auto cvar                   = SAGE_GET_CVAR("ValidationLevel");
        collection.eValidationLevel = SAGE_REGISTER_CVAR_ENUM(name.c_str(),
                                                              cvar.GetDescription(),
                                                              CVar::Volatile | CVar::RangeCheck,
                                                              cvar.GetInt(),
                                                              kRDValues.data(),
                                                              kRDNames.data());
        name.resize(0);
    }

    // Full screen mode
    {
        fmt::format_to(std::back_inserter(name), "{}.{}", base, "FullScreenMode");
        auto cvar                  = SAGE_GET_CVAR("FullScreenMode");
        collection.eFullScreenMode = SAGE_REGISTER_CVAR_ENUM(name.c_str(),
                                                             cvar.GetDescription(),
                                                             CVar::Volatile | CVar::RangeCheck,
                                                             cvar.GetInt(),
                                                             kFSMValues.data(),
                                                             kFSMNames.data());
        name.resize(0);
    }
}

} // namespace Sage::Core::Graphics
