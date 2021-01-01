#include "esp.hh"
#include "../sdk/classes/entity.hh"
#include "../sdk/imgui/imgui.h"
#include "../sdk/utilities/color.hh"
#include "../sdk/imgui/imgui_internal.h"
#include <algorithm>
#include "../sdk/utilities/settings.hh"
#include "../sdk/xor.hh"

void esp::player_esp( ) {
	const auto draw_circle = [ ]( const float x, const float y, const float r, const float s, const ImVec4& color ) {
		const float step = 3.14159265359 * 2.0 / s;
		for ( float a = 0; a < 3.14159265359 * 2.0; a += step ) { // NOLINT(cert-flp30-c)
			const auto x1 = r * cos( a ) + x;
			const auto y1 = r * sin( a ) + y;
			const auto x2 = r * cos( a + step ) + x;
			const auto y2 = r * sin( a + step ) + y;

			ImGui::GetCurrentWindow( )->DrawList->AddLine( ImVec2( x1, y1 ), ImVec2( x2, y2 ), ImGui::GetColorU32( color ) );
		}
	};
	const auto draw_box = []( const vec3_t& start, const vec3_t& end, const ImVec4& color ) {
		ImGui::GetOverlayDrawList( )->AddRect( ImVec2( start.x, start.y ), ImVec2( start.x + end.x, start.y + end.y ), ImGui::GetColorU32( color ) );
	};
	const auto draw_filled_box = []( const ImVec2& start, const ImVec2& end, const ImU32& color ) {
		ImGui::GetOverlayDrawList( )->AddRectFilled( ImVec2( start.x, start.y ), ImVec2( start.x + end.x, start.y + end.y ), color );
	};
	const auto draw_text = [&]( const char* text, ImVec2 pos, const float size, const ImVec4& color, const bool center = true ) {
		const auto text_size = ImGui::GetDefaultFont( )->CalcTextSizeA( size, FLT_MAX, 0.0f, text );
		if ( center )
			pos.x -= text_size.x / 2.0f;

		ImGui::GetCurrentWindow( )->DrawList->AddText( ImGui::GetDefaultFont( ), size, ImVec2( pos.x + 1, pos.y + 1 ), ImGui::GetColorU32( ImVec4( 0, 0, 0, 1 ) ), text );
		ImGui::GetCurrentWindow( )->DrawList->AddText( ImGui::GetDefaultFont( ), size, ImVec2( pos.x - 1, pos.y - 1 ), ImGui::GetColorU32( ImVec4( 0, 0, 0, 1 ) ), text );
		ImGui::GetCurrentWindow( )->DrawList->AddText( ImGui::GetDefaultFont( ), size, ImVec2( pos.x + 1, pos.y - 1 ), ImGui::GetColorU32( ImVec4( 0, 0, 0, 1 ) ), text );
		ImGui::GetCurrentWindow( )->DrawList->AddText( ImGui::GetDefaultFont( ), size, ImVec2( pos.x - 1, pos.y + 1 ), ImGui::GetColorU32( ImVec4( 0, 0, 0, 1 ) ), text );
		ImGui::GetCurrentWindow( )->DrawList->AddText( ImGui::GetDefaultFont( ), size, ImVec2( pos.x, pos.y ), ImGui::GetColorU32( color ), text );
	};

	auto* local_player = entity_t::get_local_player( );
	if ( !local_player )
		return;

	if ( settings::visuals_showfov && settings::aimbot_enable )
		draw_circle( ImGui::GetIO( ).DisplaySize.x / 2, ImGui::GetIO( ).DisplaySize.y / 2, settings::aimbot_fov * 12, settings::aimbot_fov * 11, settings::visuals_fov_color );

	const auto actors = *reinterpret_cast<uintptr_t*>( entity_t::world( )->persistent_level( ) + 0xa0 );
	if ( !actors )
		return;

	for ( auto i = 0; i < entity_t::get_actor_list( ); i++ ) {
		auto* const actor = reinterpret_cast<actor_t*>( *reinterpret_cast<uintptr_t*>( actors + i * 0x8 ) );
		if ( !actor || actor->id( ) != 16777502 || !actor->damage_handler( )->alive( ) || uintptr_t( actor ) == local_player->player_controller( )->pawn( ) || actor->dormant( ) )
			continue;
		
		if ( actor->get_object_name( ).find( _( "_PC_" ) ) == std::string::npos )
			continue;

		const auto local_team = utilities::game::read_memory<int>( utilities::game::read_memory<uintptr_t>( utilities::game::read_memory<uintptr_t>( local_player->player_controller( )->pawn( ) + 0x3c8 ) + 0x5b0 ) + 0x118 );
		const auto team = utilities::game::read_memory<int>( utilities::game::read_memory<uintptr_t>( actor->player_state( ) + 0x5b0 ) + 0x118 );
		if ( team == local_team )
			continue;
		
		const auto screen_origin = utilities::game::world_to_screen( actor->get_bone_position( 0 ), local_player->player_controller( )->camera( )->rotation( ), local_player->player_controller( )->camera( )->position( ), local_player->player_controller( )->camera( )->fov( ) );
		const auto screen_head = utilities::game::world_to_screen( actor->head_position( ) + vec3_t( 0, 0.7f, 0 ), local_player->player_controller( )->camera( )->rotation( ), local_player->player_controller( )->camera( )->position( ), local_player->player_controller( )->camera( )->fov( ) );

		const int middle = screen_origin.y - screen_head.y;
		const auto width = middle / 4;

		if ( settings::visuals_box )
			draw_box( vec3_t( screen_head.x - width, screen_head.y, 0 ), vec3_t( width * 2, middle, 0 ), settings::visuals_box_color );

		const auto health_color = color( ).hsv_to_rgb( int( actor->damage_handler( )->health( ) ) + 25, 1, 1, 255 );
		const auto health_calc = std::clamp( static_cast<int>( actor->damage_handler( )->health( ) ) * middle / 100, 0, middle );

		if ( settings::visuals_healthbar ) {
			draw_filled_box( ImVec2( screen_head.x - width - 3, screen_head.y - 1 ), ImVec2( 3, middle + 2 ), IM_COL32( 10, 10, 10, 255 ) );
			draw_filled_box( ImVec2( screen_head.x - width - 2, screen_head.y + middle - health_calc ), ImVec2( 1, health_calc ), ImGui::GetColorU32( ImVec4( health_color.r, health_color.g, health_color.b, 255 ) ) );
		}

		const auto player_state = actor->player_state( );
		if ( !player_state ) 
			continue;
		
		if ( settings::visuals_skeleton ) {
			const auto female = actor->female( );
			int skeleton[][2] = {
				{ 19, 6 }, { 6, 4 },
				{ 4, female ? 78 : 75 }, { female ? 78 : 75, female ? 75 : 76 }, { female ? 75 : 76, female ? 77 : 78 },
				{ 4, female ? 80 : 82 }, { female ? 80 : 82, female ? 82 : 83 }, { female ? 82 : 83, female ? 84 : 85 },
				{ 19, 21 }, { 21, female ? 40 : 22 }, { female ? 40 : 22, female ? 42 : 23 },
				{ 19, female ? 46 : 47 }, { female ? 46 : 47, female ? 65 : 48 }, { female ? 65 : 48, female ? 67 : 49 }
			};

			for ( auto& i : skeleton ) {
				const auto bone = actor->get_bone_position( i[0] );
				if ( bone.empty( ) )
					continue;

				const auto bone2 = actor->get_bone_position( i[1] );
				if ( bone2.empty( ) )
					continue;

				const auto bone_out = utilities::game::world_to_screen( bone, local_player->player_controller( )->camera( )->rotation( ), local_player->player_controller( )->camera( )->position( ), local_player->player_controller( )->camera( )->fov( ) );
				const auto bone2_out = utilities::game::world_to_screen( bone2, local_player->player_controller( )->camera( )->rotation( ), local_player->player_controller( )->camera( )->position( ), local_player->player_controller( )->camera( )->fov( ) );

				ImGui::GetWindowDrawList( )->AddLine( ImVec2( bone_out.x, bone_out.y ), ImVec2( bone2_out.x, bone2_out.y ), ImGui::GetColorU32( settings::visuals_skeleton_color ) );
			}
		}

		const auto name = *reinterpret_cast<fstring*>( player_state + 0x3a8 );
		if ( settings::visuals_name )
			draw_text( name.to_string( ).c_str( ), ImVec2( screen_head.x + width / 4, screen_head.y - 15 ), 13.f, settings::visuals_name_color );
	}
}

void esp::world_esp( ) {
	const auto draw_text = []( const char* text, ImVec2 pos, const float size, const ImVec4& color, const bool center = true ) {
		const auto text_size = ImGui::GetDefaultFont( )->CalcTextSizeA( size, FLT_MAX, 0.0f, text );
		if ( center )
			pos.x -= text_size.x / 2.0f;

		ImGui::GetCurrentWindow( )->DrawList->AddText( ImGui::GetDefaultFont( ), size, ImVec2( pos.x + 1, pos.y + 1 ), ImGui::GetColorU32( ImVec4( 0, 0, 0, 1 ) ), text );
		ImGui::GetCurrentWindow( )->DrawList->AddText( ImGui::GetDefaultFont( ), size, ImVec2( pos.x - 1, pos.y - 1 ), ImGui::GetColorU32( ImVec4( 0, 0, 0, 1 ) ), text );
		ImGui::GetCurrentWindow( )->DrawList->AddText( ImGui::GetDefaultFont( ), size, ImVec2( pos.x + 1, pos.y - 1 ), ImGui::GetColorU32( ImVec4( 0, 0, 0, 1 ) ), text );
		ImGui::GetCurrentWindow( )->DrawList->AddText( ImGui::GetDefaultFont( ), size, ImVec2( pos.x - 1, pos.y + 1 ), ImGui::GetColorU32( ImVec4( 0, 0, 0, 1 ) ), text );
		ImGui::GetCurrentWindow( )->DrawList->AddText( ImGui::GetDefaultFont( ), size, ImVec2( pos.x, pos.y ), ImGui::GetColorU32( color ), text );
	};

	auto* local_player = entity_t::get_local_player( );
	if ( !local_player )
		return;

	const auto actors = *reinterpret_cast<uintptr_t*>( entity_t::world( )->persistent_level( ) + 0xa0 );
	if ( !actors )
		return;

	for ( auto i = 0; i < entity_t::get_actor_list( ); i++ ) {
		auto* const actor = reinterpret_cast<actor_t*>( *reinterpret_cast<uintptr_t*>( actors + i * 0x8 ) );
		if ( !actor || actor->id( ) != 16777502 )
			continue;

		const auto local_team = utilities::game::read_memory<int>( utilities::game::read_memory<uintptr_t>( utilities::game::read_memory<uintptr_t>( local_player->player_controller( )->pawn( ) + 0x3c8 ) + 0x5b0 ) + 0x118 );
		const auto team = utilities::game::read_memory<int>( utilities::game::read_memory<uintptr_t>( actor->player_state( ) + 0x5b0 ) + 0x118 );
		if ( team == local_team )
			continue;

		const auto screen_utils_origin = utilities::game::world_to_screen( actor->root_component( )->relative_position( ), local_player->player_controller( )->camera( )->rotation( ), local_player->player_controller( )->camera( )->position( ), local_player->player_controller( )->camera( )->fov( ) );
		if ( settings::visuals_drone && actor->get_object_name( ).find( _( "Drone" ) ) != std::string::npos ) 
			draw_text( _( "drone" ), ImVec2( screen_utils_origin.x, screen_utils_origin.y ), 12.f, settings::visuals_drone_color );
		

		if ( settings::visuals_camera && actor->get_object_name( ).find( _( "PossessableCamera" ) ) != std::string::npos ) {
			draw_text( _( "camera" ), ImVec2( screen_utils_origin.x, screen_utils_origin.y ), 12.f, settings::visuals_camera_color );
		
		if ( settings::visuals_tripwire && actor->get_object_name( ).find( _( "TripWire" ) ) != std::string::npos ) 
			draw_text( _( "tripwire" ), ImVec2( screen_utils_origin.x, screen_utils_origin.y ), 12.f, settings::visuals_tripwire_color );
		}
	}
}
