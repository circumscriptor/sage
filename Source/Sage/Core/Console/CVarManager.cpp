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
/// @file CVarManager.cpp
/// @brief ...
///
/// @copyright Copyright (c) 2022
///
///

#include "CVarManager.hpp"

#include <libcfg/cfg.hpp>

#ifndef SAGE_CVAR_HASHTABLE_SIZE_HINT
    #define SAGE_CVAR_HASHTABLE_SIZE_HINT 0
#endif

namespace Sage::Core::Console {

CVarManager::CVarManager() : mCVarManager(cfg::CVarManager::createInstance(SAGE_CVAR_HASHTABLE_SIZE_HINT)) {}

CVarManager::~CVarManager() {
    cfg::CVarManager::destroyInstance(mCVarManager);
}

CVar CVarManager::Find(CStringType name) const {
    return CVar{mCVarManager->findCVar(name)};
}

CVar CVarManager::RegisterBool(CStringType        name,
                               CStringType        description,
                               Flags              flags,
                               BoolType           value,
                               const CVarManager* source) {
    if (source != nullptr) {
        if (auto cvar = source->Find(name); cvar) {
            value = cvar.GetBool();
        }
    }
    return CVar{mCVarManager->registerCVarBool(name, description, FlagsType(flags), value)};
}

CVar CVarManager::RegisterInt(CStringType        name,
                              CStringType        description,
                              Flags              flags,
                              IntType            value,
                              IntType            minValue,
                              IntType            maxValue,
                              const CVarManager* source) {
    if (source != nullptr) {
        if (auto cvar = source->Find(name); cvar) {
            value = cvar.GetInt();
        }
    }
    return CVar{mCVarManager->registerCVarInt(name, description, FlagsType(flags), value, minValue, maxValue)};
}

CVar CVarManager::RegisterFloat(CStringType        name,
                                CStringType        description,
                                Flags              flags,
                                FloatType          value,
                                FloatType          minValue,
                                FloatType          maxValue,
                                const CVarManager* source) {
    if (source != nullptr) {
        if (auto cvar = source->Find(name); cvar) {
            value = cvar.GetFloat();
        }
    }
    return CVar{mCVarManager->registerCVarFloat(name, description, FlagsType(flags), value, minValue, maxValue)};
}

CVar CVarManager::RegisterString(CStringType        name,
                                 CStringType        description,
                                 Flags              flags,
                                 const StringType&  value,
                                 CStringType*       allowedStrings,
                                 CompletionCallback callback,
                                 const CVarManager* source) {
    if (source != nullptr) {
        if (auto cvar = source->Find(name); cvar) {
            auto* result = mCVarManager->registerCVarString(name,
                                                            description,
                                                            FlagsType(flags),
                                                            cvar.GetString(),
                                                            allowedStrings,
                                                            callback);
            return CVar{result};
        }
    }
    return CVar{mCVarManager->registerCVarString(name, description, FlagsType(flags), value, allowedStrings, callback)};
}

CVar CVarManager::RegisterEnum(CStringType        name,
                               CStringType        description,
                               Flags              flags,
                               IntType            value,
                               const IntType*     enumConstants,
                               const CStringType* constNames,
                               const CVarManager* source) {
    if (source != nullptr) {
        if (auto cvar = source->Find(name); cvar) {
            value = cvar.GetInt();
        }
    }
    return CVar{mCVarManager->registerCVarEnum(name,
                                               description,
                                               FlagsType(flags),
                                               value,
                                               enumConstants,
                                               const_cast<CStringType*>(constNames))};
}

} // namespace Sage::Core::Console
