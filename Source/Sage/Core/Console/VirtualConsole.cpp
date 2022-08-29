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
#include "ThirdParty/cfg.hpp"

#include <Sage/Core/IO/Internal/FileIOCallbacksSDL.hpp>
#include <Sage/Core/IO/Path.hpp>
#include <iostream>

#ifndef SAGE_VIRTUAL_CONSOLE_MARK
    #define SAGE_VIRTUAL_CONSOLE_MARK "> "
#endif

#ifndef SAGE_CONFIG_FILE_NAME
    #define SAGE_CONFIG_FILE_NAME "sage.cfg"
#endif

namespace Sage::Core::Console {

class VirtualConsoleImpl : public VirtualConsole, public cfg::SimpleCommandTerminal {
  public:

    SAGE_CLASS_DELETE_COPY_AND_MOVE(VirtualConsoleImpl)

    VirtualConsoleImpl(cfg::CommandManager* cmdManager, cfg::CVarManager* cvarManager, const char* newLineMark) :
        cfg::SimpleCommandTerminal(cmdManager, cvarManager, newLineMark) {}

    ~VirtualConsoleImpl() override = default;

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

    void SaveConfig(const std::string& file) override {
        auto saveConfigCmd = fmt::format("saveConfig \"{}\"", file);

        auto execMode = getCommandExecMode();
        setCommandExecMode(cfg::CommandExecMode::Immediate);
        execCmdLine(saveConfigCmd.c_str());
        setCommandExecMode(execMode);
    }

    void ReloadConfig(const std::string& file) override {
        // TODO: Check files end with cfg or ini
        auto reloadConfigCmd = fmt::format("reloadConfig \"{}\" -force", file);

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

static void ErrorCallback(const char* message, [[maybe_unused]] void* user) {
    SAGE_LOG_ERROR(message);
}

std::shared_ptr<CVarManager>    VirtualConsole::sCVarManager;
std::shared_ptr<VirtualConsole> VirtualConsole::sVirtualConsole;

bool VirtualConsole::Initialize() {
    static IO::Internal::FileIOCallbacksSDL sFileIO;
    cfg::setErrorCallback(ErrorCallback, nullptr);
    cfg::setFileIOCallbacks(&sFileIO);

    sCVarManager = std::make_shared<CVarManager>();

    auto virtualConsoleImpl = std::make_shared<VirtualConsoleImpl>(sCVarManager->mCmdManager,
                                                                   sCVarManager->mCVarManager,
                                                                   SAGE_VIRTUAL_CONSOLE_MARK);

    sVirtualConsole = virtualConsoleImpl;

    cfg::registerDefaultCommands(sCVarManager->mCmdManager, virtualConsoleImpl.get());
    return true;
}

void VirtualConsole::Shutdown() {
    sVirtualConsole.reset();
    sCVarManager.reset();

    cfg::setFileIOCallbacks(nullptr);
    cfg::setErrorCallback(nullptr, nullptr);
}

void VirtualConsole::LoadConfig() {
    std::string configFile = fmt::format("{}{}", Sage::Core::IO::Path::User(), SAGE_CONFIG_FILE_NAME);
    SAGE_LOG_INFO("Using config file: {}", configFile);

    if (auto* RWops = SDL_RWFromFile(configFile.c_str(), "r"); RWops == nullptr) {
        SAGE_LOG_INFO("Creating default config file. Path: {}", configFile);
        Sage::Core::Console::VirtualConsole::Get()->SaveConfig(configFile);
    } else {
        SDL_RWclose(RWops);
    }

    Sage::Core::Console::VirtualConsole::Get()->ReloadConfig(configFile);
}

} // namespace Sage::Core::Console
