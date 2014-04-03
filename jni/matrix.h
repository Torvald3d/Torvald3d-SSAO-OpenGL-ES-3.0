#ifndef MATRIX_H
#define MATRIX_H

#include <arm_neon.h>
#include "vector.h"

/* matrix indices:
 *  0  4  8 12
 *  1  5  9 13
 *  2  6 10 14
 *  3  7 11 15
 */

const float DEG2RAD = 3.141593f / 180.0f;

class mat4 {
public:
	float m[16];

	void identity();
	mat4 operator*(mat4& n);
	void setFrustum(float fov, float aspect, float zNear, float zFar);
	void lookAt(vec3 &eye, vec3 &target, vec3 &up);
	void translate(float x, float y, float z);
	void rotate(float a, float x, float y, float z);
	void scale(float x, float y, float z);
	void setPos(float x, float y, float z);
	void setRot(float a, float x, float y, float z);
	void setScale(float x, float y, float z);
	bool invert();
};

inline void __attribute__ ((always_inline)) mat4::identity() {
	m[0] = m[5] = m[10] = m[15] = 1.0f;
	m[1] = m[2] = m[3] = m[4] = m[6] = m[7] = m[8] = m[9] = m[11] = m[12] = m[13] = m[14] = 0.0f;
}

inline mat4 __attribute__ ((always_inline)) mat4::operator*(mat4& n) {
	mat4 r;
	float *r0 = r.m;
	float *r1 = m;
	float *r2 = n.m;

	asm volatile (
		"vld1.32    {d0, d1}, [%1]!     \n\t"   //q0 = r2
		"vld1.32    {d2, d3}, [%1]!     \n\t"   //q1 = r2+4
		"vld1.32    {d4, d5}, [%1]!     \n\t"   //q2 = r2+8
		"vld1.32    {d6, d7}, [%1]      \n\t"   //q3 = r2+12
		"vld1.32    {d16, d17}, [%0]!   \n\t"   //q8 = r1
		"vld1.32    {d18, d19}, [%0]!   \n\t"   //q9 = r1+4
		"vld1.32    {d20, d21}, [%0]!   \n\t"   //q10 = r1+8
		"vld1.32    {d22, d23}, [%0]    \n\t"   //q11 = r1+12

		"vmul.f32   q12, q8, d0[0]      \n\t"   //q12 = q8 * d0[0]
		"vmul.f32   q13, q8, d2[0]      \n\t"   //q13 = q8 * d2[0]
		"vmul.f32   q14, q8, d4[0]      \n\t"   //q14 = q8 * d4[0]
		"vmul.f32   q15, q8, d6[0]      \n\t"   //q15 = q8 * d6[0]
		"vmla.f32   q12, q9, d0[1]      \n\t"   //q12 += q9 * d0[1]
		"vmla.f32   q13, q9, d2[1]      \n\t"   //q13 += q9 * d2[1]
		"vmla.f32   q14, q9, d4[1]      \n\t"   //q14 += q9 * d4[1]
		"vmla.f32   q15, q9, d6[1]      \n\t"   //q15 += q9 * d6[1]
		"vmla.f32   q12, q10, d1[0]     \n\t"   //q12 += q10 * d1[0]
		"vmla.f32   q13, q10, d3[0]     \n\t"   //q13 += q10 * d3[0]
		"vmla.f32   q14, q10, d5[0]     \n\t"   //q14 += q10 * d5[0]
		"vmla.f32   q15, q10, d7[0]     \n\t"   //q15 += q10 * d7[0]
		"vmla.f32   q12, q11, d1[1]     \n\t"   //q12 += q11 * d1[1]
		"vmla.f32   q13, q11, d3[1]     \n\t"   //q13 += q11 * d3[1]
		"vmla.f32   q14, q11, d5[1]     \n\t"   //q14 += q11 * d5[1]
		"vmla.f32   q15, q11, d7[1]     \n\t"   //q15 += q11 * d7[1]

		"vst1.32    {d24, d25}, [%2]!   \n\t"   //d = q12
		"vst1.32    {d26, d27}, [%2]!   \n\t"   //d+4 = q13
		"vst1.32    {d28, d29}, [%2]!   \n\t"   //d+8 = q14
		"vst1.32    {d30, d31}, [%2]    \n\t"   //d+12 = q15

		: "+r"(r1), "+r"(r2), "+r"(r0)
		:
		: "q0", "q1", "q2", "q3", "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15",
		"memory"
	);

	return r;
}

inline void __attribute__ ((always_inline)) mat4::setFrustum(float fov, float aspect, float zNear, float zFar) {
	float h = 1.0f/tanf(fov*DEG2RAD);
	float negDepth = zNear-zFar;

	m[0] = h / aspect;
	m[1] = 0;
	m[2] = 0;
	m[3] = 0;

	m[4] = 0;
	m[5] = h;
	m[6] = 0;
	m[7] = 0;

	m[8] = 0;
	m[9] = 0;
	m[10] = (zFar + zNear)/negDepth;
	m[11] = -1;

	m[12] = 0;
	m[13] = 0;
	m[14] = 2.0f*(zNear*zFar)/negDepth;
	m[15] = 0;
}

inline void __attribute__ ((always_inline)) mat4::lookAt(vec3 &eye, vec3 &target, vec3 &up) {
	vec3 zAxis = eye-target;
	zAxis.normalize();
	vec3 xAxis = up.cross(zAxis);
	xAxis.normalize();
	vec3 yAxis = zAxis.cross(xAxis);

	m[ 0] = xAxis.v[0];
	m[ 1] = yAxis.v[0];
	m[ 2] = zAxis.v[0];
	m[ 3] = 0;

	m[ 4] = xAxis.v[1];
	m[ 5] = yAxis.v[1];
	m[ 6] = zAxis.v[1];
	m[ 7] = 0;

	m[ 8] = xAxis.v[2];
	m[ 9] = yAxis.v[2];
	m[10] = zAxis.v[2];
	m[11] = 0;

	m[12] = -xAxis.dot(eye);
	m[13] = -yAxis.dot(eye);
	m[14] = -zAxis.dot(eye);
	m[15] = 1;
}

inline void __attribute__ ((always_inline)) mat4::translate(float x, float y, float z) {
	m[12] = m[0]*x + m[4]*y + m[ 8]*z + m[12];
	m[13] = m[1]*x + m[5]*y + m[ 9]*z + m[13];
	m[14] = m[2]*x + m[6]*y + m[10]*z + m[14];
	m[15] = m[3]*x + m[7]*y + m[11]*z + m[15];
}

inline void __attribute__ ((always_inline)) mat4::rotate(float a, float x, float y, float z) {
	float xx, yy, zz, xy, yz, zx, xs, ys, zs, one_c, s, c, mag;
	mat4 r;
	s = sinf(a*DEG2RAD);
	c = cosf(a*DEG2RAD);
	mag = sqrtf(x*x + y*y + z*z);
	if (mag<0.0001f)
		return;

	x /= mag;
	y /= mag;
	z /= mag;

	xx = x*x;
	yy = y*y;
	zz = z*z;
	xy = x*y;
	yz = y*z;
	zx = z*x;
	xs = x*s;
	ys = y*s;
	zs = z*s;
	one_c = 1.0f-c;

	r.m[ 0] = one_c*xx+c;
	r.m[ 4] = one_c*xy-zs;
	r.m[ 8] = one_c*zx+ys;
	r.m[12] = 0.0f;

	r.m[ 1] = one_c*xy+zs;
	r.m[ 5] = one_c*yy+c;
	r.m[ 9] = one_c*yz-xs;
	r.m[13] = 0.0f;

	r.m[ 2] = one_c*zx-ys;
	r.m[ 6] = one_c*yz+xs;
	r.m[10] = one_c*zz+c;
	r.m[14] = 0.0f;

	r.m[ 3] = 0.0f;
	r.m[ 7] = 0.0f;
	r.m[11] = 0.0f;
	r.m[15] = 1.0f;

	*this = *this*r;
}

inline void __attribute__ ((always_inline)) mat4::setRot(float a, float x, float y, float z) {

}

inline void __attribute__ ((always_inline)) mat4::scale(float x, float y, float z) {
	m[ 0] *= x;
	m[ 1] *= x;
	m[ 2] *= x;
	m[ 3] *= x;

	m[ 4] *= y;
	m[ 5] *= y;
	m[ 6] *= y;
	m[ 7] *= y;

	m[ 8] *= z;
	m[ 9] *= z;
	m[10] *= z;
	m[11] *= z;
}

inline void __attribute__ ((always_inline)) mat4::setPos(float x, float y, float z) {
	m[12] = x;
	m[13] = y;
	m[14] = z;
}

inline void __attribute__ ((always_inline)) mat4::setScale(float x, float y, float z) {
	m[ 0] = x;
	m[ 5] = y;
	m[10] = z;
}

inline bool __attribute__ ((always_inline)) mat4::invert() {
	float inv[16], det;
	int i;

	inv[0] = m[5]  * m[10] * m[15] -
			 m[5]  * m[11] * m[14] -
			 m[9]  * m[6]  * m[15] +
			 m[9]  * m[7]  * m[14] +
			 m[13] * m[6]  * m[11] -
			 m[13] * m[7]  * m[10];

	inv[4] = -m[4]  * m[10] * m[15] +
			 m[4]  * m[11] * m[14] +
			 m[8]  * m[6]  * m[15] -
			 m[8]  * m[7]  * m[14] -
			 m[12] * m[6]  * m[11] +
			 m[12] * m[7]  * m[10];

	inv[8] = m[4]  * m[9] * m[15] -
			 m[4]  * m[11] * m[13] -
			 m[8]  * m[5] * m[15] +
			 m[8]  * m[7] * m[13] +
			 m[12] * m[5] * m[11] -
			 m[12] * m[7] * m[9];

	inv[12] = -m[4]  * m[9] * m[14] +
			  m[4]  * m[10] * m[13] +
			  m[8]  * m[5] * m[14] -
			  m[8]  * m[6] * m[13] -
			  m[12] * m[5] * m[10] +
			  m[12] * m[6] * m[9];

	inv[1] = -m[1]  * m[10] * m[15] +
			 m[1]  * m[11] * m[14] +
			 m[9]  * m[2] * m[15] -
			 m[9]  * m[3] * m[14] -
			 m[13] * m[2] * m[11] +
			 m[13] * m[3] * m[10];

	inv[5] = m[0]  * m[10] * m[15] -
			 m[0]  * m[11] * m[14] -
			 m[8]  * m[2] * m[15] +
			 m[8]  * m[3] * m[14] +
			 m[12] * m[2] * m[11] -
			 m[12] * m[3] * m[10];

	inv[9] = -m[0]  * m[9] * m[15] +
			 m[0]  * m[11] * m[13] +
			 m[8]  * m[1] * m[15] -
			 m[8]  * m[3] * m[13] -
			 m[12] * m[1] * m[11] +
			 m[12] * m[3] * m[9];

	inv[13] = m[0]  * m[9] * m[14] -
			  m[0]  * m[10] * m[13] -
			  m[8]  * m[1] * m[14] +
			  m[8]  * m[2] * m[13] +
			  m[12] * m[1] * m[10] -
			  m[12] * m[2] * m[9];

	inv[2] = m[1]  * m[6] * m[15] -
			 m[1]  * m[7] * m[14] -
			 m[5]  * m[2] * m[15] +
			 m[5]  * m[3] * m[14] +
			 m[13] * m[2] * m[7] -
			 m[13] * m[3] * m[6];

	inv[6] = -m[0]  * m[6] * m[15] +
			 m[0]  * m[7] * m[14] +
			 m[4]  * m[2] * m[15] -
			 m[4]  * m[3] * m[14] -
			 m[12] * m[2] * m[7] +
			 m[12] * m[3] * m[6];

	inv[10] = m[0]  * m[5] * m[15] -
			  m[0]  * m[7] * m[13] -
			  m[4]  * m[1] * m[15] +
			  m[4]  * m[3] * m[13] +
			  m[12] * m[1] * m[7] -
			  m[12] * m[3] * m[5];

	inv[14] = -m[0]  * m[5] * m[14] +
			  m[0]  * m[6] * m[13] +
			  m[4]  * m[1] * m[14] -
			  m[4]  * m[2] * m[13] -
			  m[12] * m[1] * m[6] +
			  m[12] * m[2] * m[5];

	inv[3] = -m[1] * m[6] * m[11] +
			 m[1] * m[7] * m[10] +
			 m[5] * m[2] * m[11] -
			 m[5] * m[3] * m[10] -
			 m[9] * m[2] * m[7] +
			 m[9] * m[3] * m[6];

	inv[7] = m[0] * m[6] * m[11] -
			 m[0] * m[7] * m[10] -
			 m[4] * m[2] * m[11] +
			 m[4] * m[3] * m[10] +
			 m[8] * m[2] * m[7] -
			 m[8] * m[3] * m[6];

	inv[11] = -m[0] * m[5] * m[11] +
			  m[0] * m[7] * m[9] +
			  m[4] * m[1] * m[11] -
			  m[4] * m[3] * m[9] -
			  m[8] * m[1] * m[7] +
			  m[8] * m[3] * m[5];

	inv[15] = m[0] * m[5] * m[10] -
			  m[0] * m[6] * m[9] -
			  m[4] * m[1] * m[10] +
			  m[4] * m[2] * m[9] +
			  m[8] * m[1] * m[6] -
			  m[8] * m[2] * m[5];

	det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

	if (det == 0)
		return false;

	det = 1.0 / det;

	for (i = 0; i < 16; i++)
		m[i] = inv[i] * det;

	return true;
}

#endif
