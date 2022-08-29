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

// SDL
#include <SDL2/SDL.h>
#include <SDL2/SDL_clipboard.h>
#include <memory>

namespace Sage::Core::Graphics {

using namespace Diligent;

static inline ImGuiKey KeycodeToImGuiKey(int keycode) {
    switch (keycode) {
        case SDLK_TAB:
            return ImGuiKey_Tab;
        case SDLK_LEFT:
            return ImGuiKey_LeftArrow;
        case SDLK_RIGHT:
            return ImGuiKey_RightArrow;
        case SDLK_UP:
            return ImGuiKey_UpArrow;
        case SDLK_DOWN:
            return ImGuiKey_DownArrow;
        case SDLK_PAGEUP:
            return ImGuiKey_PageUp;
        case SDLK_PAGEDOWN:
            return ImGuiKey_PageDown;
        case SDLK_HOME:
            return ImGuiKey_Home;
        case SDLK_END:
            return ImGuiKey_End;
        case SDLK_INSERT:
            return ImGuiKey_Insert;
        case SDLK_DELETE:
            return ImGuiKey_Delete;
        case SDLK_BACKSPACE:
            return ImGuiKey_Backspace;
        case SDLK_SPACE:
            return ImGuiKey_Space;
        case SDLK_RETURN:
            return ImGuiKey_Enter;
        case SDLK_ESCAPE:
            return ImGuiKey_Escape;
        case SDLK_QUOTE:
            return ImGuiKey_Apostrophe;
        case SDLK_COMMA:
            return ImGuiKey_Comma;
        case SDLK_MINUS:
            return ImGuiKey_Minus;
        case SDLK_PERIOD:
            return ImGuiKey_Period;
        case SDLK_SLASH:
            return ImGuiKey_Slash;
        case SDLK_SEMICOLON:
            return ImGuiKey_Semicolon;
        case SDLK_EQUALS:
            return ImGuiKey_Equal;
        case SDLK_LEFTBRACKET:
            return ImGuiKey_LeftBracket;
        case SDLK_BACKSLASH:
            return ImGuiKey_Backslash;
        case SDLK_RIGHTBRACKET:
            return ImGuiKey_RightBracket;
        case SDLK_BACKQUOTE:
            return ImGuiKey_GraveAccent;
        case SDLK_CAPSLOCK:
            return ImGuiKey_CapsLock;
        case SDLK_SCROLLLOCK:
            return ImGuiKey_ScrollLock;
        case SDLK_NUMLOCKCLEAR:
            return ImGuiKey_NumLock;
        case SDLK_PRINTSCREEN:
            return ImGuiKey_PrintScreen;
        case SDLK_PAUSE:
            return ImGuiKey_Pause;
        case SDLK_KP_0:
            return ImGuiKey_Keypad0;
        case SDLK_KP_1:
            return ImGuiKey_Keypad1;
        case SDLK_KP_2:
            return ImGuiKey_Keypad2;
        case SDLK_KP_3:
            return ImGuiKey_Keypad3;
        case SDLK_KP_4:
            return ImGuiKey_Keypad4;
        case SDLK_KP_5:
            return ImGuiKey_Keypad5;
        case SDLK_KP_6:
            return ImGuiKey_Keypad6;
        case SDLK_KP_7:
            return ImGuiKey_Keypad7;
        case SDLK_KP_8:
            return ImGuiKey_Keypad8;
        case SDLK_KP_9:
            return ImGuiKey_Keypad9;
        case SDLK_KP_PERIOD:
            return ImGuiKey_KeypadDecimal;
        case SDLK_KP_DIVIDE:
            return ImGuiKey_KeypadDivide;
        case SDLK_KP_MULTIPLY:
            return ImGuiKey_KeypadMultiply;
        case SDLK_KP_MINUS:
            return ImGuiKey_KeypadSubtract;
        case SDLK_KP_PLUS:
            return ImGuiKey_KeypadAdd;
        case SDLK_KP_ENTER:
            return ImGuiKey_KeypadEnter;
        case SDLK_KP_EQUALS:
            return ImGuiKey_KeypadEqual;
        case SDLK_LCTRL:
            return ImGuiKey_LeftCtrl;
        case SDLK_LSHIFT:
            return ImGuiKey_LeftShift;
        case SDLK_LALT:
            return ImGuiKey_LeftAlt;
        case SDLK_LGUI:
            return ImGuiKey_LeftSuper;
        case SDLK_RCTRL:
            return ImGuiKey_RightCtrl;
        case SDLK_RSHIFT:
            return ImGuiKey_RightShift;
        case SDLK_RALT:
            return ImGuiKey_RightAlt;
        case SDLK_RGUI:
            return ImGuiKey_RightSuper;
        case SDLK_APPLICATION:
            return ImGuiKey_Menu;
        case SDLK_0:
            return ImGuiKey_0;
        case SDLK_1:
            return ImGuiKey_1;
        case SDLK_2:
            return ImGuiKey_2;
        case SDLK_3:
            return ImGuiKey_3;
        case SDLK_4:
            return ImGuiKey_4;
        case SDLK_5:
            return ImGuiKey_5;
        case SDLK_6:
            return ImGuiKey_6;
        case SDLK_7:
            return ImGuiKey_7;
        case SDLK_8:
            return ImGuiKey_8;
        case SDLK_9:
            return ImGuiKey_9;
        case SDLK_a:
            return ImGuiKey_A;
        case SDLK_b:
            return ImGuiKey_B;
        case SDLK_c:
            return ImGuiKey_C;
        case SDLK_d:
            return ImGuiKey_D;
        case SDLK_e:
            return ImGuiKey_E;
        case SDLK_f:
            return ImGuiKey_F;
        case SDLK_g:
            return ImGuiKey_G;
        case SDLK_h:
            return ImGuiKey_H;
        case SDLK_i:
            return ImGuiKey_I;
        case SDLK_j:
            return ImGuiKey_J;
        case SDLK_k:
            return ImGuiKey_K;
        case SDLK_l:
            return ImGuiKey_L;
        case SDLK_m:
            return ImGuiKey_M;
        case SDLK_n:
            return ImGuiKey_N;
        case SDLK_o:
            return ImGuiKey_O;
        case SDLK_p:
            return ImGuiKey_P;
        case SDLK_q:
            return ImGuiKey_Q;
        case SDLK_r:
            return ImGuiKey_R;
        case SDLK_s:
            return ImGuiKey_S;
        case SDLK_t:
            return ImGuiKey_T;
        case SDLK_u:
            return ImGuiKey_U;
        case SDLK_v:
            return ImGuiKey_V;
        case SDLK_w:
            return ImGuiKey_W;
        case SDLK_x:
            return ImGuiKey_X;
        case SDLK_y:
            return ImGuiKey_Y;
        case SDLK_z:
            return ImGuiKey_Z;
        case SDLK_F1:
            return ImGuiKey_F1;
        case SDLK_F2:
            return ImGuiKey_F2;
        case SDLK_F3:
            return ImGuiKey_F3;
        case SDLK_F4:
            return ImGuiKey_F4;
        case SDLK_F5:
            return ImGuiKey_F5;
        case SDLK_F6:
            return ImGuiKey_F6;
        case SDLK_F7:
            return ImGuiKey_F7;
        case SDLK_F8:
            return ImGuiKey_F8;
        case SDLK_F9:
            return ImGuiKey_F9;
        case SDLK_F10:
            return ImGuiKey_F10;
        case SDLK_F11:
            return ImGuiKey_F11;
        case SDLK_F12:
            return ImGuiKey_F12;
        default:
            return ImGuiKey_None;
    }
}

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
