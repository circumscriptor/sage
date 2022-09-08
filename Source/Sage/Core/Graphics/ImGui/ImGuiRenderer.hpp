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

#include <DeviceContext.h>
#include <Sage/Core/BasicTypes.hpp>
#include <Sage/Core/ClassDefinitions.hpp>
#include <Sage/Core/Graphics/GraphicsContext.hpp>

// ImGui
#include <imgui/imgui.h>

// Diligent
#include <BasicMath.hpp>
#include <GraphicsTypes.h>
#include <MapHelper.hpp>
#include <RefCntAutoPtr.hpp>

namespace Sage::Core::Graphics {

using namespace Diligent;

class ImGuiRenderer {
  public:

    SAGE_CLASS_DELETE_COPY_AND_MOVE(ImGuiRenderer)

    ImGuiRenderer(TEXTURE_FORMAT colorBufferFormat,
                  TEXTURE_FORMAT depthBufferFormat,
                  bool           baseVertexSupported,
                  UInt32         vertexBufferSize,
                  UInt32         indexBufferSize) :
        mColorBufferFormat{colorBufferFormat},
        mDepthBufferFormat{depthBufferFormat},
        mBaseVertexSupported{baseVertexSupported},
        mVertexBufferSize{vertexBufferSize},
        mIndexBufferSize{indexBufferSize} {
        // Initialize buffers
        Create();
    }

    ~ImGuiRenderer() = default;

    void UpdateIO(ImGuiIO& imGuiIO) const {
        imGuiIO.BackendRendererName = "ImGui Renderer";
        if (mBaseVertexSupported) {
            imGuiIO.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;
        }
    }

    void NewFrame(UInt32 width, UInt32 height, SURFACE_TRANSFORM preTransform) noexcept {
        mRenderSurfaceWidth  = width;
        mRenderSurfaceHeight = height;
        mSurfacePreTransform = preTransform;
    }

    void RenderDrawData(ImDrawData* drawData, IDeviceContext* context) {
        // Avoid rendering when minimized
        if (drawData->DisplaySize.x <= 0.0F || drawData->DisplaySize.y <= 0.0F) {
            return;
        }

        GrowVertexBuffer(drawData);
        GrowIndexBuffer(drawData);
        UpdateBuffers(context, drawData);
        UpdateConstants(context, drawData);
        SetupRenderState(context, drawData);
        RenderCommandLists(context, drawData);
    }

    [[nodiscard]] UInt32 GetRenderSurfaceWidth() const noexcept {
        return mRenderSurfaceWidth;
    }

    [[nodiscard]] UInt32 GetRenderSurfaceHeight() const noexcept {
        return mRenderSurfaceHeight;
    }

    static constexpr float4
    TransformClipRect(SURFACE_TRANSFORM preTransform, const float2& size, const float4& rect) noexcept {
        switch (preTransform) {
            case SURFACE_TRANSFORM_IDENTITY:
                return rect;
            case SURFACE_TRANSFORM_ROTATE_90: {
                float2 rp0{rect.x, rect.y};
                float2 rp1{rect.z, rect.w};
                return float4{size.y - rp1.y, rp0.x, size.y - rp0.y, rp1.x};
            }
            case SURFACE_TRANSFORM_ROTATE_180: {
                float2 rp0{rect.x, rect.y};
                float2 rp1{rect.z, rect.w};
                return float4{size.x - rp1.x, size.y - rp1.y, size.x - rp0.x, size.y - rp0.y};
            }
            case SURFACE_TRANSFORM_ROTATE_270: {
                float2 rp0{rect.x, rect.y};
                float2 rp1{rect.z, rect.w};
                return float4{rp0.y, size.x - rp1.x, rp1.y, size.x - rp0.x};
            }
            // case SURFACE_TRANSFORM_OPTIMAL:
            // case SURFACE_TRANSFORM_HORIZONTAL_MIRROR:
            // case SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_90:
            // case SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_180:
            // case SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_270:
            default:
                return rect;
        }
    }

  private:

    struct BufferOffset {
        UInt32 vertex;
        UInt32 index;
    };

    void CreatePSO();

    void CreateCB();

    void CreateTexture();

    void Create() {
        CreatePSO();
        CreateCB();
        CreateTexture();
    }

    void GrowVertexBuffer(ImDrawData* drawData) {
        if (!mVertexBuffer || int(mVertexBufferSize) < drawData->TotalVtxCount) {
            mVertexBuffer.Release();
            while (int(mVertexBufferSize) < drawData->TotalVtxCount) {
                mVertexBufferSize *= 2;
            }

            BufferDesc bufferDesc{"ImGui vertex buffer",
                                  mVertexBufferSize * sizeof(ImDrawVert),
                                  BIND_VERTEX_BUFFER,
                                  USAGE_DYNAMIC,
                                  CPU_ACCESS_WRITE};

            mDevice->CreateBuffer(bufferDesc, nullptr, &mVertexBuffer);
        }
    }

    void GrowIndexBuffer(ImDrawData* drawData) {
        if (!mIndexBuffer || int(mIndexBufferSize) < drawData->TotalIdxCount) {
            mIndexBuffer.Release();
            while (int(mIndexBufferSize) < drawData->TotalIdxCount) {
                mIndexBufferSize *= 2;
            }

            BufferDesc bufferDesc{"ImGui index buffer",
                                  mIndexBufferSize * sizeof(ImDrawIdx),
                                  BIND_INDEX_BUFFER,
                                  USAGE_DYNAMIC,
                                  CPU_ACCESS_WRITE};

            mDevice->CreateBuffer(bufferDesc, nullptr, &mIndexBuffer);
        }
    }

    void UpdateBuffers(IDeviceContext* context, ImDrawData* drawData) {
        MapHelper<ImDrawVert> vertices{context, mVertexBuffer, MAP_WRITE, MAP_FLAG_DISCARD};
        MapHelper<ImDrawIdx>  indices{context, mIndexBuffer, MAP_WRITE, MAP_FLAG_DISCARD};

        ImDrawVert* vtxDst = vertices;
        ImDrawIdx*  idxDst = indices;

        for (Int32 cmdListID = 0; cmdListID < drawData->CmdListsCount; cmdListID++) {
            const ImDrawList* cmdList = drawData->CmdLists[cmdListID];

            std::memcpy(vtxDst, cmdList->VtxBuffer.Data, cmdList->VtxBuffer.Size * sizeof(ImDrawVert));
            std::memcpy(idxDst, cmdList->IdxBuffer.Data, cmdList->IdxBuffer.Size * sizeof(ImDrawIdx));

            vtxDst += cmdList->VtxBuffer.Size;
            idxDst += cmdList->IdxBuffer.Size;
        }
    }

    void UpdateConstants(IDeviceContext* context, ImDrawData* drawData) {
        float L = drawData->DisplayPos.x;
        float R = drawData->DisplayPos.x + drawData->DisplaySize.x;
        float T = drawData->DisplayPos.y;
        float B = drawData->DisplayPos.y + drawData->DisplaySize.y;

        float4x4 projection{
            {   2.0F / (R - L),              0.0F, 0.0F, 0.0F},
            {             0.0F,    2.0F / (T - B), 0.0F, 0.0F},
            {             0.0F,              0.0F, 0.5F, 0.0F},
            {(R + L) / (L - R), (T + B) / (B - T), 0.5F, 1.0F}
        };

        switch (mSurfacePreTransform) {
            case SURFACE_TRANSFORM_IDENTITY:
                break;
            case SURFACE_TRANSFORM_ROTATE_90:
                projection *= float4x4::RotationZ(-PI_F * 0.5F);
                break;
            case SURFACE_TRANSFORM_ROTATE_180:
                projection *= float4x4::RotationZ(-PI_F * 1.0F);
                break;
            case SURFACE_TRANSFORM_ROTATE_270:
                projection *= float4x4::RotationZ(-PI_F * 1.5F);
                break;

            // case SURFACE_TRANSFORM_OPTIMAL:
            // case SURFACE_TRANSFORM_HORIZONTAL_MIRROR:
            // case SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_90:
            // case SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_180:
            // case SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_270:
            default:
                break;
        }

        // Upload to constant buffer
        {
            MapHelper<float4x4> cbData(context, mVertexConstantBuffer, MAP_WRITE, MAP_FLAG_DISCARD);
            *cbData = projection;
        }
    }

    void SetupRenderState(IDeviceContext* context, ImDrawData* drawData) {
        context->SetVertexBuffers(0,
                                  1,
                                  &mVertexBuffer,
                                  nullptr,
                                  RESOURCE_STATE_TRANSITION_MODE_TRANSITION,
                                  SET_VERTEX_BUFFERS_FLAG_RESET);
        context->SetIndexBuffer(mIndexBuffer, 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
        context->SetPipelineState(mPipeline);

        const std::array<float, 4> blend_factor = {0.F, 0.F, 0.F, 0.F};
        context->SetBlendFactors(blend_factor.data());

        Viewport viewPort;
        viewPort.Width    = static_cast<float>(mRenderSurfaceWidth) * drawData->FramebufferScale.x;
        viewPort.Height   = static_cast<float>(mRenderSurfaceHeight) * drawData->FramebufferScale.y;
        viewPort.MinDepth = 0.0F;
        viewPort.MaxDepth = 1.0F;
        viewPort.TopLeftX = viewPort.TopLeftY = 0;
        context->SetViewports(1,
                              &viewPort,
                              Uint32(float(mRenderSurfaceWidth) * drawData->FramebufferScale.x),
                              Uint32(float(mRenderSurfaceHeight) * drawData->FramebufferScale.y));
    }

    Diligent::ITextureView* RenderCommands(Diligent::IDeviceContext* context,
                                           ImDrawData*               drawData,
                                           const ImDrawList*         cmdList,
                                           const BufferOffset&       globalOffset,
                                           Diligent::ITextureView*   lastTextureView) {
        for (const ImDrawCmd& cmd : cmdList->CmdBuffer) {
            if (cmd.UserCallback != nullptr) {
                // User callback, registered via ImDrawList::AddCallback()
                // (ImDrawCallback_ResetRenderState is a special callback value used by the user to request the
                // renderer to reset render state.)
                if (cmd.UserCallback == ImDrawCallback_ResetRenderState) {
                    SetupRenderState(context, drawData);
                } else {
                    cmd.UserCallback(cmdList, &cmd);
                }
            } else {
                // Apply scissor/clipping rectangle
                float4 clipRect{
                    (cmd.ClipRect.x - drawData->DisplayPos.x) * drawData->FramebufferScale.x,
                    (cmd.ClipRect.y - drawData->DisplayPos.y) * drawData->FramebufferScale.y,
                    (cmd.ClipRect.z - drawData->DisplayPos.x) * drawData->FramebufferScale.x,
                    (cmd.ClipRect.w - drawData->DisplayPos.y) * drawData->FramebufferScale.y //
                };

                // Apply pre-transform
                float2 displaySize{drawData->DisplaySize.x, drawData->DisplaySize.y};
                clipRect                         = TransformClipRect(mSurfacePreTransform, displaySize, clipRect);
                std::array<Rect, 1> scissorRects = {
                    Rect{Int32(clipRect.x), Int32(clipRect.y), Int32(clipRect.z), Int32(clipRect.w)}
                };
                context->SetScissorRects(scissorRects.size(),
                                         scissorRects.data(),
                                         Uint32(float(mRenderSurfaceWidth) * drawData->FramebufferScale.x),
                                         Uint32(float(mRenderSurfaceHeight) * drawData->FramebufferScale.y));
                // Bind texture
                auto* textureView = reinterpret_cast<ITextureView*>(cmd.TextureId);
                VERIFY_EXPR(textureView);

                if (textureView != lastTextureView) {
                    lastTextureView = textureView;
                    mTexture->Set(textureView);
                    context->CommitShaderResources(mSRB, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
                }

                DrawIndexedAttribs drawAttributes{cmd.ElemCount,
                                                  sizeof(ImDrawIdx) == sizeof(Uint16) ? VT_UINT16 : VT_UINT32,
                                                  DRAW_FLAG_VERIFY_STATES};

                drawAttributes.FirstIndexLocation = cmd.IdxOffset + globalOffset.index;
                if (mBaseVertexSupported) {
                    drawAttributes.BaseVertex = cmd.VtxOffset + globalOffset.vertex;
                } else {
                    std::array<IBuffer*, 1> vertexBuffers = {mVertexBuffer};
                    std::array<Uint64, 1>   vtxOffsets    = {sizeof(ImDrawVert) *
                                                             (size_t{cmd.VtxOffset} + size_t{globalOffset.vertex})};

                    context->SetVertexBuffers(0,
                                              vertexBuffers.size(),
                                              vertexBuffers.data(),
                                              vtxOffsets.data(),
                                              RESOURCE_STATE_TRANSITION_MODE_TRANSITION,
                                              SET_VERTEX_BUFFERS_FLAG_NONE);
                }
                context->DrawIndexed(drawAttributes);
            }
        }
        return lastTextureView;
    }

    void RenderCommandLists(Diligent::IDeviceContext* context, ImDrawData* drawData) {
        BufferOffset globalOffset{0, 0};

        ITextureView* lastTextureView = nullptr;
        for (Int32 cmdListID = 0; cmdListID < drawData->CmdListsCount; cmdListID++) {
            const ImDrawList* cmdList = drawData->CmdLists[cmdListID];

            lastTextureView = RenderCommands(context, drawData, cmdList, globalOffset, lastTextureView);

            globalOffset.index += cmdList->IdxBuffer.Size;
            globalOffset.vertex += cmdList->VtxBuffer.Size;
        }
    }

    RefCntAutoPtr<IRenderDevice> mDevice;

    RefCntAutoPtr<IBuffer>                mVertexBuffer;
    RefCntAutoPtr<IBuffer>                mIndexBuffer;
    RefCntAutoPtr<IBuffer>                mVertexConstantBuffer;
    RefCntAutoPtr<IPipelineState>         mPipeline;
    RefCntAutoPtr<ITextureView>           mFontSRV;
    RefCntAutoPtr<IShaderResourceBinding> mSRB;
    IShaderResourceVariable*              mTexture{nullptr};

    const TEXTURE_FORMAT mColorBufferFormat;
    const TEXTURE_FORMAT mDepthBufferFormat;
    UInt32               mVertexBufferSize{0};
    UInt32               mIndexBufferSize{0};
    UInt32               mRenderSurfaceWidth{0};
    UInt32               mRenderSurfaceHeight{0};
    SURFACE_TRANSFORM    mSurfacePreTransform{SURFACE_TRANSFORM_IDENTITY};
    bool                 mBaseVertexSupported{false};
};

} // namespace Sage::Core::Graphics
