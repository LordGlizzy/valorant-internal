#pragma once
#include <locale>
#include "../classes/vector3d.hh"
#include <Windows.h>
#include <d3d9types.h>
#include "../lazyimporter.hh"

#define DEG2RAD( x  )  ( ( float )( x ) * ( float )( ( float )PI / 180.f ) )
#define RAD2DEG( x  )  ( ( float )( x ) * ( float )( 180.f / PI ) )
constexpr auto PI = 3.14159265358979323846f;

template <class T>
struct tarray {
	friend struct fstring;
public:
	tarray( ) {
		data = nullptr;
		count = Max = 0;
	};

	[[nodiscard]] size_t num( ) const {
		return count;
	};

	T& operator[]( size_t i ) {
		return data[i];
	};

	const T& operator[]( size_t i ) const {
		return data[i];
	};

	[[nodiscard]] bool valid_index( const size_t i ) const {
		return i < num( );
	}

	T* data;
	int32_t count;
	int32_t Max;
};

struct fstring : tarray<wchar_t> {
	[[nodiscard]] std::string to_string( ) const {
		const auto length = std::wcslen( data );
		std::string str( length, '\0' );
		std::use_facet<std::ctype<wchar_t>>( std::locale( ) ).narrow( data, data + length, '?', &str[0] );
		return str;
	}
};

namespace utilities {
	namespace game {
		vec3_t world_to_screen( const vec3_t& world_loc, const vec3_t& rotation, const vec3_t& position, float fov );

		template <typename type>
		type read_memory( const uintptr_t ptr ) {
			if ( !LI_FN( IsBadReadPtr )( reinterpret_cast<void*>( ptr ), sizeof( type ) ) )
				return *reinterpret_cast<type*>( ptr );

			return 0;
		}
	}

	namespace math {
		float get_distance( const vec3_t& v1, const vec3_t& v2 );
		D3DMATRIX to_matrix( const vec3_t& rotation, const vec3_t& origin );
	}

	namespace global {
		inline uintptr_t base;
	}
};