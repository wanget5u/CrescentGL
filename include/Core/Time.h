#pragma once
#include "Core/Core.h"

namespace Crescent {

class Time {
public:
	static constexpr f32 FIXED_TIMESTEP = 1.0f / 60.0f;
	static constexpr f32 MAX_ALLOWED_DELTA = 0.25f;

	static void OnUpdate(f32 currentEngineTime) {
		f32 frameTime = currentEngineTime - s_LastFrameTime;
		s_LastFrameTime = currentEngineTime;

		if (frameTime > MAX_ALLOWED_DELTA) {
			frameTime = MAX_ALLOWED_DELTA;
		}

		s_VariableDeltaTime = frameTime;
		s_TotalTime = currentEngineTime;
		s_Accumulator += frameTime;
	}

	static bool AccumulatorHasSubstep() {
		return s_Accumulator >= FIXED_TIMESTEP;
	}

	static void ConsumeSubstep() {
		s_Accumulator -= FIXED_TIMESTEP;
	}

	static f32 GetVariableDeltaTime() { return s_VariableDeltaTime; }
	static f32 GetFixedDeltaTime() { return FIXED_TIMESTEP; }
	static f32 GetTotalTime() { return s_TotalTime; }
	// how far are we between the current and the fixed tick
	// returns 0.0 to 1.0 value
	static f32 GetSubstepAlpha() { return s_Accumulator / FIXED_TIMESTEP; }
	static f32 GetFPS() { return 1.0f / s_VariableDeltaTime; }

private:
	inline static f32 s_VariableDeltaTime = 0.0f;
	inline static f32 s_LastFrameTime = 0.0f;
	inline static f32 s_TotalTime = 0.0f;
	inline static f32 s_Accumulator = 0.0f;
};
}