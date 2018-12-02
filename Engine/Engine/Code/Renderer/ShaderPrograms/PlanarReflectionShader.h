#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>

using namespace DirectX;

class Mesh;
class ParticleSystemComponent;
class RenderToTexture;
class ParticleShader;
class DXInputLayouts;

class PlanarReflectionShader
{
public:
	PlanarReflectionShader();
	~PlanarReflectionShader();

	void Render(std::vector<Mesh*> reflectionMeshes, 
		        std::vector<Mesh*> reflectiveMeshes,
		        std::vector<ParticleSystemComponent*> reflectiveParticles, 
		        ParticleShader* particleShader, 
		        DXInputLayouts* inputLayouts);

private:

	RenderToTexture* _reflectionMap;

	// compiled shaders
	ID3D11VertexShader* _planarVertexShader;
	ID3D11PixelShader*  _planarPixelShader;
	ID3D11VertexShader* _reflectionMapVertexShader;
	ID3D11PixelShader*  _reflectionMapPixelShader;

	// constant buffers
	ID3D11Buffer* _constantBufferPlanarVertex;
	ID3D11Buffer* _constantBufferPlanarPixelAmbDir;
	ID3D11Buffer* _constantBufferPlanarPixelPoint;
	ID3D11Buffer* _constantBufferReflectionMapVertex;

	// the shader bytecode
	ID3D10Blob* _planarVertexShaderByteCode;
	ID3D10Blob* _planarPixelShaderByteCode;

	ID3D10Blob* _reflectionMapVertexShaderByteCode;
	ID3D10Blob* _reflectionMapPixelShaderByteCode;

	struct ConstantVertexPlanar
	{
		XMFLOAT4X4 world;
		XMFLOAT4X4 view;
		XMFLOAT4X4 projection;
		XMFLOAT4X4 lightView;
		XMFLOAT4X4 lightProjection;
		XMFLOAT4X4 reflectionView;

		XMFLOAT3 camPos;
		float    pad1;
		XMFLOAT2 uvOffset;
		XMFLOAT2 pad2;
	};

	struct ConstantAmbientDirectionalPixelPlanar
	{
		XMFLOAT4 ambientColor;
		XMFLOAT4 dirDiffuseColor;
		XMFLOAT3 lightDir;
		float    reflectiveFraction;
	};

	struct ConstantPointPixelPlanar
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

	struct ConstantVertexReflectionMap
	{
		XMFLOAT4X4 world;
		XMFLOAT4X4 view;
		XMFLOAT4X4 projection;
		XMFLOAT4   clipingPlane;
	};
};

