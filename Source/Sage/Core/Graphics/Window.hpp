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
/// @file Window.hpp
/// @brief ...
///
/// @copyright Copyright (c) 2022
///
///

#pragma once

#include <Sage/Core/Graphics/GraphicsContext.hpp>
#include <Sage/Core/Graphics/RenderView.hpp>

namespace Sage::Core::Graphics {

class Window {
  public:

    Window() {}

    ~Window() {}

  private:

    SDL_Window* mWindow;

    std::unique_ptr<IGraphicsContext> mGraphics;
};

} // namespace Sage::Core::Graphics