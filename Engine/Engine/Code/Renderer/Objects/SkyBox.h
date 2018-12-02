#pragma once
#include <d3d11.h>
#include <DirectXMath.h>

using namespace DirectX;

class SkyBox
{
public:
	SkyBox(const wchar_t* textureFile);
	~SkyBox();
	
	// load cubemap
	void LoadCubemap(const wchar_t* file);

	// upload buffers and render
	void Render(bool useReflectViewMatrix = false);

	// set the skybox active/inactive
	void setActive(bool active) { _isActive = active; }
	
private:

	// create box mesh
	void CreateBox();

	// constant buffers
	ID3D11Buffer* _constantBufferVertex;

	// compiled shaders
	ID3D11VertexShader* _vertexShader;
	ID3D11PixelShader*  _pixelShader;

	// the shader bytecode
	ID3D10Blob* _vertexShaderByteCode;
	ID3D10Blob* _pixelShaderByteCode;

	// vertex and index buffers
	ID3D11Buffer* _vertexBuffer;
	ID3D11Buffer* _indexBuffer;

	// num vertices and indices
	unsigned int _numVertices;
	unsigned int _numIndices;

	// cubemap texture
	ID3D11ShaderResourceView* _texture;

	// is skybox active
	bool _isActive;

	// vertex input data structure
	struct VertexData
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
		XMFLOAT3 normal;
		XMFLOAT3 tangent;
		XMFLOAT3 binormal;
	};

	// vertex constant buffer
	struct ConstantVertex
	{
		XMFLOAT4X4 world;
		XMFLOAT4X4 view;
		XMFLOAT4X4 projection;
	};

};

