#pragma once
#include <Core/Core.h>
#include <Math/Constants.h>

namespace Crescent::Math {

template <typename Number> [[nodiscard]]
constexpr Number Floor(Number a) {
	i64 i_a = static_cast<i64>(a);
	if (a < static_cast<Number>(i_a)) {
		return static_cast<Number>(i_a - 1);
	}
	return static_cast<Number>(i_a);
}

template <typename Number> [[nodiscard]]
constexpr Number Ceil(Number a) {
	return -Floor(-a);
}

template <typename Number> [[nodiscard]]
constexpr Number Min(Number a, Number b) {
	if (a < b) {
		return a;
	}
	return b;
}

template <typename Number> [[nodiscard]]
constexpr Number Max(Number a, Number b) {
	if (a > b) {
		return a;
	}
	return b;
}

template <typename Number> [[nodiscard]]
constexpr Number Mod(Number a, Number b) {
	Number n = Floor(a / b);
	return a - n * b;
}

template <typename Number> [[nodiscard]]
constexpr Number Abs(Number number) {
	if constexpr (std::is_signed_v<Number>) {
		if (number < 0) {
			return -number;
		}
	}
	return number;
}

template <typename Number> [[nodiscard]]
constexpr Number IsNearlyZero(Number number) {
	return Abs(number) < Epsilon;
}

}
