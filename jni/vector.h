#ifndef VECTOR_H
#define VECTOR_H

#include <cmath>

class vec2 {
public:
	float v[2];

	vec2() {}
	vec2(float x, float y);
	void normalize();
	bool operator==(const vec2& rhs);
	vec2 operator-(const vec2& rhs);
	vec2 operator+(const vec2& rhs);
	vec2 operator*(const vec2& rhs);
	void operator+=(const vec2& rhs);
	vec2 operator*(float a);
	static float distance(const vec2 &v1, const vec2 &v2);
};

class vec3 {
public:
	float v[3];

	vec3() {}
	vec3(float x, float y, float z);
	void normalize();
	float length();
	float dot(const vec3& rhs);
	vec3 cross(const vec3& rhs);
	vec3 operator-(const vec3& rhs);
	void operator+=(const vec3& rhs);
	void operator*=(float a);
	void operator/=(float a);
	void operator+=(float a);
	bool operator==(const vec3& rhs);
	vec3 operator-();
};

class vec3us {
public:
	unsigned short v[3];
};

class vec3i {
public:
	unsigned int v[3];
};

inline __attribute__ ((always_inline)) vec2::vec2(float x, float y) {
	v[0] = x;
	v[1] = y;
}

inline void __attribute__ ((always_inline)) vec2::normalize() {
	float s = v[0]*v[0]+v[1]*v[1];
	if (s==0.0f) {
		v[0] = 0.0f;
		v[1] = 0.0f;
		return;
	}
	float invLength = 1.0f/sqrtf(s);
	v[0] *= invLength;
	v[1] *= invLength;
}

inline bool __attribute__ ((always_inline)) vec2::operator==(const vec2& rhs) {
	return v[0]==rhs.v[0] && v[1]==rhs.v[1];
}

inline vec2 __attribute__ ((always_inline)) vec2::operator-(const vec2& rhs) {
	vec2 r;
	r.v[0] = v[0]-rhs.v[0];
	r.v[1] = v[1]-rhs.v[1];
	return r;
}

inline vec2 __attribute__ ((always_inline)) vec2::operator+(const vec2& rhs) {
	vec2 r;
	r.v[0] = v[0]+rhs.v[0];
	r.v[1] = v[1]+rhs.v[1];
	return r;
}

inline vec2 __attribute__ ((always_inline)) vec2::operator*(const vec2& rhs) {
	vec2 r;
	r.v[0] = v[0]*rhs.v[0];
	r.v[1] = v[1]*rhs.v[1];
	return r;
}

inline void __attribute__ ((always_inline)) vec2::operator+=(const vec2& rhs) {
	v[0] += rhs.v[0];
	v[1] += rhs.v[1];
}

inline vec2 __attribute__ ((always_inline)) vec2::operator*(float a) {
	vec2 r;
	r.v[0] = v[0]*a;
	r.v[1] = v[1]*a;
	return r;
}

inline float __attribute__ ((always_inline)) vec2::distance(const vec2 &v1, const vec2 &v2) {
	return sqrtf((v1.v[0]-v2.v[0])*(v1.v[0]-v2.v[0])+(v1.v[1]-v2.v[1])*(v1.v[1]-v2.v[1]));
}

inline __attribute__ ((always_inline)) vec3::vec3(float x, float y, float z) {
	v[0] = x;
	v[1] = y;
	v[2] = z;
}

inline void __attribute__ ((always_inline)) vec3::normalize() {
	float s = v[0]*v[0]+v[1]*v[1]+v[2]*v[2];
	if (s==0.0f) {
		v[0] = 0.0f;
		v[1] = 0.0f;
		v[2] = 0.0f;
		return;
	}
	float invLength = 1.0f/sqrtf(s);
	v[0] *= invLength;
	v[1] *= invLength;
	v[2] *= invLength;
}

inline float __attribute__ ((always_inline)) vec3::length() {
	return sqrtf(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]);
}

inline float __attribute__ ((always_inline)) vec3::dot(const vec3& rhs) {
	return v[0]*rhs.v[0]+v[1]*rhs.v[1]+v[2]*rhs.v[2];
}

inline vec3 __attribute__ ((always_inline)) vec3::cross(const vec3& rhs) {
	return vec3(v[1]*rhs.v[2]-v[2]*rhs.v[1], v[2]*rhs.v[0]-v[0]*rhs.v[2], v[0]*rhs.v[1]-v[1]*rhs.v[0]);
}

inline vec3 __attribute__ ((always_inline)) vec3::operator-(const vec3& rhs) {
	vec3 r;
	r.v[0] = v[0]-rhs.v[0];
	r.v[1] = v[1]-rhs.v[1];
	r.v[2] = v[2]-rhs.v[2];
	return r;
}

inline void __attribute__ ((always_inline)) vec3::operator+=(const vec3& rhs) {
	v[0] += rhs.v[0];
	v[1] += rhs.v[1];
	v[2] += rhs.v[2];
}

inline void __attribute__ ((always_inline)) vec3::operator*=(float a) {
	v[0] *= a;
	v[1] *= a;
	v[2] *= a;
}

inline void __attribute__ ((always_inline)) vec3::operator/=(float a) {
	v[0] /= a;
	v[1] /= a;
	v[2] /= a;
}

inline void __attribute__ ((always_inline)) vec3::operator+=(float a) {
	v[0] += a;
	v[1] += a;
	v[2] += a;
}

inline vec3 __attribute__ ((always_inline)) vec3::operator-() {
	vec3 r;
	r.v[0] = -v[0];
	r.v[1] = -v[1];
	r.v[2] = -v[2];
	return r;
}

inline bool __attribute__ ((always_inline)) vec3::operator==(const vec3& rhs) {
	return v[0]==rhs.v[0] && v[1]==rhs.v[1] && v[2]==rhs.v[2];
}

#endif // VECTOR_H
