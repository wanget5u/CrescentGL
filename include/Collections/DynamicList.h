#pragma once
#include <cassert>
#include <utility>
#include <cstring>
#include <memory>
#include <new>

#include "Math/Arithmetic.h"

namespace Crescent::Collections {
/// Over-aligned dynamic array. Handles SIMD-aligned data enforcing the alignment constraints during alloc.
/// Avoids heap-allocation overhead when the type is trivial using `std::memcpy`
template <typename T, size_t MinAlign = 16>
struct DynamicList {
	DynamicList() = default;
	DynamicList(DynamicList const&) = delete;
	/// Move Constructor
	DynamicList(DynamicList&& other) noexcept : m_Data(other.m_Data), m_Size(other.m_Size), m_Capacity(other.m_Capacity) {
		other.m_Data = nullptr;
		other.m_Size = 0;
		other.m_Capacity = 0;
	}
	~DynamicList() {
		Clear();
		FreeStorage(m_Data);
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////
	DynamicList& operator=(DynamicList const&) = delete;
	DynamicList& operator=(DynamicList&& other) noexcept {
		if (this != &other) {
			Clear();
			FreeStorage(m_Data);
			m_Data = std::exchange(other.m_Data, nullptr);
			m_Size = std::exchange(other.m_Size, 0);
			m_Capacity = std::exchange(other.m_Capacity, 0);
		}
		return *this;
	}
	[[nodiscard]] const T& operator[](size_t index) const {
		assert(index < m_Size && "DynamicList index out of bounds.");
		return m_Data[index];
	}
	[[nodiscard]] T& operator[](size_t index) {
		assert(index < m_Size && "DynamicList index out of bounds.");
		return m_Data[index];
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Copies the item into the back of the list and reallocates if is full
	void PushBack(T const& value) {
		if (m_Size >= m_Capacity) {
			// check if the value is already in the data
			if (&value >= m_Data && &value < m_Data + m_Size) {
				const size_t index = &value - m_Data;
				Reserve(GetNewCapacity());
				new (&m_Data[m_Size]) T(std::move(m_Data[index]));
				m_Size++;
				return;
			}
			Reserve(GetNewCapacity());
		}
		new (&m_Data[m_Size]) T(value);
		m_Size++;
	}
	/// Moves and item into the back of the list and reallocates if is full
	void PushBack(T&& value) {
		if (m_Size >= m_Capacity) {
			Reserve(GetNewCapacity());
		}
		new (&m_Data[m_Size]) T(std::move(value));
		m_Size++;
	}
	void PopBack() {
		assert(m_Size > 0 && "Cannot pop from empty list.");
		m_Size--;
		if constexpr (std::is_trivially_destructible_v<T> == false) {
			m_Data[m_Size].~T();
		}
	}
	/// Constructs elements in-place at the end of the list. Avoids extra copies
	template <typename... Args>
	T& EmplaceBack(Args&&... args) {
		if (m_Size >= m_Capacity) {
			Reserve(GetNewCapacity());
		}
		new (&m_Data[m_Size]) T(std::forward<Args>(args)...);
		return m_Data[m_Size++];
	}
	/// Reserves memory for at least newCapacity elements. Alignment constraints
	/// are kept for GPU-bound data structures
	void Reserve(size_t const newCapacity) {
		if (newCapacity <= m_Capacity) { return; }
		// allocating with over-alignment for vectors/matrices
		T* newStorage = static_cast<T*>(::operator new(newCapacity * sizeof(T), std::align_val_t{MIN_ALIGNMENT}));
		if (m_Size > 0) {
			// check if we can safely delete it from the memory (std::memcpy requirement)
			if constexpr (std::is_trivially_copyable_v<T> == true) {
				// copy the old chunk of memory to the new location
				std::memcpy(static_cast<void*>(newStorage), m_Data, m_Size * sizeof(T));
			}
			else if constexpr (std::is_nothrow_move_constructible_v<T> == true || std::is_nothrow_copy_constructible_v<T> == true) {
				for (size_t a = 0; a < m_Size; a++) {
					new (&newStorage[a]) T(std::move_if_noexcept(m_Data[a]));
				}
				if constexpr (std::is_trivially_destructible_v<T> == false) {
					for (size_t a = 0; a < m_Size; a++) {
						m_Data[a].~T();
					}
				}
			}
			else {
				size_t a{0};
				try {
					for (; a < m_Size; a++) {
						// creating new objects and invoking their constructors one by one
						// std::move_if_noexcept prevents data corruption
						new (&newStorage[a]) T(std::move_if_noexcept(m_Data[a]));
					}
				} catch (...) {
					// cleanup for newly created objects, if their
					// constructor throws an exception for some reason
					for (size_t b = 0; b < a; ++b) {
						newStorage[b].~T();
					}
					FreeStorage(newStorage);
					throw;
				}
				// destroying old objects if the migration to
				// the new chunk of memory was successful
				if constexpr (std::is_trivially_destructible_v<T> == false) {
					for (size_t b = 0; b < m_Size; b++) {
						m_Data[b].~T();
					}
				}
			}
		}
		FreeStorage(m_Data);
		m_Data = newStorage;
		m_Capacity = newCapacity;
	}
	/// Destroys all elements without releasing reserved memory chunk capacity
	void Clear() {
		// only calling the destructors when the data is not trivial
		if constexpr (std::is_trivially_destructible_v<T> == false) {
			for (size_t a = 0; a < m_Size; a++) {
				m_Data[a].~T();
			}
		}
		m_Size = 0;
	}
	/// Generating a procedural mesh or loading assets, the amount of vertices
	/// is known upfront. Calling PushBack/EmplaceBack forces thousands of redundant
	/// capacity checks and branch evaluations, and it doesn't align with the SIMD.
	void ResizeUninitialized(size_t const newSize) {
		static_assert(std::is_trivially_constructible_v<T> && std::is_trivially_destructible_v<T>,
				  "ResizeUninitialized can only be used with trivial types to prevent Undefined Behavior on destruction.");
		if (newSize > m_Capacity) {
			Reserve(newSize);
		}
		m_Size = newSize;
	}
	void Append(const T* data, size_t const count) {
		if (count == 0) { return; }
		size_t offset = 0;
		bool isSelfAliased = (data >= m_Data && data < m_Data + m_Capacity);
		if (isSelfAliased == true) {
			offset = data - m_Data;
		}
		if (m_Size + count > m_Capacity) {
			size_t targetCapacity = Math::Max(GetNewCapacity(), m_Size + count);
			Reserve(targetCapacity);
			if (isSelfAliased == true) {
				data = m_Data + offset;
			}
		}
		if constexpr (std::is_trivially_copyable_v<T> == true) {
			if (isSelfAliased == true) {
				std::memmove(m_Data + m_Size, data, count * sizeof(T));
			}
			else {
				std::memcpy(m_Data + m_Size, data, count * sizeof(T));
			}
		}
		m_Size += count;
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Data Access
	[[nodiscard]] 	    size_t GetSize()	        const noexcept { return m_Size; }
	[[nodiscard]]       size_t GetSizeInBytes()     const noexcept { return m_Size * sizeof(T); }
	[[nodiscard]] 	    size_t GetCapacity()        const noexcept { return m_Capacity; }
	[[nodiscard]]       size_t GetCapacityInBytes() const noexcept { return m_Capacity * sizeof(T); }
	[[nodiscard]] const     T* GetData()	        const noexcept { return m_Data; }
	[[nodiscard]]		    T* GetData()	              noexcept { return m_Data; }
	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Iterator Endpoints
	[[nodiscard]] const T* cbegin() const noexcept { return m_Data; }
	[[nodiscard]]		T* begin()	      noexcept { return m_Data; }
	[[nodiscard]] const T* cend()   const noexcept { return m_Data + m_Size; }
	[[nodiscard]]		T* end()		  noexcept { return m_Data + m_Size; }
	[[nodiscard]] const T* data()   const noexcept { return m_Data; }
	[[nodiscard]]       T* data()         noexcept { return m_Data; }
private:
	static constexpr size_t MIN_ALIGNMENT = (alignof(T) > MinAlign) ? alignof(T) : MinAlign;
	T* m_Data			  {nullptr};
	std::size_t m_Size    {};
	std::size_t m_Capacity{};
	/// Frees specifically aligned storage
	void FreeStorage(T* storage) {
		if (storage != nullptr) {
			::operator delete(storage, std::align_val_t{MIN_ALIGNMENT});
		}
	}
	/// Returns new capacity restricted to a single rule of list expansion
	[[nodiscard]] size_t GetNewCapacity() const noexcept {
		return (m_Capacity > 0) ? (m_Capacity + (m_Capacity >> 1)) : 4;
	}
};

}
