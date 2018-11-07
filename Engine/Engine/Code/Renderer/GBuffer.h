#pragma once
#include <d3d11.h>

class GBuffer
{
public:
	GBuffer();
	~GBuffer();

	// sets the rendertargets of this buffer
	void SetRenderTargets();

	// get All SRV's in this buffer
	ID3D11ShaderResourceView**& GetSrvArray() { return _srvArray; }

private:

	// creates all rander targets
	void CreateRenderTargets();

	// srv and render target arrays
	ID3D11ShaderResourceView** _srvArray;
	ID3D11RenderTargetView**   _renderTargetArray;
};

