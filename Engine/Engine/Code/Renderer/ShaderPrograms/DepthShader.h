#pragma once
#include <d3d11.h>
#include <vector>
#include <DirectXMath.h>

class Mesh;
class InstancedModel;

using namespace DirectX;

class DepthShader
{
public:
	DepthShader();
	~DepthShader();

	// renders all meshes to the depth map
	void RenderDepth(std::vector<Mesh*>& meshes);
	void RenderDepthInstanced(std::vector<InstancedModel*>& models);


	// get byte code from shaders
	ID3D10Blob*& GetVertexShaderByteCode() { return _vertexShaderByteCode; }
	ID3D10Blob*& GetPixelShaderByteCode()  { return _pixelShaderByteCode; }

private:

	// compiled shaders
	ID3D11VertexShader* _vertexShader;
	ID3D11VertexShader* _vertexShaderInstanced;
	ID3D11PixelShader*  _pixelShader;

	// constant buffers
	ID3D11Buffer* _constantBufferVertex;
	
	// the shader bytecode
	ID3D10Blob* _vertexShaderByteCode;
	ID3D10Blob* _vertexShaderByteCodeInstanced;
	ID3D10Blob* _pixelShaderByteCode;

	// constant data structure
	struct ConstantVertex
	{
		XMFLOAT4X4 worldViewProj;
	};
};

