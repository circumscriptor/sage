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
/// @file ImGuiInterface.cpp
/// @brief ...
///
/// @copyright Copyright (c) 2022
///
///

#include "ImGuiInterface.hpp"

#include "ImGuiRenderer.hpp"
#include "Internal/KeycodeToImGuiKey.hpp"
#include "imgui/imgui.h"

// SDL
#include <SDL2/SDL.h>
#include <SDL2/SDL_clipboard.h>
#include <memory>

namespace Sage::Core::Graphics {

using namespace Diligent;

class ImGuiInterfaceImpl : public IImGuiInterface, public ImGuiRenderer {
  public:

    SAGE_CLASS_DELETE(ImGuiInterfaceImpl)

    ImGuiInterfaceImpl(ImGuiContext*                     context,
                       std::shared_ptr<IGraphicsContext> graphics,
                       UInt32                            vertexBufferSize,
                       UInt32                            indexBufferSize) :
        ImGuiRenderer(std::dynamic_pointer_cast<Internal::GraphicsContext>(std::move(graphics)),
                      vertexBufferSize,
                      indexBufferSize),
        mContext{context} {
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
    }

    ~ImGuiInterfaceImpl() override {
        if (mClipboardText != nullptr) {
            SDL_free(mClipboardText);
        }
        ImGui::DestroyContext(mContext);
    }

    void NewFrame(float deltaTime) override {
        ImGui::SetCurrentContext(mContext);

        ImGuiRenderer::NewFrame();

        ImGuiIO& imIO      = ImGui::GetIO();
        imIO.DisplaySize.x = float(GetRenderSurfaceWidth());
        imIO.DisplaySize.y = float(GetRenderSurfaceHeight());
        imIO.DeltaTime     = deltaTime;
        ImGui::NewFrame();
    }

    void EndFrame() override {
        ImGui::EndFrame();
    }

    void Render() override {
        ImGui::Render();
        RenderDrawData(ImGui::GetDrawData());
    }

    void ProcessEvent(const SDL_Event& event) override {
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

                ImGuiKey key = Internal::KeycodeToImGuiKey(event.key.keysym.sym);
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

  private:

    static void SetClipboardText(void* /*user*/, const char* text) {
        SDL_SetClipboardText(text);
    }

    static const char* GetClipboardText(void* user) {
        auto* interface = static_cast<ImGuiInterfaceImpl*>(user);
        if (interface->mClipboardText != nullptr) {
            SDL_free(interface->mClipboardText);
        }
        interface->mClipboardText = interface->mClipboardText = SDL_GetClipboardText();
        return interface->mClipboardText;
    }

    char*         mClipboardText{nullptr}; // Allocated by SDL
    ImGuiContext* mContext{};
};

std::shared_ptr<IImGuiInterface> IImGuiInterface::CreateInstance(std::shared_ptr<IGraphicsContext> graphics,
                                                                 UInt32                            vertexBufferSize,
                                                                 UInt32                            indexBufferSize) {
    return std::make_shared<ImGuiInterfaceImpl>(ImGui::CreateContext(),
                                                std::move(graphics),
                                                vertexBufferSize,
                                                indexBufferSize);
}

} // namespace Sage::Core::Graphics
