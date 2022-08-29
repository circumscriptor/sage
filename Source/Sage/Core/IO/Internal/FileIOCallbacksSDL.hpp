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
/// @file FileIOCallbacksSDL.hpp
/// @brief ...
///
/// @copyright Copyright (c) 2022
///
///

#pragma once

#include <SDL2/SDL_error.h>
#include <SDL2/SDL_rwops.h>
#include <Sage/Core/Console/Log.hpp>
#include <Sage/Core/Console/ThirdParty/cfg.hpp>
#include <array>

namespace Sage::Core::IO::Internal {

class FileIOCallbacksSDL : public cfg::FileIOCallbacks {
  public:

    bool open(cfg::FileHandle* outHandle, const char* filename, cfg::FileOpenMode mode) override {
        if (outHandle == nullptr || filename == nullptr || *filename == '\0') {
            return false;
        }

        SDL_RWops* RWops = SDL_RWFromFile(filename, (mode == cfg::FileOpenMode::Read ? "rt" : "wt+"));
        if (RWops == nullptr) {
            SAGE_LOG_WARN("Failed to open: {}", filename);
            (*outHandle) = nullptr;
            return false;
        }

        *outHandle = RWops;
        return true;
    }

    void close(cfg::FileHandle file) override {
        if (file == nullptr) {
            return;
        }

        SDL_RWclose(static_cast<SDL_RWops*>(file));
    }

    bool isAtEOF(cfg::FileHandle file) const override {
        if (file == nullptr) {
            return true;
        }

        auto* RWops = static_cast<SDL_RWops*>(file);
        char  tmp   = 0;
        return SDL_RWread(RWops, &tmp, sizeof(tmp), 1) == 0 || tmp == EOF;
    }

    void rewind(cfg::FileHandle file) override {
        if (file == nullptr) {
            return;
        }

        auto* RWops = static_cast<SDL_RWops*>(file);
        SDL_RWseek(RWops, 0, RW_SEEK_SET);
    }

    bool readLine(cfg::FileHandle file, char* outBuffer, int bufferSize) override {
        if (file == nullptr || outBuffer == nullptr || bufferSize <= 0) {
            return false;
        }

        auto* RWops = static_cast<SDL_RWops*>(file);

        const size_t count = SDL_RWread(RWops, outBuffer, sizeof(char), bufferSize);

        int nRead = 0;
        for (; nRead < count; ++nRead) {
            if (outBuffer[nRead] == '\n') {
                outBuffer[nRead] = '\0';
                break;
            }
        }

        SDL_RWseek(RWops, -int(count) + nRead, RW_SEEK_CUR);
        return true;
    }

    bool writeString(cfg::FileHandle file, const char* str) override {
        if (file == nullptr || str == nullptr || *str == '\0') {
            return false;
        }

        auto* RWops = static_cast<SDL_RWops*>(file);

        if (SDL_RWwrite(RWops, str, std::strlen(str), 1) < 1) {
            SAGE_LOG_ERROR("In FileIO \"writeString\" failed with message: {}", SDL_GetError());
            return false;
        }
        return true;
    }

    bool writeFormat(cfg::FileHandle file, const char* fmt, ...) override CFG_PRINTF_FUNC(3, 4) {
        if (file == nullptr || fmt == nullptr) {
            return false;
        }

        std::array<char, 2048> tmpString{};

        va_list vaList;
        va_start(vaList, fmt);
        const int result = std::vsnprintf(tmpString.data(), tmpString.size(), fmt, vaList);
        va_end(vaList);

        if (result <= 0) {
            SAGE_LOG_ERROR(R"(In FileIO "writeFormat" failed with message: "vsnprintf" wrote 0 or less characters)");
            return false;
        }
        return writeString(file, tmpString.data());
    }
};

} // namespace Sage::Core::IO::Internal
