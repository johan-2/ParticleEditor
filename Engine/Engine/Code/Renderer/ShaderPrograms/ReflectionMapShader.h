#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
#include "RenderToTexture.h"

using namespace DirectX;

class Mesh;
class ParticleSystemComponent;
class ParticleShader;
class DXInputLayouts;

class ReflectionMapShader
{
public:
	ReflectionMapShader();
	~ReflectionMapShader();

	void GenerateReflectionMap(std::vector<Mesh*>& reflectiveOpaqueMeshes,
		std::vector<Mesh*>& reflectiveAlphaMeshes,
		std::vector<ParticleSystemComponent*>& reflectiveParticles,
		ParticleShader*& particleShader,
		DXInputLayouts*& inputLayouts, Mesh*& reflectionMesh, XMFLOAT4 clipPlane);

	// gets the reflection map srv
	ID3D11ShaderResourceView* GetReflectionMap() { return _reflectionMap->GetRenderTargetSRV(); }

private:

	// render texture
	RenderToTexture* _reflectionMap;

	// shaders
	ID3D11VertexShader* _reflectionMapVertexShader;
	ID3D11PixelShader*  _reflectionMapPixelShaderOpaque;
	ID3D11PixelShader*  _reflectionMapPixelShaderAlpha;

	// bytecode
	ID3D10Blob* _reflectionMapVertexShaderByteCode;
	ID3D10Blob* _reflectionMapPixelShaderByteCodeOpaque;
	ID3D10Blob* _reflectionMapPixelShaderByteCodeAlpha;

	// constant buffers
	ID3D11Buffer* _CBVertex;
	ID3D11Buffer* _CBPixelAmbDir;
	ID3D11Buffer* _CBPixelPoint;

	struct CBVertex
	{
		XMFLOAT4X4 world;
		XMFLOAT4X4 worldViewProj;
		XMFLOAT4   clipingPlane;
	};

	struct CBAmbDirPixel
	{
		XMFLOAT4 ambientColor;
		XMFLOAT4 dirDiffuseColor;
		XMFLOAT3 lightDir;
		float    pad;
	};

	struct CBPointPixel
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