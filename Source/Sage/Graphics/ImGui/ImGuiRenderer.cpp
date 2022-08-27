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
/// @file ImGuiRenderer.cpp
/// @brief ...
///
/// @copyright Copyright (c) 2022
///
///

#include "ImGuiRenderer.hpp"

#include <BasicMath.hpp>
#include <MapHelper.hpp>
#include <array>

static const char* VertexShaderHLSL = R"(
cbuffer Constants
{
    float4x4 ProjectionMatrix;
}
struct VSInput
{
    float2 pos : ATTRIB0;
    float2 uv  : ATTRIB1;
    float4 col : ATTRIB2;
};
struct PSInput
{
    float4 pos : SV_POSITION;
    float4 col : COLOR;
    float2 uv  : TEXCOORD;
};
void main(in VSInput VSIn, out PSInput PSIn)
{
    PSIn.pos = mul(ProjectionMatrix, float4(VSIn.pos.xy, 0.0, 1.0));
    PSIn.col = VSIn.col;
    PSIn.uv  = VSIn.uv;
}
)";

static const char* PixelShaderHLSL = R"(
struct PSInput
{
    float4 pos : SV_POSITION;
    float4 col : COLOR;
    float2 uv  : TEXCOORD;
};
Texture2D    Texture;
SamplerState Texture_sampler;
float4 main(in PSInput PSIn) : SV_Target
{
    return PSIn.col * Texture.Sample(Texture_sampler, PSIn.uv);
}
)";

static const char* VertexShaderGLSL = R"(
#ifdef VULKAN
#   define BINDING(X) layout(binding=X)
#   define OUT_LOCATION(X) layout(location=X) // Requires separable programs
#else
#   define BINDING(X)
#   define OUT_LOCATION(X)
#endif
BINDING(0) uniform Constants
{
    mat4 ProjectionMatrix;
};
layout(location = 0) in vec2 in_pos;
layout(location = 1) in vec2 in_uv;
layout(location = 2) in vec4 in_col;
OUT_LOCATION(0) out vec4 vsout_col;
OUT_LOCATION(1) out vec2 vsout_uv;
#ifndef GL_ES
out gl_PerVertex
{
    vec4 gl_Position;
};
#endif
void main()
{
    gl_Position = ProjectionMatrix * vec4(in_pos.xy, 0.0, 1.0);
    vsout_col = in_col;
    vsout_uv  = in_uv;
}
)";

static const char* PixelShaderGLSL = R"(
#ifdef VULKAN
#   define BINDING(X) layout(binding=X)
#   define IN_LOCATION(X) layout(location=X) // Requires separable programs
#else
#   define BINDING(X)
#   define IN_LOCATION(X)
#endif
BINDING(0) uniform sampler2D Texture;
IN_LOCATION(0) in vec4 vsout_col;
IN_LOCATION(1) in vec2 vsout_uv;
layout(location = 0) out vec4 psout_col;
void main()
{
    psout_col = vsout_col * texture(Texture, vsout_uv);
}
)";

// clang-format off

// glslangValidator.exe -V -e main --vn VertexShader_SPIRV ImGUI.vert

static constexpr uint32_t VertexShader_SPIRV[] =
{
    0x07230203,0x00010000,0x0008000a,0x00000028,0x00000000,0x00020011,0x00000001,0x0006000b,
	0x00000001,0x4c534c47,0x6474732e,0x3035342e,0x00000000,0x0003000e,0x00000000,0x00000001,
	0x000b000f,0x00000000,0x00000004,0x6e69616d,0x00000000,0x0000000a,0x00000016,0x00000020,
	0x00000022,0x00000025,0x00000026,0x00030003,0x00000002,0x000001a4,0x00040005,0x00000004,
	0x6e69616d,0x00000000,0x00060005,0x00000008,0x505f6c67,0x65567265,0x78657472,0x00000000,
	0x00060006,0x00000008,0x00000000,0x505f6c67,0x7469736f,0x006e6f69,0x00030005,0x0000000a,
	0x00000000,0x00050005,0x0000000e,0x736e6f43,0x746e6174,0x00000073,0x00080006,0x0000000e,
	0x00000000,0x6a6f7250,0x69746365,0x614d6e6f,0x78697274,0x00000000,0x00030005,0x00000010,
	0x00000000,0x00040005,0x00000016,0x705f6e69,0x0000736f,0x00050005,0x00000020,0x756f7376,
	0x6f635f74,0x0000006c,0x00040005,0x00000022,0x635f6e69,0x00006c6f,0x00050005,0x00000025,
	0x756f7376,0x76755f74,0x00000000,0x00040005,0x00000026,0x755f6e69,0x00000076,0x00050048,
	0x00000008,0x00000000,0x0000000b,0x00000000,0x00030047,0x00000008,0x00000002,0x00040048,
	0x0000000e,0x00000000,0x00000005,0x00050048,0x0000000e,0x00000000,0x00000023,0x00000000,
	0x00050048,0x0000000e,0x00000000,0x00000007,0x00000010,0x00030047,0x0000000e,0x00000002,
	0x00040047,0x00000010,0x00000022,0x00000000,0x00040047,0x00000010,0x00000021,0x00000000,
	0x00040047,0x00000016,0x0000001e,0x00000000,0x00040047,0x00000020,0x0000001e,0x00000000,
	0x00040047,0x00000022,0x0000001e,0x00000002,0x00040047,0x00000025,0x0000001e,0x00000001,
	0x00040047,0x00000026,0x0000001e,0x00000001,0x00020013,0x00000002,0x00030021,0x00000003,
	0x00000002,0x00030016,0x00000006,0x00000020,0x00040017,0x00000007,0x00000006,0x00000004,
	0x0003001e,0x00000008,0x00000007,0x00040020,0x00000009,0x00000003,0x00000008,0x0004003b,
	0x00000009,0x0000000a,0x00000003,0x00040015,0x0000000b,0x00000020,0x00000001,0x0004002b,
	0x0000000b,0x0000000c,0x00000000,0x00040018,0x0000000d,0x00000007,0x00000004,0x0003001e,
	0x0000000e,0x0000000d,0x00040020,0x0000000f,0x00000002,0x0000000e,0x0004003b,0x0000000f,
	0x00000010,0x00000002,0x00040020,0x00000011,0x00000002,0x0000000d,0x00040017,0x00000014,
	0x00000006,0x00000002,0x00040020,0x00000015,0x00000001,0x00000014,0x0004003b,0x00000015,
	0x00000016,0x00000001,0x0004002b,0x00000006,0x00000018,0x00000000,0x0004002b,0x00000006,
	0x00000019,0x3f800000,0x00040020,0x0000001e,0x00000003,0x00000007,0x0004003b,0x0000001e,
	0x00000020,0x00000003,0x00040020,0x00000021,0x00000001,0x00000007,0x0004003b,0x00000021,
	0x00000022,0x00000001,0x00040020,0x00000024,0x00000003,0x00000014,0x0004003b,0x00000024,
	0x00000025,0x00000003,0x0004003b,0x00000015,0x00000026,0x00000001,0x00050036,0x00000002,
	0x00000004,0x00000000,0x00000003,0x000200f8,0x00000005,0x00050041,0x00000011,0x00000012,
	0x00000010,0x0000000c,0x0004003d,0x0000000d,0x00000013,0x00000012,0x0004003d,0x00000014,
	0x00000017,0x00000016,0x00050051,0x00000006,0x0000001a,0x00000017,0x00000000,0x00050051,
	0x00000006,0x0000001b,0x00000017,0x00000001,0x00070050,0x00000007,0x0000001c,0x0000001a,
	0x0000001b,0x00000018,0x00000019,0x00050091,0x00000007,0x0000001d,0x00000013,0x0000001c,
	0x00050041,0x0000001e,0x0000001f,0x0000000a,0x0000000c,0x0003003e,0x0000001f,0x0000001d,
	0x0004003d,0x00000007,0x00000023,0x00000022,0x0003003e,0x00000020,0x00000023,0x0004003d,
	0x00000014,0x00000027,0x00000026,0x0003003e,0x00000025,0x00000027,0x000100fd,0x00010038
};

static constexpr uint32_t FragmentShader_SPIRV[] =
{
	0x07230203,0x00010000,0x0008000a,0x00000018,0x00000000,0x00020011,0x00000001,0x0006000b,
	0x00000001,0x4c534c47,0x6474732e,0x3035342e,0x00000000,0x0003000e,0x00000000,0x00000001,
	0x0008000f,0x00000004,0x00000004,0x6e69616d,0x00000000,0x00000009,0x0000000b,0x00000014,
	0x00030010,0x00000004,0x00000007,0x00030003,0x00000002,0x000001a4,0x00040005,0x00000004,
	0x6e69616d,0x00000000,0x00050005,0x00000009,0x756f7370,0x6f635f74,0x0000006c,0x00050005,
	0x0000000b,0x756f7376,0x6f635f74,0x0000006c,0x00040005,0x00000010,0x74786554,0x00657275,
	0x00050005,0x00000014,0x756f7376,0x76755f74,0x00000000,0x00040047,0x00000009,0x0000001e,
	0x00000000,0x00040047,0x0000000b,0x0000001e,0x00000000,0x00040047,0x00000010,0x00000022,
	0x00000000,0x00040047,0x00000010,0x00000021,0x00000000,0x00040047,0x00000014,0x0000001e,
	0x00000001,0x00020013,0x00000002,0x00030021,0x00000003,0x00000002,0x00030016,0x00000006,
	0x00000020,0x00040017,0x00000007,0x00000006,0x00000004,0x00040020,0x00000008,0x00000003,
	0x00000007,0x0004003b,0x00000008,0x00000009,0x00000003,0x00040020,0x0000000a,0x00000001,
	0x00000007,0x0004003b,0x0000000a,0x0000000b,0x00000001,0x00090019,0x0000000d,0x00000006,
	0x00000001,0x00000000,0x00000000,0x00000000,0x00000001,0x00000000,0x0003001b,0x0000000e,
	0x0000000d,0x00040020,0x0000000f,0x00000000,0x0000000e,0x0004003b,0x0000000f,0x00000010,
	0x00000000,0x00040017,0x00000012,0x00000006,0x00000002,0x00040020,0x00000013,0x00000001,
	0x00000012,0x0004003b,0x00000013,0x00000014,0x00000001,0x00050036,0x00000002,0x00000004,
	0x00000000,0x00000003,0x000200f8,0x00000005,0x0004003d,0x00000007,0x0000000c,0x0000000b,
	0x0004003d,0x0000000e,0x00000011,0x00000010,0x0004003d,0x00000012,0x00000015,0x00000014,
	0x00050057,0x00000007,0x00000016,0x00000011,0x00000015,0x00050085,0x00000007,0x00000017,
	0x0000000c,0x00000016,0x0003003e,0x00000009,0x00000017,0x000100fd,0x00010038
};
// clang-format on

static const char* ShadersMSL = R"(
#include <metal_stdlib>
#include <simd/simd.h>
using namespace metal;
struct VSConstants
{
    float4x4 ProjectionMatrix;
};
struct VSIn
{
    float2 pos [[attribute(0)]];
    float2 uv  [[attribute(1)]];
    float4 col [[attribute(2)]];
};
struct VSOut
{
    float4 col [[user(locn0)]];
    float2 uv  [[user(locn1)]];
    float4 pos [[position]];
};
vertex VSOut vs_main(VSIn in [[stage_in]], constant VSConstants& Constants [[buffer(0)]])
{
    VSOut out = {};
    out.pos = Constants.ProjectionMatrix * float4(in.pos, 0.0, 1.0);
    out.col = in.col;
    out.uv  = in.uv;
    return out;
}
struct PSOut
{
    float4 col [[color(0)]];
};
fragment PSOut ps_main(VSOut in [[stage_in]],
                       texture2d<float> Texture [[texture(0)]],
                       sampler Texture_sampler  [[sampler(0)]])
{
    PSOut out = {};
    out.col = in.col * Texture.sample(Texture_sampler, in.uv);
    return out;
}
)";

using namespace Diligent;

static float4 TransformClipRect(SURFACE_TRANSFORM surfacePreTransform, const ImVec2& DisplaySize, const float4& rect) {
    switch (surfacePreTransform) {
        case SURFACE_TRANSFORM_IDENTITY:
            return rect;

        case SURFACE_TRANSFORM_ROTATE_90: {
            // The image content is rotated 90 degrees clockwise. The origin is in the left-top corner.
            //
            //                                                             DsplSz.y
            //                a.x                                            -a.y     a.y     Old origin
            //              0---->|                                       0------->|<------| /
            //           0__|_____|____________________                0__|________|_______|/
            //            | |     '                    |                | |        '       |
            //        a.y | |     '                    |            a.x | |        '       |
            //           _V_|_ _ _a____b               |               _V_|_ _d'___a'      |
            //            A |     |    |               |                  |   |    |       |
            //  DsplSz.y  | |     |____|               |                  |   |____|       |
            //    -a.y    | |     d    c               |                  |   c'   b'      |
            //           _|_|__________________________|                  |                |
            //              A                                             |                |
            //              |-----> Y'                                    |                |
            //         New Origin                                         |________________|
            //
            float2 a{rect.x, rect.y};
            float2 c{rect.z, rect.w};
            return float4 //
                {
                    DisplaySize.y - c.y, // min_x = c'.x
                    a.x,                 // min_y = a'.y
                    DisplaySize.y - a.y, // max_x = a'.x
                    c.x                  // max_y = c'.y
                };
        }

        case SURFACE_TRANSFORM_ROTATE_180: {
            // The image content is rotated 180 degrees clockwise. The origin is in the left-top corner.
            //
            //                a.x                                               DsplSz.x - a.x
            //              0---->|                                         0------------------>|
            //           0__|_____|____________________                 0_ _|___________________|______
            //            | |     '                    |                  | |                   '      |
            //        a.y | |     '                    |        DsplSz.y  | |              c'___d'     |
            //           _V_|_ _ _a____b               |          -a.y    | |              |    |      |
            //              |     |    |               |                 _V_|_ _ _ _ _ _ _ |____|      |
            //              |     |____|               |                    |              b'   a'     |
            //              |     d    c               |                    |                          |
            //              |__________________________|                    |__________________________|
            //                                         A                                               A
            //                                         |                                               |
            //                                     New Origin                                      Old Origin
            float2 a{rect.x, rect.y};
            float2 c{rect.z, rect.w};
            return float4 //
                {
                    DisplaySize.x - c.x, // min_x = c'.x
                    DisplaySize.y - c.y, // min_y = c'.y
                    DisplaySize.x - a.x, // max_x = a'.x
                    DisplaySize.y - a.y  // max_y = a'.y
                };
        }

        case SURFACE_TRANSFORM_ROTATE_270: {
            // The image content is rotated 270 degrees clockwise. The origin is in the left-top corner.
            //
            //              0  a.x     DsplSz.x-a.x   New Origin              a.y
            //              |---->|<-------------------|                    0----->|
            //          0_ _|_____|____________________V                 0 _|______|_________
            //            | |     '                    |                  | |      '         |
            //            | |     '                    |                  | |      '         |
            //        a.y_V_|_ _ _a____b               |        DsplSz.x  | |      '         |
            //              |     |    |               |          -a.x    | |      '         |
            //              |     |____|               |                  | |      b'___c'   |
            //              |     d    c               |                  | |      |    |    |
            //  DsplSz.y _ _|__________________________|                 _V_|_ _ _ |____|    |
            //                                                              |      a'   d'   |
            //                                                              |                |
            //                                                              |________________|
            //                                                              A
            //                                                              |
            //                                                            Old origin
            float2 a{rect.x, rect.y};
            float2 c{rect.z, rect.w};
            return float4 //
                {
                    a.y,                 // min_x = a'.x
                    DisplaySize.x - c.x, // min_y = c'.y
                    c.y,                 // max_x = c'.x
                    DisplaySize.x - a.x  // max_y = a'.y
                };
        }

        case SURFACE_TRANSFORM_OPTIMAL:
            UNEXPECTED("SURFACE_TRANSFORM_OPTIMAL is only valid as parameter during swap chain initialization.");
            return rect;

        case SURFACE_TRANSFORM_HORIZONTAL_MIRROR:
        case SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_90:
        case SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_180:
        case SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_270:
            UNEXPECTED("Mirror transforms are not supported");
            return rect;

        default:
            UNEXPECTED("Unknown transform");
            return rect;
    }
}

namespace Sage::Graphics {

ImGuiRenderer::ImGuiRenderer(IRenderDevice* device,
                             TEXTURE_FORMAT backBufferFormat,
                             TEXTURE_FORMAT depthBufferFormat,
                             Uint32         initialVertexBufferSize,
                             Uint32         initialIndexBufferSize) :
    mDevice{device},
    mBackBufferFormat{backBufferFormat},
    mDepthBufferFormat{depthBufferFormat},
    mBaseVertexSupported{bool(device->GetAdapterInfo().DrawCommand.CapFlags & DRAW_COMMAND_CAP_FLAG_BASE_VERTEX)},
    mVertexBufferSize{initialVertexBufferSize},
    mIndexBufferSize{initialIndexBufferSize} {
    // Set ImGui flags
    IMGUI_CHECKVERSION();
    ImGuiIO& imIO = ImGui::GetIO();

    imIO.BackendRendererName = "ImGuiRenderer (Diligent)";
    if (mBaseVertexSupported) {
        imIO.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;
    }

    // Initialize buffers
    Create();
}

void ImGuiRenderer::GrowVertexBuffer(ImDrawData* drawData) {
    if (!mVertexBuffer || int(mVertexBufferSize) < drawData->TotalVtxCount) {
        mVertexBuffer.Release();
        while (int(mVertexBufferSize) < drawData->TotalVtxCount) {
            mVertexBufferSize *= 2;
        }

        BufferDesc bufferDesc{"Imgui vertex buffer",
                              mVertexBufferSize * sizeof(ImDrawVert),
                              BIND_VERTEX_BUFFER,
                              USAGE_DYNAMIC,
                              CPU_ACCESS_WRITE};

        mDevice->CreateBuffer(bufferDesc, nullptr, &mVertexBuffer);
    }
}

void ImGuiRenderer::GrowIndexBuffer(ImDrawData* drawData) {
    if (!mIndexBuffer || int(mIndexBufferSize) < drawData->TotalIdxCount) {
        mIndexBuffer.Release();
        while (int(mIndexBufferSize) < drawData->TotalIdxCount) {
            mIndexBufferSize *= 2;
        }

        BufferDesc bufferDesc{"Imgui index buffer",
                              mIndexBufferSize * sizeof(ImDrawIdx),
                              BIND_INDEX_BUFFER,
                              USAGE_DYNAMIC,
                              CPU_ACCESS_WRITE};

        mDevice->CreateBuffer(bufferDesc, nullptr, &mIndexBuffer);
    }
}

void ImGuiRenderer::UpdateBuffers(IDeviceContext* context, ImDrawData* drawData) {
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

void ImGuiRenderer::UpdateConstants(IDeviceContext* context, ImDrawData* drawData) {
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

    // Bake pre-transform into projection
    switch (mSurfacePreTransform) {
        case SURFACE_TRANSFORM_IDENTITY:
            // Nothing to do
            break;

        case SURFACE_TRANSFORM_ROTATE_90:
            // The image content is rotated 90 degrees clockwise.
            projection *= float4x4::RotationZ(-PI_F * 0.5F);
            break;

        case SURFACE_TRANSFORM_ROTATE_180:
            // The image content is rotated 180 degrees clockwise.
            projection *= float4x4::RotationZ(-PI_F * 1.0F);
            break;

        case SURFACE_TRANSFORM_ROTATE_270:
            // The image content is rotated 270 degrees clockwise.
            projection *= float4x4::RotationZ(-PI_F * 1.5F);
            break;

        case SURFACE_TRANSFORM_OPTIMAL:
            UNEXPECTED("SURFACE_TRANSFORM_OPTIMAL is only valid as parameter during swap chain initialization.");
            break;

        case SURFACE_TRANSFORM_HORIZONTAL_MIRROR:
        case SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_90:
        case SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_180:
        case SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_270:
            UNEXPECTED("Mirror transforms are not supported");
            break;

        default:
            UNEXPECTED("Unknown transform");
    }

    // Upload to constant buffer
    {
        MapHelper<float4x4> cbData(context, mVertexConstantBuffer, MAP_WRITE, MAP_FLAG_DISCARD);
        *cbData = projection;
    }
}

void ImGuiRenderer::SetupRenderState(IDeviceContext* context, ImDrawData* drawData) {
    std::array<IBuffer*, 1> vertexBuffers = {mVertexBuffer};
    context->SetVertexBuffers(0,
                              vertexBuffers.size(),
                              vertexBuffers.data(),
                              nullptr,
                              RESOURCE_STATE_TRANSITION_MODE_TRANSITION,
                              SET_VERTEX_BUFFERS_FLAG_RESET);
    context->SetIndexBuffer(mIndexBuffer, 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
    context->SetPipelineState(mPSO);

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

ITextureView* ImGuiRenderer::RenderCommands(IDeviceContext*     context,
                                            ImDrawData*         drawData,
                                            const ImDrawList*   cmdList,
                                            const BufferOffset& globalOffset,
                                            ITextureView*       lastTextureView) {
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
            clipRect                         = TransformClipRect(mSurfacePreTransform, drawData->DisplaySize, clipRect);
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

void ImGuiRenderer::RenderCommandLists(IDeviceContext* context, ImDrawData* drawData) {
    BufferOffset globalOffset{0, 0};

    ITextureView* lastTextureView = nullptr;
    for (Int32 cmdListID = 0; cmdListID < drawData->CmdListsCount; cmdListID++) {
        const ImDrawList* cmdList = drawData->CmdLists[cmdListID];

        lastTextureView = RenderCommands(context, drawData, cmdList, globalOffset, lastTextureView);

        globalOffset.index += cmdList->IdxBuffer.Size;
        globalOffset.vertex += cmdList->VtxBuffer.Size;
    }
}

void ImGuiRenderer::RenderDrawData(IDeviceContext* context, ImDrawData* drawData) {
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

void ImGuiRenderer::CreatePSO() {
    ShaderCreateInfo shaderCI;
    shaderCI.UseCombinedTextureSamplers = true;
    shaderCI.SourceLanguage             = SHADER_SOURCE_LANGUAGE_DEFAULT;

    const auto DeviceType = mDevice->GetDeviceInfo().Type;

    RefCntAutoPtr<IShader> vertexShader;
    {
        shaderCI.Desc.ShaderType = SHADER_TYPE_VERTEX;
        shaderCI.Desc.Name       = "Imgui VS";
        switch (DeviceType) {
            case RENDER_DEVICE_TYPE_VULKAN:
                shaderCI.ByteCode     = VertexShader_SPIRV;
                shaderCI.ByteCodeSize = sizeof(VertexShader_SPIRV);
                break;

            case RENDER_DEVICE_TYPE_D3D11:
            case RENDER_DEVICE_TYPE_D3D12:
                shaderCI.Source = VertexShaderHLSL;
                break;

            case RENDER_DEVICE_TYPE_GL:
            case RENDER_DEVICE_TYPE_GLES:
                shaderCI.Source = VertexShaderGLSL;
                break;

            case RENDER_DEVICE_TYPE_METAL:
                shaderCI.Source     = ShadersMSL;
                shaderCI.EntryPoint = "vs_main";
                break;

            default:
                UNEXPECTED("Unknown render device type");
        }
        mDevice->CreateShader(shaderCI, &vertexShader);
    }

    RefCntAutoPtr<IShader> pixelShader;
    {
        shaderCI.Desc.ShaderType = SHADER_TYPE_PIXEL;
        shaderCI.Desc.Name       = "Imgui PS";
        switch (DeviceType) {
            case RENDER_DEVICE_TYPE_VULKAN:
                shaderCI.ByteCode     = FragmentShader_SPIRV;
                shaderCI.ByteCodeSize = sizeof(FragmentShader_SPIRV);
                break;

            case RENDER_DEVICE_TYPE_D3D11:
            case RENDER_DEVICE_TYPE_D3D12:
                shaderCI.Source = PixelShaderHLSL;
                break;

            case RENDER_DEVICE_TYPE_GL:
            case RENDER_DEVICE_TYPE_GLES:
                shaderCI.Source = PixelShaderGLSL;
                break;

            case RENDER_DEVICE_TYPE_METAL:
                shaderCI.Source     = ShadersMSL;
                shaderCI.EntryPoint = "ps_main";
                break;

            default:
                UNEXPECTED("Unknown render device type");
        }
        mDevice->CreateShader(shaderCI, &pixelShader);
    }

    GraphicsPipelineStateCreateInfo psoCI;

    psoCI.PSODesc.Name = "ImGUI PSO";
    auto& pipeline     = psoCI.GraphicsPipeline;

    pipeline.NumRenderTargets             = 1;
    pipeline.RTVFormats[0]                = mBackBufferFormat;
    pipeline.DSVFormat                    = mDepthBufferFormat;
    pipeline.PrimitiveTopology            = PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    pipeline.RasterizerDesc.CullMode      = CULL_MODE_NONE;
    pipeline.RasterizerDesc.ScissorEnable = True;
    pipeline.DepthStencilDesc.DepthEnable = False;

    psoCI.pVS = vertexShader;
    psoCI.pPS = pixelShader;

    auto& rt0                 = pipeline.BlendDesc.RenderTargets[0];
    rt0.BlendEnable           = True;
    rt0.SrcBlend              = BLEND_FACTOR_SRC_ALPHA;
    rt0.DestBlend             = BLEND_FACTOR_INV_SRC_ALPHA;
    rt0.BlendOp               = BLEND_OPERATION_ADD;
    rt0.SrcBlendAlpha         = BLEND_FACTOR_INV_SRC_ALPHA;
    rt0.DestBlendAlpha        = BLEND_FACTOR_ZERO;
    rt0.BlendOpAlpha          = BLEND_OPERATION_ADD;
    rt0.RenderTargetWriteMask = COLOR_MASK_ALL;

    const std::array<LayoutElement, 3> kVSInputs = {
        LayoutElement{0, 0, 2, VT_FLOAT32, false}, // pos
        LayoutElement{1, 0, 2, VT_FLOAT32, false}, // uv
        LayoutElement{2, 0, 4,   VT_UINT8,  true}  // col
    };

    pipeline.InputLayout.NumElements    = kVSInputs.size();
    pipeline.InputLayout.LayoutElements = kVSInputs.data();

    const std::array<ShaderResourceVariableDesc, 1> kVariables = {
        ShaderResourceVariableDesc{SHADER_TYPE_PIXEL, "Texture", SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC}
    };

    psoCI.PSODesc.ResourceLayout.NumVariables = kVariables.size();
    psoCI.PSODesc.ResourceLayout.Variables    = kVariables.data();

    SamplerDesc samplerDesc;
    samplerDesc.AddressU = TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = TEXTURE_ADDRESS_WRAP;

    const std::array<ImmutableSamplerDesc, 1> kImmutableSamplers = {
        ImmutableSamplerDesc{SHADER_TYPE_PIXEL, "Texture", samplerDesc}
    };

    psoCI.PSODesc.ResourceLayout.ImmutableSamplers    = kImmutableSamplers.data();
    psoCI.PSODesc.ResourceLayout.NumImmutableSamplers = kImmutableSamplers.size();

    mDevice->CreateGraphicsPipelineState(psoCI, &mPSO);
}

void ImGuiRenderer::CreateCB() {
    {
        BufferDesc bufferDesc{nullptr, sizeof(float4x4), BIND_UNIFORM_BUFFER, USAGE_DYNAMIC, CPU_ACCESS_WRITE};
        mDevice->CreateBuffer(bufferDesc, nullptr, &mVertexConstantBuffer);
    }
    mPSO->GetStaticVariableByName(SHADER_TYPE_VERTEX, "Constants")->Set(mVertexConstantBuffer);
}

void ImGuiRenderer::CreateTexture() {
    ImGuiIO& imIO = ImGui::GetIO();

    unsigned char* data   = nullptr;
    int            width  = 0;
    int            height = 0;
    imIO.Fonts->GetTexDataAsRGBA32(&data, &width, &height);

    TextureDesc textureDesc;
    textureDesc.Name      = "Imgui font texture";
    textureDesc.Type      = RESOURCE_DIM_TEX_2D;
    textureDesc.Width     = static_cast<Uint32>(width);
    textureDesc.Height    = static_cast<Uint32>(height);
    textureDesc.Format    = TEX_FORMAT_RGBA8_UNORM;
    textureDesc.BindFlags = BIND_SHADER_RESOURCE;
    textureDesc.Usage     = USAGE_IMMUTABLE;

    std::array<TextureSubResData, 1> mip0Data = {
        TextureSubResData{data, 4 * Uint64{textureDesc.Width}}
    };

    TextureData textureData{mip0Data.data(), mip0Data.size()};

    RefCntAutoPtr<ITexture> texture;
    mDevice->CreateTexture(textureDesc, &textureData, &texture);
    mFontSRV = texture->GetDefaultView(TEXTURE_VIEW_SHADER_RESOURCE);

    mSRB.Release();
    mPSO->CreateShaderResourceBinding(&mSRB, true);
    mTexture = mSRB->GetVariableByName(SHADER_TYPE_PIXEL, "Texture");
    VERIFY_EXPR(m_pTextureVar != nullptr);

    // Store identifier
    imIO.Fonts->TexID = (ImTextureID) mFontSRV;
}

void ImGuiRenderer::Create() {
    CreatePSO();
    CreateCB();
    CreateTexture();
}

} // namespace Sage::Graphics
