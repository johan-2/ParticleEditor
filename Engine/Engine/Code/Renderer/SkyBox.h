#pragma once

#include <d3d11.h>
#include <DirectXMath.h>

using namespace DirectX;

class SkyBox
{
public:
	SkyBox(wchar_t* textureFile);
	~SkyBox();
	
	void Render();
	
private:

	void Update();
	void CreateBox();
	void Loadtexture(wchar_t* file);

	XMFLOAT4X4 _worldMatrix;
	ID3D11Buffer* _vertexBuffer;
	ID3D11Buffer*_indexBuffer;

	unsigned int _numVertices;
	unsigned int _numIndices;

	ID3D11ShaderResourceView* _texture;


	struct VertexData
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
		XMFLOAT3 normal;
		XMFLOAT3 tangent;
		XMFLOAT3 binormal;
	};

};

