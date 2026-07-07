#pragma once
#include <atomic>
#include <new>
#include <type_traits>
#include <utility>

namespace Crescent {

template <typename T, size_t Capacity = 1024, size_t MinAlign = 64>
struct DynamicQueue {
	DynamicQueue() {
		m_Data = static_cast<T*>(::operator new(Capacity * sizeof(T), std::align_val_t{MinAlign}));
	}
	DynamicQueue(DynamicQueue const&) = delete;
	DynamicQueue(DynamicQueue&& other) noexcept : m_Data(other.m_Data) {
		m_Head.store(other.m_Head.load(std::memory_order_relaxed), std::memory_order_relaxed);
		m_Tail.store(other.m_Tail.load(std::memory_order_relaxed), std::memory_order_relaxed);
		other.m_Data = nullptr;
		other.m_Head.store(0, std::memory_order_relaxed);
		other.m_Tail.store(0, std::memory_order_relaxed);
	}
	~DynamicQueue() {
		Clear();
		FreeStorage(m_Data);
	}
	DynamicQueue& operator=(DynamicQueue const&) = delete;
	DynamicQueue& operator=(DynamicQueue&& other) noexcept {
		if (this != &other) {
			Clear();
			FreeStorage(m_Data);
			m_Data = std::exchange(other.m_Data, nullptr);
			m_Head.store(other.m_Head.load(std::memory_order_relaxed), std::memory_order_relaxed);
			m_Tail.store(other.m_Tail.load(std::memory_order_relaxed), std::memory_order_relaxed);
			other.m_Head.store(0, std::memory_order_relaxed);
			other.m_Tail.store(0, std::memory_order_relaxed);
		}
		return *this;
	}
	[[nodiscard]] bool IsEmpty() const noexcept {
		return m_Head.load(std::memory_order_relaxed) == m_Tail.load(std::memory_order_relaxed);
	}
	bool Push(const T& item) {
		const size_t currentTail = m_Tail.load(std::memory_order_relaxed);
		const size_t nextTail = (currentTail + 1) % Capacity;
		if (nextTail == m_Head.load(std::memory_order_acquire)) {
			return false;
		}
		new (&m_Data[currentTail]) T(item);
		m_Tail.store(nextTail, std::memory_order_release);
		return true;
	}
	bool Push(T&& item) {
		const size_t currentTail = m_Tail.load(std::memory_order_relaxed);
		const size_t nextTail = (currentTail + 1) % Capacity;
		if (nextTail == m_Head.load(std::memory_order_acquire)) {
			return false;
		}
		new (&m_Data[currentTail]) T(std::move(item));
		m_Tail.store(nextTail, std::memory_order_release);
		return true;
	}
	T Pop() {
		size_t currentHead = m_Head.load(std::memory_order_relaxed);
		if (currentHead == m_Tail.load(std::memory_order_acquire)) {
			return T{};
		}
		T item = std::move(m_Data[currentHead]);
		if constexpr (std::is_trivially_destructible_v<T> == false) {
			m_Data[currentHead].~T();
		}
		m_Head.store((currentHead + 1) % Capacity, std::memory_order_release);
		return item;
	}
	/// Destroys all elements without releasing reserved memory chunk capacity
	void Clear() {
		if constexpr (std::is_trivially_destructible_v<T> == false) {
			size_t currentHead = m_Head.load(std::memory_order_relaxed);
			const size_t currentTail = m_Tail.load(std::memory_order_relaxed);
			while (currentHead != currentTail) {
				m_Data[currentHead].~T();
				currentHead = (currentHead + 1) % Capacity;
			}
		}
		m_Head.store(0, std::memory_order_relaxed);
		m_Tail.store(0, std::memory_order_relaxed);
	}
private:
	T* m_Data{nullptr};
	alignas(MinAlign) std::atomic<size_t> m_Head{0};
	alignas(MinAlign) std::atomic<size_t> m_Tail{0};
	/// Frees specifically aligned storage
	void FreeStorage(T* storage) {
		if (storage != nullptr) {
			::operator delete(storage, std::align_val_t{MinAlign});
		}
	}
};

}