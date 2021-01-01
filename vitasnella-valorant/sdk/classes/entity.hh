#pragma once
#include <cstdint>
#include "vector3d.hh"
#include <string>
#include "../utilities/utilities.hh"

#define OFFSET(func, type, offset) type func { return utilities::game::read_memory<type>( uintptr_t( this ) + offset ); }  // NOLINT
#define OFFSET_VECTOR(func, type, offset) type& func { return *reinterpret_cast<type*>( uintptr_t( this ) + offset ); }  // NOLINT

class uworld_t {
public:
	OFFSET( persistent_level( ), uintptr_t, 0x38 )
	
	uintptr_t local_player( ) {
		const auto game_instance = *reinterpret_cast<uintptr_t*>( this + 0x190 );
		return utilities::game::read_memory<uintptr_t>( game_instance + 0x40 );
	}
};

class damagehandler_t {
public:
	OFFSET( alive( ), bool, 0x189 )
	OFFSET( health( ), float, 0x190 )
};

class component_t {
public:
	OFFSET_VECTOR( relative_position( ), vec3_t, 0x184 )
};

class camera_t {
public:
	OFFSET_VECTOR( position( ), vec3_t, 0x11d0 )
	OFFSET_VECTOR( rotation( ), vec3_t, 0x11dc )
	OFFSET( fov( ), float, 0x11e8 )
};

class actor_t {
public:
	OFFSET( base_eye_height( ), float, 0x3b4 )
	OFFSET( damage_handler( ), damagehandler_t*, 0xa50 )
	OFFSET( mesh( ), uintptr_t, 0x408 )
	OFFSET( player_state( ), uintptr_t, 0x3c8 )
	OFFSET( dormant( ), bool, 0x100 )
	OFFSET( root_component( ), component_t*, 0x238 )
	OFFSET( id( ), uint32_t, 0x3C )

	vec3_t get_bone_position( int id );
	std::string get_object_name( );
	bool female( );
	vec3_t head_position( ) {
		const auto actor_position = this->root_component( )->relative_position( );
		auto headpos = actor_position;
		headpos.z += this->base_eye_height( ) + 5;

		return headpos;
	}
};

class controller_t {
public:
	OFFSET_VECTOR( viewangles( ), vec3_t, 0x418 )
	OFFSET( pawn( ), uintptr_t, 0x430 )
	OFFSET( camera( ), camera_t*, 0x448 )
};

class entity_t {
public:
	OFFSET( player_controller( ), controller_t*, 0x38 )

	static uworld_t* world( );
	static entity_t* get_local_player( );
	static int get_actor_list( );
	bool visible( uintptr_t entity );
};