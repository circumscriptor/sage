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
/// @file BasicTypes.hpp
/// @brief Basic types
///
/// @copyright Copyright (c) 2022
///
///

#pragma once

namespace Sage {

//
//
// Signed integer types
//
//

using SInt8  = signed char;      //!< 8-bit signed integer
using SInt16 = signed short;     //!< 16-bit signed integer
using SInt32 = signed int;       //!< 32-bit signed integer
using SInt64 = signed long long; //!< 64-bit signed integer

//
//
// Unsigned integer types
//
//

using UInt8  = unsigned char;      //!< 8-bit unsigned integer
using UInt16 = unsigned short;     //!< 16-bit unsigned integer
using UInt32 = unsigned int;       //!< 32-bit unsigned integer
using UInt64 = unsigned long long; //!< 64-bit unsigned integer

//
//
// Floating point number types
//
//

using Float32 = float;  //!< 32-bit floating point number
using Float64 = double; //!< 64-bit floating point number

//
//
// Size type
//
//

///
/// @brief Size type (empty)
///
/// @tparam S Size of void pointer in bytes
///
template<unsigned S>
struct SizeType {};

///
/// @brief  Size type (32-bit)
///
template<>
struct SizeType<4> {
    using SignedType   = SInt32; //!< Signed type (32-bit)
    using UnsignedType = UInt32; //!< Unsigned type (32-bit)
};

///
/// @brief Size type (64-bit)
///
template<>
struct SizeType<8> {
    using SignedType   = SInt64; //!< Signed type (64-bit)
    using UnsignedType = UInt64; //!< Unsigned type (64-bit)
};

using PVoid = void*;                                 //!< Void pointer
using SSize = SizeType<sizeof(PVoid)>::SignedType;   //!< Size signed integer
using USize = SizeType<sizeof(PVoid)>::UnsignedType; //!< Size unsigned integer

//
//
// Alternative names
//
//

using Byte = UInt8; //!< 8-bit unsigned integer

//
//
// Static asserts
//
//

static_assert(sizeof(SInt8) == 1);
static_assert(sizeof(SInt16) == 2);
static_assert(sizeof(SInt32) == 4);
static_assert(sizeof(SInt64) == 8);

static_assert(sizeof(UInt8) == 1);
static_assert(sizeof(UInt16) == 2);
static_assert(sizeof(UInt32) == 4);
static_assert(sizeof(UInt64) == 8);

static_assert(sizeof(SSize) == sizeof(PVoid));
static_assert(sizeof(USize) == sizeof(PVoid));

} // namespace Sage
