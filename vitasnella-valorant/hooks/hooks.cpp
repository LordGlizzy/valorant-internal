#include "hooks.hh"
#include <mutex>
#include "../sdk/imgui/imgui.h"
#include "../sdk/imgui/imgui_impl_dx11.h"
#include "../sdk/utilities/settings.hh"
#include "../features/esp.hh"
#include "../sdk/config.hh"
#include "../sdk/xor.hh"
#include "../sdk/lazyimporter.hh"
#include "../sdk/utilities/utilities.hh"

typedef HRESULT ( *present_fn )( IDXGISwapChain*, UINT, UINT );
inline present_fn original_present{ };

typedef HRESULT ( *resize_fn )( IDXGISwapChain*, UINT, UINT, UINT, DXGI_FORMAT, UINT );
inline resize_fn original_resize{ };

void hooks::init( ) {
	WNDCLASSEXA wc
	{
		sizeof( WNDCLASSEX ),
		CS_CLASSDC,
		[ ]( const HWND window, const UINT message, const WPARAM wparam, const LPARAM lparam ) -> LRESULT
		{
			return LI_FN( DefWindowProcA ).cached( )( window, message, wparam, lparam );
		},
		0L,
		0L,
		LI_FN( GetModuleHandleA ).cached( )( nullptr ),
		nullptr,
		nullptr,
		nullptr,
		_( "BBV" ),
		nullptr
	};
	
	LI_FN( RegisterClassExA )( &wc );

	auto level = D3D_FEATURE_LEVEL_11_0;
	DXGI_SWAP_CHAIN_DESC sd;
	{
		ZeroMemory( &sd, sizeof sd );
		sd.BufferCount = 1;
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.OutputWindow = LI_FN( FindWindowA )( _( "UnrealWindow" ), nullptr );
		sd.SampleDesc.Count = 1;
		sd.Windowed = TRUE;
		sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	}

	IDXGISwapChain* swap_chain = nullptr;
	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* context = nullptr;

	LI_FN( D3D11CreateDeviceAndSwapChain )( nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, &level, 1, D3D11_SDK_VERSION, &sd, &swap_chain, &device, nullptr, &context );

	auto* swap_chainvtable = reinterpret_cast<uintptr_t*>( swap_chain );
	swap_chainvtable = reinterpret_cast<uintptr_t*>( swap_chainvtable[0] );

	DWORD old_protect;
	original_present = reinterpret_cast<present_fn>( reinterpret_cast<DWORD_PTR*>( swap_chainvtable[8] ) );
	LI_FN( VirtualProtect )( swap_chainvtable, 0x1000, PAGE_EXECUTE_READWRITE, &old_protect );
	swap_chainvtable[8] = reinterpret_cast<DWORD_PTR>( present );
	LI_FN( VirtualProtect )( swap_chainvtable, 0x1000, old_protect, &old_protect );

	DWORD old_protect_resize;
	original_resize = reinterpret_cast<resize_fn>( reinterpret_cast<DWORD_PTR*>( swap_chainvtable[13] ) );
	LI_FN( VirtualProtect )( swap_chainvtable, 0x1000, PAGE_EXECUTE_READWRITE, &old_protect_resize );
	swap_chainvtable[13] = reinterpret_cast<DWORD_PTR>( resize );
	LI_FN( VirtualProtect )( swap_chainvtable, 0x1000, old_protect_resize, &old_protect_resize );
}

bool menu_open = false;
WNDPROC owndproc;

__int64 __stdcall hk_wndproc( const HWND hwnd, const UINT msg, const WPARAM param, const LPARAM lparam ) {
	if ( msg == WM_KEYDOWN && param == VK_INSERT )
		menu_open = !menu_open;

	if ( menu_open ) {
		ImGui_ImplWin32_WndProcHandler( hwnd, msg, param, lparam );
	}
	
	return LI_FN( CallWindowProcA )( owndproc, hwnd, msg, param, lparam );
}

HRESULT __stdcall hooks::present( IDXGISwapChain* swapchain, const UINT sync_interval, const UINT flags ) {	
	if ( !device ) {
		if ( SUCCEEDED( swapchain->GetDevice( __uuidof(ID3D11Device), reinterpret_cast<void **>( &device ) ) ) ) {
			swapchain->GetDevice( __uuidof( device ), reinterpret_cast<void**>( &device ) );
			device->GetImmediateContext( &context );
		}

		if ( !owndproc )
			owndproc = reinterpret_cast<WNDPROC>( LI_FN( SetWindowLongPtrA )( LI_FN( FindWindowA )( _( "UnrealWindow" ), nullptr ), GWLP_WNDPROC, uintptr_t( hk_wndproc ) ) );

		ID3D11Texture2D* render_target = nullptr;
		if ( SUCCEEDED( swapchain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), reinterpret_cast<LPVOID*>( &render_target ) ) ) ) {
			device->CreateRenderTargetView( render_target, nullptr, &render_target_view );
			render_target->Release( );
		}

		ImGui::CreateContext( );
		ImGui_ImplDX11_Init( LI_FN( FindWindowA )( _( "UnrealWindow" ), nullptr ), device, context );

		ImGui::GetIO( ).FontDefault = ImGui::GetIO( ).Fonts->AddFontFromFileTTF( _( R"(C:\Windows\Fonts\tahoma.ttf)" ), 14.f );

		ImGui_ImplDX11_CreateDeviceObjects( );
	}

	context->OMSetRenderTargets( 1, &render_target_view, nullptr );
	ImGui_ImplDX11_NewFrame( );

	ImGui::PushStyleVar( ImGuiStyleVar_WindowBorderSize, 0.0f );
	ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f } );
	ImGui::PushStyleColor( ImGuiCol_WindowBg, { 0.0f, 0.0f, 0.0f, 0.0f } );
	ImGui::Begin( _( "##basaa" ), nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoInputs );
	{
		ImGui::SetWindowPos( ImVec2( 0, 0 ), ImGuiCond_Always );
		ImGui::SetWindowSize( ImVec2( ImGui::GetIO( ).DisplaySize.x, ImGui::GetIO( ).DisplaySize.y ), ImGuiCond_Always );

		esp::player_esp( );
		esp::world_esp( );
	}

	ImGui::End( );
	ImGui::PopStyleColor( );
	ImGui::PopStyleVar( 2 );

	if ( menu_open ) {
		ImGui::SetNextWindowSize( ImVec2( 350, 250 ), ImGuiCond_FirstUseEver );
		ImGui::Begin( _( "Vitasnella [valorant]" ), nullptr,
			ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse );
		{
			{
				if ( ImGui::BeginTabBar( _( "primary_interface_tabbar" ), ImGuiTabBarFlags_NoTooltip ) ) {
					if ( ImGui::BeginTabItem( _( "COMBAT###combat_tabitem" ) ) ) {
						ImGui::Checkbox( _( "Aimbot enabled###combat.aimbot_enabled" ), &settings::aimbot_enable );
						ImGui::SameLine( );
						ImGui::hotkey( _( "###aimbind" ), &settings::aimbot_bind );
						ImGui::Checkbox( _( "Visible only" ), &settings::aimbot_visible );
						ImGui::SliderInt( _( "FOV" ), &settings::aimbot_fov, 7, 20 );
						ImGui::SliderInt( _( "Smoothing" ), &settings::aimbot_smooth, 0, 15 );
						
						ImGui::EndTabItem( );
					}
					if ( ImGui::BeginTabItem( _( "VISUALS###visuals_tabitem" ) ) ) {
						ImGui::Checkbox( _( "Box esp" ), &settings::visuals_box );
						ImGui::SameLine( );
						ImGui::ColorEdit4( _( "##player_box_color" ), reinterpret_cast<float*>( &settings::visuals_box_color ), ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs );
						ImGui::Checkbox( _( "Skeleton esp" ), &settings::visuals_skeleton );
						ImGui::SameLine( );
						ImGui::ColorEdit4( _( "##skeleton_color" ), reinterpret_cast<float*>( &settings::visuals_skeleton_color ), ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs );
						ImGui::Checkbox( _( "Name esp" ), &settings::visuals_name );
						ImGui::SameLine( );
						ImGui::ColorEdit4( _( "##player_name_color" ), reinterpret_cast<float*>( &settings::visuals_name_color ), ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs );
						ImGui::Checkbox( _( "Aimbot FOV" ), &settings::visuals_showfov );
						ImGui::SameLine( );
						ImGui::ColorEdit4( _( "##aimbot_fov_color" ), reinterpret_cast<float*>( &settings::visuals_fov_color ), ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs );
						ImGui::Checkbox( _( "Drone esp" ), &settings::visuals_drone );
						ImGui::SameLine( );
						ImGui::ColorEdit4( _( "##drone_color" ), reinterpret_cast<float*>( &settings::visuals_drone_color ), ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs );
						ImGui::Checkbox( _( "Camera esp" ), &settings::visuals_camera );
						ImGui::SameLine( );
						ImGui::ColorEdit4( _( "##camera_color" ), reinterpret_cast<float*>( &settings::visuals_camera_color ), ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs );
						ImGui::Checkbox( _( "Tripwire esp" ), &settings::visuals_tripwire );
						ImGui::SameLine( );
						ImGui::ColorEdit4( _( "##tripwire_color" ), reinterpret_cast<float*>( &settings::visuals_tripwire_color ), ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs );
						ImGui::Checkbox( _( "Health esp" ), &settings::visuals_healthbar );
						
						ImGui::EndTabItem( );
					}
					if ( ImGui::BeginTabItem( _( "MISC###misc_tabitem" ) ) ) {
						if ( ImGui::Button( _( "Save config" ) ) )
							config.save( );
						if ( ImGui::Button( _( "Load config" ) ) )
							config.load( );

						ImGui::EndTabItem( );
					}
					ImGui::EndTabBar( );
				}
			}

			auto* background_draw_list = ImGui::GetBackgroundDrawList( );
			background_draw_list->AddRectFilled( ImVec2( ImGui::GetWindowPos( ).x, ImGui::GetWindowPos( ).y ), ImVec2( ImGui::GetWindowPos( ).x + ImGui::GetWindowSize( ).x, ImGui::GetWindowPos( ).y + ImGui::GetWindowSize( ).y ), ImColor( 28, 38, 43, 255 ) );
			ImGui::End( );
		}
	}

	ImGui::Render( );

	return original_present( swapchain, sync_interval, flags );
}

HRESULT hooks::resize( IDXGISwapChain* swapchain, const UINT buffer_count, const UINT width, const UINT height, const DXGI_FORMAT new_format, UINT swapchain_flags ) {
	menu_open = false;
	device->Release( );
	context->Release( );
	render_target_view->Release( );
	ImGui_ImplDX11_Shutdown( );
	ImGui::DestroyContext( );
	device = nullptr;
	
	return original_resize( swapchain, buffer_count, width, height, new_format, swapchain_flags );
}
