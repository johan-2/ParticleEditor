#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include "IComponent.h"

using namespace DirectX;

class Entity;

class QuadComponent : public IComponent
{
public:

	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
	};

	QuadComponent();
	~QuadComponent();

	void Init(XMFLOAT2 position, XMFLOAT2 size, wchar_t* texturePath, XMFLOAT4 color = XMFLOAT4(1, 1, 1, 1));
	void Update();

	// set/get pos
	void SetPosition(XMFLOAT2 position) { _position = position; }
	XMFLOAT2 GetPosition() { return _position; }

	// set/get size
	void SetSize(XMFLOAT2 size) { _size = size; }
	XMFLOAT2 GetSize() { return _size; }

	void SetColor(XMFLOAT4 color) { _color = color; }
	XMFLOAT4 GetColor() { return _color; }

	void SetTexture(ID3D11ShaderResourceView* tex) { _texture = tex; }
	ID3D11ShaderResourceView* GetTexture() { return _texture; }
	
	void UploadBuffers();

private:
	
	void CreateBuffers();
	void UpdateBuffers();

	ID3D11Buffer* _vertexBuffer, *_indexBuffer;	
	ID3D11ShaderResourceView* _texture;

	XMFLOAT2 _position;
	XMFLOAT2 _PrevPosition;
	XMFLOAT2 _size;
	XMFLOAT2 _prevSize;

	XMFLOAT4 _color;

	



};

