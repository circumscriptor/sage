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
/// @file ImGuiContext.cpp
/// @brief ...
///
/// @copyright Copyright (c) 2022
///
///

#include "ImGuiContext.hpp"

#include "Internal/KeycodeToImGuiKey.hpp"

// SDL
#include <SDL2/SDL.h>
#include <SDL2/SDL_clipboard.h>
#include <memory>

namespace Sage::Core::Graphics {

using namespace Diligent;

ImGuiContext::ImGuiContext(std::shared_ptr<GraphicsContext> graphics, Uint32 vertexBufferSize, Uint32 indexBufferSize) :
    mGraphics{std::move(graphics)},
    mContext(ImGui::CreateContext()) {
    ImGui::SetCurrentContext(mContext);
    ImGuiIO& imIO    = ImGui::GetIO();
    imIO.IniFilename = nullptr;
    imIO.LogFilename = nullptr;

    // imIO.BackendFlags |= ImGuiBackendFlags_HasMouseCursors; // Can change OS cursor shape
    imIO.BackendFlags |= ImGuiBackendFlags_HasSetMousePos; // Can move OS cursor

    imIO.SetClipboardTextFn = SetClipboardText;
    imIO.GetClipboardTextFn = GetClipboardText;
    imIO.UserData           = this;

    // ImGui::GetMainViewport()->PlatformHandleRaw = hWnd;

    const auto& SCDesc = mGraphics->GetSwapchain()->GetDesc();

    mRenderer = std::make_unique<ImGuiRenderer>(mGraphics->GetDevice(),
                                                SCDesc.ColorBufferFormat,
                                                SCDesc.DepthBufferFormat,
                                                vertexBufferSize,
                                                indexBufferSize);
}

ImGuiContext::~ImGuiContext() {
    if (mClipboardText != nullptr) {
        SDL_free(mClipboardText);
    }

    ImGui::DestroyContext(mContext);
}

void ImGuiContext::NewFrame(float deltaTime) {
    ImGui::SetCurrentContext(mContext);

    const auto& SCDesc = mGraphics->GetSwapchain()->GetDesc();
    mRenderer->NewFrame(SCDesc.Width, SCDesc.Height, SCDesc.PreTransform);

    ImGuiIO& imIO      = ImGui::GetIO();
    imIO.DisplaySize.x = float(SCDesc.Width);
    imIO.DisplaySize.y = float(SCDesc.Height);
    imIO.DeltaTime     = deltaTime;
    ImGui::NewFrame();
}

void ImGuiContext::EndFrame() {
    ImGui::SetCurrentContext(mContext);
    ImGui::EndFrame();
}

void ImGuiContext::Render(UInt32 contextID) {
    ImGui::SetCurrentContext(mContext);
    ImGui::Render();
    mRenderer->RenderDrawData(mGraphics->GetContext(contextID), ImGui::GetDrawData());
}

void ImGuiContext::ProcessEvent(const SDL_Event& event) {
    ImGui::SetCurrentContext(mContext);
    ImGuiIO& imIO = ImGui::GetIO();

    switch (event.type) {
        case SDL_MOUSEMOTION: {
            imIO.AddMousePosEvent(float(event.motion.x), float(event.motion.y));
        } break;

        case SDL_MOUSEWHEEL: {
            float wheelX = (event.wheel.x > 0) ? 1.0F : (event.wheel.x < 0) ? -1.0F : 0.0F;
            float wheelY = (event.wheel.y > 0) ? 1.0F : (event.wheel.y < 0) ? -1.0F : 0.0F;
            imIO.AddMouseWheelEvent(wheelX, wheelY);
        } break;

        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP: {
            int mouseButton = -1;
            if (event.button.button == SDL_BUTTON_LEFT) {
                mouseButton = 0;
            } else if (event.button.button == SDL_BUTTON_RIGHT) {
                mouseButton = 1;
            } else if (event.button.button == SDL_BUTTON_MIDDLE) {
                mouseButton = 2;
            } else if (event.button.button == SDL_BUTTON_X1) {
                mouseButton = 3;
            } else if (event.button.button == SDL_BUTTON_X2) {
                mouseButton = 4;
            } else {
                break;
            }
            imIO.AddMouseButtonEvent(mouseButton, (event.type == SDL_MOUSEBUTTONDOWN));

        } break;

        case SDL_TEXTINPUT: {
            imIO.AddInputCharactersUTF8(static_cast<const char*>(event.text.text));
        } break;

        case SDL_KEYDOWN:
        case SDL_KEYUP: {
            auto mods = UInt32(event.key.keysym.mod);
            imIO.AddKeyEvent(ImGuiKey_ModCtrl, (mods & UInt32(KMOD_CTRL)) != 0);
            imIO.AddKeyEvent(ImGuiKey_ModShift, (mods & UInt32(KMOD_SHIFT)) != 0);
            imIO.AddKeyEvent(ImGuiKey_ModAlt, (mods & UInt32(KMOD_ALT)) != 0);
            imIO.AddKeyEvent(ImGuiKey_ModSuper, (mods & UInt32(KMOD_GUI)) != 0);

            ImGuiKey key = KeycodeToImGuiKey(event.key.keysym.sym);
            imIO.AddKeyEvent(key, (event.type == SDL_KEYDOWN));
            imIO.SetKeyEventNativeData(
                key,
                event.key.keysym.sym,
                event.key.keysym.scancode,
                event.key.keysym.scancode); // To support legacy indexing (<1.87 user code). Legacy backend uses
                                            // SDLK_*** as indices to IsKeyXXX() functions.
        } break;

        case SDL_WINDOWEVENT: {
            Uint8 windowEvent = event.window.event;
            if (windowEvent == SDL_WINDOWEVENT_ENTER) {
                // Entered
            } else if (windowEvent == SDL_WINDOWEVENT_LEAVE) {
                // Left
            } else if (windowEvent == SDL_WINDOWEVENT_FOCUS_GAINED) {
                imIO.AddFocusEvent(true);
            } else if (windowEvent == SDL_WINDOWEVENT_FOCUS_LOST) {
                imIO.AddFocusEvent(false);
            }
        } break;
    }
}

void ImGuiContext::SetClipboardText(void* /*user*/, const char* text) {
    SDL_SetClipboardText(text);
}

const char* ImGuiContext::GetClipboardText(void* user) {
    auto* imguiSDL = static_cast<ImGuiContext*>(user);
    if (imguiSDL->mClipboardText != nullptr) {
        SDL_free(imguiSDL->mClipboardText);
    }
    imguiSDL->mClipboardText = imguiSDL->mClipboardText = SDL_GetClipboardText();
    return imguiSDL->mClipboardText;
}

} // namespace Sage::Core::Graphics
