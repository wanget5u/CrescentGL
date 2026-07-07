#include "Core/Time.h"

namespace Crescent {

f32 Time::s_VariableDeltaTime = 0.0f;
f32 Time::s_LastFrameTime     = 0.0f;
f32 Time::s_TotalTime         = 0.0f;
f32 Time::s_Accumulator       = 0.0f;

void Time::OnUpdate(f32 const currentEngineTime) {
	f32 frameTime = currentEngineTime - s_LastFrameTime;
	s_LastFrameTime = currentEngineTime;
	if (frameTime > MAX_ALLOWED_DELTA) {
		frameTime = MAX_ALLOWED_DELTA;
	}
	s_VariableDeltaTime = frameTime;
	s_TotalTime = currentEngineTime;
	s_Accumulator += frameTime;
}

bool Time::AccumulatorHasSubstep() { return s_Accumulator >= FIXED_TIMESTEP; }
void Time::ConsumeSubstep() { s_Accumulator -= FIXED_TIMESTEP; }
f32 Time::GetVariableDeltaTime() { return s_VariableDeltaTime; }
f32 Time::GetVariableDeltaTimeMs() { return s_VariableDeltaTime * 1000.0f; }
f32 Time::GetFixedDeltaTime() { return FIXED_TIMESTEP; }
f32 Time::GetTotalTime() { return s_TotalTime; }
f32 Time::GetSubstepAlpha() { return s_Accumulator / FIXED_TIMESTEP; }
f32 Time::GetFPS() { return 1.0f / s_VariableDeltaTime; }

}
