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
/// @file GraphicsContext.cpp
/// @brief ...
///
/// @copyright Copyright (c) 2022
///
///

#include "GraphicsContext.hpp"

#include "GraphicsCVars.hpp"

#include <SDL2/SDL_error.h>
#include <SDL2/SDL_video.h>
#include <Sage/Core/Console/Log.hpp>
#include <Sage/Core/Exports.hpp>

// Diligent
#include <DeviceContext.h>
#include <EngineFactory.h>
#include <GraphicsTypes.h>
#include <NativeWindow.h>
#include <RefCntAutoPtr.hpp>
#include <RenderDevice.h>
#include <SwapChain.h>

#if PLATFORM_WIN32
    #define ENGINE_DLL 1
#endif

#if GL_SUPPORTED || GLES_SUPPORTED
    #include <EngineFactoryOpenGL.h>
#endif

#if VULKAN_SUPPORTED
    #include <EngineFactoryVk.h>
#endif

#if D3D11_SUPPORTED
    #include <EngineFactoryD3D11.h>
#endif

#if D3D12_SUPPORTED
    #include <EngineFactoryD3D12.h>
#endif

#if PLATFORM_UNIVERSAL_WINDOWS
    #include <windows.ui.core.h>
#endif

// SDL
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>

// stdlib
#include <algorithm>
#include <array>
#include <memory>
#include <vector>

#ifndef SAGE_WINDOW_TITLE
    #define SAGE_WINDOW_TITLE "Sage"
#endif

#ifndef SAGE_DEFAULT_WINDOW_WIDTH
    #define SAGE_DEFAULT_WINDOW_WIDTH 1280
#endif

#ifndef SAGE_DEFAULT_WINDOW_HEIGHT
    #define SAGE_DEFAULT_WINDOW_HEIGHT 720
#endif

namespace Sage::Core::Graphics {

using namespace Diligent;
using namespace Console;

static inline spdlog::level::level_enum DebugMessageSeverityToLogLevel(DEBUG_MESSAGE_SEVERITY severity) {
    switch (severity) {
        case Diligent::DEBUG_MESSAGE_SEVERITY_INFO:
            return spdlog::level::info;
        case Diligent::DEBUG_MESSAGE_SEVERITY_WARNING:
            return spdlog::level::warn;
        case Diligent::DEBUG_MESSAGE_SEVERITY_ERROR:
            return spdlog::level::err;
        case Diligent::DEBUG_MESSAGE_SEVERITY_FATAL_ERROR:
            return spdlog::level::critical;
    }
}

static void DiligentDebugCallback(enum DEBUG_MESSAGE_SEVERITY  severity,
                                  const char*                  message,
                                  [[maybe_unused]] const char* function,
                                  [[maybe_unused]] const char* file,
                                  [[maybe_unused]] int         line) {
    auto level = DebugMessageSeverityToLogLevel(severity);
    if (function != nullptr && file != nullptr) {
        SAGE_LOG(level, "[diligent] [{}:{}] ({}) {}", file, line, function, message);
    } else {
        SAGE_LOG(level, "[diligent] {}", message);
    }
}

bool GetNativeWindow(SDL_Window* windowSDL, Diligent::NativeWindow& nativeWindow) {
    SDL_SysWMinfo syswm{};
    SDL_VERSION(&syswm.version);
    if (SDL_GetWindowWMInfo(windowSDL, &syswm) == 0) {
        SAGE_LOG_ERROR("Failed to retrieve native window handle: {}", SDL_GetError());
        return false;
    }

#if PLATFORM_WIN32
    if (syswm.subsystem == SDL_SYSWM_WINDOWS) {
        nativeWindow.hWnd = syswm.info.win.window;
        return true;
    }
#elif PLATFORM_UNIVERSAL_WINDOWS
    if (syswm.subsystem == SDL_SYSWM_WINRT) {
        ABI::Windows::UI::Core::ICoreWindow* coreWindow;
        if (!FAILED(syswm.info.winrt.window->QueryInterface(&coreWindow)) {
            nativeWindow.pCoreWindow = coreWindow;
            return true;
		} 
        SAGE_LOG_ERROR("Failed to query CoreWindow interface");
        return false;
    }
#elif PLATFORM_ANDROID
    if (syswm.subsystem == SDL_SYSWM_ANDROID) {
        nativeWindow.pAWindow = syswm.info.android.window;
        return true;
    }
#elif PLATFORM_LINUX
    if (syswm.subsystem == SDL_SYSWM_X11) {
        nativeWindow.WindowId       = syswm.info.x11.window;
        nativeWindow.pDisplay       = syswm.info.x11.display;
        nativeWindow.pXCBConnection = nullptr;
        return true;
    }
#elif PLATFORM_MACOS
    // TODO: Convert syswm.info.cocoa.window from NSWindow to NSView
#elif PLATFORM_IOS || PLATFORM_TVOS
    // TODO: Convert syswm.info.uikit.window from UIWindow to CALayer
#elif PLATFORM_EMSCRIPTEN
    // TODO: ?
#endif
    SAGE_LOG_ERROR("Native window handle subsystem does not match platform");
    return false;
}

static bool AddContextCI(std::vector<Diligent::ImmediateContextCreateInfo>& contextCIs,
                         Diligent::GraphicsAdapterInfo&                     adapter,
                         const char*                                        name,
                         Diligent::COMMAND_QUEUE_TYPE                       queueType,
                         Diligent::QUEUE_PRIORITY                           queuePriority) {
    auto& queues = adapter.Queues;
    for (Uint32 i = 0, count = adapter.NumQueues; i < count; ++i) {
        auto& queue = queues[i];

        if (queue.MaxDeviceContexts == 0) {
            continue;
        }

        if ((queue.QueueType & COMMAND_QUEUE_TYPE_PRIMARY_MASK) == queueType) {
            queue.MaxDeviceContexts -= 1;
            contextCIs.emplace_back(name, UInt8(i), queuePriority);
            return true;
        }
    }
    return false;
}

const char* GraphicsContext::DeviceTypeToString(Diligent::RENDER_DEVICE_TYPE deviceType) {
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
            return "INVALID";
    }
}

GraphicsContext::GraphicsContext() :
    bRetryRDInit{SAGE_GET_CVAR("RetryRDInit")},
    iSyncInterval{SAGE_GET_CVAR("SyncInterval")},
    iResolutionX{SAGE_GET_CVAR("ResolutionX")},
    iResolutionY{SAGE_GET_CVAR("ResolutionY")},
    eRenderDevice{SAGE_GET_CVAR("RenderDevice")},
    eValidationLevel{SAGE_GET_CVAR("ValidationLevel")},
    eFullScreenMode{SAGE_GET_CVAR("FullScreenMode")} {
    if (!Initialize()) {
        throw std::exception("failed to initialize graphics context");
    }
}

bool GraphicsContext::Initialize() {
    if (!InitializeWindow()) {
        return false;
    }

    Diligent::NativeWindow nativeWindow;
    if (!GetNativeWindow(mWindow, nativeWindow)) {
        SAGE_LOG_ERROR("Failed to retrieve native window handle");
        return false;
    }

    auto   renderDevice    = eRenderDevice.GetInt();
    auto   validationLevel = eValidationLevel.GetInt();
    Result result          = InitializeGraphics(nativeWindow,
                                       static_cast<RENDER_DEVICE_TYPE>(renderDevice),
                                       static_cast<VALIDATION_LEVEL>(validationLevel));

    if (result != Result::kNoError && bRetryRDInit.GetBool()) {
        auto selected = std::find(GraphicsCVars::kRDValues.begin(), GraphicsCVars::kRDValues.end(), renderDevice);
        for (auto it = GraphicsCVars::kRDValues.begin(), end = GraphicsCVars::kRDValues.end(); it != end; ++it) {
            if (it == selected) {
                continue;
            }

            auto deviceType = static_cast<RENDER_DEVICE_TYPE>(*it);
            SAGE_LOG_WARN("Retrying RD initialization, API: {}", DeviceTypeToString(deviceType));

            result = InitializeGraphics(nativeWindow, deviceType, static_cast<VALIDATION_LEVEL>(validationLevel));
            if (result == Result::kNoError) {
                break;
            }
        }
    }

    return result == Result::kNoError;
}

bool GraphicsContext::InitializeWindow() {
    if (mWindow != nullptr) {
        SAGE_LOG_WARN("Window is already created");
        return false;
    }

    int    xPos  = SDL_WINDOWPOS_CENTERED;
    int    yPos  = SDL_WINDOWPOS_CENTERED;
    int    xRes  = int(iResolutionX.GetInt());
    int    yRes  = int(iResolutionY.GetInt());
    UInt32 flags = SDL_WINDOW_SHOWN;

    auto fullScreenMode = static_cast<GraphicsCVars::FullScreenMode>(eFullScreenMode.GetInt());
    switch (fullScreenMode) {
        case GraphicsCVars::kFullScreen: {
            flags |= UInt32(SDL_WINDOW_FULLSCREEN);
        } break;
        case GraphicsCVars::kFullScreenDesktop: {
            flags |= UInt32(SDL_WINDOW_FULLSCREEN_DESKTOP);
        } break;
        case GraphicsCVars::kFullScreenBorderless: {
            if (SDL_DisplayMode mode; SDL_GetDesktopDisplayMode(0, &mode) == 0) {
                flags |= UInt32(SDL_WINDOW_BORDERLESS);
                xRes = mode.w;
                yRes = mode.h;
            } else {
                SAGE_LOG_WARN("Cannot set fullscreen borderless mode, falling back to windowed mode");
                fullScreenMode = GraphicsCVars::kWindowed;
                eFullScreenMode.SetInt(CVar::IntType(fullScreenMode));
                eFullScreenMode.ClearModified();
            }
        } break;
        default:
            break;
    }

    if ((xRes == 0 || yRes == 0) && fullScreenMode != GraphicsCVars::kFullScreenDesktop) {
        if (SDL_DisplayMode mode; SDL_GetDesktopDisplayMode(0, &mode) == 0) {
            xRes = mode.w;
            yRes = mode.h;
        } else {
            xRes = SAGE_DEFAULT_WINDOW_WIDTH;
            yRes = SAGE_DEFAULT_WINDOW_HEIGHT;
            SAGE_LOG_WARN("Failed to retrieve desktop display mode. Error: {}", SDL_GetError());
        }
    }

    mWindow = SDL_CreateWindow(SAGE_WINDOW_TITLE, xPos, yPos, xRes, yRes, flags);
    if (mWindow == nullptr) {
        SAGE_LOG_ERROR("Failed to create SDL window. Error: {}", SDL_GetError());
        return false;
    }

    SDL_GetWindowSize(mWindow, &xRes, &yRes);
    iResolutionX.SetInt(xRes);
    iResolutionY.SetInt(yRes);
    iResolutionX.ClearModified();
    iResolutionY.ClearModified();
    return true;
}

GraphicsContext::Result GraphicsContext::InitializeGraphics(const Diligent::NativeWindow& nativeWindow,
                                                            Diligent::RENDER_DEVICE_TYPE  deviceType,
                                                            Diligent::VALIDATION_LEVEL    validationLevel) {
    // Reset if re-initializing engine
    mContexts.clear();
    mDevice    = nullptr;
    mSwapchain = nullptr;

    // Start with no error
    Result result = kNoError;

    switch (deviceType) {
#if !GLES_SUPPORTED
        case RENDER_DEVICE_TYPE_GL:
            result = InitializeGraphicsOpenGL(nativeWindow, validationLevel);
            break;
#endif
#if GLES_SUPPORTED
        case RENDER_DEVICE_TYPE_GLES:
            result = InitializeGraphicsOpenGL(nativeWindow, validationLevel);
            break;
#endif
        case RENDER_DEVICE_TYPE_VULKAN:
            result = InitializeGraphicsVulkan(nativeWindow, validationLevel);
            break;
        case RENDER_DEVICE_TYPE_D3D11:
            result = InitializeGraphicsD3D11(nativeWindow, validationLevel);
            break;
        case RENDER_DEVICE_TYPE_D3D12:
            result = InitializeGraphicsD3D12(nativeWindow, validationLevel);
            break;
        default:
            result = Result::kInvalidDeviceType;
            break;
    }

    switch (result) {
        case Result::kInvalidDeviceType:
            SAGE_LOG_ERROR("Requested device type {} is not valid or not supported on this platform",
                           DeviceTypeToString(deviceType));
            break;
        case Result::kFailedLibrary:
            SAGE_LOG_ERROR("Failed to initialize {} library", DeviceTypeToString(deviceType));
            break;
        case Result::kNoAdapters:
            SAGE_LOG_ERROR("No available {} adapters", DeviceTypeToString(deviceType));
            break;
        case Result::kFailedRenderDevice:
            SAGE_LOG_ERROR("Failed to create {} render device", DeviceTypeToString(deviceType));
            break;
        case Result::kFailedSwapchain:
            SAGE_LOG_ERROR("Failed to create {} swapchain", DeviceTypeToString(deviceType));
            break;
        case Result::kNoError: {
            const auto& version = mDevice->GetDeviceInfo().APIVersion;
            SAGE_LOG_INFO("Initialized graphics: {} {}.{}",
                          DeviceTypeToString(deviceType),
                          version.Major,
                          version.Minor);
            SAGE_LOG_INFO("Initialized adapter: {}", mDevice->GetAdapterInfo().Description);
        } break;
    }

    return result;
}

UInt32 GraphicsContext::Enumerate(Diligent::Version minVersion, std::vector<Diligent::GraphicsAdapterInfo>& adapters) {
    UInt32 adaptersCount = 0;
    mFactory->EnumerateAdapters(minVersion, adaptersCount, nullptr);

    if (adaptersCount > 0) {
        adapters.resize(adaptersCount);
        mFactory->EnumerateAdapters(minVersion, adaptersCount, adapters.data());
    }
    return adaptersCount;
}

void GraphicsContext::Clear() {
    // RTV = Render Target View
    auto* rtv = mSwapchain->GetCurrentBackBufferRTV();
    // DSV = Depth Stencil View
    auto* dsv = mSwapchain->GetDepthBufferDSV();

    mContexts[0]->SetRenderTargets(1, &rtv, dsv, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
    mContexts[0]->ClearRenderTarget(rtv, kClearColor.data(), Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
    mContexts[0]->ClearDepthStencil(dsv,
                                    Diligent::CLEAR_DEPTH_FLAG,
                                    1.F,
                                    0,
                                    Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
}

void GraphicsContext::Present() {
    mSwapchain->Present(iSyncInterval.GetInt());
}

void GraphicsContext::LoadContexts(const std::vector<Diligent::IDeviceContext*>& contexts) {
    // Apply reference counters to contexts
    mContexts.clear();
    mContexts.reserve(contexts.size());
    for (auto* context : contexts) {
        mContexts.emplace_back(context);
    }
}

GraphicsContext::Result GraphicsContext::InitializeGraphicsOpenGL(const Diligent::NativeWindow& nativeWindow,
                                                                  Diligent::VALIDATION_LEVEL    validationLevel) {
#if GL_SUPPORTED || GLES_SUPPORTED
    #if ENGINE_DLL
    auto* GetEngineFactoryOpenGL = LoadGraphicsEngineOpenGL();
    #endif

    auto* factoryOpenGL = GetEngineFactoryOpenGL();
    mFactory            = factoryOpenGL;
    mFactory->SetMessageCallback(DiligentDebugCallback);

    EngineGLCreateInfo engineCI;
    engineCI.Window = nativeWindow;

    #if GL_SUPPORTED
    engineCI.GraphicsAPIVersion = {4, 0};
    auto deviceType             = RENDER_DEVICE_TYPE_GL;
    #elif GLES_SUPPORTED
    engineCI.GraphicsAPIVersion = {3, 0};
    auto deviceType             = RENDER_DEVICE_TYPE_GLES;
    #endif

    engineCI.SetValidationLevel(validationLevel);

    SwapChainDesc swapchainDesc;

    std::vector<Diligent::ImmediateContextCreateInfo> contextCIs;
    {
        std::vector<Diligent::GraphicsAdapterInfo> adapters;

        if (Enumerate(engineCI.GraphicsAPIVersion, adapters) == 0) {
            return Result::kNoAdapters;
        }

        ModifyCreateInfo(deviceType, engineCI, swapchainDesc, adapters, contextCIs);

        engineCI.pImmediateContextInfo = contextCIs.data();
        engineCI.NumImmediateContexts  = Uint32(contextCIs.size());
    }

    mImmediateContextsCount = 1;
    std::vector<IDeviceContext*> contexts(mImmediateContextsCount + engineCI.NumDeferredContexts);
    factoryOpenGL->CreateDeviceAndSwapChainGL(engineCI, &mDevice, contexts.data(), swapchainDesc, &mSwapchain);

    if (!mDevice) {
        return Result::kFailedRenderDevice;
    }

    if (!mSwapchain) {
        return Result::kFailedSwapchain;
    }

    LoadContexts(contexts);

    return Result::kNoError;
#else
    return Result::kInvalidDeviceType;
#endif
}

GraphicsContext::Result GraphicsContext::InitializeGraphicsVulkan(const Diligent::NativeWindow& nativeWindow,
                                                                  Diligent::VALIDATION_LEVEL    validationLevel) {
#if VULKAN_SUPPORTED
    #if ENGINE_DLL
    auto* GetEngineFactoryVk = LoadGraphicsEngineVk();
    #endif

    auto* factoryVk = GetEngineFactoryVk();
    mFactory        = factoryVk;
    mFactory->SetMessageCallback(DiligentDebugCallback);

    EngineVkCreateInfo engineCI;
    engineCI.GraphicsAPIVersion = {1, 0};
    engineCI.SetValidationLevel(validationLevel);

    SwapChainDesc swapchainDesc;

    std::vector<Diligent::ImmediateContextCreateInfo> contextCIs;
    {
        std::vector<Diligent::GraphicsAdapterInfo> adapters;

        if (Enumerate(engineCI.GraphicsAPIVersion, adapters) == 0) {
            return Result::kNoAdapters;
        }

        ModifyCreateInfo(RENDER_DEVICE_TYPE_VULKAN, engineCI, swapchainDesc, adapters, contextCIs);

        engineCI.pImmediateContextInfo = contextCIs.data();
        engineCI.NumImmediateContexts  = Uint32(contextCIs.size());
    }

    mImmediateContextsCount = std::max(1U, engineCI.NumImmediateContexts);
    std::vector<IDeviceContext*> contexts(mImmediateContextsCount + engineCI.NumDeferredContexts);

    factoryVk->CreateDeviceAndContextsVk(engineCI, &mDevice, contexts.data());

    if (!mDevice) {
        return Result::kFailedRenderDevice;
    }

    factoryVk->CreateSwapChainVk(mDevice, contexts[0], swapchainDesc, nativeWindow, &mSwapchain);

    if (!mSwapchain) {
        return Result::kFailedSwapchain;
    }

    LoadContexts(contexts);

    return Result::kNoError;
#else
    return Result::kInvalidDeviceType;
#endif
}

GraphicsContext::Result GraphicsContext::InitializeGraphicsD3D11(const Diligent::NativeWindow& nativeWindow,
                                                                 Diligent::VALIDATION_LEVEL    validationLevel) {
#if D3D11_SUPPORTED
    #if ENGINE_DLL
    auto* GetEngineFactoryD3D11 = LoadGraphicsEngineD3D11();
    #endif

    auto* factoryD3D11 = GetEngineFactoryD3D11();
    mFactory           = factoryD3D11;
    mFactory->SetMessageCallback(DiligentDebugCallback);

    EngineD3D11CreateInfo engineCI;
    engineCI.GraphicsAPIVersion = {11, 0};
    engineCI.SetValidationLevel(validationLevel);

    SwapChainDesc swapchainDesc;

    std::vector<Diligent::ImmediateContextCreateInfo> contextCIs;
    {
        std::vector<Diligent::GraphicsAdapterInfo> adapters;

        if (Enumerate(engineCI.GraphicsAPIVersion, adapters) == 0) {
            return Result::kNoAdapters;
        }

        ModifyCreateInfo(RENDER_DEVICE_TYPE_D3D11, engineCI, swapchainDesc, adapters, contextCIs);

        engineCI.pImmediateContextInfo = contextCIs.data();
        engineCI.NumImmediateContexts  = Uint32(contextCIs.size());
    }

    mImmediateContextsCount = std::max(1U, engineCI.NumImmediateContexts);
    std::vector<IDeviceContext*> contexts(mImmediateContextsCount + engineCI.NumDeferredContexts);

    factoryD3D11->CreateDeviceAndContextsD3D11(engineCI, &mDevice, contexts.data());

    if (!mDevice) {
        return Result::kFailedRenderDevice;
    }

    // TODO: Initialize FullScreenModeDesc
    factoryD3D11
        ->CreateSwapChainD3D11(mDevice, contexts[0], swapchainDesc, FullScreenModeDesc{}, nativeWindow, &mSwapchain);

    if (!mSwapchain) {
        return Result::kFailedSwapchain;
    }

    LoadContexts(contexts);

    return Result::kNoError;
#else
    return Reset::kInvalidDeviceType;
#endif
}

GraphicsContext::Result GraphicsContext::InitializeGraphicsD3D12(const Diligent::NativeWindow& nativeWindow,
                                                                 Diligent::VALIDATION_LEVEL    validationLevel) {
#if D3D12_SUPPORTED
    #if ENGINE_DLL
    auto* GetEngineFactoryD3D12 = LoadGraphicsEngineD3D12();
    #endif

    auto* factoryD3D12 = GetEngineFactoryD3D12();
    mFactory           = factoryD3D12;
    mFactory->SetMessageCallback(DiligentDebugCallback);

    if (!factoryD3D12->LoadD3D12()) {
        return Result::kFailedLibrary;
    }

    EngineD3D12CreateInfo engineCI;
    engineCI.GraphicsAPIVersion = {12, 0};
    engineCI.SetValidationLevel(validationLevel);

    SwapChainDesc swapchainDesc;

    std::vector<Diligent::ImmediateContextCreateInfo> contextCIs;
    {
        std::vector<Diligent::GraphicsAdapterInfo> adapters;

        if (Enumerate(engineCI.GraphicsAPIVersion, adapters) == 0) {
            return Result::kNoAdapters;
        }

        ModifyCreateInfo(RENDER_DEVICE_TYPE_D3D12, engineCI, swapchainDesc, adapters, contextCIs);

        engineCI.pImmediateContextInfo = contextCIs.data();
        engineCI.NumImmediateContexts  = Uint32(contextCIs.size());
    }

    mImmediateContextsCount = std::max(1U, engineCI.NumImmediateContexts);
    std::vector<IDeviceContext*> contexts(mImmediateContextsCount + engineCI.NumDeferredContexts);

    factoryD3D12->CreateDeviceAndContextsD3D12(engineCI, &mDevice, contexts.data());

    if (!mDevice) {
        return Result::kFailedRenderDevice;
    }

    // TODO: Initialize FullScreenModeDesc
    factoryD3D12
        ->CreateSwapChainD3D12(mDevice, contexts[0], swapchainDesc, FullScreenModeDesc{}, nativeWindow, &mSwapchain);

    if (!mSwapchain) {
        return Result::kFailedSwapchain;
    }

    LoadContexts(contexts);

    return Result::kNoError;
#else
    return Reset::kInvalidDeviceType;
#endif
}

//
//
//
//
// Modify create info
//
//
//
//

void GraphicsContext::ModifyCreateInfo([[maybe_unused]] RENDER_DEVICE_TYPE      deviceType,
                                       EngineCreateInfo&                        engineCI,
                                       [[maybe_unused]] SwapChainDesc&          swapchainDesc,
                                       std::vector<GraphicsAdapterInfo>&        adapters,
                                       std::vector<ImmediateContextCreateInfo>& contextCIs) {
    (void) deviceType;
    (void) swapchainDesc;

#if PLATFORM_MACOS
    swapchainDesc.BufferCount = 3;
#endif

    // Select adapter with most queues (prefer discrete adapters)
    {
        engineCI.AdapterId = 0;
        Uint32 maxQueues   = 0;
        for (Uint32 adapterId = 0, count = Uint32(adapters.size()); adapterId < count; ++adapterId) {
            auto& adapter = adapters[adapterId];

            if (adapter.NumQueues < maxQueues ||
                (adapter.NumQueues == maxQueues && adapter.Type != ADAPTER_TYPE_DISCRETE)) {
                continue;
            }

            engineCI.AdapterId = adapterId;
            maxQueues          = adapter.NumQueues;
        }
    }

    auto& adapter = adapters[engineCI.AdapterId];
    AddContextCI(contextCIs, adapter, "GraphicsContext", COMMAND_QUEUE_TYPE_GRAPHICS, QUEUE_PRIORITY_HIGH);

    // TODO: Add validation macro

    if (adapter.Type == ADAPTER_TYPE_DISCRETE) { // TODO: add force option
        AddContextCI(contextCIs, adapter, "Transfer", COMMAND_QUEUE_TYPE_TRANSFER, QUEUE_PRIORITY_MEDIUM);
        AddContextCI(contextCIs, adapter, "Compute", COMMAND_QUEUE_TYPE_COMPUTE, QUEUE_PRIORITY_MEDIUM);
    }

    engineCI.Features.NativeFence                   = DEVICE_FEATURE_STATE_OPTIONAL;
    engineCI.Features.TimestampQueries              = DEVICE_FEATURE_STATE_OPTIONAL;
    engineCI.Features.TransferQueueTimestampQueries = DEVICE_FEATURE_STATE_OPTIONAL;
}

} // namespace Sage::Core::Graphics
