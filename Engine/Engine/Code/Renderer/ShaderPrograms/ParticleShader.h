#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>

using namespace DirectX;

class ParticleSystemComponent;

class ParticleShader
{
public:
	ParticleShader();
	~ParticleShader();

	// render all particle systems
	void RenderParticles(const std::vector<ParticleSystemComponent*>& systems, bool useReflectViewMatrix = false, float reflectHeight = 0.0f);

	// get byte code from shaders
	ID3D10Blob*& GetVertexShaderByteCode() { return _vertexShaderByteCode; }
	ID3D10Blob*& GetPixelShaderByteCode() { return _pixelShaderByteCode; }

private:

	// the shaders
	ID3D11VertexShader* _vertexShader;
	ID3D11PixelShader*  _pixelShader;

	// the shader bytecode
	ID3D10Blob* _vertexShaderByteCode;
	ID3D10Blob* _pixelShaderByteCode;

	// constant vertex buffer
	ID3D11Buffer* _constantBufferVertex;

	// constant vertex structure
	struct ConstantVertex
	{
		XMFLOAT4X4 view;
		XMFLOAT4X4 projection;
	};
};

