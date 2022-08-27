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
/// @file GraphicsEngine.hpp
/// @brief ...
///
/// @copyright Copyright (c) 2022
///
///

#pragma once

#include "Sage/Console/VirtualConsole.hpp"

#include <Sage/BasicTypes.hpp>
#include <Sage/Console/CVarManager.hpp>

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

namespace Sage::Graphics {

///
/// @brief GraphicsEngine service
///
///
class GraphicsContext {
  public:

    static constexpr std::array<float, 4> kClearColor = {0.F, 0.F, 0.F, 1.F}; //!< Default clear color

    enum Result {
        kNoError,
        kFailedLibrary,
        kNoAdapters,
        kFailedRenderDevice,
        kFailedSwapchain,
        kInvalidDeviceType
    };

    SAGE_CLASS_DELETE_COPY_AND_MOVE(GraphicsContext)

    ///
    /// @brief Convert device type to string
    ///
    /// @param deviceType Device type
    /// @return Null-terminated string
    ///
    static const char* DeviceTypeToString(Diligent::RENDER_DEVICE_TYPE deviceType);

    GraphicsContext();

    ~GraphicsContext() = default;

    [[nodiscard]] UInt32 ImmediateContextsCount() const {
        return mImmediateContextsCount;
    }

    [[nodiscard]] SDL_Window* GetWindow() {
        return mWindow;
    }

    [[nodiscard]] Diligent::IRenderDevice* GetDevice() {
        return mDevice;
    }

    [[nodiscard]] Diligent::ISwapChain* GetSwapchain() {
        return mSwapchain;
    }

    [[nodiscard]] Diligent::IDeviceContext* GetContext(UInt32 index) {
        // TODO: Assert index < mContexts.size()
        return mContexts[index];
    }

    UInt32 Enumerate(Diligent::Version minVersion, std::vector<Diligent::GraphicsAdapterInfo>& adapters);

    void Present();

    void Clear();

  private:

    bool Initialize();

    bool InitializeWindow();

    Result InitializeGraphics(const Diligent::NativeWindow& nativeWindow,
                              Diligent::RENDER_DEVICE_TYPE  deviceType,
                              Diligent::VALIDATION_LEVEL    validationLevel);

    Result InitializeGraphicsOpenGL(const Diligent::NativeWindow& nativeWindow,
                                    Diligent::VALIDATION_LEVEL    validationLevel);

    Result InitializeGraphicsVulkan(const Diligent::NativeWindow& nativeWindow,
                                    Diligent::VALIDATION_LEVEL    validationLevel);

    Result InitializeGraphicsD3D11(const Diligent::NativeWindow& nativeWindow,
                                   Diligent::VALIDATION_LEVEL    validationLevel);

    Result InitializeGraphicsD3D12(const Diligent::NativeWindow& nativeWindow,
                                   Diligent::VALIDATION_LEVEL    validationLevel);

    void ModifyCreateInfo(Diligent::RENDER_DEVICE_TYPE                       deviceType,
                          Diligent::EngineCreateInfo&                        engineCI,
                          Diligent::SwapChainDesc&                           swapchainDesc,
                          std::vector<Diligent::GraphicsAdapterInfo>&        adapters,
                          std::vector<Diligent::ImmediateContextCreateInfo>& contextCIs);

    void LoadContexts(const std::vector<Diligent::IDeviceContext*>& contexts);

    SDL_Window*                                                    mWindow{nullptr};
    Diligent::RefCntAutoPtr<Diligent::IEngineFactory>              mFactory;
    Diligent::RefCntAutoPtr<Diligent::IRenderDevice>               mDevice;
    Diligent::RefCntAutoPtr<Diligent::ISwapChain>                  mSwapchain;
    std::vector<Diligent::RefCntAutoPtr<Diligent::IDeviceContext>> mContexts;
    UInt32                                                         mImmediateContextsCount{};

    Console::CVar bRetryRDInit;
    Console::CVar iSyncInterval;
    Console::CVar iResolutionX;
    Console::CVar iResolutionY;
    Console::CVar eRenderDevice;
    Console::CVar eValidationLevel;
    Console::CVar eFullScreenMode;
};

} // namespace Sage::Graphics
