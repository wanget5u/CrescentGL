#pragma once
#include <functional>
#include <variant>
#include <unordered_map>
#include <vector>

#include "Core/Core.h"

namespace Crescent {
	struct Scene;
	struct Node;
}

namespace Crescent::UI {
enum class EventType {
	NodeSelected,
	SceneTabChanged,
	DebugToggle,
	ToolbarActionTriggered
};
struct Event {
	EventType Type;
	std::variant<std::monostate, Node*, Scene*, bool, i32> Data{std::monostate{}};

};
using EventCallback = std::function<void(Event const&)>;
struct EventDispatcher {
	static u32 Subscribe(EventType type, EventCallback callback);
	static void Unsubscribe(EventType type, u32 id);
	static void Emit(Event const& event);
private:
	inline static std::unordered_map<EventType, std::vector<std::pair<u32, EventCallback>>> s_Listeners;
	inline static u32 s_NextID{0};
};
}
