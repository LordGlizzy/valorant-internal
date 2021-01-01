#pragma once
#include <cmath>
#include <Windows.h>
#include <d3d9types.h>

class vec3_t {
public:
	vec3_t( );
	vec3_t( float, float, float );
	~vec3_t( );

	float x{ }, y{ }, z{ };

	void make_absolute( ) {
		x = std::abs( x );
		y = std::abs( y );
		z = std::abs( z );
	}

	vec3_t& operator+=( const vec3_t& v ) {
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	vec3_t& operator-=( const vec3_t& v ) {
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}

	vec3_t& operator*=( const float v ) {
		x *= v;
		y *= v;
		z *= v;
		return *this;
	}

	vec3_t operator+( const vec3_t& v ) {
		return vec3_t{ x + v.x, y + v.y, z + v.z };
	}

	vec3_t operator-( const vec3_t& v ) {
		return vec3_t{ x - v.x, y - v.y, z - v.z };
	}

	vec3_t& operator+( const float& v ) {
		x = x + v;
		y = y + v;
		z = z + v;
		return *this;
	}

	vec3_t& operator-( const float& v ) {
		x = x - v;
		y = y - v;
		z = z - v;
		return *this;
	}

	vec3_t operator*( const float v ) {
		return vec3_t{ x * v, y * v, z * v };
	}

	vec3_t operator/( const float fl ) const {
		return vec3_t( x / fl, y / fl, z / fl );
	}

	vec3_t operator+( const vec3_t& v ) const {
		return vec3_t( x + v.x, y + v.y, z + v.z );
	}

	bool operator==( const vec3_t& v ) const {
		return x == v.x && y == v.y && z == v.z;
	}

	bool operator!=( const vec3_t& v ) const {
		return x != v.x || y != v.y || z != v.z;
	}

	vec3_t operator*( const float fl ) const {
		return vec3_t( x * fl, y * fl, z * fl );
	}

	vec3_t operator*( const vec3_t& v ) const {
		return vec3_t( x * v.x, y * v.y, z * v.z );
	}

	vec3_t operator-( const vec3_t& v ) const {
		return vec3_t( x - v.x, y - v.y, z - v.z );
	}

	vec3_t& operator/=( const float fl ) {
		x /= fl;
		y /= fl;
		z /= fl;
		return *this;
	}

	float operator[]( const int i ) const {
		// ReSharper disable CppCStyleCast
		return ( ( float* ) this )[i];
		// ReSharper restore CppCStyleCast
	}

	float& operator[]( const int i ) {
		return reinterpret_cast<float*>( this )[i];
	}

	[[nodiscard]] float length( ) const {
		return sqrt( x * x + y * y + z * z );
	}

	[[nodiscard]] float length_sqr( ) const {
		return x * x + y * y + z * z;
	}

	[[nodiscard]] float length_2d( ) const {
		return sqrt( x * x + y * y );
	}

	[[nodiscard]] float dist_to( const vec3_t& other ) const {
		vec3_t delta;

		delta.x = x - other.x;
		delta.y = y - other.y;
		delta.z = z - other.z;

		return delta.length( );
	}

	void clamp( ) {
		if ( x > 75.f ) x = 75.f;
		else if ( x < -75.f ) x = -75.f;
		if ( z < -180 ) z += 360.0f;
		else if ( z > 180 ) z -= 360.0f;

		y = 0.f;
	}

	void normalize( ) {
		while ( this->x > 89.0f )
			this->x -= 180.0f;

		while ( this->x < -89.0f )
			this->x += 180.0f;

		while ( this->y > 180.0f )
			this->y -= 360.0f;

		while ( this->y < -180.0f )
			this->y += 360.0f;
	}

	[[nodiscard]] float dot( const vec3_t& other ) const {
		return x * other.x + y * other.y + z * other.z;
	}

	[[nodiscard]] bool empty( ) const {
		return x == 0 || y == 0 || z == 0;
	}

	void init( const float ix = 0.0f, const float iy = 0.0f, const float iz = 0.0f ) {
		x = ix;
		y = iy;
		z = iz;
	}
};

struct vec4_t {
	float x;
	float y;
	float z;
	float w;
};

struct transform {
	vec4_t rot;
	vec3_t translation;
	char pad[4];
	vec3_t scale;
	char pad1[4];

	[[nodiscard]] D3DMATRIX matrix_scale( ) const {
		D3DMATRIX m;
		m._41 = translation.x;
		m._42 = translation.y;
		m._43 = translation.z;

		const auto x2 = rot.x + rot.x;
		const auto y2 = rot.y + rot.y;
		const auto z2 = rot.z + rot.z;

		const auto xx2 = rot.x * x2;
		const auto yy2 = rot.y * y2;
		const auto zz2 = rot.z * z2;
		m._11 = ( 1.0f - ( yy2 + zz2 ) ) * scale.x;
		m._22 = ( 1.0f - ( xx2 + zz2 ) ) * scale.y;
		m._33 = ( 1.0f - ( xx2 + yy2 ) ) * scale.z;

		const auto yz2 = rot.y * z2;
		const auto wx2 = rot.w * x2;
		m._32 = ( yz2 - wx2 ) * scale.z;
		m._23 = ( yz2 + wx2 ) * scale.y;

		const auto xy2 = rot.x * y2;
		const auto wz2 = rot.w * z2;
		m._21 = ( xy2 - wz2 ) * scale.y;
		m._12 = ( xy2 + wz2 ) * scale.x;

		const auto xz2 = rot.x * z2;
		const auto wy2 = rot.w * y2;
		m._31 = ( xz2 + wy2 ) * scale.z;
		m._13 = ( xz2 - wy2 ) * scale.x;

		m._14 = 0.0f;
		m._24 = 0.0f;
		m._34 = 0.0f;
		m._44 = 1.0f;

		return m;
	}
};

inline D3DMATRIX matrix_multiplication( const D3DMATRIX pM1, const D3DMATRIX pM2 ) {
	D3DMATRIX out;
	out._11 = pM1._11 * pM2._11 + pM1._12 * pM2._21 + pM1._13 * pM2._31 + pM1._14 * pM2._41;
	out._12 = pM1._11 * pM2._12 + pM1._12 * pM2._22 + pM1._13 * pM2._32 + pM1._14 * pM2._42;
	out._13 = pM1._11 * pM2._13 + pM1._12 * pM2._23 + pM1._13 * pM2._33 + pM1._14 * pM2._43;
	out._14 = pM1._11 * pM2._14 + pM1._12 * pM2._24 + pM1._13 * pM2._34 + pM1._14 * pM2._44;
	out._21 = pM1._21 * pM2._11 + pM1._22 * pM2._21 + pM1._23 * pM2._31 + pM1._24 * pM2._41;
	out._22 = pM1._21 * pM2._12 + pM1._22 * pM2._22 + pM1._23 * pM2._32 + pM1._24 * pM2._42;
	out._23 = pM1._21 * pM2._13 + pM1._22 * pM2._23 + pM1._23 * pM2._33 + pM1._24 * pM2._43;
	out._24 = pM1._21 * pM2._14 + pM1._22 * pM2._24 + pM1._23 * pM2._34 + pM1._24 * pM2._44;
	out._31 = pM1._31 * pM2._11 + pM1._32 * pM2._21 + pM1._33 * pM2._31 + pM1._34 * pM2._41;
	out._32 = pM1._31 * pM2._12 + pM1._32 * pM2._22 + pM1._33 * pM2._32 + pM1._34 * pM2._42;
	out._33 = pM1._31 * pM2._13 + pM1._32 * pM2._23 + pM1._33 * pM2._33 + pM1._34 * pM2._43;
	out._34 = pM1._31 * pM2._14 + pM1._32 * pM2._24 + pM1._33 * pM2._34 + pM1._34 * pM2._44;
	out._41 = pM1._41 * pM2._11 + pM1._42 * pM2._21 + pM1._43 * pM2._31 + pM1._44 * pM2._41;
	out._42 = pM1._41 * pM2._12 + pM1._42 * pM2._22 + pM1._43 * pM2._32 + pM1._44 * pM2._42;
	out._43 = pM1._41 * pM2._13 + pM1._42 * pM2._23 + pM1._43 * pM2._33 + pM1._44 * pM2._43;
	out._44 = pM1._41 * pM2._14 + pM1._42 * pM2._24 + pM1._43 * pM2._34 + pM1._44 * pM2._44;

	return out;
}