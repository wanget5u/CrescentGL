#pragma once
#include <Core/Core.h>

namespace Crescent::Math {

constexpr f32 PI = 3.1415926535f;

template <typename Number>
inline constexpr Number Power(Number value, u32 power) {
	if (power == 0) {
		return static_cast<Number>(1);
	}
	Number result = static_cast<Number>(1);
	Number base = value;
	while (power > 0) {
		if (power & 1) {
			result *= base;
		}
		base *= base;
		power >>= 1; // division by 2
	}
	return result;
}

template <typename Number>
inline constexpr Number Floor(Number a) {
	i64 i_a = static_cast<i64>(a);
	if (a < static_cast<Number>(i_a)) {
		return static_cast<Number>(i_a - 1);
	}
	return static_cast<Number>(i_a);
}

template <typename Number>
inline constexpr Number Ceil(Number a) {
	return -Floor(-a);
}

template <typename Number>
inline constexpr Number Min(Number a, Number b) {
	if (a < b) {
		return a;
	}
	return b;
}

template <typename Number>
inline constexpr Number Max(Number a, Number b) {
	if (a > b) {
		return a;
	}
	return b;
}

template <typename Number>
inline constexpr Number Mod(Number a, Number b) {
	Number n = Floor(a / b);
	return a - n * b;
}

template <typename Number>
inline constexpr Number ToRadian(Number degrees) {
	return degrees * (static_cast<Number>(PI) / static_cast<Number>(180));
}

template <typename Number>
inline constexpr Number Lerp(Number a, Number b, Number t) {
	return a + t * (b - a);
}

template <typename Number>
inline constexpr Number Clamp(Number number, Number min, Number max) {
	if (number < min) { return min;}
	if (number > max) { return max; }
	return number;
}

template <typename Number>
inline constexpr Number SmoothStep(Number edge0, Number edge1, Number x) {
	Number t = Clamp((x - edge0) / (edge1 - edge0), static_cast<Number>(0), static_cast<Number>(1));
	return Power(t, 2) * (static_cast<Number>(3) - static_cast<Number>(2) * t);
}

template <typename Number>
inline constexpr Number InvSqrt(Number number) {
	if (number <= 0) {
		return 0;
	}

	if constexpr (sizeof(Number) == 4) {
		// 32-bit float logic
		f32 x2 = number * 0.5f;
		f32 y = number;
		u32 i = std::bit_cast<u32>(y);
		i = 0x5f3759df - (i >> 1);
		y = std::bit_cast<f32>(i);
		y = y * (1.5f - (x2 * y * y));
		return static_cast<Number>(y);
	}
	else if constexpr (sizeof(Number) == 8) {
		// 64-bit float logic
		f64 x2 = number * 0.5;
		f64 y = number;
		u64 i = std::bit_cast<u64>(y);
		i = 0x5fe6eb50c7b537a9 - (i >> 1);
		y = std::bit_cast<f64>(i);
		y = y * (1.5 - (x2 * y * y));
		return static_cast<Number>(y);
	}
}

template <typename Number>
inline constexpr Number Sqrt(Number number) {
	return number * InvSqrt(number);
}

}