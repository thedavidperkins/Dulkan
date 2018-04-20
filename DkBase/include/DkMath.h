#ifndef DK_MATH_H
#define DK_MATH_H

#include <array>
#include "DkCommon.h"

namespace math {
	const float PI = 3.1415926536f;
	// VECTOR

	template<uint n>
	class vec {
	public:
		vec(std::array<float, n> init) : _v(init) {}
		vec(float f) : _v() {
			std::fill(_v.begin(), _v.end(), f);
		}
		vec() : vec(0.f) {}
		vec(const vec<n>& rhs) : _v(rhs._v) {}
		vec<n>& operator=(const vec<n>& rhs) {
			_v = rhs._v;
			return (*this);
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
		vec<n> operator*(float scal) const {
			vec<n> ret;
			for (uint iter = 0; iter < n; ++iter) {
				ret[iter] = _v[iter] * scal;
			}
			return ret;
		}

		vec<n> operator*(int scal) const {
			vec<n> ret;
			for (uint iter = 0; iter < n; ++iter) {
				ret[iter] = _v[iter] * (float)scal;
			}
			return ret;
		}

		vec<n> operator*(double scal) const {
			vec<n> ret;
			for (uint iter = 0; iter < n; ++iter) {
				ret[iter] = _v[iter] * (float)scal;
			}
			return ret;
		}

		// scalar division
		template<typename T>
		vec<n> operator/(T scal) const {
			return (*this) * (1.f / (float)scal);
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

		float norm() const {
			return sqrt(dot(*this, *this));
		}

	private:
		std::array<float, n> _v;
	};

	class vec2 : public vec<2> {
	public:
		vec2(float x, float y) : vec<2>({ x , y }) {}
		vec2(float f) : vec<2>(f) {}
		vec2() : vec<2>() {}
		vec2(const vec<2>& rhs) : vec<2>(rhs) {}
		vec2& operator=(const vec<2>& rhs) {
			(*this)[0] = rhs[0];
			(*this)[1] = rhs[1];
			return (*this);
		}
	};

	class vec3 : public vec<3> {
	public:
		vec3(float x, float y, float z) : vec<3>({ x , y, z }) {}
		vec3(float f) : vec<3>(f) {}
		vec3() : vec<3>() {}
		vec3(const vec<3>& rhs) : vec<3>(rhs) {}
		vec3& operator=(const vec<3>& rhs) {
			for (uint i = 0; i < 3; ++i) (*this)[i] = rhs[i];
			return (*this);
		}
		vec3(const vec<4>& rhs) : vec<3>() {
			for (uint i = 0; i < 3; ++i) (*this)[i] = rhs[i];
		}
		vec3& operator=(const vec<4>& rhs) {
			for (uint i = 0; i < 3; ++i) (*this)[i] = rhs[i];
			return(*this);
		}
	};

	class vec4 : public vec<4> {
	public:
		vec4(float x, float y, float z, float w) : vec<4>({ x , y, z, w }) {}
		vec4(float f) : vec<4>(f) {}
		vec4() : vec<4>() {}
		vec4(const vec<4>& rhs) : vec<4>(rhs) {}
		vec4& operator=(const vec<4>& rhs) {
			(*this)[0] = rhs[0];
			(*this)[1] = rhs[1];
			(*this)[2] = rhs[2];
			(*this)[3] = rhs[3];
			return (*this);
		}
		vec4(const vec<3>& rhs) : vec<4>() {
			for (uint i = 0; i < 3; ++i) (*this)[i] = rhs[i];
			(*this)[3] = 1.f;
		}
		vec4& operator=(const vec<3>& rhs) {
			for (uint i = 0; i < 3; ++i) (*this)[i] = rhs[i];
			(*this)[3] = 1.f;
			return (*this);
		}
	};

	// MATRIX

	template <uint n, uint m>
	class mat {
	public:
		mat(std::array<float, n * m> init) : _m(init) {}
		mat(float f) : _m() {
			std::fill(_m.begin(), _m.end(), f);
		}
		mat() : mat(0.f) {}
		mat(const mat<n, m>& rhs) : _m(rhs._m) {}
		mat<n, m> operator=(const mat<n, m>& rhs) {
			_m = rhs._m;
			return (*this);
		}

		// indexing
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

		// get copy of row
		vec<m> row(uint r) const {
			if (r < 0 || r > n) {
				throw std::runtime_error("Error: matrix row index out of bounds");
			}
			vec<m> ret;
			for (uint iter = 0; iter < m; ++iter) {
				ret[iter] = _m[iter + m * r];
			}
			return ret;
		}

		// get copy of column
		vec<n> col(uint c) const {
			if (c < 0 || c > m) {
				throw std::runtime_error("Error: matrix column index out of bounds");
			}
			vec<n> ret;
			for (uint iter = 0; iter < n; ++iter) {
				ret[iter] = _m[c + m * iter];
			}
			return ret;
		}

		// right scalar multiplication
		mat<n, m> operator*(float scal) const {
			mat<n, m> ret;
			for (uint i = 0; i < n; ++i) {
				for (uint j = 0; j < m; ++j) {
					ret(i, j) = scal * (*this)(i, j);
				}
			}
			return ret;
		}

		mat<n, m> operator*(int scal) const {
			mat<n, m> ret;
			for (uint i = 0; i < n; ++i) {
				for (uint j = 0; j < m; ++j) {
					ret(i, j) = (float)scal * (*this)(i, j);
				}
			}
			return ret;
		}

		mat<n, m> operator*(double scal) const {
			mat<n, m> ret;
			for (uint i = 0; i < n; ++i) {
				for (uint j = 0; j < m; ++j) {
					ret(i, j) = (float)scal * (*this)(i, j);
				}
			}
			return ret;
		}

		// scalar division
		mat<n, m> operator/(float scal) const {
			return (*this) * (1.f / scal);
		}

		mat<n, m> operator/(int scal) const {
			return (*this) * (1.f / (float)scal);
		}

		mat<n, m> operator/(double scal) const {
			return (*this) * (1.f / (float)scal);
		}

		// matrix addition
		mat<n, m> operator+(const mat<n, m>& other) const {
			mat<n, m> ret;
			for (uint i = 0; i < n; ++i) {
				for (uint j = 0; j < m; ++j) {
					ret(i, j) = (*this)(i, j) + other(i, j);
				}
			}
			return ret;
		}

		// matrix subtraction
		mat<n, m> operator-(const mat<n, m>& other) const {
			return (*this) + (-other);
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
				zx, zy, zz
			}) {}

		mat3(float f) : mat<3, 3>(f) {}
		mat3() : mat<3, 3>() {}
		mat3(const mat<3, 3>& rhs) : mat<3, 3>(rhs) {}
		mat3(const mat<4, 4>& rhs) : mat<3, 3>() {
			for (uint i = 0; i < 3; ++i) {
				for (uint j = 0; j < 3; ++j) {
					(*this)(i, j) = rhs(i, j);
				}
			}
		}
		mat3& operator=(const mat<3, 3>& rhs) {
			for (uint i = 0; i < 3; ++i) {
				for (uint j = 0; j < 3; ++j) {
					(*this)(i, j) = rhs(i, j);
				}
			}
			return (*this);
		}
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
				wx, wy, wz, ww 
			}) {}
		mat4(float f) :	mat<4, 4>(f) {}
		mat4() : mat<4, 4>() {}
		mat4(const mat<3, 3>& rhs) : mat<4, 4>() {
			for (uint i = 0; i < 3; ++i) {
				for (uint j = 0; j < 3; ++j) {
					(*this)(i, j) = rhs(i, j);
				}
			}
			(*this)(3, 3) = 1.f;
		}
		mat4(const mat<4, 4>& rhs) : mat<4, 4>(rhs) {}
		mat4& operator=(const mat<4, 4>& rhs) {
			for (uint i = 0; i < 4; ++i) {
				for (uint j = 0; j < 4; ++j) {
					(*this)(i, j) = rhs(i, j);
				}
			}
			return (*this);
		}
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
	template<uint n>
	vec<n> operator*(float scal, const vec<n>& v) {
		return v * scal;
	}

	template<uint n>
	vec<n> operator*(int scal, const vec<n>& v) {
		return v * (float)scal;
	}

	template<uint n>
	vec<n> operator*(double scal, const vec<n>& v) {
		return v * (float)scal;
	}

	// negation
	template<uint n>
	vec<n> operator-(const vec<n>& v) {
		return v * (-1.f);
	}

	// scalar product
	template<uint n>
	float dot(const vec<n>& v1, const vec<n>& v2) {
		float ret = 0.f;
		for (uint iter = 0; iter < n; ++iter) {
			ret += v1[iter] * v2[iter];
		}
		return ret;
	}

	// normalize
	template<uint n>
	vec<n> normalize(const vec<n>& v) {
		if (v.norm() == 0.f) {
			throw std::runtime_error("Cannot normalize the zero vector.");
		}
		return v / v.norm();
	}

	// R3 cross product
	vec<3> cross(const vec<3>& v1, const vec<3>& v2);

	// MATRIX OPERATIONS ====================================

	template<uint n, uint m>
	bool operator==(const mat<n, m>& m1, const mat<n, m>& m2) {
		for (uint i = 0; i < n; ++i) {
			for (uint j = 0; j < m; ++j) {
				if (m1(i, j) != m2(i, j)) {
					return false;
				}
			}
		}
		return true;
	}

	// matrix multiplication
	template<uint n, uint m, uint p>
	mat<n, m> operator*(const mat<n, p>& m1, const mat<p, m>& m2) {
		mat<n, m> ret;
		for (uint i = 0; i < n; ++i) {
			for (uint j = 0; j < m; ++j) {
				ret(i, j) = dot(m1.row(i), m2.col(j));
			}
		}
		return ret;
	}

	// transpose
	template<uint n, uint m>
	mat<m, n> transpose(const mat<n, m>& mt) {
		mat<m, n> ret;
		for (uint i = 0; i < n; ++i) {
			for (uint j = 0; j < m; ++j) {
				ret(j, i) = mt(i, j);
			}
		}
		return ret;
	}

	// left scalar multiplication
	template<uint n, uint m>
	mat<n, m> operator*(float scal, const mat<n, m>& mt) {
		return mt * scal;
	}

	template<uint n, uint m>
	mat<n, m> operator*(int scal, const mat<n, m>& mt) {
		return mt * (float)scal;
	}

	template<uint n, uint m>
	mat<n, m> operator*(double scal, const mat<n, m>& mt) {
		return mt * (float)scal;
	}

	// matrix negation
	template<uint n, uint m>
	mat<n, m> operator-(const mat<n, m> mt) {
		return mt * (-1.f);
	}

	// identity matrix
	template<uint n>
	mat<n, n> ident() {
		mat<n, n> ret;
		for (uint i = 0; i < n; ++i) {
			ret(i, i) = 1.f;
		}
		return ret;
	}

	// matrix vector multiplication
	template<uint m, uint n>
	vec<m> operator*(mat<m, n> mt, vec<n> v) {
		vec<m> ret;
		for (uint i = 0; i < m; ++i) {
			ret[i] = dot(mt.row(i), v);
		}
		return ret;
	}

	float determinant(const mat<3, 3>& m);
	mat<3, 3> inverse(const mat<3, 3>& m);
	mat<3, 3> rotation(float angle, const vec<3>& axis);
	mat<4, 4> scale(float sx, float sy, float sz);
	mat<4, 4> translate(float tx, float ty, float tz);
	mat<4, 4> lookAt(const vec<3>& eye, const vec<3>& center, const vec<3>& up);
	mat<4, 4> perspective(float fovy, float aspect, float zNear, float zFar);
}
#endif // !DK_MATH_H

