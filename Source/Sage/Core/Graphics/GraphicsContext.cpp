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
#include "Internal/Enumerate.hpp"
#include "Internal/GetNativeWindow.hpp"
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
using namespace Internal;

bool GraphicsContext::IsDeviceTypeSupported(RENDER_DEVICE_TYPE deviceType) {
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

GraphicsContext::Result GraphicsContext::InitializeGraphics(const NativeWindow& nativeWindow,
                                                            RENDER_DEVICE_TYPE  deviceType,
                                                            VALIDATION_LEVEL    validationLevel,
                                                            ISwapChain**        swapChain) {
    SwapChainDesc swapchainDesc;

    // Reset if re-initializing engine
    mContexts.clear();
    mDevice  = nullptr;
    mFactory = nullptr;

    // Context pointers storage
    std::vector<IDeviceContext*>            contexts;
    std::vector<ImmediateContextCreateInfo> contextCIs;

    // Start with no error
    Result result = kNoError;

    // Helper lambda
    auto SetupContexts = [&](EngineCreateInfo& engineCI) -> Result {
        std::vector<GraphicsAdapterInfo> adapters;
        engineCI.SetValidationLevel(validationLevel);

        mFactory->SetMessageCallback(DiligentDebugCallback);

        if (Enumerate(mFactory, engineCI.GraphicsAPIVersion, adapters) == 0) {
            return kNoAdapters;
        }

        if (!ModifyCreateInfo(deviceType, engineCI, swapchainDesc, adapters, contextCIs)) {
            return kFailedModifyCI;
        }

        engineCI.pImmediateContextInfo = contextCIs.data();
        engineCI.NumImmediateContexts  = Uint32(contextCIs.size());

        if (deviceType == RENDER_DEVICE_TYPE_GL || deviceType == RENDER_DEVICE_TYPE_GLES) {
            engineCI.NumImmediateContexts = 1;
            engineCI.NumDeferredContexts  = 0;
        }

        mImmediateContextsCount = std::max(1U, engineCI.NumImmediateContexts);
        contexts.resize(mImmediateContextsCount + engineCI.NumDeferredContexts, nullptr);
        return kNoError;
    };

    switch (deviceType) {
#if GL_SUPPORTED || GLES_SUPPORTED
        case RENDER_DEVICE_TYPE_GL:
        case RENDER_DEVICE_TYPE_GLES: {
    #if !GL_SUPPORTED
            if (deviceType == RENDER_DEVICE_TYPE_GL) {
                result = kInvalidDeviceType;
                break;
            }
    #endif

    #if !GLES_SUPPORTED
            if (deviceType == RENDER_DEVICE_TYPE_GLES) {
                result = kInvalidDeviceType;
                break;
            }
    #endif

    #if ENGINE_DLL
            auto* GetEngineFactoryOpenGL = LoadGraphicsEngineOpenGL();
    #endif

            auto* factoryOpenGL = GetEngineFactoryOpenGL();
            mFactory            = factoryOpenGL;

            EngineGLCreateInfo engineCI;
            engineCI.Window = nativeWindow;

    #if GL_SUPPORTED
            engineCI.GraphicsAPIVersion = {4, 0};
            auto deviceType             = RENDER_DEVICE_TYPE_GL;
    #elif GLES_SUPPORTED
            engineCI.GraphicsAPIVersion = {3, 0};
            auto deviceType             = RENDER_DEVICE_TYPE_GLES;
    #endif

            if (result = SetupContexts(engineCI); result != kNoError) {
                break;
            }

            // Create device and swap chain
            factoryOpenGL->CreateDeviceAndSwapChainGL(engineCI, &mDevice, contexts.data(), swapchainDesc, swapChain);

            if (!mDevice) {
                result = kFailedRenderDevice;
                break;
            }

            if (*swapChain == nullptr) {
                result = kFailedSwapchain;
                break;
            }

            result = kNoError;
        } break;
#endif
#if VULKAN_SUPPORTED
        case RENDER_DEVICE_TYPE_VULKAN: {
    #if ENGINE_DLL
            auto* GetEngineFactoryVk = LoadGraphicsEngineVk();
    #endif

            auto* factoryVulkan = GetEngineFactoryVk();
            mFactory            = factoryVulkan;

            EngineVkCreateInfo engineCI;
            engineCI.GraphicsAPIVersion = {1, 0};

            if (result = SetupContexts(engineCI); result != kNoError) {
                break;
            }

            mImmediateContextsCount = std::max(1U, engineCI.NumImmediateContexts);
            contexts.resize(mImmediateContextsCount + engineCI.NumDeferredContexts, nullptr);

            // Create device
            factoryVulkan->CreateDeviceAndContextsVk(engineCI, &mDevice, contexts.data());
            if (!mDevice) {
                result = kFailedRenderDevice;
                break;
            }

            // Create swap chain
            factoryVulkan->CreateSwapChainVk(mDevice, contexts[0], swapchainDesc, nativeWindow, swapChain);
            if (swapChain == nullptr) {
                result = kFailedSwapchain;
                break;
            }

            result = kNoError;
        } break;
#endif
#if D3D11_SUPPORTED
        case RENDER_DEVICE_TYPE_D3D11: {
    #if ENGINE_DLL
            auto* GetEngineFactoryD3D11 = LoadGraphicsEngineD3D11();
    #endif

            auto* factoryD3D11 = GetEngineFactoryD3D11();
            mFactory           = factoryD3D11;

            EngineD3D11CreateInfo engineCI;
            engineCI.GraphicsAPIVersion = {11, 0};

            if (result = SetupContexts(engineCI); result != kNoError) {
                break;
            }

            // Create device
            factoryD3D11->CreateDeviceAndContextsD3D11(engineCI, &mDevice, contexts.data());
            if (!mDevice) {
                result = kFailedRenderDevice;
                break;
            }

            // Create swap chain
            factoryD3D11->CreateSwapChainD3D11(mDevice, contexts[0], swapchainDesc, {}, nativeWindow, swapChain);
            if (*swapChain == nullptr) {
                result = kFailedSwapchain;
                break;
            }

            result = kNoError;
        } break;
#endif
#if D3D12_SUPPORTED
        case RENDER_DEVICE_TYPE_D3D12: {
    #if ENGINE_DLL
            auto* GetEngineFactoryD3D12 = LoadGraphicsEngineD3D12();
    #endif

            auto* factoryD3D12 = GetEngineFactoryD3D12();
            mFactory           = factoryD3D12;

            if (!factoryD3D12->LoadD3D12()) {
                return kFailedLibrary;
            }

            EngineD3D12CreateInfo engineCI;
            engineCI.GraphicsAPIVersion = {12, 0};

            if (result = SetupContexts(engineCI); result != kNoError) {
                break;
            }

            // Create device
            factoryD3D12->CreateDeviceAndContextsD3D12(engineCI, &mDevice, contexts.data());
            if (!mDevice) {
                result = kFailedRenderDevice;
                break;
            }

            factoryD3D12->CreateSwapChainD3D12(mDevice, contexts[0], swapchainDesc, {}, nativeWindow, swapChain);
            if (*swapChain == nullptr) {
                result = kFailedSwapchain;
                break;
            }

            result = kNoError;
        } break;
#endif
        default:
            result = kInvalidDeviceType;
            break;
    }

    switch (result) {
        case kInvalidDeviceType:
            SAGE_LOG_ERROR("Requested device type {} is not valid or not supported on this platform",
                           DeviceTypeToString(deviceType));
            break;
        case kFailedLibrary:
            SAGE_LOG_ERROR("Failed to initialize {} library", DeviceTypeToString(deviceType));
            break;
        case kNoAdapters:
            SAGE_LOG_ERROR("No available {} adapters", DeviceTypeToString(deviceType));
            break;
        case kFailedRenderDevice:
            SAGE_LOG_ERROR("Failed to create {} render device", DeviceTypeToString(deviceType));
            break;
        case kFailedSwapchain:
            SAGE_LOG_ERROR("Failed to create {} swapchain", DeviceTypeToString(deviceType));
            break;
        case kFailedModifyCI:
            SAGE_LOG_ERROR("Failed to modify {} create info", DeviceTypeToString(deviceType));
            break;
        case kNoError: {
            const auto& version = mDevice->GetDeviceInfo().APIVersion;
            SAGE_LOG_INFO("Graphics API: {}", DeviceTypeToString(deviceType));
            SAGE_LOG_INFO("Graphics API version: {}.{}", version.Major, version.Minor);
            SAGE_LOG_INFO("Graphics adapter: {}", mDevice->GetAdapterInfo().Description);

            // Load contexts to smart pointers
            mContexts.clear();
            mContexts.reserve(contexts.size());
            for (auto* context : contexts) {
                mContexts.emplace_back(context);
            }
        } break;
    }
    return result;
}

SwapChain GraphicsContext::CreateSwapChain(const NativeWindow& nativeWindow, UInt32 ctx) {
    if (ctx >= mImmediateContextsCount) {
        throw std::exception("context is not an immediate context");
    }

    RefCntAutoPtr<ISwapChain> swapChain;

    SwapChainDesc swapchainDesc;
    swapchainDesc.IsPrimary = false;
#if PLATFORM_MACOS
    swapchainDesc.BufferCount = 3;
#endif

    Result result = kNoError;

    const auto deviceType = mDevice->GetDeviceInfo().Type;
    switch (deviceType) {
#if VULKAN_SUPPORTED
        case RENDER_DEVICE_TYPE_VULKAN: {
            auto* factoryVulkan = dynamic_cast<IEngineFactoryVk*>(mFactory.RawPtr());
            factoryVulkan->CreateSwapChainVk(mDevice, mContexts[ctx], swapchainDesc, nativeWindow, &swapChain);
            if (!swapChain) {
                result = kFailedSwapchain;
            }
        } break;
#endif
#if D3D11_SUPPORTED
        case RENDER_DEVICE_TYPE_D3D11: {
            auto* factoryD3D11 = dynamic_cast<IEngineFactoryD3D11*>(mFactory.RawPtr());
            factoryD3D11->CreateSwapChainD3D11(mDevice, mContexts[ctx], swapchainDesc, {}, nativeWindow, &swapChain);
            if (!swapChain) {
                result = kFailedSwapchain;
            }
        } break;
#endif
#if D3D12_SUPPORTED
        case RENDER_DEVICE_TYPE_D3D12: {
            auto* factoryD3D12 = dynamic_cast<IEngineFactoryD3D12*>(mFactory.RawPtr());
            factoryD3D12->CreateSwapChainD3D12(mDevice, mContexts[ctx], swapchainDesc, {}, nativeWindow, &swapChain);
            if (!swapChain) {
                result = kFailedSwapchain;
            }
            break;
        } break;
#endif
        default:
            result = kInvalidDeviceType;
            break;
    }

    if (result != kNoError) {
        if (result == kFailedSwapchain) {
            SAGE_LOG_ERROR("Failed to create {} swapchain", DeviceTypeToString(deviceType));
        }
        if (result == kInvalidDeviceType) {
            SAGE_LOG_ERROR("Swap chain creation not supported. API: {}", DeviceTypeToString(deviceType));
        }
        throw std::exception("failed to create swap chain");
    }
    return SwapChain{mContexts[ctx], swapChain};
}

} // namespace Sage::Core::Graphics
