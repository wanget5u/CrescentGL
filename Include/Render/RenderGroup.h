#pragma once
#include "Collection/DynamicList.h"

namespace Crescent {
struct IRenderGroup {
	virtual ~IRenderGroup() = default;
	virtual void FlushLoad() = 0;
	virtual void FlushUnload() = 0;
	virtual void Clear() = 0;
};
template <typename T>
struct RenderGroup : IRenderGroup {
	DynamicList<T*> Registered{};
	DynamicList<T*> Staged{};

void Register(T* instance, const bool isBatchLoading) {
	if (isBatchLoading) {
		Staged.PushBack(instance);
	}
	else {
		Registered.PushBack(instance);
	}
}
void Unregister(T* instance, const bool isBatchUnloading) {
	if (isBatchUnloading) {
		for (size_t a = 0; a < Registered.GetSize(); ++a) {
			if (Registered[a] == instance) {
				Registered[a] = nullptr;
				break;
			}
		}
	}
	else {
		Registered.Remove(instance);
		Staged.Remove(instance);
	}
}
void FlushLoad() override {
	if (Staged.IsEmpty() == false) {
		Registered.Reserve(Registered.GetSize() + Staged.GetSize());
		for (size_t a = 0; a < Staged.GetSize(); ++a) {
			Registered.PushBack(Staged[a]);
		}
		Staged.Clear();
	}
}
void FlushUnload() override {
	size_t writeIndex = 0;
	for (size_t readIndex = 0; readIndex < Registered.GetSize(); ++readIndex) {
		if (Registered[readIndex] != nullptr) {
			Registered[writeIndex] = Registered[readIndex];
			writeIndex++;
		}
	}
	while (Registered.GetSize() > writeIndex) {
		Registered.PopBack();
	}
}
void Clear() override {
	Registered.Clear();
	Staged.Clear();
}
};
}