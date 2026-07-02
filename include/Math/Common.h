#pragma once

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

inline constexpr f32 ToRadian(f32 degrees) {
	return degrees * (PI / 180.0f);
}

inline constexpr f32 Lerp(f32 a, f32 b, f32 t) {
	return a + t * (b - a);
}

inline constexpr f32 Clamp(f32 value, f32 min, f32 max) {
	if (value < min) { return min;}
	if (value > max) { return max; }
	return value;
}

inline constexpr f32 SmoothStep(f32 edge0, f32 edge1, f32 x) {
	f32 t = Clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
	return Power(t, 2) * (3.0f - 2.0f * t);
}

}