#include "DXManager.h"
#include <iostream>
#include "SystemDefs.h"
#include "DXBlendStates.h"
#include "DXRasterizerStates.h"
#include "DXDepthStencilStates.h"
#include "DXSamplerStates.h"
#include "HardwareProperties.h"
#include "DXErrorHandler.h"

DXManager::DXManager()
{
}

DXManager::~DXManager()
{	
	delete _DXDepthStencilStates;
	delete _DXRasterizerStates;
	delete _DXBlendStates;
	delete _DXSamplerStates;

	_renderTargetView->Release();
	_depthStencilView->Release();

	_device->Release();
	_devCon->Release();
	_swapChain->Release();
}

void DXManager::Initialize(HWND hwnd,int screenWidth, int screenHeight, bool vsync, bool fullscreen)
{		
	//store vsync setting
	_vsyncEnabled = vsync;

	// fetch the harware settings (refreshrate etc)
	_hardwareProperties = new HardwareProperties(screenWidth, screenHeight);
		
	// create the swapchain, devices and render target
	CreateSwapchainAndRenderTarget(hwnd, fullscreen, screenWidth, screenHeight);
	
	// create the depth texture and depth stencil views
	CreateDepthStencilViews(screenWidth, screenHeight);
			
	// create depth stencil states, rasterizer states, blend states and sampler states
	_DXDepthStencilStates = new DXDepthStencilStates(_device, _devCon);	
	_DXRasterizerStates   = new DXRasterizerStates(_device, _devCon);
	_DXBlendStates        = new DXBlendStates(_device, _devCon);
	_DXSamplerStates      = new DXSamplerStates(_device, _devCon);

	// Create and set viewport
	CreateViewport(screenWidth, screenHeight);	
}

void DXManager::CreateSwapchainAndRenderTarget(HWND hwnd, bool fullscreen, int screenWidth, int screenHeight) 
{
	// pointer to fetch the backbuffer texture from swapchain
	ID3D11Texture2D* backBufferPtr = 0;

	// is inverted becuase swapchain takes a bool for windowed 
	_fullscreen = !fullscreen;

	HRESULT result;

	// swapchain description and d3d level
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	D3D_FEATURE_LEVEL    featureLevel;

	// set the back buffer to 32-bit
	ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC)); 
	swapChainDesc.BufferCount       = 1; 
	swapChainDesc.BufferDesc.Width  = screenWidth;
	swapChainDesc.BufferDesc.Height = screenHeight;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; 

	// set the refresh rate to match monitor if vSync is enabled
	if (_vsyncEnabled) 
	{
		const HardwareInfo& info = _hardwareProperties->GetHardwareInfo();

		swapChainDesc.BufferDesc.RefreshRate.Numerator   = info.numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = info.denominator;
	}
	else 
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator   = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	// set the swapchain description
	swapChainDesc.BufferUsage                 = DXGI_USAGE_RENDER_TARGET_OUTPUT;	
	swapChainDesc.OutputWindow                = hwnd;
	swapChainDesc.SampleDesc.Count            = 1;
	swapChainDesc.SampleDesc.Quality          = 0;		
	swapChainDesc.Windowed                    = _fullscreen;	
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling          = DXGI_MODE_SCALING_UNSPECIFIED;	
	swapChainDesc.SwapEffect                  = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags                       = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; 
	featureLevel                              = D3D_FEATURE_LEVEL_11_1;
	
	// create the swapchain and d3d interfaces
	result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL,
		                                   0, &featureLevel, 1, D3D11_SDK_VERSION,
		                                   &swapChainDesc, &_swapChain, &_device, NULL, &_devCon);

	if (FAILED(result))
		DX_ERROR::PrintError(result, "failed to create swapchain and devices");

	// get pointer to the backbuffer texture so we can create the defualt rendertarget view	
	result = _swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
	if (FAILED(result))
		DX_ERROR::PrintError(result, "failed to get back buffer from swapchain");

	// create the main rendertarget and set it active
	result = _device->CreateRenderTargetView(backBufferPtr, NULL, &_renderTargetView);
	if (FAILED(result))
		DX_ERROR::PrintError(result, "failed to create Render target from back buffer ptr");

	// delete texture after creation of rendertarget
	backBufferPtr->Release();

	// set topology
	_devCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void DXManager::CreateDepthStencilViews(int screenWidth, int screenHeight) 
{
	HRESULT result;
	
	// depth texture
	ID3D11Texture2D* depthTex2D;

	// setup the description for TEXTURE2D
	D3D11_TEXTURE2D_DESC depthStencilTexDesc;
	ZeroMemory(&depthStencilTexDesc, sizeof(D3D11_TEXTURE2D_DESC));
	depthStencilTexDesc.Width              = screenWidth;
	depthStencilTexDesc.Height             = screenHeight;
	depthStencilTexDesc.MipLevels          = 1;
	depthStencilTexDesc.ArraySize          = 1;
	depthStencilTexDesc.Format             = DXGI_FORMAT_R24G8_TYPELESS;
	depthStencilTexDesc.SampleDesc.Count   = 1;
	depthStencilTexDesc.SampleDesc.Quality = 0;
	depthStencilTexDesc.Usage              = D3D11_USAGE_DEFAULT;
	depthStencilTexDesc.BindFlags          = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	depthStencilTexDesc.CPUAccessFlags     = 0;
	depthStencilTexDesc.MiscFlags          = 0;

	// setup description for depthstencilview 
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	depthStencilViewDesc.Format             = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension      = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;
	depthStencilViewDesc.Flags              = 0;

	// setup description for shaderresource
	D3D11_SHADER_RESOURCE_VIEW_DESC resourceViewDesc;
	ZeroMemory(&resourceViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	resourceViewDesc.Format                    = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	resourceViewDesc.ViewDimension             = D3D11_SRV_DIMENSION_TEXTURE2D;
	resourceViewDesc.Texture2D.MostDetailedMip = 0;
	resourceViewDesc.Texture2D.MipLevels       = 1;

	// create the depth texture using the description
	result = _device->CreateTexture2D(&depthStencilTexDesc, NULL, &depthTex2D);
	if (FAILED(result))
		DX_ERROR::PrintError(result, "failed to create depth stencil texture");

	// create the default depthstencilview
	result = _device->CreateDepthStencilView(depthTex2D, &depthStencilViewDesc, &_depthStencilView);
	if (FAILED(result))
		DX_ERROR::PrintError(result, "failed to create defualt depth stencil view");

	depthTex2D->Release();
}

// create defualt viewport based on screen size
void DXManager::CreateViewport(int screenWidth, int screenHeight)
{		
	_viewport.Width    = (float)screenWidth;
	_viewport.Height   = (float)screenHeight;
	_viewport.MinDepth = 0.0f;
	_viewport.MaxDepth = 1.0f;
	_viewport.TopLeftX = 0.0f;
	_viewport.TopLeftY = 0.0f;

	_devCon->RSSetViewports(1, &_viewport);
}

// clears render target and depth/stencil buffers
void DXManager::ClearRenderTarget(float r, float g, float b, float a)
{
	float color[4] = { r,g,b,a };

	// clear rendertarget and depth
	_devCon->ClearRenderTargetView(_renderTargetView, color);
	_devCon->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 1.0f);

}
void DXManager::PresentScene()
{
	//present depending on vsync
	if (_vsyncEnabled)
		_swapChain->Present(1, 0);
	else
		_swapChain->Present(0, 0);
}

// set render target and/or depthstencil
// NOTE: null can be sent in as render target if we only care about having a depth stencil view bound
void DXManager::SetBackBuffer()
{
	_devCon->OMSetRenderTargets(1, &_renderTargetView, _depthStencilView);
}

// set a viewport
void DXManager::SetViewport(D3D11_VIEWPORT* viewport, bool setDefault)
{
	setDefault ? _devCon->RSSetViewports(1, &_viewport) : _devCon->RSSetViewports(1, viewport);
}

// sets fullscreen or toggles the fullscreen mode
void DXManager::SetFullscreen(bool fullscreen, bool toggle)
{
	toggle ? _swapChain->SetFullscreenState(!_fullscreen, NULL) : _swapChain->SetFullscreenState(fullscreen, NULL);
	toggle ? _fullscreen = !_fullscreen : _fullscreen = fullscreen;
}

