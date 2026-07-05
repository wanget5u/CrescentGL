#pragma once
#include <cmath>
#include <Core/Core.h>
#include "Core/Log.h"
#include "Math/Constants.h"
#include "Math/Arithmetic.h"
#include "Math/Exponential.h"
#include "Math/Interpolation.h"
#include "Vector3.h"

namespace Crescent::Math {

struct Vector4 {
	union {
		struct { f32 x; f32 y; f32 z; f32 w; };
		f32 data[4]{};
	};

	explicit constexpr Vector4() noexcept : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
	explicit constexpr Vector4(f32 const x, f32 const y, f32 const z, f32 const w) noexcept
		: x(x), y(y), z(z), w(w) {}
	explicit constexpr Vector4(Vector3 const& xyz, f32 const w = 1.0f) noexcept
		: x(xyz.x), y(xyz.y), z(xyz.z), w(w) {}
	explicit constexpr Vector4(Vector2 const& xy, f32 const z = 0.0f, f32 const w = 1.0f) noexcept
		: x(xy.x), y(xy.y), z(z), w(w) {}
	explicit constexpr Vector4(Vector2 const& xy, Vector2 const& zw) noexcept
		: x(xy.x), y(xy.y), z(zw.x), w(zw.y) {}

	[[nodiscard]] static constexpr Vector4 Zero()		noexcept { return Vector4( 0.0f,  0.0f,  0.0f, 0.0f); }
	[[nodiscard]] static constexpr Vector4 One()		noexcept { return Vector4( 1.0f,  1.0f,  1.0f, 1.0f); }
	[[nodiscard]] static constexpr Vector4 Up()			noexcept { return Vector4( 0.0f,  1.0f,  0.0f, 0.0f); }
	[[nodiscard]] static constexpr Vector4 Down()		noexcept { return Vector4( 0.0f, -1.0f,  0.0f, 0.0f); }
	[[nodiscard]] static constexpr Vector4 Left()		noexcept { return Vector4(-1.0f,  0.0f,  0.0f, 0.0f); }
	[[nodiscard]] static constexpr Vector4 Right()		noexcept { return Vector4( 1.0f,  0.0f,  0.0f, 0.0f); }
	[[nodiscard]] static constexpr Vector4 Forward()	noexcept { return Vector4( 0.0f,  0.0f,  1.0f, 0.0f); }
	[[nodiscard]] static constexpr Vector4 Back()		noexcept { return Vector4( 0.0f,  0.0f, -1.0f, 0.0f); }

	constexpr Vector4& operator+=(Vector4 const& other) noexcept {
		x += other.x; y += other.y; z += other.z; w += other.w;
		return *this;
	}
	[[nodiscard]] constexpr Vector4 operator+(Vector4 const& other) const noexcept {
		Vector4 result = *this;
		result += other;
		return result;
	}
	constexpr Vector4& operator-=(Vector4 const& other) noexcept {
		x -= other.x; y -= other.y; z -= other.z; w -= other.w;
		return *this;
	}
	[[nodiscard]] constexpr Vector4 operator-(Vector4 const& other) const noexcept {
		Vector4 result = *this;
		result -= other;
		return result;
	}
	constexpr Vector4& operator*=(f32 const scalar) noexcept {
		x *= scalar; y *= scalar; z *= scalar; w *= scalar;
		return *this;
	}
	[[nodiscard]] constexpr Vector4 operator*(f32 const scalar) const noexcept {
		Vector4 result = *this;
		result *= scalar;
		return result;
	}
	constexpr Vector4& operator*=(Vector4 const& other) noexcept {
		x *= other.x; y *= other.y; z *= other.z; w *= other.w;
		return *this;
	}
	[[nodiscard]] constexpr Vector4 operator*(Vector4 const& other) const noexcept {
		Vector4 result = *this;
		result *= other;
		return result;
	}
	constexpr Vector4& operator/=(f32 const scalar) noexcept {
		if (Math::IsNearlyZero(scalar)) {
			Log::Warning("Vector4 division scalar IsNearlyZero.");
			*this = Zero();
			return *this;
		}
		f32 invScalar = 1.0f / scalar;
		x *= invScalar; y *= invScalar; z *= invScalar; w *= invScalar;
		return *this;
	}
	[[nodiscard]] constexpr Vector4 operator/(f32 const scalar) const noexcept {
		Vector4 result = *this;
		result /= scalar;
		return result;
	}
	constexpr Vector4& operator/=(Vector4 const& other) noexcept {
		x = Math::IsNearlyZero(other.x) ? 0.0f : x / other.x;
		y = Math::IsNearlyZero(other.y) ? 0.0f : y / other.y;
		z = Math::IsNearlyZero(other.z) ? 0.0f : z / other.z;
		w = Math::IsNearlyZero(other.w) ? 0.0f : w / other.w;
		return *this;
	}
	[[nodiscard]] constexpr Vector4 operator/(Vector4 const& other) const noexcept {
		Vector4 result = *this;
		result /= other;
		return result;
	}
	[[nodiscard]] constexpr Vector4 operator-() const noexcept {
		return Vector4(-x, -y, -z, -w);
	}
	[[nodiscard]] constexpr bool operator==(Vector4 const& other) const noexcept {
		for (u8 a = 0; a < 4; a++) {
			if (Math::Abs(data[a] - other.data[a]) > Epsilon) {
				return false;
			}
		}
		return true;
	}
	[[nodiscard]] constexpr bool operator!=(Vector4 const& other) const noexcept {
		return !(*this == other);
	}
	[[nodiscard]] constexpr f32& operator[](size_t const index) noexcept {
		return data[index];
	}
	[[nodiscard]] constexpr f32 operator[](size_t const index) const noexcept {
		return data[index];
	}

	[[nodiscard]] static constexpr Vector4 Min(Vector4 const& a, Vector4 const& b) noexcept {
		return Vector4(Math::Min(a.x, b.x), Math::Min(a.y, b.y), Math::Min(a.z, b.z), Math::Min(a.w, b.w));
	}
	[[nodiscard]] static constexpr Vector4 Max(Vector4 const& a, Vector4 const& b) noexcept {
		return Vector4(Math::Max(a.x, b.x), Math::Max(a.y, b.y), Math::Max(a.z, b.z), Math::Max(a.w, b.w));
	}
	[[nodiscard]] static constexpr Vector4 Lerp(Vector4 const& start, Vector4 const& end, f32 const t) noexcept {
		return start + (end - start) * t;
	}
	[[nodiscard]] static constexpr f32 Distance(Vector4 const& a, Vector4 const& b) noexcept {
		return (a - b).Length();
	}
	[[nodiscard]] static constexpr f32 DistanceSquared(Vector4 const& a, Vector4 const& b) noexcept {
		return (a - b).LengthSquared();
	}
	[[nodiscard]] static constexpr Vector4 Clamp(Vector4 const& value, Vector4 const& min, Vector4 const& max) noexcept {
		return Vector4(
			Math::Clamp(value.x, min.x, max.x),
			Math::Clamp(value.y, min.y, max.y),
			Math::Clamp(value.z, min.z, max.z),
			Math::Clamp(value.w, min.w, max.w)
		);
	}
	[[nodiscard]] static f32 AngleBetween(Vector4 const& a, Vector4 const& b) noexcept {
		const f32 lengthProduct = a.Length() * b.Length();
		if (Math::IsNearlyZero(lengthProduct) == true) {
			return 0.0f;
		}
		const f32 cosTheta = a.Dot(b) / lengthProduct;
		const f32 clampedCos = Math::Clamp(cosTheta, -1.0f, 1.0f);
		return std::acos(clampedCos);
	}
	[[nodiscard]] static f32 AngleBetweenNormalized(Vector4 const& a, Vector4 const& b) noexcept {
		return std::acos(Math::Clamp(a.Dot(b), -1.0f, 1.0f));
	}

	[[nodiscard]] constexpr Vector2 XY() const noexcept {
		return Vector2(x, y);
	}
	[[nodiscard]] constexpr Vector3 XYZ() const noexcept {
		return Vector3(x, y, z);
	}
	[[nodiscard]] constexpr f32 Dot(Vector4 const& other) const noexcept {
		return x * other.x + y * other.y + z * other.z + w * other.w;
	}
	[[nodiscard]] constexpr Vector4 Cross(Vector4 const& other) const noexcept {
		return Vector4(
			y * other.z - z * other.y,
			z * other.x - x * other.z,
			x * other.y - y * other.x,
			0.0f
		);
	}
	[[nodiscard]] constexpr f32 Length() const noexcept {
		return Sqrt(Power(x, 2) + Power(y, 2) + Power(z , 2) + Power(w, 2));
	}
	[[nodiscard]] constexpr f32 LengthSquared() const noexcept {
		return Power(x, 2) + Power(y, 2) + Power(z, 2) + Power(w, 2);
	}
	[[nodiscard]] constexpr Vector4 Normalized() const noexcept {
		f32 length = Length();
		if (IsNearlyZero() == false) {
			f32 invLength = 1.0f / length;
			return Vector4(x * invLength, y * invLength, z * invLength, w * invLength);
		}
		return Zero();
	}
	[[nodiscard]] constexpr bool IsNearlyZero() const noexcept {
		return LengthSquared() <= Power(Epsilon, 2);
	}
	[[nodiscard]] constexpr Vector4 Abs() const noexcept {
		return Vector4(x < 0.0f ? -x : x, y < 0.0f ? -y : y, z < 0.0f ? -z : z, w < 0.0f ? -w : w);
	}
	constexpr void Normalize() noexcept {
		*this = Normalized();
	}
};

constexpr inline Vector4 operator*(f32 const scalar, Vector4 const& vector) noexcept {
	return vector * scalar;
}

}
