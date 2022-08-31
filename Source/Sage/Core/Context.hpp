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
/// @file Context.hpp
/// @brief ...
///
/// @copyright Copyright (c) 2022
///
///

#pragma once

#include <Sage/Core/ClassDefinitions.hpp>
#include <Sage/Core/Console/VirtualConsole.hpp>
#include <Sage/Core/Graphics/GraphicsContext.hpp>
#include <Sage/Core/Graphics/ImGui/ImGuiContext.hpp>
#include <Sage/Core/Timer.hpp>

namespace Sage::Core {

class Context {
  public:

    SAGE_CLASS_DELETE_COPY_AND_MOVE(Context)

    explicit Context(Console::IVirtualConsole& console);

    ~Context();

    void Update();

    void Render();

    void ProcessEvents(const SDL_Event& event);

    [[nodiscard]] bool ShouldDestroy() const noexcept {
        return mShouldDestroy;
    }

  private:

    Console::IVirtualConsole&                  mConsole;
    const Console::IVirtualConsole::ContextID  mContextID;
    std::shared_ptr<Graphics::GraphicsContext> mGraphics;
    std::shared_ptr<Graphics::ImGuiContext>    mImGui;
    std::shared_ptr<Timer>                     mTimer;
    bool                                       mShowImGui{true};
    bool                                       mShouldDestroy{false};
};

} // namespace Sage::Core
