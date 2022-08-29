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
/// @file GraphicsCVars.hpp
/// @brief ...
///
/// @copyright Copyright (c) 2022
///
///

#pragma once

#include <GraphicsTypes.h>
#include <Sage/Core/Console/VirtualConsole.hpp>
#include <array>

namespace Sage::Core::Graphics {

struct GraphicsCVars {
    static constexpr Console::CVar::IntType kMaxSyncInterval = 2;

    static constexpr std::array<Console::CVar::IntType, 3> kVLValues{
        Diligent::VALIDATION_LEVEL_DISABLED, // Disabled validation
        Diligent::VALIDATION_LEVEL_1,        // Standard validation
        Diligent::VALIDATION_LEVEL_2         // All validation
    };

    static constexpr std::array<Console::CVar::CStringType, 4> kVLNames{
        "disable", // Disabled validation
        "level_1", // Standard validation
        "level_2", // All validation
        nullptr    // end
    };

    static constexpr std::array<Console::CVar::IntType, 6> kRDValues{
        Diligent::RENDER_DEVICE_TYPE_D3D11,  // D3D11 device
        Diligent::RENDER_DEVICE_TYPE_D3D12,  // D3D12 device
        Diligent::RENDER_DEVICE_TYPE_GL,     // OpenGL device
        Diligent::RENDER_DEVICE_TYPE_GLES,   // OpenGLES device
        Diligent::RENDER_DEVICE_TYPE_VULKAN, // Vulkan device
        Diligent::RENDER_DEVICE_TYPE_METAL   // Metal device
    };

    static constexpr std::array<Console::CVar::CStringType, 7> kRDNames{
        "dx11", // D3D11 device
        "dx12", // D3D12 device
        "gl",   // OpenGL device
        "gles", // OpenGLES device
        "vk",   // Vulkan device
        "mtl",  // Metal device
        nullptr // end
    };

    enum FullScreenMode {
        kWindowed,
        kFullScreen,
        kFullScreenDesktop,
        kFullScreenBorderless
    };

    static constexpr std::array<Console::CVar::IntType, 4> kFSMValues{
        FullScreenMode::kWindowed,            // Windowed mode
        FullScreenMode::kFullScreen,          // Fullscreen mode
        FullScreenMode::kFullScreenDesktop,   // Fullscreen (desktop) mode
        FullScreenMode::kFullScreenBorderless // Fullscreen (borderless) mode
    };

    static constexpr std::array<Console::CVar::CStringType, 5> kFSMNames{
        "windowed",             // Windowed mode
        "fullScreen",           // Fullscreen mode
        "fullScreenDesktop",    // Fullscreen (desktop) mode
        "fullScreenBorderless", // Fullscreen (borderless) mode
        nullptr                 // end
    };

    static void Register() {
        using namespace Sage::Core::Console;

        SAGE_REGISTER_CVAR_BOOL("RetryRDInit",
                                "retry render device initialization in case of failure",
                                CVar::Persistent | CVar::InitOnly,
                                true);

        SAGE_REGISTER_CVAR_INT("SyncInterval",
                               "synchronization (swap) interval",
                               CVar::Persistent,
                               1,
                               0,
                               kMaxSyncInterval);

        SAGE_REGISTER_CVAR_INT("ResolutionX",
                               "window resolution x-coord",
                               CVar::Persistent | CVar::RangeCheck,
                               0,
                               0,
                               16384);

        SAGE_REGISTER_CVAR_INT("ResolutionY",
                               "window resolution y-coord",
                               CVar::Persistent | CVar::RangeCheck,
                               0,
                               0,
                               16384);

        SAGE_REGISTER_CVAR_ENUM("RenderDevice",
                                "render device type",
                                CVar::Persistent | CVar::RangeCheck,
                                kRDValues[0],
                                kRDValues.data(),
                                kRDNames.data());

        SAGE_REGISTER_CVAR_ENUM("ValidationLevel",
                                "validation level",
                                CVar::Volatile | CVar::RangeCheck,
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
};

} // namespace Sage::Core::Graphics