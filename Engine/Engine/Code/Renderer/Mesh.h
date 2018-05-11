#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include "TransformComponent.h"

#define AMBIENT  1 << 0
#define DIRECTIONAL  1 << 1
#define CAST_SHADOW_DIR  1 << 2
#define RECIVE_SHADOW_DIR  1 << 3
#define LIGHTS_ALL  1 << 4
#define HAS_ALPHA  1 << 5
#define POINT 1 << 6

using namespace DirectX;

class Mesh
{
public:
	
	Mesh(Entity* parent, unsigned int FLAGS, wchar_t* diffuseMap, wchar_t* normalMap, wchar_t* specularMap);
	~Mesh();

	struct VertexData
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
		XMFLOAT3 normal;
		XMFLOAT3 tangent;
		XMFLOAT3 binormal;
	};

	void CreateBuffers(VertexData* verticesData, unsigned long* indicesData, unsigned int numVertices, unsigned int numIndices);
	void UploadBuffers();

	XMFLOAT4X4 GetWorldMatrix() { return _transform->GetWorldMatrix(); }
	unsigned int GetNumIndices() { return _numIndices; }
	unsigned int GetNumVertices() { return _numVertices; }
	ID3D11ShaderResourceView** GetTextureArray() { return _textures; }

	unsigned int GetFlags() { return _FLAGS; }

	const XMFLOAT2& GetUvOffset() { return _uvOffset; }
	void SetUvOffset(XMFLOAT2 offset) { _uvOffset = offset; }

	XMFLOAT3 GetPosition() { return _transform->GetPositionVal(); }

	float _distance;
	
private:

	
	void AddRemoveToRenderer(bool add);

	TransformComponent* _transform;
	ID3D11Buffer* _vertexBuffer;
	ID3D11Buffer*_indexBuffer;

	unsigned int _numVertices;
	unsigned int _numIndices;

	ID3D11ShaderResourceView* _textures[3];

	XMFLOAT2 _uvOffset;
	
	unsigned int _FLAGS;
};

