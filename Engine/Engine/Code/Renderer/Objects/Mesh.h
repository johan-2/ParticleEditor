#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include "TransformComponent.h"
#include "Color32.h";

#define DEFERRED        1 << 0
#define ALPHA_FORWARD   1 << 1
#define CAST_SHADOW_DIR 1 << 2
#define WIREFRAME_COLOR 1 << 3

using namespace DirectX;

class Mesh
{
public:	
	Mesh(Entity* parent, unsigned int FLAGS, wchar_t* diffuseMap, wchar_t* normalMap, wchar_t* specularMap);
	~Mesh();

	// vertex data structure
	struct VertexData
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
		XMFLOAT3 normal;
		XMFLOAT3 tangent;
		XMFLOAT3 binormal;
		Color32  color;
	};

	// create vertex and index buffers
	void CreateBuffers(VertexData* verticesData, unsigned long* indicesData, unsigned int numVertices, unsigned int numIndices);

	// uploads the buffers before rendering
	void UploadBuffers();

	// get the world matrix of the transform this mesh belongs to
	// get the position of the transform this mesh belongs to
	const XMFLOAT4X4& GetWorldMatrix()   { return _transform->GetWorldMatrix(); }
	const XMFLOAT3& GetPosition()        { return _transform->GetPositionRef(); }

	// get number of vertices and indices
	unsigned int GetNumVertices() { return _numVertices; }
	unsigned int GetNumIndices()  { return _numIndices; }

	// get all textures this mesh uses
	ID3D11ShaderResourceView** GetTextureArray() { return _textures; }

	// get/set rendering flags
	unsigned int GetFlags() { return _FLAGS; }
	void SetFlags(unsigned int flags) { _FLAGS = flags; }

	// set and get the offset of uv-coordinates for this mesh
	void SetUvOffset(XMFLOAT2 offset) { _uvOffset = offset; }
	const XMFLOAT2& GetUvOffset()     { return _uvOffset; }

	// get and set distance to camera
	const float& GetDistanceFromCamera()     { return _distance; }
	void SetDistanceToCamera(float distance) { _distance = distance; }

	// adds and removes this mesh to/from the renderer
	void AddRemoveToRenderer(bool add);

private:
	
	// pointers to the vertex/index buffers
	ID3D11Buffer* _vertexBuffer;
	ID3D11Buffer*_indexBuffer;

	// count of vertices/indices
	unsigned int _numVertices;
	unsigned int _numIndices;

	// texture array
	ID3D11ShaderResourceView* _textures[3];

	// uv offset
	XMFLOAT2 _uvOffset;
	
	// rendering flags
	unsigned int _FLAGS;

	// distance from camera
	float _distance;

	// pointer to transform component
	TransformComponent* _transform;
};

