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
/// @file CVar.cpp
/// @brief ...
///
/// @copyright Copyright (c) 2022
///
///

#include "CVar.hpp"

#include <array>
#include <libcfg/cfg.hpp>
#include <memory>

namespace Sage::Core::Console {

//
//
// CVar
//
//

CVar::CStringType CVar::GetName() const {
    return mCVar->getNameCString();
}

CVar::CStringType CVar::GetDescription() const {
    return mCVar->getDescCString();
}

CVar::Flags CVar::GetFlags() const {
    return CVar::Flags(mCVar->getFlags());
}

CVar::Type CVar::GetType() const {
    switch (mCVar->getType()) {
        case cfg::CVar::Type::Int:
            return CVar::Int;
        case cfg::CVar::Type::Bool:
            return CVar::Bool;
        case cfg::CVar::Type::Float:
            return CVar::Float;
        case cfg::CVar::Type::String:
            return CVar::String;
        case cfg::CVar::Type::Enum:
            return CVar::Enum;
    }
    return CVar::TypeInvalid;
}

CVar::CStringType CVar::GetTypeString() const {
    return mCVar->getTypeCString();
}

CVar::NumberFormat CVar::GetNumberFormat() const {
    switch (mCVar->getNumberFormat()) {
        case cfg::CVar::NumberFormat::Binary:
            return CVar::Binary;
        case cfg::CVar::NumberFormat::Octal:
            return CVar::Octal;
        case cfg::CVar::NumberFormat::Decimal:
            return CVar::Decimal;
        case cfg::CVar::NumberFormat::Hexadecimal:
            return CVar::Hexadecimal;
    }
    return CVar::NumberFormatInvalid;
}

bool CVar::IsModified() const {
    return mCVar->isModified();
}

void CVar::ClearModified() {
    return mCVar->clearModified();
}

CVar::IntType CVar::GetInt() const {
    return mCVar->getIntValue();
}

CVar::BoolType CVar::GetBool() const {
    return mCVar->getBoolValue();
}

CVar::FloatType CVar::GetFloat() const {
    return mCVar->getFloatValue();
}

CVar::StringType CVar::GetString() const {
    return mCVar->getStringValue();
}

bool CVar::SetInt(IntType value) {
    return mCVar->setIntValue(value);
}

bool CVar::SetBool(BoolType value) {
    return mCVar->setBoolValue(value);
}

bool CVar::SetFloat(FloatType value) {
    return mCVar->setFloatValue(value);
}

bool CVar::SetString(StringType value) {
    return mCVar->setStringValue(std::move(value));
}

} // namespace Sage::Core::Console
