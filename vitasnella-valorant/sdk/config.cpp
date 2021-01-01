#include "config.hh"
#include <fstream>
#include <iomanip>
#include <filesystem>
#include <Windows.h>
#include "json.h"
#include "xor.hh"
#include "utilities/settings.hh"

nlohmann::json json;
c_config config;
#pragma warning(disable : 4996)

void assign_item( item& item ) {
	if ( json[item.name.c_str( )].empty( ) )
		return;

	if ( item.type == _( "float" ) )
		*reinterpret_cast<float*>( item.value ) = json[item.name.c_str( )].get<float>( );
	if ( item.type == _( "int" ) )
		*reinterpret_cast<int*>( item.value ) = json[item.name.c_str( )].get<int>( );
	if ( item.type == _( "bool" ) )
		*reinterpret_cast<bool*>( item.value ) = json[item.name.c_str( )].get<bool>( );
	if ( item.type == _( "float [4]" ) )
		for ( auto i = 0; i < 4; i++ )
			reinterpret_cast<float*>( item.value )[i] = json[item.name.c_str( )][std::to_string( i ).c_str( )].get<float>( );
}

void save_item( item& item ) {
	if ( item.type == _( "float" ) )
		json[item.name.c_str( )] = *reinterpret_cast<float*>( item.value );
	if ( item.type == _( "int" ) )
		json[item.name.c_str( )] = *reinterpret_cast<int*>( item.value );
	if ( item.type == _( "bool" ) )
		json[item.name.c_str( )] = *reinterpret_cast<bool*>( item.value );
	if ( item.type == _( "float [4]" ) )
		for ( auto i = 0; i < 4; i++ )
			json[item.name.c_str( )][std::to_string( i ).c_str( )] = reinterpret_cast<float*>( item.value )[i];
}

bool c_config::init( ) {
	if ( !std::filesystem::exists( _( "configs" ) ) ) {
		if ( !std::filesystem::create_directory( _( "configs" ) ) ) {
			return false;
		}
	}

	item( settings::aimbot_enable, _( "aimbot_enable" ), m_items );
	item( settings::aimbot_bind, _( "aimbot_bind" ), m_items );
	item( settings::aimbot_visible, _( "aimbot_visible" ), m_items );
	item( settings::aimbot_fov, _( "aimbot_fov" ), m_items );
	item( settings::aimbot_smooth, _( "aimbot_smooth" ), m_items );

	item( settings::visuals_box, _( "visuals_box" ), m_items );
	item( settings::visuals_box_color, _( "visuals_box_color" ), m_items );
	item( settings::visuals_healthbar, _( "visuals_healthbar" ), m_items );
	item( settings::visuals_skeleton, _( "visuals_skeleton" ), m_items );
	item( settings::visuals_skeleton_color, _( "visuals_skeleton_color" ), m_items );
	item( settings::visuals_name, _( "visuals_name" ), m_items );
	item( settings::visuals_name_color, _( "visuals_name_color" ), m_items );
	item( settings::visuals_showfov, _( "visuals_showfov" ), m_items );
	item( settings::visuals_fov_color, _( "visuals_fov_color" ), m_items );
	item( settings::visuals_camera, _( "visuals_camera" ), m_items );
	item( settings::visuals_camera_color, _( "visuals_camera_color" ), m_items );
	item( settings::visuals_drone, _( "visuals_drone" ), m_items );
	item( settings::visuals_drone_color, _( "visuals_drone_color" ), m_items );

	return true;
}

bool c_config::save( ) {
	const std::string m_directory = _( "configs" );
	auto output_file = std::ofstream( m_directory + _( "/" ) + _( "sgasgas.vitasnella" ) );

	if ( !output_file.good( ) )
		return false;

	for ( auto& item : m_items )
		save_item( item );

	output_file << std::setw( 4 ) << json << std::endl;
	output_file.close( );

	return true;
}

bool c_config::load( ) {
	const std::string m_directory = _( "configs" );

	auto input_file = std::ifstream( m_directory + "/" + _( "sgasgas.vitasnella" ) );
	if ( !input_file.good( ) )
		return false;

	json << input_file;
	
	for ( auto& item : m_items )
		assign_item( item );

	input_file.close( );

	return true;
}
