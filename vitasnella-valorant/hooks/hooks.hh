#pragma once
#include <Windows.h>
#include <d3d11.h>

namespace hooks {
	void init( );

	inline ID3D11Device* device = nullptr;
	inline ID3D11DeviceContext* context = nullptr;
	inline ID3D11RenderTargetView* render_target_view = nullptr;
	HRESULT __stdcall present( IDXGISwapChain*, UINT, UINT );
	HRESULT resize( IDXGISwapChain* swapchain, UINT buffer_count, UINT width, UINT height, DXGI_FORMAT new_format, UINT swapchain_flags );
};
