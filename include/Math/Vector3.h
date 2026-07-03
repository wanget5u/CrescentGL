#pragma once
#include <Core/Core.h>
#include "Common.h"

namespace Crescent::Math {

struct Vector3 {
	union {
		struct { f32 x; f32 y; f32 z; };
		f32 data[3]{};
	};
	constexpr Vector3() noexcept : x(0.0f), y(0.0f), z(0.0f) {}
	constexpr Vector3(f32 const x, f32 const y, f32 const z) noexcept : x(x), y(y), z(z) {}
	///////////////////////////////////////////////////////////////////////////
	/// Presets
	[[nodiscard]] static constexpr Vector3 Zero()		noexcept { return Vector3( 0.0f,  0.0f,  0.0f); }
	[[nodiscard]] static constexpr Vector3 One()		noexcept { return Vector3( 1.0f,  1.0f,  1.0f); }
	///////////////////////////////////////////////////////////////////////////
	[[nodiscard]] static constexpr Vector3 Up()			noexcept { return Vector3( 0.0f,  1.0f,  0.0f); }
	[[nodiscard]] static constexpr Vector3 Down()		noexcept { return Vector3( 0.0f, -1.0f,  0.0f); }
	[[nodiscard]] static constexpr Vector3 Left()		noexcept { return Vector3(-1.0f,  0.0f,  0.0f); }
	[[nodiscard]] static constexpr Vector3 Right()		noexcept { return Vector3( 1.0f,  0.0f,  0.0f); }
	[[nodiscard]] static constexpr Vector3 Forward()	noexcept { return Vector3( 0.0f,  0.0f,  1.0f); }
	[[nodiscard]] static constexpr Vector3 Back()		noexcept { return Vector3( 0.0f,  0.0f, -1.0f); }
	///////////////////////////////////////////////////////////////////////////
	/// Operator overloads
	Vector3& operator+=(Vector3 const& other) noexcept {
		x += other.x; y += other.y; z += other.z;
		return *this;
	}
	Vector3 operator+(Vector3 const& other) const noexcept {
		Vector3 result = *this;
		result += other;
		return result;
	}
	Vector3& operator-=(Vector3 const& other) noexcept {
		x -= other.x; y -= other.y; z -= other.z;
		return *this;
	}
	Vector3 operator-(Vector3 const& other) const noexcept {
		Vector3 result = *this;
		result -= other;
		return result;
	}
	Vector3& operator*=(f32 const scalar) noexcept {
		x *= scalar; y *= scalar; z *= scalar;
		return *this;
	}
	Vector3 operator*(f32 const scalar) const noexcept {
		Vector3 result = *this;
		result *= scalar;
		return result;
	}
	Vector3 operator*(Vector3 const& other) const noexcept {
		return Vector3(x * other.x, y * other.y, z * other.z);
	}
	Vector3& operator/=(f32 const scalar) noexcept {
		f32 invScalar = 1.0f / scalar;
		x *= invScalar; y *= invScalar; z *= invScalar;
		return *this;
	}
	Vector3 operator/(f32 const scalar) const noexcept {
		Vector3 result = *this;
		result /= scalar;
		return result;
	}
	Vector3 operator-() const noexcept {
		return Vector3(-x, -y, -z);
	}
	[[nodiscard]] constexpr f32& operator[](size_t const index) noexcept {
		return data[index];
	}
	[[nodiscard]] constexpr f32 operator[](size_t const index) const noexcept {
		return data[index];
	}
	///////////////////////////////////////////////////////////////////////////
	/// Functions
	[[nodiscard]] static constexpr Vector3 Min(Vector3 const& a, Vector3 const& b) noexcept {
		return Vector3(Min(a.x, b.x), Min(a.y, b.y), Min(a.z, b.z));
	}
	[[nodiscard]] static constexpr Vector3 Max(Vector3 const& a, Vector3 const& b) noexcept {
		return Vector3(Max(a.x, b.x), Max(a.y, b.y), Max(a.z, b.z));
	}
	static constexpr Vector3 Lerp(Vector3 const& start, Vector3 const& end, f32 t) noexcept {
		return start * (1.0f - t) + end * t;
	}
	[[nodiscard]] static constexpr f32 Distance(Vector3 const& a, Vector3 const& b) noexcept {
		return (a - b).Length();
	}
	[[nodiscard]] static constexpr f32 DistanceSquared(Vector3 const& a, Vector3 const& b) noexcept {
		return (a - b).LengthSquared();
	}
	[[nodiscard]] static constexpr Vector3 Clamp(Vector3 const& value, Vector3 const& min, Vector3 const& max) noexcept {
		return Vector3(
			Clamp(value.x, min.x, max.x),
			Clamp(value.y, min.y, max.y),
			Clamp(value.z, min.z, max.z)
		);
	}
	///////////////////////////////////////////////////////////////////////////
	/// Methods
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
		return Sqrt(Power(x, 2) + Power(y, 2) + Power(z , 2));
	}
	[[nodiscard]] constexpr f32 LengthSquared() const noexcept {
		return Power(x, 2) + Power(y, 2) + Power(z, 2);
	}
	[[nodiscard]] constexpr Vector3 Normalized() const noexcept {
		f32 length = Length();
		if (length > 0.0f) {
			f32 invLength = 1.0f / length;
			return Vector3(x * invLength, y * invLength, z * invLength);
		}
		return Zero();
	}
	[[nodiscard]] constexpr bool IsNearlyZero(f32 const epsilon = 0.001f) const noexcept {
		return LengthSquared() <= Power(epsilon, 2);
	}
	[[nodiscard]] constexpr Vector3 Abs() const noexcept {
		return Vector3(x < 0.0f ? -x : x, y < 0.0f ? -y : y, z < 0.0f ? -z : z);
	}
	void Normalize() noexcept {
		*this = Normalized();
	}
};

inline Vector3 operator*(f32 const scalar, Vector3 const& vector) noexcept{
	return vector * scalar;
}

}