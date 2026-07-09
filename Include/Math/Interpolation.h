#pragma once
#include "Math/Exponential.h"

namespace Crescent::Math {

template <typename Number> [[nodiscard]]
constexpr Number Lerp(Number a, Number b, Number t) {
	return a + t * (b - a);
}

template <typename Number> [[nodiscard]]
constexpr Number Clamp(Number number, Number min, Number max) {
	if (number < min) { return min; }
	if (max < number) { return max; }
	return number;
}

template <typename Number> [[nodiscard]]
constexpr Number SmoothStep(Number edge0, Number edge1, Number x) {
	Number t = Clamp((x - edge0) / (edge1 - edge0), static_cast<Number>(0), static_cast<Number>(1));
	return Power(t, 2) * (static_cast<Number>(3) - static_cast<Number>(2) * t);
}

}
