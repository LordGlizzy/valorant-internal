#include "aimbot.hh"
#include "../sdk/classes/entity.hh"
#include "../sdk/utilities/utilities.hh"
#include "../sdk/utilities/settings.hh"

actor_t* closest_enemy( ) {
	const auto get_fov = []( const vec3_t& source, vec3_t aim_angle ) {
		aim_angle -= source;

		if ( aim_angle.x > 180.f )
			aim_angle.x -= 360.f;
		else if ( aim_angle.x < -180.f )
			aim_angle.x += 360.f;

		if ( aim_angle.y > 180.f )
			aim_angle.y -= 360.f;
		else if ( aim_angle.y < -180.f )
			aim_angle.y += 360.f;

		aim_angle.make_absolute( );

		return aim_angle.x + aim_angle.y;
	};

	actor_t* best_actor = nullptr;
	auto fov = settings::aimbot_fov;

	auto* local_player = entity_t::get_local_player( );
	if ( !local_player )
		return nullptr;

	const auto actors = *reinterpret_cast<uintptr_t*>( entity_t::world( )->persistent_level( ) + 0xa0 );
	if ( !actors )
		return nullptr;

	for ( auto i = 0; i < entity_t::get_actor_list( ); i++ ) {
		auto* const actor = reinterpret_cast<actor_t*>( *reinterpret_cast<uintptr_t*>( actors + i * 0x8 ) );
		if ( !actor || actor->id( ) != 16777502 || !actor->damage_handler( )->alive( ) || uintptr_t( actor ) == local_player->player_controller( )->pawn( ) || actor->dormant( ) )
			continue;
		
		const auto local_team = utilities::game::read_memory<int>( utilities::game::read_memory<uintptr_t>( utilities::game::read_memory<uintptr_t>( local_player->player_controller( )->pawn( ) + 0x3c8 ) + 0x5b0 ) + 0x118 );
		const auto team = utilities::game::read_memory<int>( utilities::game::read_memory<uintptr_t>( actor->player_state( ) + 0x5b0 ) + 0x118 );
		if ( team == local_team )
			continue;

		if ( settings::aimbot_visible && !local_player->visible( uintptr_t( actor ) ) )
			continue;
		
		const auto relative_pos = actor->head_position( ) - local_player->player_controller( )->camera( )->position( );
		const auto yaw = atan2( relative_pos.y, relative_pos.x ) * 180.0f / PI;
		const auto pitch = -( acos( relative_pos.z / utilities::math::get_distance( local_player->player_controller( )->camera( )->position( ), actor->head_position( ) ) ) * 180.0f / PI - 90.0f );

		const auto calculated_fov = get_fov( local_player->player_controller( )->viewangles( ), vec3_t( pitch, yaw, 0.f ) );
		if ( calculated_fov > fov )
			continue;

		fov = calculated_fov;
		best_actor = actor;
	}

	return best_actor;
}

void aimbot::initialize( ) {
	if ( !settings::aimbot_enable )
		return;

	if ( !LI_FN( GetAsyncKeyState )( settings::aimbot_bind ) )
		return;

	auto* local_player = entity_t::get_local_player( );
	if ( !local_player )
		return;

	auto* actor = closest_enemy( );
	if ( !actor )
		return;
	
	const auto relative_pos = actor->head_position( ) - local_player->player_controller( )->camera( )->position( );
	const auto pitch = -( acos( relative_pos.z / utilities::math::get_distance( local_player->player_controller( )->camera( )->position( ), actor->head_position( ) ) ) * 180.0f / PI - 90.0f );
	const auto yaw = atan2( relative_pos.y, relative_pos.x ) * 180.0f / PI;

	const auto aimangles = settings::aimbot_smooth ? vec3_t( ( pitch - local_player->player_controller( )->camera( )->rotation( ).x ) / settings::aimbot_smooth + local_player->player_controller( )->camera( )->rotation( ).x, ( yaw - local_player->player_controller( )->camera( )->rotation( ).y ) / settings::aimbot_smooth + local_player->player_controller( )->camera( )->rotation( ).y, 0.f ) : vec3_t( pitch, yaw, 0.f );
	local_player->player_controller( )->viewangles( ) = aimangles;
}
