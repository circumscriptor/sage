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
/// @file RenderView.hpp
/// @brief ...
///
/// @copyright Copyright (c) 2022
///
///

#pragma once

#include <Sage/Core/Graphics/RenderTarget.hpp>

// Diligent
#include <PipelineState.h>

namespace Sage::Core::Graphics {

using namespace Diligent;

///
/// @brief Render target for rendering to Window region
///
///
class RenderView : public RenderTarget {
  public:

    SAGE_CLASS_DELETE_COPY_AND_MOVE(RenderView)

    ///
    /// @brief Construct a new RenderView object
    ///
    /// @param device Render device
    /// @param context Device context
    /// @param colorFormat RT color texture format
    /// @param depthFormat RT depth texture format
    /// @param pipeline Pipeline used to render the render target
    ///
    RenderView(IRenderDevice*  device,
               IDeviceContext* context,
               TEXTURE_FORMAT  colorFormat,
               TEXTURE_FORMAT  depthFormat,
               IPipelineState* pipeline) :
        RenderTarget(device, context, colorFormat, depthFormat),
        mPipeline{pipeline} {}

    ///
    /// @brief Destroy the RenderView object
    ///
    ///
    ~RenderView() = default;

    // Hide RenderTarget::Resize
    void Resize(UInt32, UInt32) = delete;

    ///
    /// @brief Resize render target and create resource binding
    ///
    /// @param viewport New viewport
    /// @param variableName Variable name of the render target binding in pixel shader
    ///
    void Resize(const Viewport& viewport, const char* variableName) {
        mViewport = viewport;
        RenderTarget::Resize(UInt32(mViewport.Width), UInt32(mViewport.Height));

        mResources.Release();
        mPipeline->CreateShaderResourceBinding(&mResources, true);
        mResources->GetVariableByName(SHADER_TYPE_PIXEL, variableName)->Set(mColorResource);
    }

    ///
    /// @brief Set viewport and commit resources
    ///
    ///
    void Commit() {
        mContext->SetViewports(1, &mViewport, 0, 0);
        mContext->SetPipelineState(mPipeline);
        mContext->CommitShaderResources(mResources, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
    }

  private:

    RefCntAutoPtr<IPipelineState>         mPipeline;
    RefCntAutoPtr<IShaderResourceBinding> mResources;
    Viewport                              mViewport;
};

} // namespace Sage::Core::Graphics
