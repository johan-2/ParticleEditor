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
	ID3D11ShaderResourceView* GetRenderSRV() { return _renderTexture->GetRenderTargetSRV(); }
	ID3D11ShaderResourceView* GetDepthSRV()  { return _renderTexture->GetDepthStencilSRV(); }

private:

	// render texture
	RenderToTexture* _renderTexture;

	// shaders
	ID3D11VertexShader* _vertexShader;
	ID3D11PixelShader*  _pixelShader;

	// bytecode
	ID3D10Blob* _vertexShaderByteCode;
	ID3D10Blob* _pixelShaderByteCodeOpaque;;

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