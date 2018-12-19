#include "ReflectionMapShader.h"
#include "ShaderHelpers.h"
#include "DXManager.h"
#include "DXBlendStates.h"
#include "DXDepthStencilStates.h"
#include "CameraManager.h"
#include "LightManager.h"
#include "Mesh.h"
#include "Systems.h"
#include "SystemDefs.h"
#include "Entity.h"
#include "QuadComponent.h"
#include "ParticleShader.h"
#include "SkyDome.h"

ReflectionMapShader::ReflectionMapShader()
{
	// create shaders
	SHADER_HELPERS::CreateVertexShader(L"shaders/vertexReflectionMap.vs", _reflectionMapVertexShader, _reflectionMapVertexShaderByteCode);
	SHADER_HELPERS::CreatePixelShader(L"shaders/pixelReflectionMapOpaque.ps", _reflectionMapPixelShaderOpaque, _reflectionMapPixelShaderByteCodeOpaque);
	SHADER_HELPERS::CreatePixelShader(L"shaders/pixelReflectionMapAlpha.ps", _reflectionMapPixelShaderAlpha, _reflectionMapPixelShaderByteCodeAlpha);

	// create constant buffers
	SHADER_HELPERS::CreateConstantBuffer(_CBVertex);
	SHADER_HELPERS::CreateConstantBuffer(_CBPixelAmbDir);
	SHADER_HELPERS::CreateConstantBuffer(_CBPixelPoint);

	// create render texture
	_reflectionMap = new RenderToTexture(SCREEN_WIDTH, SCREEN_HEIGHT, false);
}

ReflectionMapShader::~ReflectionMapShader()
{
	_reflectionMapVertexShader->Release();
	_reflectionMapPixelShaderOpaque->Release();
	_reflectionMapPixelShaderAlpha->Release();

	_reflectionMapVertexShaderByteCode->Release();
	_reflectionMapPixelShaderByteCodeOpaque->Release();
	_reflectionMapPixelShaderByteCodeAlpha->Release();

	_CBVertex->Release();
}

void ReflectionMapShader::GenerateReflectionMap(std::vector<Mesh*>& reflectiveOpaqueMeshes,
	std::vector<Mesh*>& reflectiveAlphaMeshes,
	std::vector<ParticleSystemComponent*>& reflectiveParticles,
	ParticleShader*& particleShader,
	DXInputLayouts*& inputLayouts,
	Mesh*& reflectionMesh,
	XMFLOAT4 clipPlane)
{

	// get systems
	DXManager&     DXM = *Systems::dxManager;
	CameraManager& CM  = *Systems::cameraManager;
	LightManager&  LM  = *Systems::lightManager;

	// get device context
	ID3D11DeviceContext*& devCon = DXM.GetDeviceCon();

	// get game camera and shadow camera
	CameraComponent* camera      = CM.GetCurrentCameraGame();
	CameraComponent* cameraLight = CM.GetCurrentCameraDepthMap();

	// get direction light
	LightDirectionComponent*& directionalLight = LM.GetDirectionalLight();

	// get reflect data of this mesh
	Mesh::ReflectiveData reflectData = reflectionMesh->GetReflectiveData();

	// create constant buffer structures
	CBAmbDirPixel constantAmbDirPixel;
	CBVertex      constantVertex;

	// set ambient and directional light properties for pixel shader
	XMStoreFloat4(&constantAmbDirPixel.ambientColor,    XMLoadFloat4(&LM.GetAmbientColor()));
	XMStoreFloat4(&constantAmbDirPixel.dirDiffuseColor, XMLoadFloat4(&directionalLight->GetLightColor()));
	XMStoreFloat3(&constantAmbDirPixel.lightDir,        XMLoadFloat3(&directionalLight->GetLightDirectionInv()));

	// update pixel shader constant buffer fro point lights
	SHADER_HELPERS::UpdateConstantBuffer((void*)LM.GetCBPointBuffer(), sizeof(CBPoint) * LM.GetNumPointLights(), _CBPixelPoint);
	SHADER_HELPERS::UpdateConstantBuffer((void*)&constantAmbDirPixel, sizeof(CBAmbDirPixel), _CBPixelAmbDir);

	// set vertex data that all meshes share
	XMStoreFloat4(&constantVertex.clipingPlane, XMLoadFloat4(&clipPlane));
	XMStoreFloat4x4(&constantVertex.view,       XMLoadFloat4x4(&camera->GetReflectionViewMatrix(reflectionMesh->GetPosition().y)));
	XMStoreFloat4x4(&constantVertex.projection, XMLoadFloat4x4(&camera->GetProjectionMatrix()));

	// clear our reflection map render texture and set it to active
	_reflectionMap->ClearRenderTarget(0, 0, 0, 1, false);
	_reflectionMap->SetRendertarget(false, false);

	// render skybox first
	if (reflectData.reflectSkybox)
		Systems::renderer->GetSkybox()->Render(true);

	// set opaque blend state
	DXM.BlendStates()->SetBlendState(BLEND_STATE::BLEND_OPAQUE);

	// set shaders that will handle rednering opaque meshes to reflectionmap
	devCon->VSSetShader(_reflectionMapVertexShader, NULL, 0);
	devCon->PSSetShader(_reflectionMapPixelShaderOpaque, NULL, 0);

	// set constant buffer for the vertex and pixel shader
	devCon->VSSetConstantBuffers(0, 1, &_CBVertex);
	devCon->PSSetConstantBuffers(0, 1, &_CBPixelAmbDir);
	devCon->PSSetConstantBuffers(1, 1, &_CBPixelPoint);

	// loop over all opaque meshes that is set to cast reflections
	for (int y = 0; y < reflectiveOpaqueMeshes.size(); y++)
	{
		// get world matrix of mesh and update the buffer
		XMStoreFloat4x4(&constantVertex.world, XMLoadFloat4x4(&reflectiveOpaqueMeshes[y]->GetWorldMatrix()));
		SHADER_HELPERS::UpdateConstantBuffer((void*)&constantVertex, sizeof(CBVertex), _CBVertex);

		// get the texture array of mesh
		ID3D11ShaderResourceView** meshTextures = reflectiveOpaqueMeshes[y]->GetTextureArray();

		// specular map is not sent to this shader tho this is not calculated
		ID3D11ShaderResourceView* texArray[3] = { meshTextures[0], meshTextures[1], meshTextures[3] };
		devCon->PSSetShaderResources(0, 3, texArray);

		// upload and draw the mesh
		reflectiveOpaqueMeshes[y]->UploadBuffers();

		devCon->DrawIndexed(reflectiveOpaqueMeshes[y]->GetNumIndices(), 0, 0);
	}

	// set shader and blend state if we have any alpha meshes
	// that is being reflected, also sort these from the pos
	// of the camera reflection matrix
	if (reflectiveAlphaMeshes.size() > 0)
	{
		XMFLOAT3 cameraReflectPos = camera->GetComponent<TransformComponent>()->GetPositionRef();
		cameraReflectPos.y -= 2.0f * (cameraReflectPos.y - reflectionMesh->GetPosition().y);

		DXM.BlendStates()->SetBlendState(BLEND_STATE::BLEND_ALPHA);
		devCon->PSSetShader(_reflectionMapPixelShaderAlpha, NULL, 0);
		SHADER_HELPERS::MeshSort(reflectiveAlphaMeshes, cameraReflectPos, true);
	}

	// loop over all alpha meshes that is set to cast reflections
	for (int y = 0; y < reflectiveAlphaMeshes.size(); y++)
	{
		// get world matrix of mesh and update the buffer
		XMStoreFloat4x4(&constantVertex.world, XMLoadFloat4x4(&reflectiveAlphaMeshes[y]->GetWorldMatrix()));
		SHADER_HELPERS::UpdateConstantBuffer((void*)&constantVertex, sizeof(CBVertex), _CBVertex);

		// get the texture array of mesh
		ID3D11ShaderResourceView** meshTextures = reflectiveAlphaMeshes[y]->GetTextureArray();

		// specular map is not sent to this shader tho this is not calculated
		ID3D11ShaderResourceView* texArray[2] = { meshTextures[0], meshTextures[1] };
		devCon->PSSetShaderResources(0, 2, texArray);

		// upload and draw the mesh
		reflectiveAlphaMeshes[y]->UploadBuffers();

		devCon->DrawIndexed(reflectiveAlphaMeshes[y]->GetNumIndices(), 0, 0);
	}

	// render all particles to the texture if set to recive particle reflections
	if (reflectData.reflectParticles)
	{
		inputLayouts->SetInputLayout(INPUT_LAYOUT_TYPE::LAYOUTPARTICLE);
		particleShader->RenderParticles(reflectiveParticles, true, reflectionMesh->GetPosition().y);
		inputLayouts->SetInputLayout(INPUT_LAYOUT_TYPE::LAYOUT3D);
	}
}
