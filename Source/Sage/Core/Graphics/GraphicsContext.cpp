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
#include "Internal/DiligentDebugCallback.hpp"
#include "Internal/GetNativeWindow.hpp"
#include "Internal/GraphicsContextImpl.hpp"
#include "Internal/ModifyCreateInfo.hpp"

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
#include <exception>

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

namespace Internal {

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

bool GraphicsContext::IsDeviceTypeSupported(Diligent::RENDER_DEVICE_TYPE deviceType) {
    switch (deviceType) {
#if GL_SUPPORTED
        case RENDER_DEVICE_TYPE_GL:
#endif
#if GLES_SUPPORTED
        case RENDER_DEVICE_TYPE_GLES:
#endif
#if VULKAN_SUPPORTED
        case RENDER_DEVICE_TYPE_VULKAN:
#endif
#if D3D11_SUPPORTED
        case RENDER_DEVICE_TYPE_D3D11:
#endif
#if D3D12_SUPPORTED
        case RENDER_DEVICE_TYPE_D3D12:
            return true;
#endif
        case RENDER_DEVICE_TYPE_METAL: // Metal is not supported
        default:
            return false;
    }
}

GraphicsContext::GraphicsContext(std::shared_ptr<Console::IVirtualConsole> console,
                                 Console::IVirtualConsole::ContextID       contextID) :
    IGraphicsContext(std::move(console), contextID) {
    if (!InitializeWindow()) {
        throw std::exception("failed to initialize window");
    }
    if (!GetNativeWindow(mWindow, mNativeWindow)) {
        throw std::exception("failed to get native handle");
    }
    if (!InitializeGraphics()) {
        throw std::exception("failed to initialize graphics device");
    }
}

GraphicsContext::GraphicsContext(std::shared_ptr<Console::IVirtualConsole> console,
                                 Console::IVirtualConsole::ContextID       contextID,
                                 std::shared_ptr<GraphicsContext>          base) :
    IGraphicsContext(std::move(console), contextID) {
    if (!InitializeWindow()) {
        throw std::exception("failed to initialize window");
    }
    if (!GetNativeWindow(mWindow, mNativeWindow)) {
        throw std::exception("failed to get native handle");
    }
    if (!InitializeGraphicsFromBase(std::move(base))) {
        throw std::exception("failed to initialize graphics device");
    }
}

GraphicsContext::~GraphicsContext() {
    SDL_DestroyWindow(mWindow);
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
    mSwapchain->Present(CVars().iSyncInterval.GetInt());
}

bool GraphicsContext::InitializeWindow() {
    int    xPos  = SDL_WINDOWPOS_CENTERED;
    int    yPos  = SDL_WINDOWPOS_CENTERED;
    int    xRes  = int(CVars().iResolutionX.GetInt());
    int    yRes  = int(CVars().iResolutionY.GetInt());
    UInt32 flags = SDL_WINDOW_SHOWN;

    switch (static_cast<GraphicsCVars::FullScreenMode>(CVars().eFullScreenMode.GetInt())) {
        case GraphicsCVars::kFullScreen:
            flags |= UInt32(SDL_WINDOW_FULLSCREEN);
        case GraphicsCVars::kWindowed:
            if ((xRes == 0 || yRes == 0)) {
                if (SDL_DisplayMode mode; SDL_GetDesktopDisplayMode(0, &mode) == 0) {
                    xRes = mode.w;
                    yRes = mode.h;
                } else {
                    xRes = SAGE_DEFAULT_WINDOW_WIDTH;
                    yRes = SAGE_DEFAULT_WINDOW_HEIGHT;
                    SAGE_LOG_WARN("Failed to retrieve desktop display mode. Error: {}", SDL_GetError());
                }
            }
            break;
        case GraphicsCVars::kFullScreenDesktop:
            flags |= UInt32(SDL_WINDOW_FULLSCREEN_DESKTOP);
            break;
        case GraphicsCVars::kFullScreenBorderless:
            if (SDL_DisplayMode mode; SDL_GetDesktopDisplayMode(0, &mode) == 0) {
                flags |= UInt32(SDL_WINDOW_BORDERLESS);
                xRes = mode.w;
                yRes = mode.h;
            } else {
                SAGE_LOG_WARN("Cannot set fullscreen borderless mode, falling back to windowed mode");
                if (xRes == 0 || yRes == 0) {
                    xRes = mode.w;
                    yRes = mode.h;
                }
                CVars().eFullScreenMode.SetInt(CVar::IntType(GraphicsCVars::kWindowed));
                CVars().eFullScreenMode.ClearModified();
            }
            break;
        default:
            break;
    }

    mWindow = SDL_CreateWindow(SAGE_WINDOW_TITLE, xPos, yPos, xRes, yRes, flags);
    if (mWindow == nullptr) {
        SAGE_LOG_ERROR("Failed to create SDL window. Error: {}", SDL_GetError());
        return false;
    }

    SDL_GetWindowSize(mWindow, &xRes, &yRes);
    CVars().iResolutionX.SetInt(xRes);
    CVars().iResolutionY.SetInt(yRes);
    CVars().iResolutionX.ClearModified();
    CVars().iResolutionY.ClearModified();
    return true;
}

bool GraphicsContext::InitializeGraphics() {
    auto renderDevice    = CVars().eRenderDevice.GetInt();
    auto validationLevel = CVars().eValidationLevel.GetInt();

    Result result = InitializeGraphics(static_cast<RENDER_DEVICE_TYPE>(renderDevice),
                                       static_cast<VALIDATION_LEVEL>(validationLevel));

    if (result != Result::kNoError && CVars().bRetryRDInit.GetBool()) {
        auto selected = std::find(GraphicsCVars::kRenderDeviceValues.begin(),
                                  GraphicsCVars::kRenderDeviceValues.end(),
                                  renderDevice);

        for (auto renderDeviceValue : GraphicsCVars::kRenderDeviceValues) {
            auto deviceType = static_cast<RENDER_DEVICE_TYPE>(renderDeviceValue);
            if (deviceType == renderDevice) {
                continue;
            }

            SAGE_LOG_WARN("Retrying RD initialization, API: {}", DeviceTypeToString(deviceType));

            result = InitializeGraphics(deviceType, static_cast<VALIDATION_LEVEL>(validationLevel));
            if (result == Result::kNoError) {
                CVars().eRenderDevice.SetInt(renderDeviceValue);
                CVars().eRenderDevice.ClearModified();
                break;
            }
        }
    }
    return result == Result::kNoError;
}

bool GraphicsContext::InitializeGraphicsFromBase(std::shared_ptr<GraphicsContext> base) {
    std::shared_ptr<GraphicsContext> source = std::move(base);

    mFactory                = source->mFactory;
    mDevice                 = source->mDevice;
    mContexts               = source->mContexts;
    mImmediateContextsCount = source->mImmediateContextsCount;

    SwapChainDesc swapchainDesc;
    swapchainDesc.IsPrimary = false;
#if PLATFORM_MACOS
    swapchainDesc.BufferCount = 3;
#endif

    Result result = Result::kNoError;

    const auto& deviceInfo = source->GetDevice()->GetDeviceInfo();
    switch (deviceInfo.Type) {
#if VULKAN_SUPPORTED
        case RENDER_DEVICE_TYPE_VULKAN: {
            auto* factoryVulkan = dynamic_cast<IEngineFactoryVk*>(mFactory.RawPtr());
            factoryVulkan->CreateSwapChainVk(mDevice, mContexts[0], swapchainDesc, mNativeWindow, &mSwapchain);
            if (!mSwapchain) {
                result = Result::kFailedSwapchain;
            }
        } break;
#endif
#if D3D11_SUPPORTED
        case RENDER_DEVICE_TYPE_D3D11: {
            auto* factoryD3D11 = dynamic_cast<IEngineFactoryD3D11*>(mFactory.RawPtr());
            factoryD3D11->CreateSwapChainD3D11(mDevice, mContexts[0], swapchainDesc, {}, mNativeWindow, &mSwapchain);
            if (!mSwapchain) {
                result = Result::kFailedSwapchain;
            }
        } break;
#endif
#if D3D12_SUPPORTED
        case RENDER_DEVICE_TYPE_D3D12: {
            auto* factoryD3D12 = dynamic_cast<IEngineFactoryD3D12*>(mFactory.RawPtr());
            factoryD3D12->CreateSwapChainD3D12(mDevice, mContexts[0], swapchainDesc, {}, mNativeWindow, &mSwapchain);
            if (!mSwapchain) {
                result = Result::kFailedSwapchain;
            }
            break;
        } break;
#endif
        default:
            result = Result::kInvalidDeviceType;
            break;
    }

    if (result == Result::kFailedSwapchain) {
        SAGE_LOG_ERROR("Failed to create {} swapchain", DeviceTypeToString(deviceInfo.Type));
    } else if (result == Result::kInvalidDeviceType) {
        SAGE_LOG_ERROR("Unsupported device type, cannot initialize from {} base", DeviceTypeToString(deviceInfo.Type));
    }
    return result == Result::kNoError;
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

GraphicsContext::Result GraphicsContext::InitializeGraphics(Diligent::RENDER_DEVICE_TYPE deviceType,
                                                            Diligent::VALIDATION_LEVEL   validationLevel) {
    SwapChainDesc swapchainDesc;

    // Reset if re-initializing engine
    mContexts.clear();
    mDevice    = nullptr;
    mSwapchain = nullptr;

    // Context pointers storage
    std::vector<IDeviceContext*> contexts;

    // Start with no error
    Result result = kNoError;

    auto setupContexts = [&](Diligent::EngineCreateInfo&                        engineCI,
                             std::vector<Diligent::ImmediateContextCreateInfo>& contextCIs) {
        std::vector<Diligent::GraphicsAdapterInfo> adapters;

        if (Enumerate(engineCI.GraphicsAPIVersion, adapters) == 0) {
            return false;
        }

        ModifyCreateInfo(deviceType, engineCI, swapchainDesc, adapters, contextCIs);

        engineCI.pImmediateContextInfo = contextCIs.data();
        engineCI.NumImmediateContexts  = Uint32(contextCIs.size());
        return true;
    };

    switch (deviceType) {
#if GL_SUPPORTED || GLES_SUPPORTED
        case RENDER_DEVICE_TYPE_GL:
        case RENDER_DEVICE_TYPE_GLES: {
    #if !GL_SUPPORTED
            if (deviceType == RENDER_DEVICE_TYPE_GL) {
                result = Result::kInvalidDeviceType;
                break;
            }
    #endif

    #if !GLES_SUPPORTED
            if (deviceType == RENDER_DEVICE_TYPE_GLES) {
                result = Result::kInvalidDeviceType;
                break;
            }
    #endif

    #if ENGINE_DLL
            auto* GetEngineFactoryOpenGL = LoadGraphicsEngineOpenGL();
    #endif

            auto* factoryOpenGL = GetEngineFactoryOpenGL();
            mFactory            = factoryOpenGL;
            mFactory->SetMessageCallback(DiligentDebugCallback);

            EngineGLCreateInfo engineCI;
            engineCI.Window = mNativeWindow;

    #if GL_SUPPORTED
            engineCI.GraphicsAPIVersion = {4, 0};
            auto deviceType             = RENDER_DEVICE_TYPE_GL;
    #elif GLES_SUPPORTED
            engineCI.GraphicsAPIVersion = {3, 0};
            auto deviceType             = RENDER_DEVICE_TYPE_GLES;
    #endif

            engineCI.SetValidationLevel(validationLevel);

            std::vector<Diligent::ImmediateContextCreateInfo> contextCIs;
            if (!setupContexts(engineCI, contextCIs)) {
                result = Result::kNoAdapters;
                break;
            }

            mImmediateContextsCount = 1;
            contexts.resize(mImmediateContextsCount + engineCI.NumDeferredContexts, nullptr);
            factoryOpenGL->CreateDeviceAndSwapChainGL(engineCI, &mDevice, contexts.data(), swapchainDesc, &mSwapchain);

            if (!mDevice) {
                result = Result::kFailedRenderDevice;
                break;
            }

            if (!mSwapchain) {
                result = Result::kFailedSwapchain;
                break;
            }

            result = Result::kNoError;
        } break;
#endif
#if VULKAN_SUPPORTED
        case RENDER_DEVICE_TYPE_VULKAN: {
    #if ENGINE_DLL
            auto* GetEngineFactoryVk = LoadGraphicsEngineVk();
    #endif

            auto* factoryVulkan = GetEngineFactoryVk();
            mFactory            = factoryVulkan;
            mFactory->SetMessageCallback(DiligentDebugCallback);

            EngineVkCreateInfo engineCI;
            engineCI.GraphicsAPIVersion = {1, 0};
            engineCI.SetValidationLevel(validationLevel);

            std::vector<Diligent::ImmediateContextCreateInfo> contextCIs;
            if (!setupContexts(engineCI, contextCIs)) {
                result = Result::kNoAdapters;
                break;
            }

            mImmediateContextsCount = std::max(1U, engineCI.NumImmediateContexts);
            contexts.resize(mImmediateContextsCount + engineCI.NumDeferredContexts, nullptr);

            if (factoryVulkan->CreateDeviceAndContextsVk(engineCI, &mDevice, contexts.data()); !mDevice) {
                result = Result::kFailedRenderDevice;
                break;
            }

            if (factoryVulkan->CreateSwapChainVk(mDevice, contexts[0], swapchainDesc, mNativeWindow, &mSwapchain);
                !mSwapchain) {
                result = Result::kFailedSwapchain;
                break;
            }

            result = Result::kNoError;
        } break;
#endif
#if D3D11_SUPPORTED
        case RENDER_DEVICE_TYPE_D3D11: {
    #if ENGINE_DLL
            auto* GetEngineFactoryD3D11 = LoadGraphicsEngineD3D11();
    #endif

            auto* factoryD3D11 = GetEngineFactoryD3D11();
            mFactory           = factoryD3D11;
            mFactory->SetMessageCallback(DiligentDebugCallback);

            EngineD3D11CreateInfo engineCI;
            engineCI.GraphicsAPIVersion = {11, 0};
            engineCI.SetValidationLevel(validationLevel);

            std::vector<Diligent::ImmediateContextCreateInfo> contextCIs;
            if (!setupContexts(engineCI, contextCIs)) {
                result = Result::kNoAdapters;
                break;
            }

            mImmediateContextsCount = std::max(1U, engineCI.NumImmediateContexts);
            contexts.resize(mImmediateContextsCount + engineCI.NumDeferredContexts, nullptr);

            if (factoryD3D11->CreateDeviceAndContextsD3D11(engineCI, &mDevice, contexts.data()); !mDevice) {
                result = Result::kFailedRenderDevice;
                break;
            }

            // TODO: Initialize FullScreenModeDesc
            if (factoryD3D11->CreateSwapChainD3D11(mDevice,
                                                   contexts[0],
                                                   swapchainDesc,
                                                   FullScreenModeDesc{},
                                                   mNativeWindow,
                                                   &mSwapchain);
                !mSwapchain) {
                result = Result::kFailedSwapchain;
                break;
            }

            result = Result::kNoError;
        } break;
#endif
#if D3D12_SUPPORTED
        case RENDER_DEVICE_TYPE_D3D12: {
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

            std::vector<Diligent::ImmediateContextCreateInfo> contextCIs;
            if (!setupContexts(engineCI, contextCIs)) {
                result = Result::kNoAdapters;
                break;
            }

            mImmediateContextsCount = std::max(1U, engineCI.NumImmediateContexts);
            contexts.resize(mImmediateContextsCount + engineCI.NumDeferredContexts, nullptr);

            if (factoryD3D12->CreateDeviceAndContextsD3D12(engineCI, &mDevice, contexts.data()); !mDevice) {
                result = Result::kFailedRenderDevice;
                break;
            }

            // TODO: Initialize FullScreenModeDesc
            if (factoryD3D12->CreateSwapChainD3D12(mDevice,
                                                   contexts[0],
                                                   swapchainDesc,
                                                   FullScreenModeDesc{},
                                                   mNativeWindow,
                                                   &mSwapchain);
                !mSwapchain) {
                result = Result::kFailedSwapchain;
                break;
            }

            result = Result::kNoError;
            break;
        } break;
#endif
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
            LoadContexts(contexts);
        } break;
    }
    return result;
}

void GraphicsContext::LoadContexts(const std::vector<Diligent::IDeviceContext*>& contexts) {
    // Apply reference counters to contexts
    mContexts.clear();
    mContexts.reserve(contexts.size());
    for (auto* context : contexts) {
        mContexts.emplace_back(context);
    }
}

} // namespace Internal

std::shared_ptr<IGraphicsContext> IGraphicsContext::CreateInstance(std::shared_ptr<Console::IVirtualConsole> console,
                                                                   Console::IVirtualConsole::ContextID       contextID,
                                                                   std::shared_ptr<IGraphicsContext>         base) {
    if (base) {
        return std::make_shared<Internal::GraphicsContext>(
            console,
            contextID,
            std::dynamic_pointer_cast<Internal::GraphicsContext>(std::move(base)));
    }
    return std::make_shared<Internal::GraphicsContext>(console, contextID);
}

} // namespace Sage::Core::Graphics
