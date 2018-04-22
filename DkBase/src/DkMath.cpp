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

	mat<3, 3> rotation(float angle, const vec<3>& axis) {
		vec3 normAx = normalize(axis);
		float x = normAx[0];
		float y = normAx[1];
		float z = normAx[2];
		mat3 id = ident<3>();

		mat3 aTa = {
			x * x,  x * y,  x * z,
			y * x,  y * y,  y * z,
			z * x,  z * y,  z * z
		};

		mat3 aDual = {
			 0, -z,  y,
			 z,  0, -x,
			-y,  x,  0
		};

		aDual = aDual;

		float radAngle = angle * PI / 180.f;

		mat3 rotation = id * cos(radAngle) + aTa * (1 - cos(radAngle)) + aDual * sin(radAngle);

		return rotation;
	}

	mat<4, 4> scale(float sx, float sy, float sz) {
		return mat4(
			 sx, 0.f, 0.f, 0.f,
			0.f,  sy, 0.f, 0.f,
			0.f, 0.f,  sz, 0.f,
			0.f, 0.f, 0.f, 1.f
		);
	}

	mat<4, 4> translate(float tx, float ty, float tz) {
		return mat4(
			1.f, 0.f, 0.f,  tx,
			0.f, 1.f, 0.f,  ty,
			0.f, 0.f, 1.f,  tz,
			0.f, 0.f, 0.f, 1.f
		);
	}

	mat<4, 4> lookAt(const vec<3>& eye, const vec<3>& center, const vec<3>& up) {
		vec3 w = normalize(eye - center);	// normal z
		vec3 u = normalize(cross(up, w));	// normal x
		vec3 v = cross(w, u);				// normal y

		mat4 look(
			u[0], u[1], u[2], -dot(u, eye),
			v[0], v[1], v[2], -dot(v, eye),
			w[0], w[1], w[2], -dot(w, eye),
			 0.f,  0.f,  0.f,          1.f
		);

		return look;
	}

	mat<4, 4> perspective(float fovy, float aspect, float zNear, float zFar) {
		float d = 1.f / tan((fovy * PI / 180.f) / 2.f);
		float A = zFar / (zNear - zFar);
		float B = zFar * zNear / (zNear - zFar);

		return mat4(
			d / aspect, 0.f,  0.f, 0.f,
			       0.f,  -d,  0.f, 0.f,
			       0.f, 0.f,    A,   B,
			       0.f, 0.f, -1.f, 0.f
		);
	}
}