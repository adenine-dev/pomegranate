#pragma once

// Contains things that should be everywhere, this should be included in every file.
#include <cstdint>

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

// NOTE: assumes IEEE-754, to test this uncomment the next lines
//  static_assert(std::numeric_limits<float>::is_iec559 && std::numeric_limits<double>::is_iec559,
//                "floating point types are not IEEE-754 compliant");

using f32 = float;  ///< IEEE-754 compliant 32 bit floating point type.
using f64 = double; ///< IEEE-754 compliant 64 bit floating point type.

using byte = unsigned char; ///< Unsigned byte type.
using usize = size_t;       ///< Unsigned size type.

#include "debug/logging.hpp"
