#pragma once

#include <cstdint>

using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;

using f32 = float;
using f64 = double;

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

inline constexpr u32 SCREEN_WIDTH = 1600;
inline constexpr u32 SCREEN_HEIGHT = 900;
inline constexpr f32 FIXED_TIMESTEP = 1.0f / 60.0f;
inline constexpr f32 MAX_ALLOWED_DELTA = 0.25f;