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
#include <string_view>

namespace Sage::Core::Graphics {

struct GraphicsCVarsCollection {
    Console::CVar bRetryRDInit;
    Console::CVar iSyncInterval;
    Console::CVar iResolutionX;
    Console::CVar iResolutionY;
    Console::CVar eRenderDevice;
    Console::CVar eValidationLevel;
    Console::CVar eFullScreenMode;
};

struct GraphicsCVars {
    static constexpr Console::CVar::IntType kMinSyncInterval = 0;
    static constexpr Console::CVar::IntType kMaxSyncInterval = 2;
    static constexpr Console::CVar::IntType kMinResolutionX  = 0;
    static constexpr Console::CVar::IntType kMaxResolutionX  = 16384;
    static constexpr Console::CVar::IntType kMinResolutionY  = 0;
    static constexpr Console::CVar::IntType kMaxResolutionY  = 16384;

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

    static void Register();

    static void RegisterVolatileCollection(GraphicsCVarsCollection& collection, std::string_view base);
};

} // namespace Sage::Core::Graphics