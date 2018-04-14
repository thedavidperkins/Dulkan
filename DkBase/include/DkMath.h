#ifndef DK_MATH_H
#define DK_MATH_H

#include "DkCommon.h"

namespace math {
	struct vec2 {
		union {
			struct {
				float x, y;
			};
			float v[2];
		};

		float operator[](uint ind) {
			return v[ind];
		}
	};

	struct vec3 {
		union {
			struct {
				float x, y, z;
			};
			float v[3];
		};

		float operator[](uint ind) {
			return v[ind];
		}

		~vec3() {};
	};

	struct vec4 {
		union {
			struct {
				float x, y, z, w;
			};
			float v[4];
		};

		float operator[](uint ind) {
			return v[ind];
		}
	};

	struct mat3 {
		vec3 m[3];

		vec3 operator[](uint ind) {
			return m[ind];
		}
	};

	struct mat4 {
		vec4 m[4];

		vec4 operator()(uint ind) {
			return m[ind];
		}
	};
}
#endif // !DK_MATH_H

