#include "vector3d.hh"

vec3_t::vec3_t( ) {
	x = y = z = 0.0f;
}

vec3_t::vec3_t( const float fx, const float fy, const float fz ) {
	x = fx;
	y = fy;
	z = fz;
}

vec3_t::~vec3_t( ) = default;
