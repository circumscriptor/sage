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
/// @file RenderTarget.hpp
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
#include <Texture.h>

// stdlib
#include <array>

namespace Sage::Core::Graphics {

using namespace Diligent;

///
/// @brief Render target
///
///
class RenderTarget {
  public:

    SAGE_CLASS_DELETE_COPY_AND_MOVE(RenderTarget)

    ///
    /// @brief Default clear color for render targets
    ///
    ///
    static constexpr std::array<float, 4> kClearColor = {0.25F, 0.25F, 0.25F, 1.F};

    ///
    /// @brief Construct a new RenderTarget object
    ///
    /// @param device Render device
    /// @param context Device context
    /// @param colorFormat RT color texture format
    /// @param depthFormat RT depth texture format
    ///
    RenderTarget(IRenderDevice*  device,
                 IDeviceContext* context,
                 TEXTURE_FORMAT  colorFormat,
                 TEXTURE_FORMAT  depthFormat) :
        mColorFormat{colorFormat},
        mDepthFormat{depthFormat},
        mDevice{device},
        mContext{context} {}

    ///
    /// @brief Destroy the RenderTarget object
    ///
    ///
    ~RenderTarget() = default;

    ///
    /// @brief Bind render target (sets default viewport)
    ///
    ///
    void Bind() {
        mContext->SetRenderTargets(1, &mColorTexture, mDepthTexture, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
        mContext->SetViewports(1, nullptr, 0, 0);
    }

    ///
    /// @brief Clear render target textures
    ///
    ///
    void Clear() {
        mContext->ClearRenderTarget(mColorTexture, kClearColor.data(), RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
        mContext->ClearDepthStencil(mDepthTexture, CLEAR_DEPTH_FLAG, 1.F, 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
    }

    ///
    /// @brief Resize render target textures
    ///
    /// @param width New width
    /// @param height New height
    ///
    void Resize(UInt32 width, UInt32 height) {
        ResizeColorTexture(width, height);
        ResizeDepthTexture(width, height);
    }

    ///
    /// @brief Get the color texture view as a shader resource
    ///
    /// @return Texture view as a shader resource
    ///
    [[nodiscard]] ITextureView* GetColorResource() noexcept {
        return mColorResource;
    }

    ///
    /// @brief Get the depth texture view as a shader resource
    ///
    /// @return Texture view as a shader resource
    ///
    [[nodiscard]] ITextureView* GetDepthResource() noexcept {
        return mDepthResource;
    }

  protected:

    const TEXTURE_FORMAT mColorFormat;
    const TEXTURE_FORMAT mDepthFormat;

    RefCntAutoPtr<ITextureView>   mColorTexture;
    RefCntAutoPtr<ITextureView>   mDepthTexture;
    RefCntAutoPtr<ITextureView>   mColorResource;
    RefCntAutoPtr<ITextureView>   mDepthResource;
    RefCntAutoPtr<IRenderDevice>  mDevice;
    RefCntAutoPtr<IDeviceContext> mContext;

  private:

    void ResizeColorTexture(UInt32 width, UInt32 height) {
        TextureDesc textureDesc;
        textureDesc.Name      = "RT Color texture";
        textureDesc.Type      = RESOURCE_DIM_TEX_2D;
        textureDesc.Width     = width;
        textureDesc.Height    = height;
        textureDesc.MipLevels = 1;
        textureDesc.Format    = mColorFormat;
        textureDesc.BindFlags = BIND_SHADER_RESOURCE | BIND_RENDER_TARGET;

        RefCntAutoPtr<ITexture> colorTexture;
        mDevice->CreateTexture(textureDesc, nullptr, &colorTexture);
        mColorTexture  = colorTexture->GetDefaultView(TEXTURE_VIEW_RENDER_TARGET);
        mColorResource = colorTexture->GetDefaultView(TEXTURE_VIEW_SHADER_RESOURCE);
    }

    void ResizeDepthTexture(UInt32 width, UInt32 height) {
        TextureDesc textureDesc;
        textureDesc.Name      = "RT Depth texture";
        textureDesc.Type      = RESOURCE_DIM_TEX_2D;
        textureDesc.Width     = width;
        textureDesc.Height    = height;
        textureDesc.MipLevels = 1;
        textureDesc.Format    = mDepthFormat;
        textureDesc.BindFlags = BIND_SHADER_RESOURCE | BIND_DEPTH_STENCIL;

        RefCntAutoPtr<ITexture> depthTexture;
        mDevice->CreateTexture(textureDesc, nullptr, &depthTexture);
        mDepthTexture  = depthTexture->GetDefaultView(TEXTURE_VIEW_DEPTH_STENCIL);
        mDepthResource = depthTexture->GetDefaultView(TEXTURE_VIEW_SHADER_RESOURCE);
    }
};

} // namespace Sage::Core::Graphics
