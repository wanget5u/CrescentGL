#include "UI/UIObserver.h"

namespace Crescent::UI {

u32 EventDispatcher::Subscribe(const EventType type, EventCallback callback) {
	const u32 id = ++s_NextID;
	s_Listeners[type].emplace_back(id, std::move(callback));
	return id;
}

void EventDispatcher::Unsubscribe(const EventType type, const u32 id) {
	std::unordered_map<EventType, std::vector<std::pair<u32, EventCallback>>>::iterator it
		= s_Listeners.find(type);
	if (it != s_Listeners.end()) {
		std::vector<std::pair<u32, EventCallback>>& listeners = it->second;
		for (size_t a = 0; a < listeners.size(); ++a) {
			if (listeners[a].first == id) {
				listeners.erase(listeners.begin() + a);
				break;
			}
		}
	}
}

void EventDispatcher::Emit(Event const& event) {
	if (auto it = s_Listeners.find(event.Type); it != s_Listeners.end()) {
		auto listenersCopy = it->second;
		for (auto const& [id, callback] : listenersCopy) {
			if (callback) {
				callback(event);
			}
		}
	}
}

}
