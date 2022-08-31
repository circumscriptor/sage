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
/// @file CVar.hpp
/// @brief ...
///
/// @copyright Copyright (c) 2022
///
///

#pragma once

#include <Sage/Core/BasicTypes.hpp>
#include <Sage/Core/ClassDefinitions.hpp>
#include <string>

namespace cfg {

class CVar;

} // namespace cfg

namespace Sage::Core::Console {

class CVar {
  public:

    SAGE_CLASS_DEFAULT_ALL(CVar)

    ///
    /// @brief Completion callback for valid CVar values
    ///
    ///
    using CompletionCallback = int (*)(const char*, std::string*, int);

    using FlagsType   = UInt32;
    using IntType     = SInt64;
    using BoolType    = bool;
    using FloatType   = Float64;
    using StringType  = std::string;
    using CStringType = const char*;

    ///
    /// @brief CVar flags
    ///
    ///
    enum Flags : FlagsType {
        Modified    = 0x01,
        Persistent  = 0x02,
        Volatile    = 0x04,
        ReadOnly    = 0x08,
        InitOnly    = 0x10,
        RangeCheck  = 0x20,
        UserDefined = 0x40
    };

    ///
    /// @brief Type of CVar, based on value stored
    ///
    ///
    enum Type {
        Int,
        Bool,
        Float,
        String,
        Enum,
        TypeInvalid
    };

    ///
    /// @brief Number format of CVar when converted between integer and string
    ///
    ///
    enum NumberFormat {
        Binary,
        Octal,
        Decimal,
        Hexadecimal,
        NumberFormatInvalid
    };

    ///
    /// @brief Get the name of the CVar
    ///
    /// @return Null-terminated string
    ///
    [[nodiscard]] CStringType GetName() const;

    ///
    /// @brief Get the description of the CVar
    ///
    /// @return Null-terminated string
    ///
    [[nodiscard]] CStringType GetDescription() const;

    ///
    /// @brief Get CVar flags
    ///
    /// @return Flags enum (bitfield)
    ///
    [[nodiscard]] Flags GetFlags() const;

    ///
    /// @brief Check CVar flags
    ///
    /// @param flags Flags enum (bitfield) - flags to check
    /// @return true if checked flags are set
    ///
    [[nodiscard]] bool HasFlags(Flags flags) const {
        return (FlagsType(GetFlags()) & FlagsType(flags)) == FlagsType(flags);
    }

    ///
    /// @brief Get CVar type
    ///
    /// @return Type
    ///
    [[nodiscard]] Type GetType() const;

    ///
    /// @brief Get CVar type as a string
    ///
    /// @return Null-terminated string
    ///
    [[nodiscard]] CStringType GetTypeString() const;

    ///
    /// @brief Get CVar number format
    ///
    /// @return Number format
    ///
    [[nodiscard]] NumberFormat GetNumberFormat() const;

    ///
    /// @brief Check whether modified flag is set
    ///
    /// @return true if modified flag is set
    ///
    [[nodiscard]] bool IsModified() const;

    ///
    /// @brief Clear modified flag
    ///
    ///
    void ClearModified();

    ///
    /// @brief Get CVar value as an integer
    ///
    /// @return Integer
    ///
    [[nodiscard]] IntType GetInt() const;

    ///
    /// @brief Get CVar value as a boolean
    ///
    /// @return Boolean
    ///
    [[nodiscard]] BoolType GetBool() const;

    ///
    /// @brief Get CVar value as a floating point number
    ///
    /// @return Floating point number
    ///
    [[nodiscard]] FloatType GetFloat() const;

    ///
    /// @brief Get CVar value as a string
    ///
    /// @return String
    ///
    [[nodiscard]] StringType GetString() const;

    ///
    /// @brief Set CVar value from integer
    ///
    /// @param value Integer
    /// @return true if operation succeeded
    ///
    bool SetInt(IntType value);

    ///
    /// @brief Set CVar value from boolean
    ///
    /// @param value Boolean
    /// @return true if operation succeeded
    ///
    bool SetBool(BoolType value);

    ///
    /// @brief Set CVar value from floating point number
    ///
    /// @param value Floating point number
    /// @return true if operation succeeded
    ///
    bool SetFloat(FloatType value);

    ///
    /// @brief Set CVar value from string
    ///
    /// @param value String
    /// @return true if operation succeeded
    ///
    bool SetString(StringType value);

    ///
    /// @brief Check whether stored CVar reference is valid
    ///
    /// @return true if stored CVar reference is valid
    ///
    explicit operator bool() const noexcept {
        return mCVar != nullptr;
    }

  private:

    friend class CVarManager;

    CVar(cfg::CVar* cvar) : mCVar{cvar} {}

    cfg::CVar* mCVar{nullptr}; //!< Pointer to CVar
};

inline CVar::Flags operator|(CVar::Flags flagA, CVar::Flags flagB) {
    return static_cast<CVar::Flags>(static_cast<CVar::FlagsType>(flagA) | static_cast<CVar::FlagsType>(flagB));
}

inline CVar::Flags operator&(CVar::Flags flagA, CVar::Flags flagB) {
    return static_cast<CVar::Flags>(static_cast<CVar::FlagsType>(flagA) & static_cast<CVar::FlagsType>(flagB));
}

} // namespace Sage::Core::Console
