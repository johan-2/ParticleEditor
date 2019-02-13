#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include "IComponent.h"

using namespace DirectX;

class Entity;

class QuadComponent : public IComponent
{
public:
	QuadComponent();
	~QuadComponent();

	void Init(XMFLOAT2 position, XMFLOAT2 size, wchar_t* texturePath = L"", XMFLOAT4 color = XMFLOAT4(1, 1, 1, 1), bool ignoreAlpha = false);
	void Update(const float& delta);

	void UploadBuffers();

	ID3D11ShaderResourceView* texture;
	XMFLOAT2 position;
	XMFLOAT2 size;
	XMFLOAT4 color;
	bool     ignoreAlpha;

private:
	
	void CreateBuffers();
	void UpdateBuffers();

	// index and vertex buffer
	ID3D11Buffer* _vertexBuffer;
	ID3D11Buffer* _indexBuffer;
	
	// last frame size and position
	XMFLOAT2 _prevSize;
	XMFLOAT2 _PrevPosition;

	// vertex inputs
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
	};
};

