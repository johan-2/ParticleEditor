#pragma once
#include <D3D11.h>

class DXBlendStates;
class DXRasterizerStates;
class DXDepthStencilStates;
class DXSamplerStates;
class HardwareProperties;

class DXManager
{
public:
	static DXManager& GetInstance();

	DXManager();
	~DXManager();

	void Initialize(HWND hwnd,int screenWidth, int screenHeight, bool vSync, bool fullscreen);
	void ClearRenderTarget(float r, float g, float b, float a);
	void PresentScene();	

	void SetViewport(D3D11_VIEWPORT* viewport, bool setDefault = false);
	void SetRenderTarget(ID3D11RenderTargetView* renderTarget, ID3D11DepthStencilView* depthStencil, bool setDefault = false, bool setDepthReadOnly = false);

	void SetFullscreen(bool fullscreen, bool toggle = false);

	// get DX modules
	DXBlendStates*&        BlendStates()        { return _DXBlendStates; }
	DXRasterizerStates*&   RasterizerStates()   { return _DXRasterizerStates; }
	DXDepthStencilStates*& DepthStencilStates() { return _DXDepthStencilStates; }
	DXSamplerStates*&      SamplerStates()      { return _DXSamplerStates; }

	// get hardware properties
	HardwareProperties*& GetHardwareProperties() { return _hardwareProperties; }

	// get dx interfaces
	ID3D11Device*&        GetDevice()    { return _device; }
	ID3D11DeviceContext*& GetDeviceCon() { return _devCon; }	

	// get default render/depth views
	ID3D11DepthStencilView*&   GetDefaultDepthStencilView() { return _depthStencilView; }
	ID3D11RenderTargetView*&   GetDefaultRenderTargetView() { return _renderTargetView; }

	// get srv from depth stencil texture
	ID3D11ShaderResourceView*& GetDepthSRV()                { return _depthShaderResourceView; }

private:

	// create swapchain and devices
	void CreateSwapchainAndRenderTarget(HWND hwnd, bool fullscreen, int screenWidth, int screenHeight);
	void CreateDepthStencilViews(int screenWidth, int screenHeight);

	//static instance to this class
	static DXManager* _instance;

	// DX modules
	DXBlendStates*        _DXBlendStates;
	DXRasterizerStates*   _DXRasterizerStates;
	DXDepthStencilStates* _DXDepthStencilStates;
	DXSamplerStates*      _DXSamplerStates;
	HardwareProperties*   _hardwareProperties;
	
	// create the viewport
	void CreateViewport(int screenWidth, int screenHeight);
			
	// misc settings
	bool _vsyncEnabled;
	bool _fullscreen;
	
	// dx interfaces and swapchain
	ID3D11Device*        _device;
	ID3D11DeviceContext* _devCon;
	IDXGISwapChain*      _swapChain;

	// Render and depth stencil views
	ID3D11RenderTargetView*   _renderTargetView;
	ID3D11DepthStencilView*   _depthStencilView;
	ID3D11DepthStencilView*   _depthStencilViewReadOnly;
	ID3D11ShaderResourceView* _depthShaderResourceView;

	// default viewport
	D3D11_VIEWPORT _viewport;

	
};

