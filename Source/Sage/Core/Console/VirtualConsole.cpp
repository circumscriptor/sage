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
/// @file VirtualConsole.cpp
/// @brief ...
///
/// @copyright Copyright (c) 2022
///
///

#include "VirtualConsole.hpp"

#include "Log.hpp"
#include "Sage/Core/SDL.hpp"
#include "ThirdParty/cfg.hpp"

#include <Sage/Core/IO/Internal/FileIOCallbacksSDL.hpp>
#include <Sage/Core/IO/Path.hpp>
#include <iostream>

#ifndef SAGE_VIRTUAL_CONSOLE_MARK
    #define SAGE_VIRTUAL_CONSOLE_MARK "> "
#endif

namespace Sage::Core::Console {

static void ErrorCallback(const char* message, [[maybe_unused]] void* user) {
    SAGE_LOG_ERROR(message);
}

class GlobalVirtualConsole : public IVirtualConsole, public cfg::SimpleCommandTerminal {
  public:

    SAGE_CLASS_DELETE_COPY_AND_MOVE(GlobalVirtualConsole)

    GlobalVirtualConsole() :
        cfg::SimpleCommandTerminal(CVars().mCmdManager, CVars().mCVarManager, SAGE_VIRTUAL_CONSOLE_MARK) {
        static IO::Internal::FileIOCallbacksSDL sFileIO;
        cfg::setErrorCallback(ErrorCallback, nullptr);
        cfg::setFileIOCallbacks(&sFileIO);
        cfg::registerDefaultCommands(CVars().mCmdManager, this);

        SAGE_LOG_INFO("Using config file: {}", IO::Path::Config());
    }

    ~GlobalVirtualConsole() override {
        cfg::setFileIOCallbacks(nullptr);
        cfg::setErrorCallback(nullptr, nullptr);
    }

    void print(const char* text) override {
        std::string_view view = text;
        for (auto pos = view.find('\n'); pos != std::string_view::npos; pos = view.find('\n')) {
            SAGE_LOG_DEBUG("[console] {}", view.substr(0, pos));
            view.remove_prefix(pos + 1);
        }
    }

    void printLn(const char* text) override {
        print(text);
    }

    bool handleKeyInput(int key, char chr) override {
        return cfg::SimpleCommandTerminal::handleKeyInput(key, chr);
    }

    void clear() override {
        cfg::SimpleCommandTerminal::clear();
    }

    void update() override {
        cfg::SimpleCommandTerminal::update();
    }

    void SyncWithFile() override {
        if (SDL::Get().FileExists(IO::Path::Config().data())) {
            ExecuteReloadConfig();
            ExecuteSaveConfig();
        } else {
            ExecuteSaveConfig();
            ExecuteReloadConfig();
        }
    }

    void ExecuteSaveConfig() {
        auto saveConfigCmd = fmt::format("saveConfig \"{}\"", IO::Path::Config());

        auto execMode = getCommandExecMode();
        setCommandExecMode(cfg::CommandExecMode::Immediate);
        execCmdLine(saveConfigCmd.c_str());
        setCommandExecMode(execMode);
    }

    void ExecuteReloadConfig() {
        auto reloadConfigCmd = fmt::format("reloadConfig \"{}\" -force", IO::Path::Config());

        auto execMode = getCommandExecMode();
        setCommandExecMode(cfg::CommandExecMode::Immediate);
        execCmdLine(reloadConfigCmd.c_str());
        setCommandExecMode(execMode);
    }
};

//
//
// Static
//
//

IVirtualConsole& IVirtualConsole::Get() {
    static GlobalVirtualConsole sVirtualConsole;
    return sVirtualConsole;
}

} // namespace Sage::Core::Console
