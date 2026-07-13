#pragma once
#include "Core/Core.h"
#include "Math/Arithmetic.h"
#include "Math/Vector/Vector3.h"
#include "Math/Vector/Vector4.h"

namespace Crescent::Math {

struct Matrix4x4 {
	Vector4 column0;
	Vector4 column1;
	Vector4 column2;
	Vector4 column3;

	[[nodiscard]] constexpr f32 const* Data() const noexcept { return column0.data; }
	[[nodiscard]] constexpr f32* Data() noexcept { return column0.data; }
	[[nodiscard]] constexpr f32 const* GetData() const noexcept { return column0.data; }
	[[nodiscard]] constexpr f32* GetData() noexcept { return column0.data; }

	constexpr Matrix4x4()
		: column0(1.0f, 0.0f, 0.0f, 0.0f)
		, column1(0.0f, 1.0f, 0.0f, 0.0f)
		, column2(0.0f, 0.0f, 1.0f, 0.0f)
		, column3(0.0f, 0.0f, 0.0f, 1.0f)
	{}
	explicit constexpr Matrix4x4(f32 const diagonal)
   		: column0(diagonal,     0.0f,     0.0f,     0.0f)
   		, column1(    0.0f, diagonal,     0.0f,     0.0f)
   		, column2(    0.0f,     0.0f, diagonal,     0.0f)
   		, column3(    0.0f,     0.0f,     0.0f, diagonal)
	{}
	explicit constexpr Matrix4x4(Vector4 const& c0, Vector4 const& c1, Vector4 const& c2, Vector4 const& c3)
		: column0(c0), column1(c1), column2(c2), column3(c3) {}

	[[nodiscard]] static constexpr Matrix4x4 Zero() noexcept {
		return Matrix4x4(
			Vector4(0.0f, 0.0f, 0.0f, 0.0f),
			Vector4(0.0f, 0.0f, 0.0f, 0.0f),
			Vector4(0.0f, 0.0f, 0.0f, 0.0f),
			Vector4(0.0f, 0.0f, 0.0f, 0.0f)
		);
	}
	[[nodiscard]] static constexpr Matrix4x4 GetIdentity() noexcept {
		return Matrix4x4(
			Vector4(1.0f, 0.0f, 0.0f, 0.0f),
			Vector4(0.0f, 1.0f, 0.0f, 0.0f),
			Vector4(0.0f, 0.0f, 1.0f, 0.0f),
			Vector4(0.0f, 0.0f, 0.0f, 1.0f)
		);
	}
	[[nodiscard]] static constexpr Matrix4x4 Transpose(Matrix4x4 const& m) noexcept {
		return Matrix4x4(
			Vector4(m.column0.x, m.column1.x, m.column2.x, m.column3.x),
			Vector4(m.column0.y, m.column1.y, m.column2.y, m.column3.y),
			Vector4(m.column0.z, m.column1.z, m.column2.z, m.column3.z),
			Vector4(m.column0.w, m.column1.w, m.column2.w, m.column3.w)
		);
	}
	[[nodiscard]] constexpr Matrix4x4 Transposed() const noexcept {
		return Transpose(*this);
	}
	[[nodiscard]] static constexpr Matrix4x4 Inverse(Matrix4x4 const& m) noexcept {
		f32 const coef00 = m.column2.z * m.column3.w - m.column3.z * m.column2.w;
		f32 const coef02 = m.column1.z * m.column3.w - m.column3.z * m.column1.w;
		f32 const coef03 = m.column1.z * m.column2.w - m.column2.z * m.column1.w;

		f32 const coef04 = m.column2.y * m.column3.w - m.column3.y * m.column2.w;
		f32 const coef06 = m.column1.y * m.column3.w - m.column3.y * m.column1.w;
		f32 const coef07 = m.column1.y * m.column2.w - m.column2.y * m.column1.w;

		f32 const coef08 = m.column2.y * m.column3.z - m.column3.y * m.column2.z;
		f32 const coef10 = m.column1.y * m.column3.z - m.column3.y * m.column1.z;
		f32 const coef11 = m.column1.y * m.column2.z - m.column2.y * m.column1.z;

		f32 const coef12 = m.column2.x * m.column3.w - m.column3.x * m.column2.w;
		f32 const coef14 = m.column1.x * m.column3.w - m.column3.x * m.column1.w;
		f32 const coef15 = m.column1.x * m.column2.w - m.column2.x * m.column1.w;

		f32 const coef16 = m.column2.x * m.column3.z - m.column3.x * m.column2.z;
		f32 const coef18 = m.column1.x * m.column3.z - m.column3.x * m.column1.z;
		f32 const coef19 = m.column1.x * m.column2.z - m.column2.x * m.column1.z;

		f32 const coef20 = m.column2.x * m.column3.y - m.column3.x * m.column2.y;
		f32 const coef22 = m.column1.x * m.column3.y - m.column3.x * m.column1.y;
		f32 const coef23 = m.column1.x * m.column2.y - m.column2.x * m.column1.y;

		Vector4 const fac0(coef00, coef00, coef02, coef03);
		Vector4 const fac1(coef04, coef04, coef06, coef07);
		Vector4 const fac2(coef08, coef08, coef10, coef11);
		Vector4 const fac3(coef12, coef12, coef14, coef15);
		Vector4 const fac4(coef16, coef16, coef18, coef19);
		Vector4 const fac5(coef20, coef20, coef22, coef23);

		Vector4 const vec0(m.column1.x, m.column0.x, m.column0.x, m.column0.x);
		Vector4 const vec1(m.column1.y, m.column0.y, m.column0.y, m.column0.y);
		Vector4 const vec2(m.column1.z, m.column0.z, m.column0.z, m.column0.z);
		Vector4 const vec3(m.column1.w, m.column0.w, m.column0.w, m.column0.w);

		Vector4 const inv0(vec1 * fac0 - vec2 * fac1 + vec3 * fac2);
		Vector4 const inv1(vec0 * fac0 - vec2 * fac3 + vec3 * fac4);
		Vector4 const inv2(vec0 * fac1 - vec1 * fac3 + vec3 * fac5);
		Vector4 const inv3(vec0 * fac2 - vec1 * fac4 + vec2 * fac5);

		Vector4 const signA(+1.0f, -1.0f, +1.0f, -1.0f);
		Vector4 const signB(-1.0f, +1.0f, -1.0f, +1.0f);

		Matrix4x4 inv(inv0 * signA, inv1 * signB, inv2 * signA, inv3 * signB);

		Vector4 const row0(inv.column0.x, inv.column1.x, inv.column2.x, inv.column3.x);

		Vector4 const dot0(m.column0 * row0);
		f32 const dot1 = (dot0.x + dot0.y) + (dot0.z + dot0.w);

		if (Math::IsNearlyZero(dot1)) {
			Log::Warning("Matrix4x4 inverse failed: Determinant is zero.");
			return GetIdentity();
		}

		f32 const rDet = 1.0f / dot1;
		return inv * rDet;
	}
	[[nodiscard]] constexpr Matrix4x4 Inversed() const noexcept {
		return Inverse(*this);
	}
	[[nodiscard]] static constexpr Matrix4x4 GetTranslation(Vector3 const& position) noexcept {
		return Matrix4x4(
		   Vector4(		 1.0f, 		 0.0f, 		 0.0f, 0.0f),
		   Vector4(		 0.0f, 		 1.0f, 		 0.0f, 0.0f),
		   Vector4(		 0.0f, 		 0.0f, 		 1.0f, 0.0f),
		   Vector4(position.x, position.y, position.z, 1.0f)
		);
	}
	[[nodiscard]] static constexpr Matrix4x4 GetOrthographicProjection(
		f32 const left, f32 const right, f32 const bottom, f32 const top,
		f32 const nearZ = -1.0f, f32 const farZ = 1.0f) {
		const f32 rightLeft = 1.0f / (right - left);
		const f32 topBottom = 1.0f / (top - bottom);
		const f32 farNear = 1.0f / (farZ - nearZ);
		return Matrix4x4(
			Vector4(		   2.0f * rightLeft,         	            0.0f,                      0.0f, 0.0f),
			Vector4(		       		   0.0f,         	2.0f * topBottom,                      0.0f, 0.0f),
			Vector4(		       		   0.0f,         	            0.0f,           -2.0f * farNear, 0.0f),
			Vector4(-(right + left) * rightLeft, -(top + bottom) * topBottom, -(farZ + nearZ) * farNear, 1.0f)
		);
	}
	[[nodiscard]] static constexpr Matrix4x4 GetPerspectiveProjection(
		f32 const fovRadians, f32 const aspectRatio, f32 const nearZ = 0.1f, f32 const farZ = 1000.0f) noexcept {
		if (nearZ <= 0.0f || farZ <= nearZ) {
			Log::Warning("Perspective projection requires farZ > nearZ > 0.");
			return Zero();
		}
		if (Math::IsNearlyZero(farZ - nearZ)) {
			Log::Warning("Perspective projection farZ and nearZ are too close.");
			return Zero();
		}
		const f32 tanHalfFov = Math::Tan(fovRadians / 2.0f);
		const f32 g = 1.0f / tanHalfFov;
		const f32 invDepth = 1.0f / (farZ - nearZ);
		return Matrix4x4(
		   Vector4(g / aspectRatio,          0.0f,                               0.0f,  0.0f),
		   Vector4(           0.0f,             g,                               0.0f,  0.0f),
		   Vector4(           0.0f,          0.0f,         -(farZ + nearZ) * invDepth, -1.0f),
		   Vector4(           0.0f,          0.0f,  -(2.0f * farZ * nearZ) * invDepth,  0.0f)
		);
	}
	[[nodiscard]] static constexpr Matrix4x4 GetLookAt(Vector3 const& eye, Vector3 const& target, Vector3 const& up) noexcept {
		Vector3 const forwardVector = (target - eye).Normalized();
		Vector3 const rightVector = forwardVector.Cross(up).Normalized();
		Vector3 const upVector = rightVector.Cross(forwardVector);
		return Matrix4x4(
		   Vector4(        rightVector.x,         upVector.x,        -forwardVector.x, 0.0f),
		   Vector4(        rightVector.y,         upVector.y,        -forwardVector.y, 0.0f),
		   Vector4(        rightVector.z,         upVector.z,        -forwardVector.z, 0.0f),
		   Vector4(-rightVector.Dot(eye), -upVector.Dot(eye),  forwardVector.Dot(eye), 1.0f)
		);
	}
	[[nodiscard]] static constexpr Matrix4x4 GetScale(Vector3 const& scale) noexcept {
		return Matrix4x4(
			Vector4(scale.x,    0.0f,    0.0f, 0.0f),
			Vector4(   0.0f, scale.y,    0.0f, 0.0f),
			Vector4(   0.0f,    0.0f, scale.z, 0.0f),
			Vector4(   0.0f,    0.0f,    0.0f, 1.0f)
		);
	}
	[[nodiscard]] static constexpr Matrix4x4 GetRotationX(f32 const radians) {
		return Matrix4x4(
			Vector4(1.0f,	            0.0f,                0.0f, 0.0f),
			Vector4(0.0f, Math::Cos(radians), -Math::Sin(radians), 0.0f),
			Vector4(0.0f, Math::Sin(radians),  Math::Cos(radians), 0.0f),
			Vector4(0.0f,	            0.0f,                0.0f, 1.0f)
		);
	}
	[[nodiscard]] static constexpr Matrix4x4 GetRotationY(f32 const radians) {
		return Matrix4x4(
			Vector4( Math::Cos(radians), 0.0f, Math::Sin(radians), 0.0f),
			Vector4(		       0.0f, 1.0f,               0.0f, 0.0f),
			Vector4(-Math::Sin(radians), 0.0f, Math::Cos(radians), 0.0f),
			Vector4(		       0.0f, 0.0f,               0.0f, 1.0f)
		);
	}
	[[nodiscard]] static constexpr Matrix4x4 GetRotationZ(f32 const rad) {
		return Matrix4x4(
			Vector4(Math::Cos(rad), -Math::Sin(rad), 0.0f, 0.0f),
			Vector4(Math::Sin(rad),  Math::Cos(rad), 0.0f, 0.0f),
			Vector4(	      0.0f,            0.0f, 1.0f, 0.0f),
			Vector4(	      0.0f,            0.0f, 0.0f, 1.0f)
		);
	}

	constexpr Matrix4x4& operator+=(Matrix4x4 const& other) noexcept {
		column0 += other.column0;
		column1 += other.column1;
		column2 += other.column2;
		column3 += other.column3;
		return *this;
	}
	[[nodiscard]] constexpr Matrix4x4 operator+(Matrix4x4 const& other) const noexcept {
		return Matrix4x4(*this) += other;
	}
	constexpr Matrix4x4& operator-=(Matrix4x4 const& other) noexcept {
		column0 -= other.column0;
		column1 -= other.column1;
		column2 -= other.column2;
		column3 -= other.column3;
		return *this;
	}
	[[nodiscard]] constexpr Matrix4x4 operator-(Matrix4x4 const& other) const noexcept {
		return Matrix4x4(*this) -= other;
	}
	constexpr Matrix4x4& operator*=(f32 const scalar) noexcept {
		column0 *= scalar;
		column1 *= scalar;
		column2 *= scalar;
		column3 *= scalar;
		return *this;
	}
	constexpr Vector4 operator*(Vector4 const& other) const noexcept {
		return (column0 * other.x) +
			   (column1 * other.y) +
			   (column2 * other.z) +
			   (column3 * other.w);
	}
	constexpr Matrix4x4& operator*=(Matrix4x4 const& other) noexcept {
		Matrix4x4 m = *this;
		(*this)[0]=(m.column0*other[0].x)+(m.column1*other[0].y)+(m.column2*other[0].z)+(m.column3*other[0].w);
		(*this)[1]=(m.column0*other[1].x)+(m.column1*other[1].y)+(m.column2*other[1].z)+(m.column3*other[1].w);
		(*this)[2]=(m.column0*other[2].x)+(m.column1*other[2].y)+(m.column2*other[2].z)+(m.column3*other[2].w);
		(*this)[3]=(m.column0*other[3].x)+(m.column1*other[3].y)+(m.column2*other[3].z)+(m.column3*other[3].w);
		return *this;
	}
	[[nodiscard]] constexpr Matrix4x4 operator*(f32 const scalar) const noexcept {
		return Matrix4x4(*this) *= scalar;
	}
	[[nodiscard]] constexpr Matrix4x4 operator*(Matrix4x4 const& other) const noexcept {
		return Matrix4x4(*this) *= other;
	}
	constexpr Matrix4x4& operator/=(f32 const scalar) noexcept {
		if (Math::IsNearlyZero(scalar)) {
			Log::Warning("Matrix4x4 division scalar IsNearlyZero.");
			*this = Zero();
			return *this;
		}
		f32 invScalar = 1.0f / scalar;
		column0 *= invScalar;
		column1 *= invScalar;
		column2 *= invScalar;
		column3 *= invScalar;
		return *this;
	}
	// TODO:
	// constexpr Matrix4x4& operator/=(Matrix4x4 const& other) noexcept {
	// }
	constexpr Vector4 const& operator[](size_t const index) const noexcept {
		return (&column0)[index];
	}
	constexpr Vector4& operator[](size_t const index) noexcept {
		return (&column0)[index];
	}

	[[nodiscard]] constexpr Vector4 GetRow(size_t const rowIndex) const {
		return Vector4(column0[rowIndex], column1[rowIndex], column2[rowIndex], column3[rowIndex]);
	}
	constexpr void SetRow(size_t const rowIndex, Vector4 const& row) {
		column0[rowIndex] = row.x;
		column1[rowIndex] = row.y;
		column2[rowIndex] = row.z;
		column3[rowIndex] = row.w;
	}
	constexpr void Scale(Vector3 const& scale) noexcept {
		column0 *= scale.x;
		column1 *= scale.y;
		column2 *= scale.z;
	}
	constexpr void TranslateLocal(Vector3 const& position) noexcept {
		*this *= GetTranslation(position);
	}
	constexpr void TranslateWorld(Vector3 const& position) noexcept {
		column3.x += position.x;
		column3.y += position.y;
		column3.z += position.z;
	}
	constexpr void RotateLocal(f32 const rad, Vector3 const& axis) noexcept {
		Vector3 normalizedAxis = axis.Normalized();
		const f32 cos = Math::Cos(rad);
		const f32 sin = Math::Sin(rad);
		const f32 omc = 1.0f - Math::Cos(rad);
		const f32 x = normalizedAxis.x;
		const f32 y = normalizedAxis.y;
		const f32 z = normalizedAxis.z;
		Matrix4x4 rotation(
			Vector4(  cos + x*x*omc, y*x*omc + z*sin, z*x*omc - y*sin, 0.0f),
			Vector4(x*y*omc - z*sin,   cos + y*y*omc, z*y*omc + x*sin, 0.0f),
			Vector4(x*z*omc + y*sin, y*z*omc - x*sin,   cos + z*z*omc, 0.0f),
			Vector4(           0.0f,            0.0f,            0.0f, 1.0f)
		);
		*this *= rotation;
	}
	constexpr void RotateWorld(f32 const rad, Vector3 const& axis) noexcept {
		Vector3 normalizedAxis = axis.Normalized();
		const f32 cos = Math::Cos(rad);
		const f32 sin = Math::Sin(rad);
		const f32 omc = 1.0f - Math::Cos(rad);
		const f32 x = normalizedAxis.x;
		const f32 y = normalizedAxis.y;
		const f32 z = normalizedAxis.z;
		Matrix4x4 rotation(
	  		Vector4(  cos + x*x*omc, x*y*omc + z*sin, x*z*omc - y*sin, 0.0f),
	  		Vector4(y*x*omc - z*sin,   cos + y*y*omc, y*z*omc + x*sin, 0.0f),
	  		Vector4(z*x*omc + y*sin, z*y*omc - x*sin,   cos + z*z*omc, 0.0f),
	  		Vector4(           0.0f,            0.0f,            0.0f, 1.0f)
		);
		*this = rotation * (*this);
	}
};

}
