#pragma once
#include <d3d11.h>

class RenderToTexture
{
public:
	RenderToTexture(unsigned int width, unsigned int height, bool depthOnly, bool HDR, bool addRenderDebugQuad = false, bool addDepthDebugQuad = false);
	~RenderToTexture();

	// get render texture properties
	ID3D11ShaderResourceView* GetRenderTargetSRV()      { return _renderTargetSRV; }
	ID3D11ShaderResourceView* GetDepthStencilSRV()      { return _depthStencilSRV; }
	ID3D11RenderTargetView*   GetRenderTarget()         { return _renderTargetView; }
	ID3D11DepthStencilView*   GetDepthStencil()         { return _depthStencilView; }
	ID3D11DepthStencilView*   GetDepthStencilReadOnly() { return _depthStencilView; }

	// get viewport
	D3D11_VIEWPORT* GetViewport() {	return &_viewport;	}

	// clear this render target
	void ClearRenderTarget(float r,float g, float b, float a, bool depthOnly);

	// set this rendertarget
	void SetRendertarget(bool depthOnly, bool depthReadOnly);
	void Release();
private:

	ID3D11RenderTargetView*   _renderTargetView;
	ID3D11DepthStencilView*   _depthStencilView;
	ID3D11DepthStencilView*   _depthStencilViewReadOnly;
	ID3D11ShaderResourceView* _renderTargetSRV;
	ID3D11ShaderResourceView* _depthStencilSRV;
	D3D11_VIEWPORT            _viewport;	
};

