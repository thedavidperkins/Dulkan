#include "DkMath.h"

namespace math {
	vec3 cross(const vec3& v1, const vec3& v2) {
		return { v1[1] * v2[2] - v1[2] * v2[1],
				 v1[2] * v2[0] - v1[0] * v2[2],
				 v1[0] * v2[1] - v1[1] * v2[0] };
	}
}