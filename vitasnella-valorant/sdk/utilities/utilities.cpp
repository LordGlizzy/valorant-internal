#include "utilities.hh"

vec3_t utilities::game::world_to_screen( const vec3_t& world_loc, const vec3_t& rotation, const vec3_t& position, float fov ) {
	const auto matrix = math::to_matrix( rotation, vec3_t( ) );
	const auto axisx = vec3_t( matrix.m[0][0], matrix.m[0][1], matrix.m[0][2] );
	const auto axisy = vec3_t( matrix.m[1][0], matrix.m[1][1], matrix.m[1][2] );
	const auto axisz = vec3_t( matrix.m[2][0], matrix.m[2][1], matrix.m[2][2] );

	const auto delta = world_loc - position;
	auto transformed = vec3_t( delta.dot( axisy ), delta.dot( axisz ), delta.dot( axisx ) );

	if ( transformed.z < 0.001f )
		transformed.z = 0.001f;

	const auto fov_angle = fov;
	const float center = 1920 / 2;
	const float centery = 1080 / 2;

	return vec3_t( center + transformed.x * ( center / static_cast<float>( tan( fov_angle * PI / 360 ) ) ) / transformed.z, centery - transformed.y * ( center / static_cast<float>( tan( fov_angle * PI / 360 ) ) ) / transformed.z, 0 );
}

float utilities::math::get_distance( const vec3_t& v1, const vec3_t& v2 ) {
	vec3_t out;
	out.x = v1.x - v2.x;
	out.y = v1.y - v2.y;
	out.z = v1.z - v2.z;
	return sqrt( out.x * out.x + out.y * out.y + out.z * out.z );
}

D3DMATRIX utilities::math::to_matrix( const vec3_t& rotation, const vec3_t& origin ) {
	const auto pitch = rotation.x * float( 3.14159265358979323846f ) / 180.f;
	const auto yaw = rotation.y * float( 3.14159265358979323846f ) / 180.f;
	const auto roll = rotation.z * float( 3.14159265358979323846f ) / 180.f;

	const auto SP = LI_FN(sinf)( pitch );
	const auto CP = LI_FN(cosf)( pitch );
	const auto SY = LI_FN(sinf)( yaw );
	const auto CY = LI_FN(cosf)( yaw );
	const auto SR = LI_FN(sinf)( roll );
	const auto CR = LI_FN(cosf)( roll );

	D3DMATRIX matrix;
	matrix._11 = CP * CY;
	matrix._12 = CP * SY;
	matrix._13 = SP;
	matrix._14 = 0.f;

	matrix._21 = SR * SP * CY - CR * SY;
	matrix._22 = SR * SP * SY + CR * CY;
	matrix._23 = -SR * CP;
	matrix._24 = 0.f;

	matrix._31 = -( CR * SP * CY + SR * SY );
	matrix._32 = CY * SR - CR * SP * SY;
	matrix._33 = CR * CP;
	matrix._34 = 0.f;

	matrix._41 = origin.x;
	matrix._42 = origin.y;
	matrix._43 = origin.z;
	matrix._44 = 1.f;

	return matrix;
}