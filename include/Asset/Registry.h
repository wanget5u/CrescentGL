#pragma once

#include <memory>
#include <unordered_map>

#include "Loader.h"
#include "Core/Core.h"
#include "Type/Type.h"

namespace Crescent::Asset {
struct Registry {
	static Registry& Instance() {
		static Registry s_Instance;
		return s_Instance;
	}
	template <typename T>
	std::shared_ptr<T> GetOrLoad(std::string const& filePath, Type type) {
		auto it = m_Registry.find(filePath);
		if (it != m_Registry.end()) {
			return std::static_pointer_cast<T>(it->second);
		}
		std::shared_ptr<T> asset = std::make_shared<T>();
		asset->FilePath = filePath;
		asset->Type = type;
		asset->IsReady = false;
		m_Registry[filePath] = asset;
		Loader::Instance().LoadAssetAsync(filePath, type);
		return asset;
	}
	void OnUpdate();
private:
	std::unordered_map<std::string, std::shared_ptr<Asset>> m_Registry{};
};
}
