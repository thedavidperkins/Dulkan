#ifndef DK_MATH_H
#define DK_MATH_H

#include <array>
#include "DkCommon.h"

namespace math {
	template<uint n>
	class vec {
	public:
		vec(std::array<float, n> init) : _v(init) {}
		vec() : _v() {
			for (float& el : _v) {
				el = 0.f;
			}
		}

		float& operator[](uint ind) {
			if (ind < 0 || ind >= n) {
				throw std::runtime_error("Error: vector index out of bounds");
			}
			return _v[ind];
		}

		const float& operator[](uint ind) const {
			if (ind < 0 || ind >= n) {
				throw std::runtime_error("Error: vector index out of bounds");
			}
			return _v[ind];
		}

		// right scalar multiplication
		template<typename T>
		vec<n> operator*(T scal) {
			vec<n> ret;
			for (uint iter = 0; iter < n; ++iter) {
				ret[iter] = _v[iter] * scal;
			}
			return ret;
		}

		// vector addition
		vec<n> operator+(const vec<n>& other) const {
			vec<n> ret;
			for (uint iter = 0; iter < n; ++iter) {
				ret[iter] = _v[iter] + other[iter];
			}
			return ret;
		}

		// vector subtraction
		vec<n> operator-(const vec<n>& other) const {
			return (*this) + (-other);
		}

	private:
		std::array<float, n> _v;
	};

	class vec2 : public vec<2> {
	public:
		vec2(float x, float y) : vec<2>({ x , y }) {}
		vec2(float f) : vec2(f, f) {}
		vec2() : vec2(0.f) {}
	};

	class vec3 : public vec<3> {
	public:
		vec3(float x, float y, float z) : vec<3>({ x , y, z }) {}
		vec3(float f) : vec3(f, f, f) {}
		vec3() : vec3(0.f) {}
	};

	class vec4 : public vec<4> {
	public:
		vec4(float x, float y, float z, float w) : vec<4>({ x , y, z, w }) {}
		vec4(float f) : vec4(f, f, f, f) {}
		vec4() : vec4(0.f) {}
	};

	template <uint n, uint m>
	class mat {
	public:
		mat(std::array<float, n * m> init) : _m(init) {}

		float& operator()(uint row, uint col) {
			if (row < 0 || row >= n || col < 0 || col >= m) {
				throw std::runtime_error("Error: matrix indices out of bounds");
			}
			return _m[col + m * row];
		}

		const float& operator()(uint row, uint col) const {
			if (row < 0 || row >= n || col < 0 || col >= m) {
				throw std::runtime_error("Error: matrix indices out of bounds");
			}
			return _m[col + m * row];
		}

		vec<m> row(uint r) {
			if (r < 0 || r > n) {
				throw std::runtime_error("Error: matrix row index out of bounds");
			}
			vec<m> ret(0.f);
			for (uint iter = 0; iter < m; ++iter) {
				ret[iter] = _m[iter + m * r];
			}
			return ret;
		}

		vec<n> col(uint c) {
			if (c < 0 || c > m) {
				throw std::runtime_error("Error: matrix column index out of bounds");
			}
			vec<n> ret(0.f);
			for (uint iter = 0; iter < n; ++iter) {
				ret[iter] = _m[c + m * iter];
			}
			return ret;
		}
	private:
		std::array<float, n * m> _m;
	};

	class mat3 : public mat<3, 3> {
	public:
		mat3(
			float xx, float xy, float xz,
			float yx, float yy, float yz,
			float zx, float zy, float zz
		) : mat<3, 3>({
				xx, xy, xz,
				yx, yy, yz,
				zx, zy, zz}) {}
	};

	class mat4 : public mat<4, 4> {
	public:
		mat4(
			float xx, float xy, float xz, float xw,
			float yx, float yy, float yz, float yw,
			float zx, float zy, float zz, float zw,
			float wx, float wy, float wz, float ww
		) : mat<4, 4>({
			xx, xy, xz, xw,
			yx, yy, yz, yw,
			zx, zy, zz, zw,
			wx, wy, wz, ww }) {}
	};

	// VECTOR OPERATIONS ====================================

	// vector equality
	template<uint n>
	bool operator==(const vec<n>& lhs, const vec<n>& rhs) {
		for (uint iter = 0; iter < n; ++iter) {
			if (lhs[iter] != rhs[iter]) {
				return false;
			}
		}
		return true;
	}

	// left scalar multiplication
	template<uint n, typename T>
	vec<n> operator*(T scal, vec<n> v) {
		vec<n> ret;
		for (uint iter = 0; iter < n; ++iter) {
			ret[iter] = scal * v[iter];
		}
		return ret;
	}

	// negation
	template<uint n>
	vec<n> operator-(vec<n> v) {
		vec<n> ret;
		for (uint iter = 0; iter < n; ++iter) {
			ret[iter] = -v[iter];
		}
		return ret;
	}

	// scalar product
	template<uint n>
	float dot(vec<n> v1, vec<n> v2) {
		float ret = 0.f;
		for (uint iter = 0; iter < n; ++iter) {
			ret += v1[iter] * v2[iter];
		}
		return ret;
	}

	// R3 cross product
	vec3 cross(const vec3& v1, const vec3& v2);

	// MATRIX OPERATIONS ====================================

	template<uint n, uint m, uint p>
	mat<n, m> operator*(const mat<n, p>& m1, const mat<p, m> m2) {
		mat<n, m> ret;
		for (uint iterN = 0; iterN < n; ++iterN) {
			for (uint iterM = 0; iterM < m; ++iterM) {
				ret(iterN, iterM) = dot(m1.row(iterN), m2.col(iterM));
			}
		}
		return ret;
	}
}
#endif // !DK_MATH_H

