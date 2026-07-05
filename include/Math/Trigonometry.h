#pragma once
#include <Core/Core.h>
#include "Math/Constants.h"

namespace Crescent::Math {

template <typename Number> [[nodiscard]]
constexpr Number DegToRad(Number degrees) {
	return degrees * (static_cast<Number>(PI) / static_cast<Number>(180));
}

}
