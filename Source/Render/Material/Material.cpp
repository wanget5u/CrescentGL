#include "Render/Material/Material.h"

#include <glad/glad.h>

#include "Asset/AssetLoader.h"
#include "Render/Shader/Shader.h"
#include "Asset/AssetType.h"

namespace Crescent {

u32 Material::s_IDCounter = 0;

Material::Material(std::shared_ptr<ShaderAsset> shaderAsset, Math::Vector4 const& baseColor)
	: TintColor(baseColor), Shader(std::move(shaderAsset)), ID(++s_IDCounter) {}


std::shared_ptr<ShaderAsset> Material::GetDefaultShader() {
	static std::shared_ptr<ShaderAsset> defaultShader =
		AssetLoader::Instance().GetOrLoad<ShaderAsset>("Shaders/unlit", AssetType::Shader);
	return defaultShader;
}

std::shared_ptr<Material> Material::GetDefaultMaterial() {
	static std::shared_ptr<Material> defaultMaterial = std::make_shared<Material>(
		AssetLoader::Instance().GetOrLoad<ShaderAsset>("Shaders/unlit", AssetType::Shader),
		GetDefaultColor()
	);
	return defaultMaterial;
}

Math::Vector4 Material::GetDefaultColor() noexcept {
	return Math::Vector4{ 0.7f, 0.7f, 0.7f, 1.0f };
}

std::shared_ptr<ShaderAsset> Material::GetActiveShader() const noexcept {
	if (Shader != nullptr && Shader->IsReady == true && Shader->ShaderObject != nullptr) {
		return Shader;
	}
	return GetDefaultShader();
}

void Material::Bind() const {
	std::shared_ptr<ShaderAsset> activeShader = GetActiveShader();
	if (activeShader == nullptr || activeShader->IsReady == false || activeShader->ShaderObject == nullptr) {
		return;
	}
	activeShader->ShaderObject->Use();
	activeShader->ShaderObject->TrySetVector4("u_TintColor", TintColor);
	activeShader->ShaderObject->TrySetFloat("u_MetallicFactor", MetallicFactor);
	activeShader->ShaderObject->TrySetFloat("u_RoughnessFactor", RoughnessFactor);
	BindAlbedoTexture(activeShader);
	BindMetallicTexture(activeShader);
	BindRoughnessTexture(activeShader);
	BindNormalTexture(activeShader);
	for (auto const& [name, value] : m_Bools) {
		activeShader->ShaderObject->SetBool(name, value);
	}
	for (auto const& [name, value] : m_Ints) {
		activeShader->ShaderObject->SetInt(name, value);
	}
	for (auto const& [name, value] : m_Floats) {
		activeShader->ShaderObject->SetFloat(name, value);
	}
	for (auto const& [name, value] : m_Vector2s) {
		activeShader->ShaderObject->SetVector2(name, value);
	}
	for (auto const& [name, value] : m_Vector3s) {
		activeShader->ShaderObject->SetVector3(name, value);
	}
	for (auto const& [name, value] : m_Vector4s) {
		activeShader->ShaderObject->SetVector4(name, value);
	}
	for (auto const& [name, value] : m_Matrix4x4s) {
		activeShader->ShaderObject->SetMatrix4(name, value);
	}
}

void Material::SetBool(const std::string_view name, const bool value) const {
	m_Bools[std::string(name)] = value;
	std::shared_ptr<ShaderAsset> activeShader = GetActiveShader();
	if (activeShader != nullptr && activeShader->IsReady == true && activeShader->ShaderObject != nullptr) {
		activeShader->ShaderObject->Use();
		activeShader->ShaderObject->SetBool(name, value);
	}
}

void Material::SetInt(const std::string_view name, const i32 value) const {
	m_Ints[std::string(name)] = value;
	std::shared_ptr<ShaderAsset> activeShader = GetActiveShader();
	if (activeShader != nullptr && activeShader->IsReady == true && activeShader->ShaderObject != nullptr) {
		activeShader->ShaderObject->Use();
		activeShader->ShaderObject->SetInt(name, value);
	}
}

void Material::SetFloat(const std::string_view name, const f32 value) const {
	m_Floats[std::string(name)] = value;
	std::shared_ptr<ShaderAsset> activeShader = GetActiveShader();
	if (activeShader != nullptr && activeShader->IsReady == true && activeShader->ShaderObject != nullptr) {
		activeShader->ShaderObject->Use();
		activeShader->ShaderObject->SetFloat(name, value);
	}
}

void Material::SetVector2(const std::string_view name, Math::Vector2 const& vector) const {
	m_Vector2s[std::string(name)] = vector;
	std::shared_ptr<ShaderAsset> activeShader = GetActiveShader();
	if (activeShader != nullptr && activeShader->IsReady == true && activeShader->ShaderObject != nullptr) {
		activeShader->ShaderObject->Use();
		activeShader->ShaderObject->SetVector2(name, vector);
	}
}

void Material::SetVector3(const std::string_view name, Math::Vector3 const& vector) const {
	m_Vector3s[std::string(name)] = vector;
	std::shared_ptr<ShaderAsset> activeShader = GetActiveShader();
	if (activeShader != nullptr && activeShader->IsReady == true && activeShader->ShaderObject != nullptr) {
		activeShader->ShaderObject->Use();
		activeShader->ShaderObject->SetVector3(name, vector);
	}
}

void Material::SetVector4(const std::string_view name, Math::Vector4 const& vector) const {
	m_Vector4s[std::string(name)] = vector;
	std::shared_ptr<ShaderAsset> activeShader = GetActiveShader();
	if (activeShader != nullptr && activeShader->IsReady == true && activeShader->ShaderObject != nullptr) {
		activeShader->ShaderObject->Use();
		activeShader->ShaderObject->SetVector4(name, vector);
	}
}

void Material::SetMatrix4(const std::string_view name, Math::Matrix4x4 const& matrix) const {
	m_Matrix4x4s[std::string(name)] = matrix;
	std::shared_ptr<ShaderAsset> activeShader = GetActiveShader();
	if (activeShader != nullptr && activeShader->IsReady == true && activeShader->ShaderObject != nullptr) {
		activeShader->ShaderObject->Use();
		activeShader->ShaderObject->SetMatrix4(name, matrix);
	}
}

void Material::TrySetBool(const std::string_view name, const bool value) const {
	m_Bools[std::string(name)] = value;
	std::shared_ptr<ShaderAsset> activeShader = GetActiveShader();
	if (activeShader != nullptr && activeShader->IsReady == true && activeShader->ShaderObject != nullptr) {
		activeShader->ShaderObject->Use();
		activeShader->ShaderObject->TrySetBool(name, value);
	}
}

void Material::TrySetInt(const std::string_view name, const i32 value) const {
	m_Ints[std::string(name)] = value;
	std::shared_ptr<ShaderAsset> activeShader = GetActiveShader();
	if (activeShader != nullptr && activeShader->IsReady == true && activeShader->ShaderObject != nullptr) {
		activeShader->ShaderObject->Use();
		activeShader->ShaderObject->TrySetInt(name, value);
	}
}

void Material::TrySetFloat(const std::string_view name, const f32 value) const {
	m_Floats[std::string(name)] = value;
	std::shared_ptr<ShaderAsset> activeShader = GetActiveShader();
	if (activeShader != nullptr && activeShader->IsReady == true && activeShader->ShaderObject != nullptr) {
		activeShader->ShaderObject->Use();
		activeShader->ShaderObject->TrySetFloat(name, value);
	}
}

void Material::TrySetVector2(const std::string_view name, Math::Vector2 const& vector) const {
	m_Vector2s[std::string(name)] = vector;
	std::shared_ptr<ShaderAsset> activeShader = GetActiveShader();
	if (activeShader != nullptr && activeShader->IsReady == true && activeShader->ShaderObject != nullptr) {
		activeShader->ShaderObject->Use();
		activeShader->ShaderObject->TrySetVector2(name, vector);
	}
}

void Material::TrySetVector3(const std::string_view name, Math::Vector3 const& vector) const {
	m_Vector3s[std::string(name)] = vector;
	std::shared_ptr<ShaderAsset> activeShader = GetActiveShader();
	if (activeShader != nullptr && activeShader->IsReady == true && activeShader->ShaderObject != nullptr) {
		activeShader->ShaderObject->Use();
		activeShader->ShaderObject->TrySetVector3(name, vector);
	}
}

void Material::TrySetVector4(const std::string_view name, Math::Vector4 const& vector) const {
	m_Vector4s[std::string(name)] = vector;
	std::shared_ptr<ShaderAsset> activeShader = GetActiveShader();
	if (activeShader != nullptr && activeShader->IsReady == true && activeShader->ShaderObject != nullptr) {
		activeShader->ShaderObject->Use();
		activeShader->ShaderObject->TrySetVector4(name, vector);
	}
}

void Material::TrySetMatrix4(const std::string_view name, Math::Matrix4x4 const& matrix) const {
	m_Matrix4x4s[std::string(name)] = matrix;
	std::shared_ptr<ShaderAsset> activeShader = GetActiveShader();
	if (activeShader != nullptr && activeShader->IsReady == true && activeShader->ShaderObject != nullptr) {
		activeShader->ShaderObject->Use();
		activeShader->ShaderObject->TrySetMatrix4(name, matrix);
	}
}

u32 Material::Create1x1Texture(const u8 red, const u8 green, const u8 blue, const u8 alpha) {
	u32 textureID{};
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	u8 pixel[4] = { red, green, blue, alpha };
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixel);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);
	return textureID;
}

u32 Material::GetWhiteFallbackTexture() noexcept {
	static u32 textureID = Create1x1Texture(255, 255, 255);
	return textureID;
}

u32 Material::GetNormalFallbackTexture() noexcept {
	static u32 textureID = Create1x1Texture(128, 128, 255);
	return textureID;
}

void Material::BindAlbedoTexture(std::shared_ptr<ShaderAsset> const& activeShader) const noexcept {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, AlbedoTexture != nullptr && AlbedoTexture->IsReady ? AlbedoTexture->TextureID : GetWhiteFallbackTexture());
	if (activeShader != nullptr && activeShader->ShaderObject != nullptr) {
		activeShader->ShaderObject->TrySetInt("u_AlbedoMap", 0);
	}
}

void Material::BindMetallicTexture(std::shared_ptr<ShaderAsset> const& activeShader) const noexcept {
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, MetallicTexture != nullptr && MetallicTexture->IsReady ? MetallicTexture->TextureID : GetWhiteFallbackTexture());
	if (activeShader != nullptr && activeShader->ShaderObject != nullptr) {
		activeShader->ShaderObject->TrySetInt("u_MetallicMap", 1);
	}
}

void Material::BindRoughnessTexture(std::shared_ptr<ShaderAsset> const& activeShader) const noexcept {
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, RoughnessTexture != nullptr && RoughnessTexture->IsReady ? RoughnessTexture->TextureID : GetWhiteFallbackTexture());
	if (activeShader != nullptr && activeShader->ShaderObject != nullptr) {
		activeShader->ShaderObject->TrySetInt("u_RoughnessMap", 2);
	}
}

void Material::BindNormalTexture(std::shared_ptr<ShaderAsset> const& activeShader) const noexcept {
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, NormalTexture != nullptr && NormalTexture->IsReady ? NormalTexture->TextureID : GetNormalFallbackTexture());
	if (activeShader != nullptr && activeShader->ShaderObject != nullptr) {
		activeShader->ShaderObject->TrySetInt("u_NormalMap", 3);
	}
}

}
