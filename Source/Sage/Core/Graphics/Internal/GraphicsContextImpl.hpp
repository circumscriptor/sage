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

#include <Sage/Core/ClassDefinitions.hpp>
#include <Sage/Core/Graphics/GraphicsContext.hpp>

// stdlib
#include <vector>

namespace Sage::Core::Graphics::Internal {

class GraphicsContext : public IGraphicsContext {
  public:

    SAGE_CLASS_DELETE_COPY_AND_MOVE(GraphicsContext)

    GraphicsContext(std::shared_ptr<Console::IVirtualConsole> console, Console::IVirtualConsole::ContextID contextID);

    GraphicsContext(std::shared_ptr<Console::IVirtualConsole> console,
                    Console::IVirtualConsole::ContextID       contextID,
                    std::shared_ptr<GraphicsContext>          base);

    ~GraphicsContext() override;

    void Clear() override;

    void Present() override;

    UInt32 Enumerate(Diligent::Version minVersion, std::vector<Diligent::GraphicsAdapterInfo>& adapters);

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

    ///
    /// @brief Convert device type to string
    ///
    /// @param deviceType Device type
    /// @return Null-terminated string
    ///
    static const char* DeviceTypeToString(Diligent::RENDER_DEVICE_TYPE deviceType);

    static bool IsDeviceTypeSupported(Diligent::RENDER_DEVICE_TYPE deviceType);

  private:

    bool InitializeWindow();

    bool InitializeGraphics();

    bool InitializeGraphicsFromBase(std::shared_ptr<GraphicsContext> base);

    enum Result {
        kNoError,
        kFailedLibrary,
        kNoAdapters,
        kFailedRenderDevice,
        kFailedSwapchain,
        kInvalidDeviceType
    };

    Result InitializeGraphics(Diligent::RENDER_DEVICE_TYPE deviceType, Diligent::VALIDATION_LEVEL validationLevel);

    void LoadContexts(const std::vector<Diligent::IDeviceContext*>& contexts);

    SDL_Window*                                                    mWindow{nullptr};
    Diligent::NativeWindow                                         mNativeWindow;
    Diligent::RefCntAutoPtr<Diligent::IEngineFactory>              mFactory;
    Diligent::RefCntAutoPtr<Diligent::IRenderDevice>               mDevice;
    Diligent::RefCntAutoPtr<Diligent::ISwapChain>                  mSwapchain;
    std::vector<Diligent::RefCntAutoPtr<Diligent::IDeviceContext>> mContexts;
    UInt32                                                         mImmediateContextsCount{};
};

} // namespace Sage::Core::Graphics::Internal
