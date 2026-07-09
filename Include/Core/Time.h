#pragma once
#include "Core/Core.h"

namespace Crescent {
/// Manages the Engine's timing, frame deltas, and the semi-fixed timestep accumulator.
/// Decouples the rendering frequency from gameplay/physics simulation ticks.
struct Time {
	static void OnUpdate(f32 currentEngineTime);
	static bool AccumulatorHasSubstep();
	static void ConsumeSubstep();
	[[nodiscard]] static f32 GetVariableDeltaTime();
	[[nodiscard]] static f32 GetVariableDeltaTimeMs();
	[[nodiscard]] static f32 GetFixedDeltaTime();
	[[nodiscard]] static f32 GetTotalTime();
	[[nodiscard]] static f32 GetSubstepAlpha();
	[[nodiscard]] static f32 GetFPS();
private:
	static f32 s_VariableDeltaTime;
	static f32 s_LastFrameTime;
	static f32 s_TotalTime;
	static f32 s_Accumulator;
};
}