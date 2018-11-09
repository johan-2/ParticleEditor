#pragma once
#include <D3D11.h>
#include <DirectXMath.h>
#include <vector>

using namespace DirectX;

class QuadComponent;

class QuadShader
{
public:
	QuadShader();
	~QuadShader();

	void RenderQuadUI(const std::vector<QuadComponent*>& quads);

	// get byte code from shaders
	ID3D10Blob*& GetVertexShaderByteCode() { return _vertexShaderByteCode; }
	ID3D10Blob*& GetPixelShaderByteCode()  { return _pixelShaderByteCode; }

private:

	// constant buffers
	ID3D11Buffer* _constantBufferVertex;
	ID3D11Buffer* _constantBufferPixel;

	// compiled shaders
	ID3D11VertexShader* _vertexShader;
	ID3D11PixelShader*  _pixelShader;

	// the shader bytecode
	ID3D10Blob* _vertexShaderByteCode;
	ID3D10Blob* _pixelShaderByteCode;

	// vertex canstant structure
	struct ConstantQuadUIVertex
	{
		XMFLOAT4X4 view;
		XMFLOAT4X4 projection;
	};

	// pixel constant structure
	struct ConstantQuadUIPixel
	{
		XMFLOAT4 color;
	};
};

