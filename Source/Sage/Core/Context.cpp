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
/// @file Context.cpp
/// @brief ...
///
/// @copyright Copyright (c) 2022
///
///

#include "Context.hpp"

#include <SDL2/SDL.h>

namespace Sage::Core {

using namespace Console;
using namespace Graphics;

Context::Context(std::shared_ptr<IVirtualConsole> console) :
    mConsole{std::move(console)},
    mContextID{mConsole->CreateContext()} {
    mGraphics = IGraphicsContext::CreateInstance(mConsole, mContextID);
    mImGui    = std::make_shared<ImGuiContext>(mGraphics);
    mTimer    = std::make_shared<Timer>();
}

Context::~Context() {
    mConsole->DestroyContext(mContextID);
}

void Context::Update() {
    mTimer->Update();
}

void Context::Render() {
    static const UInt32 kImGuiFlagsNoWindow =
        UInt32(ImGuiWindowFlags_NoTitleBar) | UInt32(ImGuiWindowFlags_NoMove) | UInt32(ImGuiWindowFlags_NoScrollbar) |
        UInt32(ImGuiWindowFlags_NoSavedSettings) | UInt32(ImGuiWindowFlags_NoInputs) |
        UInt32(ImGuiWindowFlags_NoBackground) | UInt32(ImGuiWindowFlags_NoDecoration) |
        UInt32(ImGuiWindowFlags_NoResize);

    mImGui->NewFrame(float(mTimer->DeltaTime()));
    {
        ImGui::SetNextWindowPos({10.F, 10.F}, ImGuiCond_Once);
        ImGui::Begin("Timing", nullptr, kImGuiFlagsNoWindow);

        ImGui::TextColored({0.F, 1.F, 0.F, 1.F}, "   FPS    AVG       MIN       MAX   ");
        ImGui::TextColored({0.F, 1.F, 0.F, 1.F},
                           "%6.0f %6.2f ms %6.2f ms %6.2f ms",
                           mTimer->AvgFrameRate(),
                           mTimer->AvgDeltaTime() * 1000.0,
                           mTimer->MinDeltaTime() * 1000.0,
                           mTimer->MaxDeltaTime() * 1000.0);
        ImGui::End();
    }

    mGraphics->Clear();

    // Render

    if (mShowImGui) {
        mImGui->Render(0);
    } else {
        mImGui->EndFrame();
    }

    mGraphics->Present();
}

void Context::ProcessEvents(const SDL_Event& event) {
    mImGui->ProcessEvent(event);

    switch (event.type) {
        case SDL_QUIT:
            mShouldDestroy = false;
            break;
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE) {
                mShouldDestroy = false;
            }
            break;
    }
}

} // namespace Sage::Core
