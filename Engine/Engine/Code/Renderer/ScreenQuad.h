#pragma once
#include <d3d11.h>
#include <DirectXMath.h>

using namespace DirectX;

class ScreenQuad
{
public:
	ScreenQuad();
	~ScreenQuad();

	void UploadBuffers();

private:

	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
	};

	void CreateBuffers();

	ID3D11Buffer* _vertexBuffer;
	ID3D11Buffer*_indexBuffer;

};

