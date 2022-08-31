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
/// @file ImGuiContext.hpp
/// @brief ...
///
/// @copyright Copyright (c) 2022
///
///

#pragma once

#include "../GraphicsContext.hpp"
#include "ImGuiRenderer.hpp"

#include <Sage/Core/Graphics/Internal/GraphicsContextImpl.hpp>

union SDL_Event;

namespace Sage::Core::Graphics {

class ImGuiContext {
  public:

    static constexpr UInt32 kDefaultInitialVBSize = 1024;
    static constexpr UInt32 kDefaultInitialIBSize = 2048;

    SAGE_CLASS_DELETE(ImGuiContext)

    ImGuiContext(std::shared_ptr<IGraphicsContext> graphics,
                 UInt32                            vertexBufferSize = kDefaultInitialVBSize,
                 UInt32                            indexBufferSize  = kDefaultInitialIBSize);

    ~ImGuiContext();

    void NewFrame(float deltaTime);

    void EndFrame();

    void Render(UInt32 contextID);

    void ProcessEvent(const SDL_Event& event);

  private:

    static const char* GetClipboardText(void* user);

    static void SetClipboardText(void* user, const char* text);

    std::shared_ptr<Internal::GraphicsContext> mGraphics;
    std::unique_ptr<ImGuiRenderer>             mRenderer;
    char*                                      mClipboardText{nullptr}; // Allocated by SDL
    ::ImGuiContext*                            mContext;
};

} // namespace Sage::Core::Graphics
