#pragma once
#include <d3d11.h>
#include <vector>
#include <DirectXMath.h>

class Mesh;
class GBuffer;
class InstancedModel;

using namespace DirectX;

class DeferredShader
{
public:
	DeferredShader();
	~DeferredShader();

	// renders all meshes to the depth map
	void RenderGeometry(std::vector<Mesh*>& meshes);
	void renderGeometryInstanced(std::vector<InstancedModel*> models);
	void RenderLightning(GBuffer*& gBuffer);

	// get byte code from shaders
	ID3D10Blob*& GetVertexGeometryShaderByteCode() { return _vertexGeometryShaderByteCode; }
	ID3D10Blob*& GetVertexGeometryShaderByteCodeInstanced() { return _vertexGeometryShaderByteCodeInstanced; }
	ID3D10Blob*& GetPixelGeometryShaderByteCode()  { return _pixelGeometryShaderByteCode; }

	ID3D10Blob*& GetVertexLightningShaderByteCode() { return _vertexLightningShaderByteCode; }
	ID3D10Blob*& GetPixelLightningShaderByteCode() { return _pixelLightningShaderByteCode; }

private:

	// compiled shaders
	ID3D11VertexShader* _vertexGeometryShader;
	ID3D11VertexShader* _vertexGeometryShaderInstanced;
	ID3D11PixelShader*  _pixelGeometryShader;

	ID3D11VertexShader* _vertexLightShader;
	ID3D11PixelShader*  _pixelLightShader;

	// constant buffers
	ID3D11Buffer* _CBGeometryVertex;
	ID3D11Buffer* _CBGeometryVertexInstanced;
	ID3D11Buffer* _CBMisc;

	// the shader bytecode
	ID3D10Blob* _vertexGeometryShaderByteCode;
	ID3D10Blob* _vertexGeometryShaderByteCodeInstanced;
	ID3D10Blob* _pixelGeometryShaderByteCode;

	ID3D10Blob* _vertexLightningShaderByteCode;
	ID3D10Blob* _pixelLightningShaderByteCode;

	// constant data structure for the geometry pass
	struct CBGeometryVertex
	{
		XMFLOAT4X4 world;
		XMFLOAT4X4 worldViewProj;
		XMFLOAT2   uvOffset;
		XMFLOAT2   pad;
	};

	struct CBGeometryVertexInstanced
	{
		XMFLOAT4X4 ViewProj;	
	};

	struct CBMiscPixel
	{
		XMFLOAT4X4 lightViewProj;
		XMFLOAT4   cameraPosition;
	};

};

