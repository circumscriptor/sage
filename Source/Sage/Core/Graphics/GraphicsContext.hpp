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
/// @file GraphicsContext.hpp
/// @brief ...
///
/// @copyright Copyright (c) 2022
///
///

#pragma once

#include <Sage/Core/BasicTypes.hpp>
#include <Sage/Core/ClassDefinitions.hpp>
#include <Sage/Core/Console/Log.hpp>
#include <Sage/Core/Graphics/SwapChain.hpp>

// Diligent
#include <DeviceContext.h>
#include <EngineFactory.h>
#include <GraphicsTypes.h>
#include <RefCntAutoPtr.hpp>
#include <RenderDevice.h>
#include <SwapChain.h>

// stdlib
#include <array>
#include <vector>

struct SDL_Window;

namespace Sage::Core::Graphics {

using namespace Diligent;

///
/// @brief Graphics context. Stores factory, render device and device contexts
///
///
class GraphicsContext {
  public:

    SAGE_CLASS_DELETE_COPY_AND_MOVE(GraphicsContext)

    ///
    /// @brief Array of render device types
    ///
    ///
    static constexpr std::array<RENDER_DEVICE_TYPE, RENDER_DEVICE_TYPE_COUNT> kDeviceTypes = {
        RENDER_DEVICE_TYPE_UNDEFINED, // Undefined
        RENDER_DEVICE_TYPE_D3D11,     // DirectX 11
        RENDER_DEVICE_TYPE_D3D12,     // DirectX 12
        RENDER_DEVICE_TYPE_GL,        // OpenGL
        RENDER_DEVICE_TYPE_GLES,      // OpenGL ES
        RENDER_DEVICE_TYPE_VULKAN,    // Vulkan
        RENDER_DEVICE_TYPE_METAL      // Metal
    };

    ///
    /// @brief Construct a new GraphicsContext object
    ///
    ///
    GraphicsContext() = default;

    ///
    /// @brief Destroy the GraphicsContext object
    ///
    ///
    ~GraphicsContext() = default;

    ///
    /// @brief Initialize graphics context and create swap chain
    ///
    /// @param nativeWindow Native window handle
    /// @param deviceType Device type
    /// @param validationLevel Validation level
    /// @param retryInit Retry initialization with different device type
    /// @return Swap chain
    ///
    SwapChain Initialize(const NativeWindow& nativeWindow,
                         RENDER_DEVICE_TYPE  deviceType,
                         VALIDATION_LEVEL    validationLevel = VALIDATION_LEVEL_DISABLED,
                         bool                retryInit       = false) {
        RefCntAutoPtr<ISwapChain> swapChain;

        // TODO: Add preferred adapter

        Result result = kNoError;
        if (IsDeviceTypeSupported(deviceType)) {
            result = InitializeGraphics(nativeWindow, deviceType, validationLevel, &swapChain);
            if (result != kNoError && !retryInit) {
                throw std::exception("failed to initialize graphics context");
            }
        } else {
            result = kInvalidDeviceType;
        }

        if (result != kNoError) {
            for (auto type : kDeviceTypes) {
                if (type == deviceType || !IsDeviceTypeSupported(type)) {
                    continue;
                }

                // TODO: Retry different adapters

                SAGE_LOG_INFO("Trying to initialize graphics context. API: {}", DeviceTypeToString(deviceType));
                result = InitializeGraphics(nativeWindow, type, validationLevel, &swapChain);
                if (result == kNoError) {
                    break;
                }
            }
            throw std::exception("failed to initialize graphics context after retrying");
        }

        return SwapChain{mContexts[0], swapChain};
    }

    ///
    /// @brief Create swap chain (not supported on OpenGL and OpenGL ES backends)
    ///
    /// @param nativeWindow Native window
    /// @param ctx Immediate context to use (offset to mContexts)
    /// @return Swap chain
    ///
    SwapChain CreateSwapChain(const NativeWindow& nativeWindow, UInt32 ctx);

    ///
    /// @brief Get render device
    ///
    /// @return Render device
    ///
    [[nodiscard]] IRenderDevice* GetDevice() {
        return mDevice;
    }

    ///
    /// @brief Get number of initialized device contexts
    ///
    /// @return Number of device contexts
    ///
    [[nodiscard]] UInt32 ContextsCount() const {
        return mContexts.size();
    }

    ///
    /// @brief Get number of initialized immediate device contexts
    ///
    /// @return Number of immediate device contexts
    ///
    [[nodiscard]] UInt32 ImmediateContextsCount() const {
        return mImmediateContextsCount;
    }

    ///
    /// @brief Get device context
    ///
    /// @param index Index of device context
    /// @return Device context
    ///
    [[nodiscard]] IDeviceContext* GetContext(UInt32 index) {
        // TODO: Assert index < ContextsCount()
        return mContexts[index];
    }

    ///
    /// @brief Convert device type to string
    ///
    /// @param deviceType Device type
    /// @return Null-terminated string
    ///
    static const char* DeviceTypeToString(RENDER_DEVICE_TYPE deviceType) {
        switch (deviceType) {
            case RENDER_DEVICE_TYPE_GL:
                return "OpenGL";
            case RENDER_DEVICE_TYPE_GLES:
                return "OpenGL ES";
            case RENDER_DEVICE_TYPE_VULKAN:
                return "Vulkan";
            case RENDER_DEVICE_TYPE_D3D11:
                return "DirectX 11";
            case RENDER_DEVICE_TYPE_D3D12:
                return "DirectX 12";
            case RENDER_DEVICE_TYPE_METAL:
                return "Metal";
            default:
                return "UNKNOWN";
        }
    }

    ///
    /// @brief Check whether device type is supported
    ///
    /// @param deviceType Device type
    /// @return true if device type is supported
    ///
    static bool IsDeviceTypeSupported(RENDER_DEVICE_TYPE deviceType);

  private:

    enum Result {
        kNoError,
        kFailedLibrary,
        kNoAdapters,
        kFailedModifyCI,
        kFailedRenderDevice,
        kFailedSwapchain,
        kInvalidDeviceType
    };

    Result InitializeGraphics(const NativeWindow& nativeWindow,
                              RENDER_DEVICE_TYPE  deviceType,
                              VALIDATION_LEVEL    validationLevel,
                              ISwapChain**        swapChain);

    RefCntAutoPtr<IEngineFactory>              mFactory;
    RefCntAutoPtr<IRenderDevice>               mDevice;
    std::vector<RefCntAutoPtr<IDeviceContext>> mContexts;
    UInt32                                     mImmediateContextsCount{};
};

} // namespace Sage::Core::Graphics
