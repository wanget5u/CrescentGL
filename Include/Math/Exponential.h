#pragma once
#include "Core/Core.h"

namespace Crescent::Math {

template <typename Number> [[nodiscard]]
constexpr Number Power(Number value, u32 power) {
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

template <typename Number> [[nodiscard]]
constexpr Number InvSqrt(Number number) {
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
		y = y * (1.5f - (x2 * y * y));
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
		y = y * (1.5 - (x2 * y * y));
		y = y * (1.5 - (x2 * y * y));
		y = y * (1.5 - (x2 * y * y));
		return static_cast<Number>(y);
	}
	return 0;
}

template <typename Number> [[nodiscard]]
constexpr Number Sqrt(Number number) {
	return number * InvSqrt(number);
}

}
