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
/// @file VirtualConsole.hpp
/// @brief ...
///
/// @copyright Copyright (c) 2022
///
///

#pragma once

#include "CVarManager.hpp"

#include <memory>
#include <string>

namespace Sage::Core::Console {

class VirtualConsole {
  public:

    VirtualConsole() = default;

    SAGE_CLASS_DELETE_COPY_AND_MOVE(VirtualConsole)

    virtual ~VirtualConsole() = default;

    static bool Initialize();

    static void Shutdown();

    static void LoadConfig();

    static std::shared_ptr<CVarManager> GetCVarManager() {
        return sCVarManager;
    }

    static std::shared_ptr<VirtualConsole> Get() {
        return sVirtualConsole;
    }

  private:

    virtual void SaveConfig(const std::string& file) = 0;

    virtual void ReloadConfig(const std::string& file) = 0;

    static std::shared_ptr<CVarManager> sCVarManager;

    static std::shared_ptr<VirtualConsole> sVirtualConsole;
};

} // namespace Sage::Core::Console

#define SAGE_GET_CVAR(Name) Sage::Core::Console::VirtualConsole::GetCVarManager()->Get(Name)

#define SAGE_REGISTER_CVAR(Type, Name, Description, Flags, ...)                                                        \
    Sage::Core::Console::VirtualConsole::GetCVarManager()->Register##Type(Name, Description, Flags, __VA_ARGS__)

#define SAGE_REGISTER_CVAR_INT(Name, Description, Flags, ...)                                                          \
    SAGE_REGISTER_CVAR(Int, Name, Description, Flags, __VA_ARGS__)

#define SAGE_REGISTER_CVAR_BOOL(Name, Description, Flags, ...)                                                         \
    SAGE_REGISTER_CVAR(Bool, Name, Description, Flags, __VA_ARGS__)

#define SAGE_REGISTER_CVAR_FLOAT(Name, Description, Flags, ...)                                                        \
    SAGE_REGISTER_CVAR(Float, Name, Description, Flags, __VA_ARGS__)

#define SAGE_REGISTER_CVAR_STRING(Name, Description, Flags, ...)                                                       \
    SAGE_REGISTER_CVAR(String, Name, Description, Flags, __VA_ARGS__)

#define SAGE_REGISTER_CVAR_ENUM(Name, Description, Flags, ...)                                                         \
    SAGE_REGISTER_CVAR(Enum, Name, Description, Flags, __VA_ARGS__)
