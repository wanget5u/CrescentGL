#pragma once
#include "Render/Mesh.h"
#include "Math/Vector/Vector3.h"

namespace Crescent {
struct BoxMesh : Mesh {
	explicit BoxMesh(f32 width = 1.0f, f32 height = 1.0f, f32 depth = 1.0f);
	explicit BoxMesh(Math::Vector3 const& size);
	~BoxMesh() override = default;
	void SetSize(f32 width, f32 height, f32 depth);
	void SetSize(Math::Vector3 const& size);
	[[nodiscard]] Math::Vector3 GetSize() const noexcept { return m_Size; }
protected:
	void GenerateGeometry() override;
	Math::Vector3 m_Size{1.0f, 1.0f, 1.0f};
};
}
