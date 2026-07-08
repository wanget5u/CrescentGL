#pragma once
#include "Math/Constants.h"
#include "Math/Trigonometry.h"

namespace Crescent::Math {

struct Quaternion {
	union {
		struct { f32 x; f32 y; f32 z; f32 w; };
		f32 data[4];
	};

	constexpr Quaternion() noexcept = default;
	explicit constexpr Quaternion(f32 const x, f32 const y, f32 const z, f32 const w) noexcept
	   : x(x), y(y), z(z), w(w) {}
	explicit constexpr Quaternion(Vector3 const& axis, f32 const angleRadians) noexcept {
		const f32 halfAngle = angleRadians * 0.5f;
		const f32 sinHalfAngle = Math::Sin(halfAngle);
		x = axis.x * sinHalfAngle;
		y = axis.y * sinHalfAngle;
		z = axis.z * sinHalfAngle;
		w = Math::Cos(halfAngle);
	}
	[[nodiscard]] static constexpr Quaternion Identity() noexcept { return Quaternion(0.0f, 0.0f, 0.0f, 1.0f); }
	[[nodiscard]] static constexpr Quaternion Zero()     noexcept { return Quaternion(0.0f, 0.0f, 0.0f, 0.0f); }
	[[nodiscard]] static constexpr Quaternion FromEulerAngles(Vector3 const& radians) noexcept {
		const f32 cy = Math::Cos(radians.y * 0.5f);
		const f32 sy = Math::Sin(radians.y * 0.5f);
		const f32 cp = Math::Cos(radians.x * 0.5f);
		const f32 sp = Math::Sin(radians.x * 0.5f);
		const f32 cr = Math::Cos(radians.z * 0.5f);
		const f32 sr = Math::Sin(radians.z * 0.5f);
		return Quaternion(
		   sr * cp * cy - cr * sp * sy,
		   cr * sp * cy + sr * cp * sy,
		   cr * cp * sy - sr * sp * cy,
		   cr * cp * cy + sr * sp * sy
		);
	}
	[[nodiscard]] static constexpr Vector3 ToEulerAngles(Quaternion const& quaternion) noexcept {
		Vector3 result{};
		const f32 sinPitch = 2.0f * (quaternion.w * quaternion.x - quaternion.y * quaternion.z);
		if (Math::Abs(sinPitch) >= 1.0f) {
			result.x = sinPitch > 0.0f ? HALF_PI : -HALF_PI;
		}
		else {
			result.x = Math::ArcSin(sinPitch);
		}
		const f32 sinYaw = 2.0f * (quaternion.w * quaternion.y + quaternion.z * quaternion.x);
		const f32 cosYaw = 1.0f - 2.0f * (quaternion.x * quaternion.x + quaternion.y * quaternion.y);
		result.y = Math::ArcTan2(sinYaw, cosYaw);
		const f32 sinRoll = 2.0f * (quaternion.w * quaternion.z + quaternion.x * quaternion.y);
		const f32 cosRoll = 1.0f - 2.0f * (quaternion.z * quaternion.z + quaternion.x * quaternion.x);
		result.z = Math::ArcTan2(sinRoll, cosRoll);
		return result;
	}
	[[nodiscard]] static constexpr Matrix4x4 ToMatrix4x4(Quaternion const& matrix) noexcept {
		const f32 xx = matrix.x * matrix.x, yy = matrix.y * matrix.y, zz = matrix.z * matrix.z;
		const f32 xy = matrix.x * matrix.y, xz = matrix.x * matrix.z, yw = matrix.y * matrix.w;
		const f32 yz = matrix.y * matrix.z, xw = matrix.x * matrix.w, zw = matrix.z * matrix.w;
		return Matrix4x4(
		   Vector4(1.0f - 2.0f * (yy + zz),        2.0f * (xy + zw),        2.0f * (xz - yw), 0.0f),
		   Vector4(       2.0f * (xy - zw), 1.0f - 2.0f * (xx + zz),        2.0f * (yz + xw), 0.0f),
		   Vector4(       2.0f * (xz + yw),        2.0f * (yz - xw), 1.0f - 2.0f * (xx + yy), 0.0f),
		   Vector4(                   0.0f,                    0.0f,                    0.0f, 1.0f)
		);
	}

	constexpr Quaternion& operator+=(Quaternion const& other) noexcept {
		x += other.x; y += other.y; z += other.z; w += other.w;
		return *this;
	}
	[[nodiscard]] constexpr Quaternion operator+(Quaternion const& other) const noexcept {
		return Quaternion(*this) += other;
	}
	constexpr Quaternion& operator-=(Quaternion const& other) noexcept {
		x -= other.x; y -= other.y; z -= other.z; w -= other.w;
		return *this;
	}
	[[nodiscard]] constexpr Quaternion operator-(Quaternion const& other) const noexcept {
		return Quaternion(*this) -= other;
	}
	constexpr Quaternion& operator*=(f32 const scalar) noexcept {
		x *= scalar; y *= scalar; z *= scalar; w *= scalar;
		return *this;
	}
	[[nodiscard]] constexpr Quaternion operator*(f32 const scalar) const noexcept {
		return Quaternion(*this) *= scalar;
	}
	[[nodiscard]] constexpr Vector3 operator*(Vector3 const& vector) const noexcept {
		const f32 x2 = x * 2.0f, y2 = y * 2.0f, z2 = z * 2.0f;
		const f32 xx = x * x2, xy = x * y2, xz = x * z2;
		const f32 yy = y * y2, yz = y * z2, zz = z * z2;
		const f32 wx = w * x2, wy = w * y2, wz = w * z2;
		Vector3 result{};
		result.x = (1.0f - (yy + zz)) * vector.x + (xy - wz) * vector.y + (xz + wy) * vector.z;
		result.y = (xy + wz) * vector.x + (1.0f - (xx + zz)) * vector.y + (yz - wx) * vector.z;
		result.z = (xz - wy) * vector.x + (yz + wx) * vector.y + (1.0f - (xx + yy)) * vector.z;
		return result;
	}
	constexpr Quaternion& operator/=(f32 const scalar) noexcept {
		if (Math::IsNearlyZero(scalar) == true) {
			Log::Warning("Quaternion division scalar IsNearlyZero.");
			*this = Zero();
			return *this;
		}
		f32 invScalar = 1.0f / scalar;
		x *= invScalar; y *= invScalar; z *= invScalar; w *= invScalar;
		return *this;
	}
	[[nodiscard]] constexpr Quaternion operator/(f32 const scalar) const noexcept {
		return Quaternion(*this) /= scalar;
	}

	[[nodiscard]] constexpr f32 Dot(Quaternion const& other) const noexcept {
		return x * other.x + y * other.y + z * other.z + w * other.w;
	}
	[[nodiscard]] constexpr f32 Length() const noexcept {
		return Math::Sqrt(LengthSquared());
	}
	[[nodiscard]] constexpr f32 LengthSquared() const noexcept {
		return (x * x) + (y * y) + (z * z) + (w * w);
	}
	[[nodiscard]] constexpr Quaternion Normalized() const noexcept {
		const f32 lengthSquared = LengthSquared();
		if (lengthSquared <= Power(Epsilon, 2)) {
			return Identity();
		}
		const f32 invLength = 1.0f / Math::Sqrt(lengthSquared);
		return Quaternion(x * invLength, y * invLength, z * invLength, w * invLength);
	}
	constexpr void Normalize() noexcept {
		*this = Normalized();
	}
	[[nodiscard]] constexpr Quaternion Conjugated() const noexcept {
		return Quaternion(-x, -y, -z, w);
	}
	[[nodiscard]] constexpr Quaternion Inversed() const noexcept {
		const f32 lengthSquared = LengthSquared();
		if (lengthSquared <= Power(Epsilon, 2)) {
			return Zero();
		}
		const f32 invLengthSquared = 1.0f / lengthSquared;
		return Quaternion(-x * invLengthSquared, -y * invLengthSquared, -z * invLengthSquared, w * invLengthSquared);
	}
	[[nodiscard]] static constexpr Quaternion Lerp(Quaternion const& start, Quaternion const& end, f32 const t) noexcept {
		return (start * (1.0f - t) + end * t).Normalized();
	}
	[[nodiscard]] static Quaternion Slerp(Quaternion start, Quaternion const& end, f32 const t) noexcept {
		f32 cosTheta = start.Dot(end);
		if (cosTheta < 0.0f) {
			cosTheta *= -1;
			start *= -1;
		}
		if (cosTheta > 1.0f - Epsilon) {
			return Lerp(start, end, t);
		}
		const f32 angle = Math::ArcCos(cosTheta);
		const f32 invSinTheta = 1.0f / Math::Sin(angle);
		const f32 weightStart = Math::Sin((1.0f - t) * angle) * invSinTheta;
		const f32 weightEnd = Math::Sin(t * angle) * invSinTheta;
		return (start * weightStart) + (end * weightEnd);
	}
};

constexpr Quaternion operator*(f32 const scalar, Quaternion const& quaternion) noexcept {
	return quaternion * scalar;
}

}
