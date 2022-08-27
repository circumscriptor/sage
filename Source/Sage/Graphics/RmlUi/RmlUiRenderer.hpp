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
/// @file RmlUiRenderer.hpp
/// @brief ...
///
/// @copyright Copyright (c) 2022
///
///

#pragma once

#include <RmlUi/Core/RenderInterface.h>
#include <Sage/ClassDefinitions.hpp>

namespace Sage::Graphics {

class RmlUiRenderer : public Rml::RenderInterface {
  public:

    SAGE_CLASS_DELETE(RmlUiRenderer)

    ~RmlUiRenderer() override = default;

    void RenderGeometry(Rml::Vertex*         vertices,
                        int                  num_vertices,
                        int*                 indices,
                        int                  num_indices,
                        Rml::TextureHandle   texture,
                        const Rml::Vector2f& translation) override {
        // Should not happen
    }

    Rml::CompiledGeometryHandle CompileGeometry(Rml::Vertex*       vertices,
                                                int                numVertices,
                                                int*               indices,
                                                int                numIndices,
                                                Rml::TextureHandle texture) override {
        return 0;
    }

    void RenderCompiledGeometry(Rml::CompiledGeometryHandle geometry, const Rml::Vector2f& translation) override {}

    void ReleaseCompiledGeometry(Rml::CompiledGeometryHandle geometry) override {}

    void EnableScissorRegion(bool enable) override {}

    void SetScissorRegion(int x, int y, int width, int height) override {}

    bool LoadTexture(Rml::TextureHandle& textureHandle,
                     Rml::Vector2i&      textureDimensions,
                     const Rml::String&  source) override {
        // Load texture, and call generate
        return false;
    }

    bool GenerateTexture(Rml::TextureHandle&  textureHandle,
                         const Rml::byte*     source,
                         const Rml::Vector2i& sourceDimensions) override {
        // Generate texture
        return false;
    }

    void ReleaseTexture(Rml::TextureHandle texture) override {}

    void SetTransform(const Rml::Matrix4f* transform) override {}
};

} // namespace Sage::Graphics
