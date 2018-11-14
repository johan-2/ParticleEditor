#pragma once
#include <d3d11.h>

class RenderToTexture
{
public:
	RenderToTexture(unsigned int width, unsigned int height, bool depthOnly);
	~RenderToTexture();

	// get render texture properties
	ID3D11ShaderResourceView* GetShaderResource() { return _shaderResoureView; }
	ID3D11RenderTargetView*   GetRenderTarget()   { return _renderTargetView; }
	ID3D11DepthStencilView*   GetDepthStencil()   { return _depthStencilView; }

	// get viewport
	D3D11_VIEWPORT* GetViewport() {	return &_viewport;	}

	// clear this render target
	void ClearRenderTarget(float r,float g, float b, float a, bool depthOnly);

	// set this rendertarget
	void SetRendertarget();

private:

	ID3D11Texture2D*          _textureView;
	ID3D11RenderTargetView*   _renderTargetView;
	ID3D11DepthStencilView*   _depthStencilView;
	ID3D11ShaderResourceView* _shaderResoureView;
	D3D11_VIEWPORT            _viewport;	
};

