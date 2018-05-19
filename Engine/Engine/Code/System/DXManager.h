#pragma once
#include <D3D11.h>

enum BLEND_STATE 
{
	BLEND_OPAQUE,
	BLEND_ALPHA,
	BLEND_ADDITIVE,
	BLEND_SUBTRACTIVE
};

enum RASTERIZER_STATE 
{
	BACKCULL,
	FRONTCULL,
	NOCULL,
	WIREFRAME
};

enum DEPTH_STATE
{
	ENABLED,
	DISABLED,
	READ_ONLY,
	MASKED_SKYBOX
};

class DXManager
{

public:
	static DXManager& GetInstance();

	DXManager();
	~DXManager();

	void Initialize(HWND hwnd,int screenWidth, int screenHeight, bool vsunc, bool fullscreen);
	void ClearRenderTarget(float r, float g, float b, float a);
	void PresentScene();
	void Shutdown();

	void SetViewport(D3D11_VIEWPORT* viewport, bool setDefault = false);
	void SetRenderTarget(ID3D11RenderTargetView* renderTarget, ID3D11DepthStencilView* depthStencil, bool setDefault = false);

	void SetBlendState(BLEND_STATE blending);
	void SetRasterizerState(RASTERIZER_STATE state);
	
	void SetZBuffer(DEPTH_STATE state);

	void SetFullscreen(bool fullscreen, bool toggle = false);

	ID3D11Device*& GetDevice(){ return _device; }
	ID3D11DeviceContext*& GetDeviceCon() {  return _devCon;  }		
	ID3D11DepthStencilView*& GetDeathStencilview() { return _depthStencilView; }
	ID3D11RenderTargetView*& GetRenderTargetView() { return _renderTargetView; }
	char* GetGPUInfo() { return _videoCardDescriptiion; }

private:
	static DXManager* _instance;

	bool CreateSwapchain(HWND hwnd, bool fullscreen, int screenWidth, int screenHeight);
	bool CreateDepthStencil(int screenWidth, int screenHeight);	
	bool CreateDepthstencilStates();
	bool CreateRasterizerStates();
	bool CreateBlendStates();
	void CreateSamplerStates();
	void CreateViewport(int screenWidth, int screenHeight);
	void GetHardwareProperties(int screenWidth, int screenHeight);
			
	bool _vsyncEnabled;
	bool _fullscreen;
	
	IDXGISwapChain*_swapChain;
	ID3D11Device* _device;
	ID3D11DeviceContext* _devCon;

	ID3D11RenderTargetView* _renderTargetView;
	ID3D11DepthStencilView* _depthStencilView;		
	
	ID3D11RasterizerState* _rasterizerBackCull;
	ID3D11RasterizerState* _rasterizerFrontCull;
	ID3D11RasterizerState* _rasterizerNoCull;
	ID3D11RasterizerState* _rasterizerWireframe;

	ID3D11BlendState* _blendStateAlpha;
	ID3D11BlendState* _blendStateOpaque;
	ID3D11BlendState* _blendStateAddetive;
	ID3D11BlendState* _blendStateSubtractive;

	ID3D11DepthStencilState* _depthStencilEnabled;
	ID3D11DepthStencilState* _depthStencilDisabled;
	ID3D11DepthStencilState* _depthStencilReadOnly;
	ID3D11DepthStencilState* _depthStencilMaskedSkybox;

	ID3D11SamplerState* _sampleStateWrap;
	ID3D11SamplerState* _sampleStateClamp;

	D3D11_VIEWPORT _viewport;


	IDXGIOutput* _adapterOutput;
	unsigned int _numerator;
	unsigned int _denominator;
	unsigned int _refreshrate;
	unsigned int  _videoCardMemory;
	char _videoCardDescriptiion[128];
	
	
	




};

