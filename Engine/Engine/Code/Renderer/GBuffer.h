#pragma once

#include <d3d11.h>

class GBuffer
{
public:
	GBuffer();
	~GBuffer();

	void SetRenderTargets();

	ID3D11ShaderResourceView**& GetSrvArray() { return _srvArray; }

private:

	void CreateRenderTargets();

	ID3D11ShaderResourceView** _srvArray;
	ID3D11RenderTargetView** _renderTargetArray;
};

