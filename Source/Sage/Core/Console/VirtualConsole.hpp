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

struct ICVarCollection {
    SAGE_CLASS_DELETE_COPY_AND_MOVE(ICVarCollection)

    ICVarCollection() = default;

    virtual ~ICVarCollection() = default;

    ///
    /// @brief Register CVars in CVar manager
    ///
    /// @param manager CVar manager
    /// @param flags Common flags set to each CVar
    /// @param source CVar manager to copy from
    ///
    virtual void Register(CVarManager& manager, CVar::Flags flags, const CVarManager* source) = 0;
};

class IVirtualConsole : public CVarManager {
  public:

    using ContextID = USize;

    enum Operation {
        kReload,
        kSave
    };

    SAGE_CLASS_DELETE_COPY_AND_MOVE(IVirtualConsole)

    IVirtualConsole();

    virtual ~IVirtualConsole();

    virtual void SyncWithFile(Operation operation) = 0;

    [[nodiscard]] virtual ContextID CreateContext() = 0;

    virtual void DestroyContext(ContextID context) = 0;

    virtual void RegisterPersistent(ICVarCollection& collection) = 0;

    virtual bool RegisterVolatile(ContextID context, ICVarCollection& collection) = 0;

    static std::shared_ptr<IVirtualConsole> CreateInstance();

  private:

    cfg::CommandManager* mCommandManager{nullptr};

  protected:

    [[nodiscard]] cfg::CommandManager* GetCommandManager() noexcept {
        return mCommandManager;
    }
};

} // namespace Sage::Core::Console
