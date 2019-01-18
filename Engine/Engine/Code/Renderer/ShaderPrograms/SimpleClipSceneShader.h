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
class InstancedModel;

class SimpleClipSceneShader
{
public:
	SimpleClipSceneShader();
	~SimpleClipSceneShader();

	void RenderScene(std::vector<Mesh*>& opaqueMeshes, std::vector<Mesh*>& alphaMeshes, std::vector<InstancedModel*>& instancedModels, XMFLOAT4 clipPlane, bool includeSkyBox = true, bool includeParticles = true);

	// gets the reflection map srv
	ID3D11ShaderResourceView* GetRenderSRV() { return _renderTexture->GetRenderTargetSRV(); }
	ID3D11ShaderResourceView* GetDepthSRV()  { return _renderTexture->GetDepthStencilSRV(); }

private:

	// render texture
	RenderToTexture* _renderTexture;

	// shaders
	ID3D11VertexShader* _vertexShader;
	ID3D11VertexShader* _vertexShaderInstanced;
	ID3D11PixelShader*  _pixelShader;

	// bytecode
	ID3D10Blob* _vertexShaderByteCode;
	ID3D10Blob* _vertexShaderByteCodeInstanced;
	ID3D10Blob* _pixelShaderByteCode;

	// constant buffers
	ID3D11Buffer* _CBVertex;
	ID3D11Buffer* _CBVertexInstanced;

	struct CBVertex
	{
		XMFLOAT4X4 world;
		XMFLOAT4X4 worldViewProj;
		XMFLOAT4   clipingPlane;
	};

	struct CBVertexInstanced
	{
		XMFLOAT4X4 viewProj;
		XMFLOAT4   clipingPlane;
	};

};