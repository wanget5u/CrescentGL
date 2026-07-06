#pragma once
#include <Core/Core.h>
#include "Core/Log.h"
#include "Math/Constants.h"
#include "Math/Arithmetic.h"
#include "Math/Trigonometry.h"
#include "Math/Exponential.h"
#include "Math/Interpolation.h"

namespace Crescent::Math {

struct Vector2 {
	union {
		struct { f32 x; f32 y; };
		f32 data[2];
	};

	constexpr Vector2() noexcept = default;
	explicit constexpr Vector2(f32 const x, f32 const y) noexcept
		: x(x), y(y) {}

	[[nodiscard]] static constexpr Vector2 Zero()		noexcept { return Vector2( 0.0f,  0.0f); }
	[[nodiscard]] static constexpr Vector2 One()		noexcept { return Vector2( 1.0f,  1.0f); }
	[[nodiscard]] static constexpr Vector2 Up()			noexcept { return Vector2( 0.0f,  1.0f); }
	[[nodiscard]] static constexpr Vector2 Down()		noexcept { return Vector2( 0.0f, -1.0f); }
	[[nodiscard]] static constexpr Vector2 Left()		noexcept { return Vector2(-1.0f,  0.0f); }
	[[nodiscard]] static constexpr Vector2 Right()		noexcept { return Vector2( 1.0f,  0.0f); }

	constexpr Vector2& operator+=(Vector2 const& other) noexcept {
		x += other.x; y += other.y;
		return *this;
	}
	[[nodiscard]] constexpr Vector2 operator+(Vector2 const& other) const noexcept {
		Vector2 result = *this;
		result += other;
		return result;
	}
	constexpr Vector2& operator-=(Vector2 const& other) noexcept {
		x -= other.x; y -= other.y;
		return *this;
	}
	[[nodiscard]] constexpr Vector2 operator-(Vector2 const& other) const noexcept {
		Vector2 result = *this;
		result -= other;
		return result;
	}
	constexpr Vector2& operator*=(f32 const scalar) noexcept {
		x *= scalar; y *= scalar;
		return *this;
	}
	[[nodiscard]] constexpr Vector2 operator*(f32 const scalar) const noexcept {
		Vector2 result = *this;
		result *= scalar;
		return result;
	}
	constexpr Vector2& operator*=(Vector2 const& other) noexcept {
		x *= other.x; y *= other.y;
		return *this;
	}
	[[nodiscard]] constexpr Vector2 operator*(Vector2 const& other) const noexcept {
		Vector2 result = *this;
		result *= other;
		return result;
	}
	constexpr Vector2& operator/=(f32 const scalar) noexcept {
		if (Math::IsNearlyZero(scalar)) {
			Log::Warning("Vector2 division scalar IsNearlyZero.");
			*this = Zero();
			return *this;
		}
		f32 invScalar = 1.0f / scalar;
		x *= invScalar; y *= invScalar;
		return *this;
	}
	[[nodiscard]] constexpr Vector2 operator/(f32 const scalar) const noexcept {
		Vector2 result = *this;
		result /= scalar;
		return result;
	}
	constexpr Vector2& operator/=(Vector2 const& other) noexcept {
		x = Math::IsNearlyZero(other.x) ? 0.0f : x / other.x;
		y = Math::IsNearlyZero(other.y) ? 0.0f : y / other.y;
		return *this;
	}
	[[nodiscard]] constexpr Vector2 operator/(Vector2 const& other) const noexcept {
		Vector2 result = *this;
		result /= other;
		return result;
	}
	[[nodiscard]] constexpr Vector2 operator-() const noexcept {
		return Vector2(-x, -y);
	}
	[[nodiscard]] constexpr bool operator==(Vector2 const& other) const noexcept {
		for (u8 a = 0; a < 2; a++) {
			if (Math::Abs(data[a] - other.data[a]) > Epsilon) {
				return false;
			}
		}
		return true;
	}
	[[nodiscard]] constexpr bool operator!=(Vector2 const& other) const noexcept {
		return !(*this == other);
	}
	[[nodiscard]] constexpr f32& operator[](size_t const index) noexcept {
		return data[index];
	}
	[[nodiscard]] constexpr f32 operator[](size_t const index) const noexcept {
		return data[index];
	}

	[[nodiscard]] static constexpr Vector2 Min(Vector2 const& a, Vector2 const& b) noexcept {
		return Vector2(Math::Min(a.x, b.x), Math::Min(a.y, b.y));
	}
	[[nodiscard]] static constexpr Vector2 Max(Vector2 const& a, Vector2 const& b) noexcept {
		return Vector2(Math::Max(a.x, b.x), Math::Max(a.y, b.y));
	}
	[[nodiscard]] static constexpr Vector2 Lerp(Vector2 const& start, Vector2 const& end, f32 const t) noexcept {
		return start + (end - start) * t;
	}
	[[nodiscard]] static constexpr f32 Distance(Vector2 const& a, Vector2 const& b) noexcept {
		return (a - b).Length();
	}
	[[nodiscard]] static constexpr f32 DistanceSquared(Vector2 const& a, Vector2 const& b) noexcept {
		return (a - b).LengthSquared();
	}
	[[nodiscard]] static constexpr Vector2 Clamp(Vector2 const& value, Vector2 const& min, Vector2 const& max) noexcept {
		return Vector2(
			Math::Clamp(value.x, min.x, max.x),
			Math::Clamp(value.y, min.y, max.y)
		);
	}
	[[nodiscard]] static f32 AngleBetween(Vector2 const& a, Vector2 const& b) noexcept {
		const f32 lengthProduct = a.Length() * b.Length();
		if (Math::IsNearlyZero(lengthProduct) == true) {
			return 0.0f;
		}
		const f32 cosTheta = a.Dot(b) / lengthProduct;
		const f32 clampedCos = Math::Clamp(cosTheta, -1.0f, 1.0f);
		return ArcCos(clampedCos);
	}
	[[nodiscard]] static f32 AngleBetweenNormalized(Vector2 const& a, Vector2 const& b) noexcept {
		return ArcCos(Math::Clamp(a.Dot(b), -1.0f, 1.0f));
	}

	[[nodiscard]] constexpr f32 Dot(Vector2 const& other) const noexcept {
		return x * other.x + y * other.y;
	}
	[[nodiscard]] constexpr f32 Cross(Vector2 const& other) const noexcept {
		return x * other.y - y * other.x;
	}
	[[nodiscard]] constexpr f32 Length() const noexcept {
		return Sqrt(Power(x, 2) + Power(y, 2));
	}
	[[nodiscard]] constexpr f32 LengthSquared() const noexcept {
		return Power(x, 2) + Power(y, 2);
	}
	[[nodiscard]] constexpr Vector2 Normalized() const noexcept {
		f32 length = Length();
		if (IsNearlyZero() == false) {
			f32 invLength = 1.0f / length;
			return Vector2(x * invLength, y * invLength);
		}
		return Zero();
	}
	[[nodiscard]] constexpr bool IsNearlyZero() const noexcept {
		return LengthSquared() <= Power(Epsilon, 2);
	}
	[[nodiscard]] constexpr Vector2 Abs() const noexcept {
		return Vector2(x < 0.0f ? -x : x, y < 0.0f ? -y : y);
	}
	constexpr void Normalize() noexcept {
		*this = Normalized();
	}
};

constexpr inline Vector2 operator*(f32 const scalar, Vector2 const& vector) noexcept {
	return vector * scalar;
}

}
