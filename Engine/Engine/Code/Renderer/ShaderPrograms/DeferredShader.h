#pragma once
#include <d3d11.h>
#include <vector>
#include <DirectXMath.h>

class Mesh;
class GBuffer;

using namespace DirectX;

class DeferredShader
{
public:
	DeferredShader();
	~DeferredShader();

	// renders all meshes to the depth map
	void RenderGeometry(std::vector<Mesh*>& meshes);
	void RenderLightning(GBuffer*& gBuffer);

	// get byte code from shaders
	ID3D10Blob*& GetVertexGeometryShaderByteCode() { return _vertexGeometryShaderByteCode; }
	ID3D10Blob*& GetPixelGeometryShaderByteCode()  { return _pixelGeometryShaderByteCode; }

	ID3D10Blob*& GetVertexLightningShaderByteCode() { return _vertexLightningShaderByteCode; }
	ID3D10Blob*& GetPixelLightningShaderByteCode() { return _pixelLightningShaderByteCode; }

private:

	// compiled shaders
	ID3D11VertexShader* _vertexGeometryShader;
	ID3D11PixelShader*  _pixelGeometryShader;

	ID3D11VertexShader* _vertexLightShader;
	ID3D11PixelShader*  _pixelLightShader;

	// constant buffers
	ID3D11Buffer* _constantBufferGeometry;
	ID3D11Buffer* _constantBufferDefPoint;
	ID3D11Buffer* _constantBufferDefAmbient;
	ID3D11Buffer* _constantBufferDefDirectional;

	// the shader bytecode
	ID3D10Blob* _vertexGeometryShaderByteCode;
	ID3D10Blob* _pixelGeometryShaderByteCode;

	ID3D10Blob* _vertexLightningShaderByteCode;
	ID3D10Blob* _pixelLightningShaderByteCode;

	// constant data structure for the geometry pass
	struct ConstantGeometryVertex
	{
		XMFLOAT4X4 world;
		XMFLOAT4X4 view;
		XMFLOAT4X4 projection;
		XMFLOAT2   uvOffset;
		XMFLOAT2   pad;
	};

	// constant data for ambient lightning in the lightning pass
	// put the camera world pos here aswell for the moment
	struct ConstantDeferredAmbient
	{
		XMFLOAT4 ambientColor;
		XMFLOAT4 cameraPosition;
	};

	// constant data for directional lightning in the lightning pass
	struct ConstantDeferredDirectional
	{
		XMFLOAT4X4 lightView;
		XMFLOAT4X4 lightProjection;
		XMFLOAT4   lightColor;		

		XMFLOAT3   lightDirection;
		float      padding;
	};

	// constant data for point lightning in the lightning pass
	struct ConstantDeferredPoint
	{
		XMFLOAT3 lightPosition;
		float    radius;

		XMFLOAT3 color;
		float    intensity;		

		float attConstant;
		float attLinear;
		float attExponential;
		int   numLights;
	};
};

