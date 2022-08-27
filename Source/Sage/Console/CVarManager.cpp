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

#include "Internal/FileIOCallbacksSDL.hpp"
#include "Log.hpp"
#include "ThirdParty/cfg.hpp"

#include <memory>

#ifndef SAGE_CVAR_HASHTABLE_SIZE_HINT
    #define SAGE_CVAR_HASHTABLE_SIZE_HINT 0
#endif

namespace Sage::Console {

using namespace Internal;

CVarManager::CVarManager() :
    mCVarManager(cfg::CVarManager::createInstance(SAGE_CVAR_HASHTABLE_SIZE_HINT)),
    mCmdManager{cfg::CommandManager::createInstance(SAGE_CVAR_HASHTABLE_SIZE_HINT, mCVarManager)} {}

CVarManager::~CVarManager() {
    cfg::CommandManager::destroyInstance(mCmdManager);
    cfg::CVarManager::destroyInstance(mCVarManager);
}

CVar CVarManager::Get(CStringType name) {
    return CVar{mCVarManager->findCVar(name)};
}

CVar CVarManager::RegisterBool(CStringType name, CStringType description, Flags flags, BoolType value) {
    return mCVarManager->registerCVarBool(name, description, FlagsType(flags), value);
}

CVar CVarManager::RegisterInt(CStringType name,
                              CStringType description,
                              Flags       flags,
                              IntType     value,
                              IntType     minValue,
                              IntType     maxValue) {
    auto* cvar = mCVarManager->registerCVarInt(name, description, FlagsType(flags), value, minValue, maxValue);
    return CVar{cvar};
}

CVar CVarManager::RegisterFloat(CStringType name,
                                CStringType description,
                                Flags       flags,
                                FloatType   value,
                                FloatType   minValue,
                                FloatType   maxValue) {
    auto* cvar = mCVarManager->registerCVarFloat(name, description, FlagsType(flags), value, minValue, maxValue);
    return CVar{cvar};
}

CVar CVarManager::RegisterString(CStringType        name,
                                 CStringType        description,
                                 Flags              flags,
                                 const StringType&  value,
                                 CStringType*       allowedStrings,
                                 CompletionCallback callback) {
    auto* cvar = mCVarManager->registerCVarString(name, description, FlagsType(flags), value, allowedStrings, callback);
    return CVar{cvar};
}

CVar CVarManager::RegisterEnum(CStringType        name,
                               CStringType        description,
                               Flags              flags,
                               IntType            value,
                               const IntType*     enumConstants,
                               const CStringType* constNames) {
    auto* cvar = mCVarManager->registerCVarEnum(name,
                                                description,
                                                FlagsType(flags),
                                                value,
                                                enumConstants,
                                                const_cast<CStringType*>(constNames));
    return CVar{cvar};
}

} // namespace Sage::Console
