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
/// @file CVarManager.hpp
/// @brief ...
///
/// @copyright Copyright (c) 2022
///
///

#pragma once

#include "CVar.hpp"

#include <memory>
#include <string_view>

namespace cfg {

class CVarManager;
class CommandManager;

} // namespace cfg

namespace Sage::Core::Console {

///
/// @brief Interface for CVar manager
///
///
class CVarManager {
    friend class VirtualConsole;

  public:

    using Flags              = CVar::Flags;
    using FlagsType          = CVar::FlagsType;
    using IntType            = CVar::IntType;
    using BoolType           = CVar::BoolType;
    using FloatType          = CVar::FloatType;
    using StringType         = CVar::StringType;
    using CStringType        = CVar::CStringType;
    using CompletionCallback = CVar::CompletionCallback;

    SAGE_CLASS_DELETE_COPY_AND_MOVE(CVarManager)

    ///
    /// @brief Construct a new CVarManager object
    ///
    ///
    CVarManager();

    ///
    /// @brief Destroy the CVarManager object
    ///
    ///
    ~CVarManager();

    ///
    /// @brief Check whether CVar manager is valid after construction
    ///
    /// @return true if CVar manager is valid
    ///
    [[nodiscard]] bool IsValid() const {
        return bool(mCVarManager) && bool(mCmdManager);
    }

    ///
    /// @brief Get CVar variable by name
    ///
    /// @param name Name of the variable
    /// @return CVar variable
    ///
    CVar Get(CStringType name);

    ///
    /// @brief Register CVar, integer type (signed 64-bit)
    ///
    /// @param name Name of the CVar
    /// @param description Description of the CVar
    /// @param flags CVar access flags
    /// @param value Initial value
    /// @param minValue Minimal value
    /// @param maxValue Maximal value
    /// @return CVar variable
    ///
    CVar RegisterInt(CStringType name,
                     CStringType description,
                     Flags       flags,
                     IntType     value,
                     IntType     minValue,
                     IntType     maxValue);

    ///
    /// @brief Register CVar, boolean type
    ///
    /// @param name Name of the CVar
    /// @param description Description of the CVar
    /// @param flags CVar access flags
    /// @param value Initial value
    /// @return CVar variable
    ///
    CVar RegisterBool(CStringType name, //
                      CStringType description,
                      Flags       flags,
                      BoolType    value);

    ///
    /// @brief Register CVar, floating point type (double)
    ///
    /// @param name Name of the CVar
    /// @param description Description of the CVar
    /// @param flags CVar access flags
    /// @param value Initial value
    /// @param minValue Minimal value
    /// @param maxValue Maximal value
    /// @return CVar variable
    ///
    CVar RegisterFloat(CStringType name,
                       CStringType description,
                       Flags       flags,
                       FloatType   value,
                       FloatType   minValue,
                       FloatType   maxValue);

    ///
    /// @brief Register CVar, string type
    ///
    /// @param name Name of the CVar
    /// @param description Description of the CVar
    /// @param flags CVar access flags
    /// @param value Initial value
    /// @param allowedStrings List of the allowed strings
    /// @param completionCallback Completion callback (use if allowed strings is nullptr)
    /// @return CVar variable
    ///
    CVar RegisterString(CStringType        name,
                        CStringType        description,
                        Flags              flags,
                        const StringType&  value,
                        CStringType*       allowedStrings,
                        CompletionCallback completionCallback = nullptr);

    ///
    /// @brief Register CVar, enumeration type
    ///
    /// @param name Name of the CVar
    /// @param description Description of the CVar
    /// @param flags CVar access flags
    /// @param value Initial value (integer)
    /// @param enumConstants Enumeration constants in integer format
    /// @param constNames Enumeration constants in string format
    /// @return CVar variable
    ///
    /// @note Order of integer and string constants must match
    ///
    CVar RegisterEnum(CStringType        name,
                      CStringType        description,
                      Flags              flags,
                      IntType            value,
                      const IntType*     enumConstants,
                      const CStringType* constNames);

    // TODO: Register commands

  private:

    cfg::CVarManager*    mCVarManager;
    cfg::CommandManager* mCmdManager;
};

} // namespace Sage::Core::Console
