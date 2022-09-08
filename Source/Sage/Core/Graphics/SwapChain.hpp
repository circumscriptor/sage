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

// Diligent
#include <DeviceContext.h>
#include <RefCntAutoPtr.hpp>
#include <RenderDevice.h>
#include <SwapChain.h>

// stdlib
#include <array>

namespace Sage::Core::Graphics {

using namespace Diligent;

class SwapChain {
  public:

    SAGE_CLASS_DELETE_COPY_AND_MOVE(SwapChain)

    ///
    /// @brief Default clear color for swap chain
    ///
    ///
    static constexpr std::array<float, 4> kClearColor{0.F, 0.F, 0.F, 1.F};

    ///
    /// @brief Construct a new SwapChain object
    ///
    /// @param context Device context
    /// @param swapChain Swap chain handle
    ///
    SwapChain(IDeviceContext* context, ISwapChain* swapChain) : mContext{context}, mSwapChain{swapChain} {}

    ///
    /// @brief Destroy the SwapChain object
    ///
    ///
    ~SwapChain() = default;

    ///
    /// @brief Bind and clear current back buffer
    ///
    ///
    void Bind() {
        auto* rtv = mSwapChain->GetCurrentBackBufferRTV();
        auto* dsv = mSwapChain->GetDepthBufferDSV();
        mContext->SetRenderTargets(1, &rtv, dsv, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
        mContext->SetViewports(1, nullptr, 0, 0);
        mContext->ClearRenderTarget(rtv, kClearColor.data(), RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
        mContext->ClearDepthStencil(dsv, CLEAR_DEPTH_FLAG, 1.F, 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
    }

    ///
    /// @brief Resize swap chain
    ///
    /// @param width New width
    /// @param height  New height
    /// @param surfaceTransform New surface transform
    ///
    void Resize(UInt32 width, UInt32 height, SURFACE_TRANSFORM surfaceTransform = SURFACE_TRANSFORM_OPTIMAL) {
        mSwapChain->Resize(width, height, surfaceTransform);
    }

    ///
    /// @brief Present swap chain
    ///
    /// @param syncInterval Synchronization interval
    ///
    void Present(UInt32 syncInterval) {
        mSwapChain->Present(syncInterval);
    }

  private:

    RefCntAutoPtr<IDeviceContext> mContext;
    RefCntAutoPtr<ISwapChain>     mSwapChain;
};

} // namespace Sage::Core::Graphics
