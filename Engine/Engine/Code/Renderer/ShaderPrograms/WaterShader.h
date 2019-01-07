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
class SimpleClipSceneShader;

class WaterShader
{
public:
	WaterShader();
	~WaterShader();

	void Render(std::vector<Mesh*>& waterMeshes);

private:
	
	// compiled shaders
	ID3D11VertexShader* _waterVertexShader;
	ID3D11PixelShader*  _waterPixelShader;
	
	// constant buffers
	ID3D11Buffer* _CBVertex;
	ID3D11Buffer* _CBPixelAmbDir;

	// the shader bytecode
	ID3D10Blob* _waterVertexShaderByteCode;
	ID3D10Blob* _waterPixelShaderByteCode;

	SimpleClipSceneShader*  _simpleClipShaderReflection;
	SimpleClipSceneShader*  _simpleClipShaderRefraction;

	struct CBVertexWater
	{
		XMFLOAT4X4 world;
		XMFLOAT4X4 worldViewProj;
		XMFLOAT4X4 worldViewProjLight;
		XMFLOAT4X4 worldViewProjReflect;

		XMFLOAT3 camPos;
		float    pad1;
		XMFLOAT2 uvOffset;
		XMFLOAT2 pad2;
	};

	struct CBAmbDirPixelWater
	{
		XMFLOAT4 ambientColor;
		XMFLOAT4 dirDiffuseColor;
		XMFLOAT3 lightDir;
		float    reflectiveFraction;
	};
};

