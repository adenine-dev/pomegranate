#pragma once

#include <cstdint>

/// @addtogroup maths
/// @{

/// @addtogroup types
/// @brief Pomegranate has various types that make using the maths slightly less verbose.
/// @{

/// @anchoredname{Fundamental types, fundamental_types}
/// These types are used throughout the codebase when a basic type is needed. Probably follow this
/// convention too.
/// @{

/// 8bit unsigned integer.
using u8 = uint8_t;
/// 16bit unsigned integer.
using u16 = uint16_t;
/// 32bit unsigned integer.
using u32 = uint32_t;
/// 64bit unsigned integer.
using u64 = uint64_t;

/// 8bit signed integer.
using i8 = int8_t;
/// 16bit signed integer.
using i16 = int16_t;
/// 32bit signed integer.
using i32 = int32_t;
/// 64bit signed integer.
using i64 = int64_t;

// BUG: ensure IEEE-754

/// 32bit floating point number, IEEE-754 compliant.
using f32 = float;
/// 64bit floating point number, IEEE-754 compliant.
using f64 = double;

/// @}
/// @}
/// @}