#include "Context.hpp"

#include <SDL2/SDL.h>

namespace Sage::Core {

using namespace Console;
using namespace Graphics;

Context::Context(IVirtualConsole& console) : mConsole{console}, mContextID{console.CreateContext()} {
    mGraphics = std::make_shared<GraphicsContext>(console, mContextID);
    mImGui    = std::make_shared<ImGuiContext>(mGraphics);
    mTimer    = std::make_shared<Timer>();
}

Context::~Context() {
    mConsole.DestroyContext(mContextID);
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
        const auto& SCDesc = mGraphics->GetSwapchain()->GetDesc();

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
