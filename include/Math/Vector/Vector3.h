#pragma once
#include <Core/Core.h>
#include "Core/Log.h"
#include "Math/Constants.h"
#include "Math/Trigonometry.h"
#include "Math/Arithmetic.h"
#include "Math/Exponential.h"
#include "Math/Interpolation.h"
#include "Vector2.h"

namespace Crescent::Math {

struct Vector3 {
	union {
		struct { f32 x; f32 y; f32 z; };
		f32 data[3];
	};

	constexpr Vector3() noexcept = default;
	explicit constexpr Vector3(f32 const x, f32 const y, f32 const z) noexcept
		: x(x), y(y), z(z) {}
	explicit constexpr Vector3(Vector2 const& xy, f32 const z = 0.0f) noexcept
		: x(xy.x), y(xy.y), z(z) {}
	explicit constexpr Vector3(f32 const x, Vector2 const& yz) noexcept
		: x(x), y(yz.x), z(yz.y) {}

	[[nodiscard]] static constexpr Vector3 Zero()		noexcept { return Vector3( 0.0f,  0.0f,  0.0f); }
	[[nodiscard]] static constexpr Vector3 One()		noexcept { return Vector3( 1.0f,  1.0f,  1.0f); }
	[[nodiscard]] static constexpr Vector3 Up()			noexcept { return Vector3( 0.0f,  1.0f,  0.0f); }
	[[nodiscard]] static constexpr Vector3 Down()		noexcept { return Vector3( 0.0f, -1.0f,  0.0f); }
	[[nodiscard]] static constexpr Vector3 Left()		noexcept { return Vector3(-1.0f,  0.0f,  0.0f); }
	[[nodiscard]] static constexpr Vector3 Right()		noexcept { return Vector3( 1.0f,  0.0f,  0.0f); }
	[[nodiscard]] static constexpr Vector3 Forward()	noexcept { return Vector3( 0.0f,  0.0f,  1.0f); }
	[[nodiscard]] static constexpr Vector3 Back()		noexcept { return Vector3( 0.0f,  0.0f, -1.0f); }

	constexpr Vector3& operator+=(Vector3 const& other) noexcept {
		x += other.x; y += other.y; z += other.z;
		return *this;
	}
	[[nodiscard]] constexpr Vector3 operator+(Vector3 const& other) const noexcept {
		Vector3 result = *this;
		result += other;
		return result;
	}
	constexpr Vector3& operator-=(Vector3 const& other) noexcept {
		x -= other.x; y -= other.y; z -= other.z;
		return *this;
	}
	[[nodiscard]] constexpr Vector3 operator-(Vector3 const& other) const noexcept {
		Vector3 result = *this;
		result -= other;
		return result;
	}
	constexpr Vector3& operator*=(f32 const scalar) noexcept {
		x *= scalar; y *= scalar; z *= scalar;
		return *this;
	}
	constexpr Vector3& operator*=(Vector3 const& other) noexcept {
		x *= other.x; y *= other.y; z *= other.z;
		return *this;
	}
	[[nodiscard]] constexpr Vector3 operator*(f32 const scalar) const noexcept {
		Vector3 result = *this;
		result *= scalar;
		return result;
	}
	[[nodiscard]] constexpr Vector3 operator*(Vector3 const& other) const noexcept {
		Vector3 result = *this;
		result *= other;
		return result;
	}
	constexpr Vector3& operator/=(f32 const scalar) noexcept {
		if (Math::IsNearlyZero(scalar)) {
			Log::Warning("Vector3 division scalar IsNearlyZero.");
			*this = Zero();
			return *this;
		}
		f32 invScalar = 1.0f / scalar;
		x *= invScalar; y *= invScalar; z *= invScalar;
		return *this;
	}
	[[nodiscard]] constexpr Vector3 operator/(f32 const scalar) const noexcept {
		Vector3 result = *this;
		result /= scalar;
		return result;
	}
	constexpr Vector3& operator/=(Vector3 const& other) noexcept {
		x = Math::IsNearlyZero(other.x) ? 0.0f : x / other.x;
		y = Math::IsNearlyZero(other.y) ? 0.0f : y / other.y;
		z = Math::IsNearlyZero(other.z) ? 0.0f : z / other.z;
		return *this;
	}
	[[nodiscard]] constexpr Vector3 operator/(Vector3 const& other) const noexcept {
		Vector3 result = *this;
		result /= other;
		return result;
	}
	[[nodiscard]] constexpr Vector3 operator-() const noexcept {
		return Vector3(-x, -y, -z);
	}
	[[nodiscard]] constexpr bool operator==(Vector3 const& other) const noexcept {
		for (u8 a = 0; a < 3; a++) {
			if (Math::Abs(data[a] - other.data[a]) > Epsilon) {
				return false;
			}
		}
		return true;
	}
	[[nodiscard]] constexpr bool operator!=(Vector3 const& other) const noexcept {
		return !(*this == other);
	}
	[[nodiscard]] constexpr f32& operator[](size_t const index) noexcept {
		return data[index];
	}
	[[nodiscard]] constexpr f32 operator[](size_t const index) const noexcept {
		return data[index];
	}

	[[nodiscard]] static constexpr Vector3 Min(Vector3 const& a, Vector3 const& b) noexcept {
		return Vector3(Math::Min(a.x, b.x), Math::Min(a.y, b.y), Math::Min(a.z, b.z));
	}
	[[nodiscard]] static constexpr Vector3 Max(Vector3 const& a, Vector3 const& b) noexcept {
		return Vector3(Math::Max(a.x, b.x), Math::Max(a.y, b.y), Math::Max(a.z, b.z));
	}
	[[nodiscard]] static constexpr Vector3 Lerp(Vector3 const& start, Vector3 const& end, f32 const t) noexcept {
		return start + (end - start) * t;
	}
	[[nodiscard]] static constexpr f32 Distance(Vector3 const& a, Vector3 const& b) noexcept {
		return (a - b).Length();
	}
	[[nodiscard]] static constexpr f32 DistanceSquared(Vector3 const& a, Vector3 const& b) noexcept {
		return (a - b).LengthSquared();
	}
	[[nodiscard]] static constexpr Vector3 Clamp(Vector3 const& value, Vector3 const& min, Vector3 const& max) noexcept {
		return Vector3(
			Math::Clamp(value.x, min.x, max.x),
			Math::Clamp(value.y, min.y, max.y),
			Math::Clamp(value.z, min.z, max.z)
		);
	}
	[[nodiscard]] static f32 AngleBetween(Vector3 const& a, Vector3 const& b) noexcept {
		const f32 lengthProduct = a.Length() * b.Length();
		if (Math::IsNearlyZero(lengthProduct) == true) {
			return 0.0f;
		}
		const f32 cosTheta = a.Dot(b) / lengthProduct;
		const f32 clampedCos = Math::Clamp(cosTheta, -1.0f, 1.0f);
		return Math::ArcCos(clampedCos);
	}
	[[nodiscard]] static f32 AngleBetweenNormalized(Vector3 const& a, Vector3 const& b) noexcept {
		return Math::ArcCos(Math::Clamp(a.Dot(b), -1.0f, 1.0f));
	}

	[[nodiscard]] constexpr Vector2 XY() const noexcept {
		return Vector2(x, y);
	}
	[[nodiscard]] constexpr f32 Dot(Vector3 const& other) const noexcept {
		return x * other.x + y * other.y + z * other.z;
	}
	[[nodiscard]] constexpr Vector3 Cross(Vector3 const& other) const noexcept {
		return Vector3(
			y * other.z - z * other.y,
			z * other.x - x * other.z,
			x * other.y - y * other.x
		);
	}
	[[nodiscard]] constexpr f32 Length() const noexcept {
		return Sqrt(Power(x, 2) + Power(y, 2) + Power(z, 2));
	}
	[[nodiscard]] constexpr f32 LengthSquared() const noexcept {
		return Power(x, 2) + Power(y, 2) + Power(z, 2);
	}
	[[nodiscard]] constexpr Vector3 Normalized() const noexcept {
		f32 length = Length();
		if (IsNearlyZero() == false) {
			f32 invLength = 1.0f / length;
			return Vector3(x * invLength, y * invLength, z * invLength);
		}
		return Zero();
	}
	[[nodiscard]] constexpr bool IsNearlyZero() const noexcept {
		return LengthSquared() <= Power(Epsilon, 2);
	}
	[[nodiscard]] constexpr Vector3 Abs() const noexcept {
		return Vector3(x < 0.0f ? -x : x, y < 0.0f ? -y : y, z < 0.0f ? -z : z);
	}
	constexpr void Normalize() noexcept {
		*this = Normalized();
	}
};

constexpr inline Vector3 operator*(f32 const scalar, Vector3 const& vector) noexcept {
	return vector * scalar;
}

}
