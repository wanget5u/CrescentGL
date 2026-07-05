#pragma once
#include "Core/Core.h"
#include "Math/Vector/Vector4.h"

namespace Crescent::Math {

struct Matrix4x4 {
	union {
		f32 data[16]{};
		f32 columns[4][4];
		struct {
			Vector4 column0;	// X basis vector (right)
			Vector4 column1;	// Y basis vector (up)
			Vector4 column2;	// Z basis vector (forward/back)
			Vector4 column3;	// W basis vector (translation/position)
		};
	};
	
	explicit constexpr Matrix4x4()
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
	[[nodiscard]] static constexpr Matrix4x4 Identity() noexcept {
		return Matrix4x4(
			Vector4(1.0f, 0.0f, 0.0f, 0.0f),
			Vector4(0.0f, 1.0f, 0.0f, 0.0f),
			Vector4(0.0f, 0.0f, 1.0f, 0.0f),
			Vector4(0.0f, 0.0f, 0.0f, 1.0f)
		);
	}
	[[nodiscard]] static constexpr Matrix4x4 Translation(Vector3 const& position) noexcept {
		return Matrix4x4(
		   Vector4(1.0f, 0.0f, 0.0f, 0.0f),
		   Vector4(0.0f, 1.0f, 0.0f, 0.0f),
		   Vector4(0.0f, 0.0f, 1.0f, 0.0f),
		   Vector4(position.x, position.y, position.z, 1.0f)
		);
	}

	[[nodiscard]] constexpr Vector4 GetRow(size_t const rowIndex) const {
		return Vector4(column0[rowIndex], column1[rowIndex], column2[rowIndex], column3[rowIndex]);
	}
	void SetRow(size_t const rowIndex, Vector4 const& row) {
		column0[rowIndex] = row.x;
		column1[rowIndex] = row.y;
		column2[rowIndex] = row.z;
		column3[rowIndex] = row.w;
	}
};

}
