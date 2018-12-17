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
class ReflectionMapShader;

class PlanarReflectionShader
{
public:
	PlanarReflectionShader();
	~PlanarReflectionShader();

	void Render(std::vector<Mesh*>& reflectionMeshes, 
		        std::vector<Mesh*>& reflectiveOpaqueMeshes,
				std::vector<Mesh*>& reflectiveAlphaMeshes,
		        std::vector<ParticleSystemComponent*>& reflectiveParticles, 
		        ParticleShader*& particleShader, 
		        DXInputLayouts*& inputLayouts,
		        ReflectionMapShader*& reflectionMapShader);

private:
	
	// compiled shaders
	ID3D11VertexShader* _planarVertexShader;
	ID3D11PixelShader*  _planarPixelShader;
	
	// constant buffers
	ID3D11Buffer* _CBVertex;
	ID3D11Buffer* _CBPixelAmbDir;
	ID3D11Buffer* _CBPixelPoint;	

	// the shader bytecode
	ID3D10Blob* _planarVertexShaderByteCode;
	ID3D10Blob* _planarPixelShaderByteCode;

	struct CBVertexPlanar
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

	struct CBAmbDirPixelPlanar
	{
		XMFLOAT4 ambientColor;
		XMFLOAT4 dirDiffuseColor;
		XMFLOAT3 lightDir;
		float    reflectiveFraction;
	};
};

