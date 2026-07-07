#pragma once
#include "Core/Core.h"

namespace Crescent {
/// Cache-friendly pseudo-random number generator
struct Random {
	static void Initialize(const u64 seed = 0x4d595df4d0f33173ULL, const u64 sequence = 1u) {
		s_Inc = (sequence << 1u) | 1u;
		s_State = 0u;
		static_cast<void>(U32());
		s_State += seed;
		static_cast<void>(U32());
	}
	[[nodiscard]] constexpr static u32 U32() noexcept {
		const u64 oldState = s_State;
		s_State = oldState * 6364136223846793005ULL + s_Inc;
		u32 const xorShifted = static_cast<u32>(((oldState >> 18u) ^ oldState) >> 27u);
		u32 const rot = static_cast<u32>(oldState >> 59u);
		return (xorShifted >> rot) | (xorShifted << ((-rot) & 31u));
	}
	[[nodiscard]] constexpr static u32 U32Range(const u32 min, const u32 max) noexcept {
		if (min >= max) { return min; }
		u32 const range = max - min + 1;
		return min + (U32() % range);
	}
	[[nodiscard]] constexpr static f32 F32() noexcept {
		return static_cast<f32>(U32()) * 0x1.0p-32f;
	}
	[[nodiscard]] constexpr static f32 F32Range(const f32 min, const f32 max) noexcept {
		if (min >= max) { return min; }
		return min + F32() * (max - min);
	}
private:
	inline static u64 s_State = 0x4d595df4d0f33173ULL;
	inline static u64 s_Inc   = 1442695040888963407ULL;
};
}