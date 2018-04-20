#include "pch.h"

#pragma warning(disable : 4244)
#include "DkMath.h"

using namespace math;

template<uint n>
static void vecNear(vec<n> first, vec<n> second, float tol = 1.e-7f) {
	for (uint i = 0; i < n; ++i) {
		ASSERT_NEAR(first[i], second[i], tol);
	}
}

// VECTOR TESTS

TEST(DkMathTests, vecIndex) {
	vec4 a(3.f, 4.f, 5.f, 6.f);
	ASSERT_EQ(3.f, a[0]);
	ASSERT_EQ(4.f, a[1]);
	ASSERT_EQ(5.f, a[2]);
	ASSERT_EQ(6.f, a[3]);
	ASSERT_ANY_THROW(a[4]);
}

TEST(DkMathTests, vecScalMult) {
	vec4 a(3.f, 4.f, 5.f, 6.f);
	vec4 by4(12.f, 16.f, 20.f, 24.f);
	ASSERT_EQ(by4, 4 * a);
	ASSERT_EQ(by4, 4.f * a);
	ASSERT_EQ(by4, 4.0 * a);
	ASSERT_EQ(by4, a * 4);
	ASSERT_EQ(by4, a * 4.f);
	ASSERT_EQ(by4, a * 4.0);
}

TEST(DkMathTests, vecScalDiv) {
	vec4 a(12.f, 16.f, 20.f, 24.f);
	vec4 by4(3.f, 4.f, 5.f, 6.f);
	ASSERT_EQ(by4, a / 4);
	ASSERT_EQ(by4, a / 4.f);
	ASSERT_EQ(by4, a / 4.0);
}

TEST(DkMathTests, vecNegate) {
	vec4 a(3.f, -4.f, -5.f, 6.f);
	vec4 exp(-3.f, 4.f, 5.f, -6.f);
	ASSERT_EQ(exp, -a);
}

TEST(DkMathTests, vecAdd) {
	vec4 a(3.f, 4.f, 3.f, 2.f);
	vec4 b(1.f, -1.f, 1.f, -1.f);
	vec4 z;
	vec4 exp(4.f, 3.f, 4.f, 1.f);
	ASSERT_EQ(exp, a + b);
	ASSERT_EQ(exp, b + a);
	ASSERT_EQ(2 * a, a + a);
	ASSERT_EQ(a, z + a);
	ASSERT_EQ(a, a + z);
}

TEST(DkMathTests, vecSub) {
	vec2 a(3.f, 4.f);
	vec2 b(1.f, -1.f);
	vec2 z;
	vec2 exp(2.f, 5.f);
	ASSERT_EQ(exp, a - b);
	ASSERT_EQ(-exp, b - a);
	ASSERT_EQ(z, a - a);
	ASSERT_EQ(-a, z - a);
	ASSERT_EQ(a, a - z);
}

TEST(DkMathTests, vecDot) {
	vec4 a(1.f, 2.f, 1.f, 5.f);
	vec4 b(3.f, -1.f, 1.f, -3.f);
	ASSERT_EQ(-13.f, dot(a, b));
	ASSERT_EQ(-13.f, dot(b, a));
	ASSERT_EQ(31.f, dot(a, a));
	ASSERT_EQ(20.f, dot(b, b));

	vec4 c({ -1.f, 5.f, 1.f, -2.f });
	ASSERT_EQ(0.f, dot(c, a));
	ASSERT_EQ(0.f, dot(a, c));
}

TEST(DkMathTests, vecNorm) {
	vec4 a(1.f, 1.f, 1.f, 1.f);
	vec4 b(3.f, 4.f, 0.f, 0.f);
	vec4 c(1.f, -1.f, 0.f, 1.f);
	vec4 z;
	float exp_a = 2.f;
	float exp_b = 5.f;
	float exp_c = sqrtf(3.f);

	ASSERT_EQ(exp_a, a.norm());
	ASSERT_EQ(exp_b, b.norm());
	ASSERT_EQ(exp_c, c.norm());
	ASSERT_EQ(0.f, z.norm());
}

TEST(DkMathTests, vecNormalize) {
	vec4 a(1.f, 1.f, 1.f, 1.f);
	vec4 b(3.f, 4.f, 0.f, 0.f);
	vec4 c(1.f, -1.f, 0.f, 1.f);
	vec4 z;
	vec4 exp_a(.5f, .5f, .5f, .5f);
	vec4 exp_b(.6f, .8f, 0.f, 0.f);
	vec4 exp_c(1.f / sqrt(3.f), -1.f / sqrt(3.f), 0.f, 1.f / sqrt(3.f));

	ASSERT_EQ(exp_a, normalize(a));
	ASSERT_EQ(exp_b, normalize(b));
	ASSERT_EQ(exp_c, normalize(c));
	ASSERT_ANY_THROW(normalize(z));
}

TEST(DkMathTests, vec3Cross) {
	vec3 a(1.f, 2.f, 3.f);
	vec3 b(-1.f, -1.f, -1.f);
	vec3 expect(1.f, -2.f, 1.f);
	vec3 z;
	ASSERT_EQ(expect, cross(a, b));
	ASSERT_EQ(-expect, cross(b, a));
	ASSERT_EQ(z, cross(a, a));
	ASSERT_EQ(z, cross(b, b));

	vec3 i(1.f, 0.f, 0.f);
	vec3 j(0.f, 1.f, 0.f);
	vec3 k(0.f, 0.f, 1.f);
	ASSERT_EQ(k, cross(i, j));
	ASSERT_EQ(i, cross(j, k));
	ASSERT_EQ(j, cross(k, i));
	ASSERT_EQ(-k, cross(j, i));
	ASSERT_EQ(-i, cross(k, j));
	ASSERT_EQ(-j, cross(i, k));
	ASSERT_EQ(k + j, cross(i, j - k));
}

// MATRIX TESTS

TEST(DkMathTests, matIndex) {
	mat4 a(
		1.f, 2.f, 0.f, 1.f,
		2.f, 1.f, 1.f, 1.f,
		3.f, 0.f, 0.f, 2.f,
		3.f, 1.f, 1.f, 1.f
	);

	ASSERT_EQ(1.f, a(0, 0));
	ASSERT_EQ(2.f, a(0, 1));
	ASSERT_EQ(2.f, a(1, 0));
	ASSERT_EQ(3.f, a(2, 0));
	ASSERT_EQ(3.f, a(3, 0));
	ASSERT_EQ(1.f, a(3, 3));
	ASSERT_ANY_THROW(a(4, 3));
	ASSERT_ANY_THROW(a(3, 4));
}

TEST(DkMathTests, matIdent) {
	mat4 id = ident<4>();
	mat4 exp(
		1.f, 0.f, 0.f, 0.f,
		0.f, 1.f, 0.f, 0.f,
		0.f, 0.f, 1.f, 0.f,
		0.f, 0.f, 0.f, 1.f
	);

	ASSERT_EQ(exp, id);
}

TEST(DkMathTests, matScalMult) {
	mat4 a(
		1.f, 2.f, 0.f, 1.f,
		2.f, 1.f, 1.f, 1.f,
		3.f, 0.f, 0.f, 2.f,
		3.f, 1.f, 1.f, 1.f
	);

	mat4 exp(
		2.f, 4.f, 0.f, 2.f,
		4.f, 2.f, 2.f, 2.f,
		6.f, 0.f, 0.f, 4.f,
		6.f, 2.f, 2.f, 2.f
	);

	ASSERT_EQ(exp, a * 2);
	ASSERT_EQ(exp, a * 2.f);
	ASSERT_EQ(exp, a * 2.0);
	ASSERT_EQ(exp, 2 * a);
	ASSERT_EQ(exp, 2.f * a);
	ASSERT_EQ(exp, 2.0 * a);
}

TEST(DkMathTests, matScalDiv) {
	mat4 a(
		2.f, 4.f, 0.f, 2.f,
		4.f, 2.f, 2.f, 2.f,
		6.f, 0.f, 0.f, 4.f,
		6.f, 2.f, 2.f, 2.f
	);

	mat4 exp(
		1.f, 2.f, 0.f, 1.f,
		2.f, 1.f, 1.f, 1.f,
		3.f, 0.f, 0.f, 2.f,
		3.f, 1.f, 1.f, 1.f
	);

	ASSERT_EQ(exp, a / 2);
	ASSERT_EQ(exp, a / 2.f);
	ASSERT_EQ(exp, a / 2.0);
}

TEST(DkMathTests, matNegate) {
	mat4 a(
		-1.f, 2.f, 0.f, 1.f,
		2.f, -1.f, -1.f, 1.f,
		-3.f, 0.f, 0.f, -2.f,
		3.f, -1.f, 1.f, 1.f
	);

	mat4 exp(
		1.f, -2.f, 0.f, -1.f,
		-2.f, 1.f, 1.f, -1.f,
		3.f, 0.f, 0.f, 2.f,
		-3.f, 1.f, -1.f, -1.f
	);

	ASSERT_EQ(exp, -a);
}

TEST(DkMathTests, matAdd) {
	mat4 a(
		1.f, 2.f, 0.f, 1.f,
		2.f, 1.f, 1.f, 1.f,
		3.f, 0.f, 0.f, 2.f,
		3.f, 1.f, 1.f, 1.f
	);

	mat4 b(
		1.f, 1.f, 0.f, 1.f,
		1.f, 1.f, 1.f, 1.f,
		1.f, 0.f, 0.f, 2.f,
		1.f, 1.f, 1.f, 1.f
	);

	mat4 exp(
		2.f, 3.f, 0.f, 2.f,
		3.f, 2.f, 2.f, 2.f,
		4.f, 0.f, 0.f, 4.f,
		4.f, 2.f, 2.f, 2.f
	);

	mat4 z;

	ASSERT_EQ(exp, a + b);
	ASSERT_EQ(exp, b + a);
	ASSERT_EQ(2.f * a, a + a);
	ASSERT_EQ(a, a + z);
	ASSERT_EQ(a, z + a);
	ASSERT_EQ(z, a + (-a));
}

TEST(DkMathTests, matSub) {
	mat4 a(
		1.f, 2.f, 0.f, 1.f,
		2.f, 1.f, 1.f, 1.f,
		3.f, 0.f, 0.f, 2.f,
		3.f, 1.f, 1.f, 1.f
	);

	mat4 b(
		1.f, 1.f, 0.f, 1.f,
		1.f, 1.f, 1.f, 1.f,
		1.f, 0.f, 0.f, 2.f,
		1.f, 1.f, 1.f, 1.f
	);

	mat4 exp(
		0.f, 1.f, 0.f, 0.f,
		1.f, 0.f, 0.f, 0.f,
		2.f, 0.f, 0.f, 0.f,
		2.f, 0.f, 0.f, 0.f
	);

	mat4 z;

	ASSERT_EQ(exp, a - b);
	ASSERT_EQ(-exp, b - a);
	ASSERT_EQ(z, a - a);
	ASSERT_EQ(a, a - z);
	ASSERT_EQ(-a, z - a);
	ASSERT_EQ(2.f * a, a - (-a));
}

TEST(DkMathTests, matMult) {
	mat4 a(
		1.f, 2.f, 0.f, 1.f,
		2.f, 1.f, 1.f, 1.f, 
		3.f, 0.f, 0.f, 2.f,
		3.f, 1.f, 1.f, 1.f
	);

	mat4 b(
		1.f, 1.f, 0.f, 1.f,
		1.f, 1.f, 1.f, 1.f,
		1.f, 0.f, 0.f, 2.f,
		1.f, 1.f, 1.f, 1.f
	);

	mat4 i(
		1.f, 0.f, 0.f, 0.f,
		0.f, 1.f, 0.f, 0.f,
		0.f, 0.f, 1.f, 0.f,
		0.f, 0.f, 0.f, 1.f
	);

	mat4 z;

	mat4 aa(
		 8.f, 5.f, 3.f, 4.f,
		10.f, 6.f, 2.f, 6.f,
		 9.f, 8.f, 2.f, 5.f,
		11.f, 8.f, 2.f, 7.f
	);

	mat4 ab(
		4.f, 4.f, 3.f, 4.f,
		5.f, 4.f, 2.f, 6.f,
		5.f, 5.f, 2.f, 5.f,
		6.f, 5.f, 2.f, 7.f
	);

	mat4 ba(
		6.f, 4.f, 2.f, 3.f,
		9.f, 4.f, 2.f, 5.f,
		7.f, 4.f, 2.f, 3.f,
		9.f, 4.f, 2.f, 5.f
	);

	ASSERT_EQ(ab, a * b);
	ASSERT_EQ(ba, b * a);
	ASSERT_EQ(aa, a * a);
	ASSERT_EQ(z, a * z);
	ASSERT_EQ(z, z * a);
	ASSERT_EQ(a, i * a);
	ASSERT_EQ(a, a * i);

	mat<3, 2> c({
		2.f, 1.f,
		1.f, 2.f,
		3.f, 0.f
	});

	mat<2, 4> d({
		1.f, 1.f, 0.f, 1.f,
		2.f, 0.f, 1.f, 1.f
	});

	mat<3, 4> cd({
		4.f, 2.f, 1.f, 3.f,
		5.f, 1.f, 2.f, 3.f,
		3.f, 3.f, 0.f, 3.f
	});

	ASSERT_EQ(cd, c * d);
	ASSERT_EQ(cd, cd * i);
}

TEST(DkMathTests, matTranspose) {
	mat4 a(
		1.f, 2.f, 0.f, 1.f,
		2.f, 1.f, 1.f, 1.f,
		3.f, 0.f, 0.f, 2.f,
		3.f, 1.f, 1.f, 1.f
	);

	mat4 sym(
		1.f, 1.f, 0.f, 1.f,
		1.f, 1.f, 1.f, 1.f,
		0.f, 1.f, 0.f, 2.f,
		1.f, 1.f, 2.f, 1.f
	);

	mat4 exp(
		1.f, 2.f, 3.f, 3.f,
		2.f, 1.f, 0.f, 1.f,
		0.f, 1.f, 0.f, 1.f,
		1.f, 1.f, 2.f, 1.f
	);

	ASSERT_EQ(exp, transpose(a));
	ASSERT_EQ(sym, transpose(sym));
	ASSERT_EQ(a, transpose(exp));
	ASSERT_EQ(a, transpose(transpose(a)));
	
	mat<3, 2> c({
		2.f, 1.f,
		1.f, 2.f,
		3.f, 0.f
	});

	mat<2, 3> exp_c({
		2.f, 1.f, 3.f,
		1.f, 2.f, 0.f
	});

	ASSERT_EQ(exp_c, transpose(c));
	ASSERT_EQ(c, transpose(exp_c));
	ASSERT_EQ(c, transpose(transpose(c)));
}

TEST(DkMathTests, mat3Determinant) {
	mat3 a(
		1.f, 2.f, 0.f,
		2.f, 1.f, 1.f,
		3.f, 0.f, 0.f
	);

	mat3 b(
		1.f, 2.f, 0.f,
		2.f, 1.f, 0.f,
		3.f, 0.f, 0.f
	);

	mat3 i(
		1.f, 0.f, 0.f,
		0.f, 1.f, 0.f,
		0.f, 0.f, 1.f
	);

	ASSERT_EQ(6.f, determinant(a));
	ASSERT_EQ(0.f, determinant(b));
	ASSERT_EQ(1.f, determinant(i));
}

TEST(DkMathTests, mat3Invert) {
	mat3 a(
		1.f, 2.f, 0.f,
		2.f, 1.f, 1.f,
		3.f, 0.f, 0.f
	);

	mat3 exp_a(
		0.f, 0.f, 1.f / 3.f,
		.5f, 0.f, -1.f / 6.f,
		-.5f, 1.f, -.5f
	);

	mat3 i(
		1.f, 0.f, 0.f,
		0.f, 1.f, 0.f,
		0.f, 0.f, 1.f
	);

	mat3 z;

	ASSERT_EQ(exp_a, inverse(a));
	ASSERT_EQ(a, inverse(exp_a));
	ASSERT_EQ(i, inverse(i));
	ASSERT_EQ(i, exp_a * inverse(exp_a));
	ASSERT_EQ(i, a * inverse(a));
	ASSERT_ANY_THROW(inverse(z));
}

// MATRIX/VECTOR INTERACTION

TEST(DkMathTests, matVecMult) {
	mat4 a(
		1.f, 1.f, 1.f, 1.f,
		1.f, 1.f, 1.f, 0.f,
		1.f, 1.f, 0.f, 0.f,
		1.f, 0.f, 0.f, 0.f
	);
	vec4 v(1.f);
	vec4 exp4(4.f, 3.f, 2.f, 1.f);

	mat<3, 4> b({
		1.f, 2.f, 1.f, 2.f,
		2.f, 1.f, 2.f, 0.f,
		0.f, 1.f, 0.f, 1.f
	});

	vec3 exp3(6.f, 5.f, 2.f);

	mat4 id = ident<4>();
	mat4 zm;
	vec4 zv;

	ASSERT_EQ(exp4, a * v);
	ASSERT_EQ(exp3, b * v);
	ASSERT_EQ(v, id * v);
	ASSERT_EQ(zv, zm * v);
}

// SPECIAL MATRIX CATEGORIES

TEST(DkMathTests, mat3Rotation) {
	vec3 i(1.f, 0.f, 0.f);
	vec3 j(0.f, 1.f, 0.f);
	vec3 k(0.f, 0.f, 1.f);

	vecNear(j, rotation(90, k) * i);
	vecNear(-i, rotation(180, k) * i);
	vecNear(-j, rotation(-90, k) * i);
	vecNear(i, rotation(0, k) * i);
	vecNear(-i, rotation(180, k) * i);
	vecNear((i + j) / sqrt(2), rotation(45, k) * i);
	vecNear(i, rotation(45, i) * i);
}

TEST(DkMathTests, mat4Scale) {
	vec4 i(1.f, 0.f, 0.f, 1.f);
	vec4 j(0.f, 1.f, 0.f, 1.f);
	vec4 k(0.f, 0.f, 1.f, 1.f);

	mat4 scl = scale(2.f, 3.f, 4.f);
	
	vec4 ie(2.f, 0.f, 0.f, 1.f);
	vec4 je(0.f, 3.f, 0.f, 1.f);
	vec4 ke(0.f, 0.f, 4.f, 1.f);

	vecNear(ie, scl * i);
	vecNear(je, scl * j);
	vecNear(ke, scl * k);
}

TEST(DkMathTests, mat4Translate) {
	vec4 a;
	a[3] = 1.f;
	mat4 tr = translate(1.f, 2.f, 3.f);
	vec4 exp(1.f, 2.f, 3.f, 1.f);

	vecNear(exp, tr * a);
}