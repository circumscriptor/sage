#pragma once

#include "ClassDefinitions.hpp"

#include <SDL2/SDL.h>
#include <Sage/Core/Console/Log.hpp>
#include <algorithm>
#include <exception>
#include <string>

namespace Sage::Core {

class SDL {
  public:

    SAGE_CLASS_DELETE_COPY_AND_MOVE(SDL)

    static SDL& Get() {
        static SDL sSDL;
        return sSDL;
    }

  private:

    SDL() {
#ifdef SDL_VIDEO_DRIVER_X11
        SDL_SetHint(SDL_HINT_VIDEODRIVER, "x11");
#endif
        if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
            throw std::exception("failed to initialize SDL");
        }
    }

    ~SDL() {
        SDL_Quit();
    }
};

} // namespace Sage::Core
