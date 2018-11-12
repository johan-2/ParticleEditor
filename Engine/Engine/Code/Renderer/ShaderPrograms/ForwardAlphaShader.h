#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>

using namespace DirectX;

class Mesh;

class ForwardAlphaShader
{
public:
	ForwardAlphaShader();
	~ForwardAlphaShader();

	void RenderForward(std::vector<Mesh*>& meshes);

private:

	// compiled shaders
	ID3D11VertexShader* _vertexShader;
	ID3D11PixelShader*  _pixelShader;

	// constant buffers
	ID3D11Buffer* _constantBufferVertex;
	ID3D11Buffer* _constantBufferPixelAmbDir;
	ID3D11Buffer* _constantBufferPixelPoint;
	
	// the shader bytecode
	ID3D10Blob* _vertexShaderByteCode;
	ID3D10Blob* _pixelShaderByteCode;

	struct ConstantVertex
	{
		XMFLOAT4X4 world;
		XMFLOAT4X4 view;
		XMFLOAT4X4 projection;
		XMFLOAT4X4 lightView;
		XMFLOAT4X4 lightProjection;

		XMFLOAT3 camPos;
		float    pad1;
		XMFLOAT2 uvOffset;
		XMFLOAT2 pad2;
	};

	struct ConstantAmbientDirectionalPixel
	{
		XMFLOAT4 ambientColor;
		XMFLOAT4 dirDiffuseColor;
		XMFLOAT4 dirSpecularColor;
		XMFLOAT3 lightDir;
		float    specularPower;
	};

	struct ConstantPointPixel
	{
		XMFLOAT3 lightPosition;
		float    radius;
		XMFLOAT3 color;
		float    intensity;
		XMFLOAT3 specularColor;
		float    specularPower;

		float attConstant;
		float attLinear;
		float attExponential;
		int   numLights;
	};
};

