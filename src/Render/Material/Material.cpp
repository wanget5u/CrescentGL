#include "Render/Material/Material.h"
#include "Render/Shader/Shader.h"
#include <glad/glad.h>
#include <utility>

namespace Crescent::Render {

Material::Material(std::shared_ptr<Render::Shader> shader, Math::Vector4 const& baseColor)
	: Shader(std::move(shader)), TintColor(baseColor) {}

std::shared_ptr<Material> Material::GetDefaultMaterial() {
	static auto defaultMaterial = std::make_shared<Material>(
		std::make_shared<Render::Shader>("Shaders/unlit.vert", "Shaders/unlit.frag"),
		GetDefaultColor()
	);
	return defaultMaterial;
}

Math::Vector4 Material::GetDefaultColor() noexcept {
	return Math::Vector4{1.0f, 0.6f, 0.8f, 1.0f};
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

void Material::Bind() const {
	if (Shader == nullptr) { return; }
	Shader->Use();
	Shader->SetVector4("u_TintColor", TintColor);
	BindAlbedoTexture();
	BindMetallicTexture();
	BindRoughnessTexture();
	BindNormalTexture();
}

u32 Material::GetWhiteFallbackTexture() noexcept {
	static u32 textureID = Create1x1Texture(255, 255, 255);
	return textureID;
}

u32 Material::GetNormalFallbackTexture() noexcept {
	static u32 textureID = Create1x1Texture(128, 128, 255);
	return textureID;
}

void Material::BindAlbedoTexture() const noexcept {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, AlbedoTextureID != 0 ? AlbedoTextureID : GetWhiteFallbackTexture());
	Shader->SetInt("u_AlbedoMap", 0);
}

void Material::BindMetallicTexture() const noexcept {
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, MetallicTextureID != 0 ? MetallicTextureID : GetWhiteFallbackTexture());
	Shader->SetInt("u_MetallicMap", 1);
}

void Material::BindRoughnessTexture() const noexcept {
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, RoughnessTextureID != 0 ? RoughnessTextureID : GetWhiteFallbackTexture());
	Shader->SetInt("u_RoughnessMap", 2);
}

void Material::BindNormalTexture() const noexcept {
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, NormalTextureID != 0 ? NormalTextureID : GetNormalFallbackTexture());
	Shader->SetInt("u_NormalMap", 3);
}

}
