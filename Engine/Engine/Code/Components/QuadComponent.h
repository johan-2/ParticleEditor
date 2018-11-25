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

	void Init(XMFLOAT2 position, XMFLOAT2 size, wchar_t* texturePath = L"", XMFLOAT4 color = XMFLOAT4(1, 1, 1, 1));
	void Update(const float& delta);

	// set quad properties
	void SetPosition(XMFLOAT2 position)            { _position = position; }
	void SetSize(XMFLOAT2 size)                    { _size = size; }
	void SetColor(XMFLOAT4 color)                  { _color = color; }
	void SetTexture(ID3D11ShaderResourceView* tex) { _texture = tex; }

	// get quad properties
	XMFLOAT2 GetPosition()                 { return _position; }
	XMFLOAT2 GetSize()                     { return _size; }
	XMFLOAT4 GetColor()                    { return _color; }
	ID3D11ShaderResourceView* GetTexture() { return _texture; }
	
	void UploadBuffers();

private:
	
	void CreateBuffers();
	void UpdateBuffers();

	// index and vertex buffer
	ID3D11Buffer* _vertexBuffer;
	ID3D11Buffer* _indexBuffer;

	// texture
	ID3D11ShaderResourceView* _texture;

	// position and size	
	XMFLOAT2 _position;
	XMFLOAT2 _size;

	// last frame size and position
	XMFLOAT2 _prevSize;
	XMFLOAT2 _PrevPosition;

	// color
	XMFLOAT4 _color;

	// vertex inputs
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
	};
};

