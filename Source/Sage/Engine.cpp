
#include "Engine.hpp"

#include "Sage/Graphics/ImGui/ThirdParty/imgui.h"

#include <GraphicsTypes.h>
#include <RmlUi/Core.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>

namespace Sage {

Engine::Engine() :
    mGraphics{std::make_shared<GraphicsContext>()},
    mImGui{std::make_shared<ImGuiContext>(mGraphics)},
    mTimer{std::make_shared<Timer>()} {}

Engine::~Engine() = default;

void Engine::Run() {
    SDL_Event event;
    bool      running = true;

    mTimer->SetInterval(0);

    while (running) {
        while (SDL_PollEvent(&event) != 0) {
            mImGui->ProcessEvent(event);
            switch (event.type) {
                case SDL_QUIT:
                    running = false;
                    break;
                case SDL_KEYDOWN:
                    if (event.key.keysym.sym == SDLK_ESCAPE) {
                        running = false;
                    }
                    break;
            }
        }

        mTimer->Update();

        static const UInt32 kImGuiFlagsNoWindow =
            UInt32(ImGuiWindowFlags_NoTitleBar) | UInt32(ImGuiWindowFlags_NoMove) |
            UInt32(ImGuiWindowFlags_NoScrollbar) | UInt32(ImGuiWindowFlags_NoSavedSettings) |
            UInt32(ImGuiWindowFlags_NoInputs) | UInt32(ImGuiWindowFlags_NoBackground) |
            UInt32(ImGuiWindowFlags_NoDecoration) | UInt32(ImGuiWindowFlags_NoResize);

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
}

} // namespace Sage
