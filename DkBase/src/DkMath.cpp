#include "DkMath.h"

namespace math {
	vec<3> cross(const vec<3>& v1, const vec<3>& v2) {
		return vec<3>({ v1[1] * v2[2] - v1[2] * v2[1],
						v1[2] * v2[0] - v1[0] * v2[2],
						v1[0] * v2[1] - v1[1] * v2[0] });
	}

	float determinant(const mat<3, 3>& m) {
		return	m(0, 0) * (m(1, 1) * m(2, 2) - m(1, 2) * m(2, 1)) -
				m(0, 1) * (m(1, 0) * m(2, 2) - m(1, 2) * m(2, 0)) +
				m(0, 2) * (m(1, 0) * m(2, 1) - m(1, 1) * m(2, 0));
	}


	mat<3, 3> inverse(const mat<3, 3>& m) {
		float det = determinant(m);
		if (det == 0) {
			throw std::runtime_error("Cannot invert singular matrix");
		}
		mat<3, 3> ret({
			m(1, 1) * m(2, 2) - m(1, 2) * m(2, 1),		m(0, 2) * m(2, 1) - m(0, 1) * m(2, 2),		m(0, 1) * m(1, 2) - m(0, 2) * m(1, 1),
			m(1, 2) * m(2, 0) - m(1, 0) * m(2, 2),		m(0, 0) * m(2, 2) - m(0, 2) * m(2, 0),		m(0, 2) * m(1, 0) - m(0, 0) * m(1, 2),
			m(1, 0) * m(2, 1) - m(1, 1) * m(2, 0),		m(0, 1) * m(2, 0) - m(0, 0) * m(2, 1),		m(0, 0) * m(1, 1) - m(0, 1) * m(1, 0)
		});

		return ret / det;
	}
}