#include "pch.h"

#pragma warning(disable : 4244)
#include "DkMath.h"

using namespace math;

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

TEST(DkMathTests, vec3Cross) {
	vec3 a(1.f, 2.f, 3.f);
	vec3 b(-1.f, -1.f, -1.f);
	vec3 expect(1.f, -2.f, 1.f);
	ASSERT_EQ(expect, cross(a, b));
	ASSERT_EQ(-expect, cross(b, a));

	vec3 i(1.f, 0.f, 0.f);
	vec3 j(0.f, 1.f, 0.f);
	vec3 k(0.f, 0.f, 1.f);
	ASSERT_EQ(k, cross(i, j));
	ASSERT_EQ(i, cross(j, k));
	ASSERT_EQ(j, cross(k, i));
	ASSERT_EQ(-k, cross(j, i));
	ASSERT_EQ(-i, cross(k, j));
	ASSERT_EQ(-j, cross(i, k));
}