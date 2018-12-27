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

class SimpleClipSceneShader
{
public:
	SimpleClipSceneShader(bool debugQuad = false, XMFLOAT2 pos = XMFLOAT2(0,0));
	~SimpleClipSceneShader();

	void RenderScene(std::vector<Mesh*>& opaqueMeshes, std::vector<Mesh*>& alphaMeshes, XMFLOAT4 clipPlane, bool includeSkyBox = true, bool includeParticles = true);

	// gets the reflection map srv
	ID3D11ShaderResourceView* GetSRV() { return _srv->GetRenderTargetSRV(); }

private:

	// render texture
	RenderToTexture* _srv;

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
};