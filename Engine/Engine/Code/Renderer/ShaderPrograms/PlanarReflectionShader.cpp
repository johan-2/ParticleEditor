#include "PlanarReflectionShader.h"
#include "ShaderHelpers.h"
#include "DXManager.h"
#include "DXBlendStates.h"
#include "DXDepthStencilStates.h"
#include "CameraManager.h"
#include "LightManager.h"
#include "Mesh.h"
#include "Systems.h"
#include "RenderToTexture.h"
#include "SystemDefs.h"
#include "Entity.h"
#include "QuadComponent.h"
#include "ParticleShader.h"
#include "SkyDome.h"
#include "ReflectionMapShader.h"

PlanarReflectionShader::PlanarReflectionShader()
{
	//create shaders
	SHADER_HELPERS::CreateVertexShader(L"shaders/vertexForwardPlanar.vs", _planarVertexShader, _planarVertexShaderByteCode);
	SHADER_HELPERS::CreatePixelShader(L"shaders/pixelForwardPlanar.ps",   _planarPixelShader, _planarPixelShaderByteCode);

	// create constant buffers
	SHADER_HELPERS::CreateConstantBuffer(_CBVertex);
	SHADER_HELPERS::CreateConstantBuffer(_CBPixelAmbDir);
	SHADER_HELPERS::CreateConstantBuffer(_CBPixelPoint);
}

PlanarReflectionShader::~PlanarReflectionShader()
{
	_planarVertexShader->Release();
	_planarPixelShader->Release();

	_planarVertexShaderByteCode->Release();
	_planarPixelShaderByteCode->Release();

	_CBPixelAmbDir->Release();
	_CBPixelPoint->Release();
	_CBVertex->Release();
}

void PlanarReflectionShader::Render(std::vector<Mesh*>& reflectionMeshes,
    std::vector<Mesh*>& reflectiveOpaqueMeshes,
    std::vector<Mesh*>& reflectiveAlphaMeshes,
    std::vector<ParticleSystemComponent*>& reflectiveParticles,
    ParticleShader*& particleShader,
    DXInputLayouts*& inputLayouts,
	ReflectionMapShader*& reflectionMapShader)
{
	if (reflectionMeshes.size() == 0)
		return;

	// get systems
	DXManager&     DXM = *Systems::dxManager;
	CameraManager& CM  = *Systems::cameraManager;
	LightManager&  LM  = *Systems::lightManager;

	// get device context
	ID3D11DeviceContext*& devCon = DXM.GetDeviceCon();

	// get game camera and shadow camera
	CameraComponent*& camera      = CM.GetCurrentCameraGame();
	CameraComponent*& cameraLight = CM.GetCurrentCameraDepthMap();

	// get camera position
	const XMFLOAT3& cameraPos = camera->GetComponent<TransformComponent>()->GetPositionRef();

	// get directional light
	LightDirectionComponent*& directionalLight = LM.GetDirectionalLight();

	// create constant buffer structures
	CBVertexPlanar      constantVertex;
	CBAmbDirPixelPlanar constantAmbDirPixel;

	// set ambient and directional light properties for pixel shader
	XMStoreFloat4(&constantAmbDirPixel.ambientColor,    XMLoadFloat4(&LM.GetAmbientColor()));
	XMStoreFloat4(&constantAmbDirPixel.dirDiffuseColor, XMLoadFloat4(&directionalLight->GetLightColor()));
	XMStoreFloat3(&constantAmbDirPixel.lightDir,        XMLoadFloat3(&directionalLight->GetLightDirectionInv()));

	// set camera matrices
	XMStoreFloat4x4(&constantVertex.view,       XMLoadFloat4x4(&camera->GetViewMatrix()));
	XMStoreFloat4x4(&constantVertex.projection, XMLoadFloat4x4(&camera->GetProjectionMatrix()));
	XMStoreFloat3(&constantVertex.camPos,       XMLoadFloat3(&cameraPos));

	// set direction light matrices
	XMStoreFloat4x4(&constantVertex.lightView,       XMLoadFloat4x4(&cameraLight->GetViewMatrix()));
	XMStoreFloat4x4(&constantVertex.lightProjection, XMLoadFloat4x4(&cameraLight->GetProjectionMatrix()));

	// get shadow map
	ID3D11ShaderResourceView* shadowMap = cameraLight->GetSRV();

	// update pixel shader constant buffer fro point lights
	SHADER_HELPERS::UpdateConstantBuffer((void*)LM.GetCBPointBuffer(), sizeof(CBPoint) * LM.GetNumPointLights(), _CBPixelPoint);
	SHADER_HELPERS::UpdateConstantBuffer((void*)&constantAmbDirPixel, sizeof(CBAmbDirPixelPlanar), _CBPixelAmbDir);

	// loop over all meshes that will project reflections onto itself
	const int numMeshes = reflectionMeshes.size();
	for (int i = 0; i < numMeshes; i++)
	{
		// get the mesh that will project the reflections
		Mesh* mesh = reflectionMeshes[i];

		// get the clip plane of the reflection mesh so we dont render anything below this plane
		XMFLOAT4 clipPlane(0, 1, 0, -mesh->GetPosition().y);

		// render the reflectionmap
		reflectionMapShader->GenerateReflectionMap(reflectiveOpaqueMeshes, reflectiveAlphaMeshes, reflectiveParticles, particleShader, inputLayouts, mesh, clipPlane);

		// set back to defualt render target and render the reflection mesh
		Systems::renderer->SetMainRenderTarget();

		// set shaders
		devCon->VSSetShader(_planarVertexShader, NULL, 0);
		devCon->PSSetShader(_planarPixelShader, NULL, 0);

		// set the vertex constant buffer, the pixel ones is already set
		devCon->VSSetConstantBuffers(0, 1, &_CBVertex);
		devCon->PSSetConstantBuffers(0, 1, &_CBPixelAmbDir);
		devCon->PSSetConstantBuffers(1, 1, &_CBPixelPoint);

		// set to alpha blending
		DXM.BlendStates()->SetBlendState(BLEND_STATE::BLEND_ALPHA);

		// set vertex constant values
		XMStoreFloat4x4(&constantVertex.world,          XMLoadFloat4x4(&mesh->GetWorldMatrix()));
		XMStoreFloat4x4(&constantVertex.reflectionView, XMLoadFloat4x4(&camera->GetReflectionViewMatrix(mesh->GetPosition().y)));
		XMStoreFloat2(&constantVertex.uvOffset,         XMLoadFloat2(&mesh->GetUvOffset()));

		// set the fraction of the reflection blending with the texture color
		constantAmbDirPixel.reflectiveFraction = mesh->GetReflectiveData().reflectiveFraction;

		// update vertex constant buffer
		SHADER_HELPERS::UpdateConstantBuffer((void*)&constantVertex,      sizeof(CBVertexPlanar),      _CBVertex);
		SHADER_HELPERS::UpdateConstantBuffer((void*)&constantAmbDirPixel, sizeof(CBAmbDirPixelPlanar), _CBPixelAmbDir);

		// get the texture array of mesh
		ID3D11ShaderResourceView** meshTextures = mesh->GetTextureArray();

		// fill texture array with all textures including the shadow map and reflection map
		ID3D11ShaderResourceView* t[6] = { meshTextures[0], meshTextures[1], meshTextures[2], meshTextures[3], shadowMap, reflectionMapShader->GetReflectionMap() };

		// set SRV's
		devCon->PSSetShaderResources(0, 6, t);

		mesh->UploadBuffers();

		devCon->DrawIndexed(mesh->GetNumIndices(), 0, 0);
	}
}
