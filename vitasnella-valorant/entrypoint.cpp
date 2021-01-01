#include <Windows.h>
#include "features/aimbot.hh"
#include "hooks/hooks.hh"
#include "sdk/config.hh"
#include "sdk/xor.hh"
#include <thread>
#include "sdk/utilities/utilities.hh"
#include "sdk/lazyimporter.hh"

int __stdcall entrypoint( ) {
	LI_FN( AllocConsole )( );
	FILE* fp;
	freopen_s( &fp, _( "CONOUT$" ), _( "w" ), stdout );
	
	utilities::global::base = uintptr_t( LI_FN( GetModuleHandleA )( nullptr ) );
	hooks::init( );
	config.init( );
	
	while ( true ) {
		aimbot::initialize( );
	}
}

bool DllMain( const HMODULE module, const DWORD call_reason, void* ) {
	if ( call_reason != DLL_PROCESS_ATTACH )
		return false;

	entrypoint( );

	return true;
}
