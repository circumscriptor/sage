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

namespace Sage::Core::Graphics {

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
    pipeline.RTVFormats[0]                = mColorBufferFormat;
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

    mDevice->CreateGraphicsPipelineState(psoCI, &mPipeline);
}

void ImGuiRenderer::CreateCB() {
    {
        BufferDesc bufferDesc{nullptr, sizeof(float4x4), BIND_UNIFORM_BUFFER, USAGE_DYNAMIC, CPU_ACCESS_WRITE};
        mDevice->CreateBuffer(bufferDesc, nullptr, &mVertexConstantBuffer);
    }
    mPipeline->GetStaticVariableByName(SHADER_TYPE_VERTEX, "Constants")->Set(mVertexConstantBuffer);
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
    mPipeline->CreateShaderResourceBinding(&mSRB, true);
    mTexture = mSRB->GetVariableByName(SHADER_TYPE_PIXEL, "Texture");
    VERIFY_EXPR(m_pTextureVar != nullptr);

    // Store identifier
    imIO.Fonts->TexID = (ImTextureID) mFontSRV;
}

} // namespace Sage::Core::Graphics
