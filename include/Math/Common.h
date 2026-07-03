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
inline constexpr Number Clamp(Number value, Number min, Number max) {
	if (value < min) { return min;}
	if (value > max) { return max; }
	return value;
}

template <typename Number>
inline constexpr Number SmoothStep(Number edge0, Number edge1, Number x) {
	Number t = Clamp((x - edge0) / (edge1 - edge0), static_cast<Number>(0), static_cast<Number>(1));
	return Power(t, 2) * (static_cast<Number>(3) - static_cast<Number>(2) * t);
}

}