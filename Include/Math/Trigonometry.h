#pragma once
#include "Core/Core.h"
#include "Math/Constants.h"
#include "Math/Exponential.h"

namespace Crescent::Math {

template <typename Number> [[nodiscard]]
constexpr Number DegreesToRadians(Number degrees) {
	return degrees * (static_cast<Number>(PI) / static_cast<Number>(180));
}

template <typename Number> [[nodiscard]]
constexpr Number RadiansToDegrees(Number radians) {
	return radians * (static_cast<Number>(180) / static_cast<Number>(PI));
}

template <typename Number>
constexpr void NormalizeAngle(Number& radians) {
	const Number pi = static_cast<Number>(PI);
	const Number twoPi = static_cast<Number>(TWO_PI);
	const Number invTwoPi = static_cast<Number>(0.15915494309189533576888376337251);
	radians -= static_cast<Number>(static_cast<i64>(radians * invTwoPi)) * twoPi;
	while (radians > pi) { radians -= twoPi; }
	while (radians < -pi) { radians += twoPi; }
}

template <typename Number> [[nodiscard]]
constexpr Number Sin(Number radians) noexcept {
	NormalizeAngle(radians);
	const Number y = Power(radians, 2);
	// 10-term Maclaurin expansion using Horner's method (degree 19)
	// Eliminates all loops/divisions and cuts multiplications in half
	return radians * (static_cast<Number>(1.0) + y * (
		static_cast<Number>(-0.16666666666666666667) + y * (
		static_cast<Number>(0.00833333333333333333) + y * (
		static_cast<Number>(-0.00019841269841269841) + y * (
		static_cast<Number>(0.00000275573192239859) + y * (
		static_cast<Number>(-2.5052108385441719e-8) + y * (
		static_cast<Number>(1.6059043836821615e-10) + y * (
		static_cast<Number>(-7.6471637318198164e-13) + y * (
		static_cast<Number>(2.8114572543455208e-15) + y *
		static_cast<Number>(-8.2206352466243300e-18)
		)))))))));
}

template <typename Number> [[nodiscard]]
constexpr Number Cos(Number radians) {
	NormalizeAngle(radians);
	const Number y = Power(radians, 2);
	// 10-term Maclaurin expansion using Horner's method (degree 18)
	// Eliminates all loops/divisions and cuts multiplications in half
	return static_cast<Number>(1.0) + y * (
		static_cast<Number>(-0.5) + y * (
		static_cast<Number>(0.04166666666666666667) + y * (
		static_cast<Number>(-0.00138888888888888889) + y * (
		static_cast<Number>(0.00002480158730158730) + y * (
		static_cast<Number>(-2.7557319223985893e-7) + y * (
		static_cast<Number>(2.0876756987868100e-9) + y * (
		static_cast<Number>(-1.1470745597729725e-11) + y * (
		static_cast<Number>(4.7794773323873850e-14) + y *
		static_cast<Number>(-1.5619206968586220e-16)
		))))))));
}

template <typename Number> [[nodiscard]]
constexpr Number ArcCos(Number radians) noexcept {
	if (radians <= static_cast<Number>(-1.0)) { return static_cast<Number>(PI); }
	if (radians >= static_cast<Number>(1.0)) { return static_cast<Number>(0.0); }
	const Number x = Abs(radians);
	// Abramowitz & Stegun 4.4.45 minimax polynomial approximation using Horner's method
	const Number result = Sqrt(static_cast<Number>(1.0) - x) * (
		static_cast<Number>(1.5707963050) + x * (
		static_cast<Number>(-0.2145988016) + x * (
		static_cast<Number>(0.0889789874) + x * (
		static_cast<Number>(-0.0501743046) + x *
		static_cast<Number>(0.0308918810)
		))));
	if (radians < 0) {
		return static_cast<Number>(PI) - result;
	}
	return result;
}

template <typename Number> [[nodiscard]]
constexpr Number ArcSin(Number radians) noexcept {
	if (radians <= static_cast<Number>(-1.0)) { return static_cast<Number>(-PI / 2.0); }
	if (radians >= static_cast<Number>(1.0)) { return static_cast<Number>(PI / 2.0); }
	const Number halfPi = static_cast<Number>(PI) / static_cast<Number>(2.0);
	return halfPi - Math::ArcCos(radians);
}

template <typename Number> [[nodiscard]]
constexpr Number Tan(Number radians) {
	const Number cos = Math::Cos(radians);
	if (IsNearlyZero(cos) == true) {
		return static_cast<Number>(0.0);
	}
	return Math::Sin(radians) / cos;
}

template <typename Number> [[nodiscard]]
constexpr Number ArcTan(Number value) noexcept {
	const Number absValue = Abs(value);
	const bool greaterThanOne = (absValue > static_cast<Number>(1.0));
	const Number z = greaterThanOne ? (static_cast<Number>(1.0) / absValue) : absValue;
	const Number y = Power(z, 2);
	// Abramowitz & Stegun 4.4.47 minimax approximation using Horner's method
	Number result = z * (static_cast<Number>(1.0) + y * (
	   static_cast<Number>(-0.3333314528) + y * (
	   static_cast<Number>(0.1999355085) + y * (
	   static_cast<Number>(-0.1420889944) + y * (
	   static_cast<Number>(0.1065626393) + y * (
	   static_cast<Number>(-0.0752896400) + y * (
	   static_cast<Number>(0.0429096138) + y * (
	   static_cast<Number>(-0.0161657367) + y *
	   static_cast<Number>(0.0028662257)
	   ))))))));
	if (greaterThanOne == true) {
		result =  static_cast<Number>(HALF_PI - result);
	}
	return value < static_cast<Number>(0.0) ? -result : result;
}

template <typename Number> [[nodiscard]]
constexpr Number ArcTan2(Number y, Number x) noexcept {
	const Number pi = static_cast<Number>(PI);
	const Number halfPi = static_cast<Number>(HALF_PI);
	if (x > static_cast<Number>(0.0)) {
		return Math::ArcTan(y / x);
	}
	if (x < static_cast<Number>(0.0)) {
		if (y >= static_cast<Number>(0.0)) {
			return Math::ArcTan(y / x) + pi;
		}
		return Math::ArcTan(y / x) - pi;
	}
	if (y > static_cast<Number>(0.0)) {
		return halfPi;
	}
	if (y < static_cast<Number>(0.0)) {
		return -halfPi;
	}
	return static_cast<Number>(0.0);
}

}
