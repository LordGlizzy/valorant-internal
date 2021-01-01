#include "entity.hh"
#include "../utilities/utilities.hh"
#include "../xor.hh"

uworld_t* entity_t::world( ) {
	return reinterpret_cast<uworld_t*>( utilities::game::read_memory<uintptr_t>( utilities::global::base + 0x627CED8 ) ); // 74 51 48 8B 1D ? ? ? ?
}

entity_t* entity_t::get_local_player( ) {
	auto* world_instance = world( );
	if ( !world_instance )
		return nullptr;

	const auto local_players = *reinterpret_cast<uintptr_t*>( world_instance->local_player( ) );
	if ( !local_players )
		return nullptr;

	return reinterpret_cast<entity_t*>( local_players );
}

int entity_t::get_actor_list( ) {
	auto* world_instance = world( );
	if ( !world_instance )
		return 0;

	return utilities::game::read_memory<int>( world_instance->persistent_level( ) + 0xa8 );
}

bool entity_t::visible( const uintptr_t entity ) {
	auto tmp = vec3_t( 0, 0, 0 );

	const auto line_sight = *reinterpret_cast<bool( __fastcall**)( uintptr_t, uintptr_t, vec3_t*, bool )>( *reinterpret_cast<uintptr_t*>( this->player_controller( ) ) + 0x6A0 );
	return line_sight( static_cast<uintptr_t>( uintptr_t( this->player_controller( ) ) ), uintptr_t( entity ), &tmp, false );
}

vec3_t actor_t::get_bone_position( const int id ) {
	const auto get_bone_index = [ & ]( const int index ) -> transform {
		const auto bone_array = *reinterpret_cast<uintptr_t*>( this->mesh( ) + 0x548 );
		if ( !bone_array )
			return { };

		return *reinterpret_cast<transform*>( bone_array + index * 0x30 );
	};

	const auto bone = get_bone_index( id );
	const auto component_world = *reinterpret_cast<transform*>( this->mesh( ) + 0x250 );

	const auto matrix = matrix_multiplication( bone.matrix_scale( ), component_world.matrix_scale( ) );
	return vec3_t( matrix._41, matrix._42, matrix._43 );
}

std::string actor_t::get_object_name( ) {
	const auto string = reinterpret_cast<fstring(__fastcall*)( uintptr_t )>( utilities::global::base + 0x2EB3020 )( uintptr_t( this ) ); // 48 8B D9 E8 ? ? ? ? 8B F0
	auto return_string = string.to_string( );

	reinterpret_cast<void(__fastcall*)( wchar_t* )>( utilities::global::base + 0x1BCDBB0 )( string.data ); // E8 ? ? ? ? 4D 8B F7
	return return_string;
}

bool actor_t::female( ) {
	const auto object_name = this->get_object_name( );
	return object_name.find( _( "Thorne" ) ) != std::string::npos || object_name.find( _( "Wushu" ) ) != std::string::npos || object_name.find( _( "Pandemic" ) ) != std::string::npos || object_name.find( _( "Clay" ) ) != std::string::npos;
}