#pragma once
#include <Core/Core.h>
#include "Math/Constants.h"

namespace Crescent::Math {

template <typename Number> [[nodiscard]]
constexpr Number ToRadian(Number degrees) {
	return degrees * (static_cast<Number>(PI) / static_cast<Number>(180));
}

}
