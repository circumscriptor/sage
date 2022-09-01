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
/// @file ImGuiRenderer.hpp
/// @brief ...
///
/// @copyright Copyright (c) 2022
///
///

#pragma once

#include <Sage/Core/BasicTypes.hpp>
#include <Sage/Core/ClassDefinitions.hpp>
#include <Sage/Core/Graphics/Internal/GraphicsContextImpl.hpp>

// ImGui
#include <imgui/imgui.h>

// Diligent
#include <GraphicsTypes.h>
#include <RefCntAutoPtr.hpp>

namespace Sage::Core::Graphics {

class ImGuiRenderer {
  public:

    SAGE_CLASS_DELETE_COPY_AND_MOVE(ImGuiRenderer)

    ImGuiRenderer(std::shared_ptr<Internal::GraphicsContext> graphics,
                  Diligent::Uint32                           vertexBufferSize,
                  Diligent::Uint32                           indexBufferSize);

    ~ImGuiRenderer() = default;

    void NewFrame() noexcept;

    void RenderDrawData(ImDrawData* drawData);

    [[nodiscard]] UInt32 GetRenderSurfaceWidth() const noexcept {
        return mRenderSurfaceWidth;
    }

    [[nodiscard]] UInt32 GetRenderSurfaceHeight() const noexcept {
        return mRenderSurfaceHeight;
    }

  private:

    struct BufferOffset {
        Diligent::Uint32 vertex;
        Diligent::Uint32 index;
    };

    void CreatePSO();

    void CreateCB();

    void CreateTexture();

    void Create();

    void GrowVertexBuffer(ImDrawData* drawData);

    void GrowIndexBuffer(ImDrawData* drawData);

    void UpdateBuffers(Diligent::IDeviceContext* context, ImDrawData* drawData);

    void UpdateConstants(Diligent::IDeviceContext* context, ImDrawData* drawData);

    void SetupRenderState(Diligent::IDeviceContext* context, ImDrawData* drawData);

    Diligent::ITextureView* RenderCommands(Diligent::IDeviceContext* context,
                                           ImDrawData*               drawData,
                                           const ImDrawList*         cmdList,
                                           const BufferOffset&       globalOffset,
                                           Diligent::ITextureView*   lastTextureView);

    void RenderCommandLists(Diligent::IDeviceContext* context, ImDrawData* drawData);

    std::shared_ptr<Internal::GraphicsContext>                mGraphics;
    Diligent::RefCntAutoPtr<Diligent::IBuffer>                mVertexBuffer;
    Diligent::RefCntAutoPtr<Diligent::IBuffer>                mIndexBuffer;
    Diligent::RefCntAutoPtr<Diligent::IBuffer>                mVertexConstantBuffer;
    Diligent::RefCntAutoPtr<Diligent::IPipelineState>         mPSO;
    Diligent::RefCntAutoPtr<Diligent::ITextureView>           mFontSRV;
    Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> mSRB;
    Diligent::IShaderResourceVariable*                        mTexture{nullptr};

    const Diligent::TEXTURE_FORMAT mBackBufferFormat;
    const Diligent::TEXTURE_FORMAT mDepthBufferFormat;
    Diligent::Uint32               mVertexBufferSize{0};
    Diligent::Uint32               mIndexBufferSize{0};
    Diligent::Uint32               mRenderSurfaceWidth{0};
    Diligent::Uint32               mRenderSurfaceHeight{0};
    Diligent::SURFACE_TRANSFORM    mSurfacePreTransform{Diligent::SURFACE_TRANSFORM_IDENTITY};
    bool                           mBaseVertexSupported{false};
};

} // namespace Sage::Core::Graphics
