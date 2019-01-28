#include "WaterShader.h"
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
#include "SimpleClipSceneShader.h"
#include "MathHelpers.h"
#include "Renderer.h"
#include "DebugQuadHandler.h"

WaterShader::WaterShader()
{
	//create shaders
	SHADER_HELPERS::CreateVertexShader(L"shaders/vertexForwardWater.vs", _waterVertexShader, _waterVertexShaderByteCode);
	SHADER_HELPERS::CreatePixelShader(L"shaders/pixelForwardWater.ps",   _waterPixelShader, _waterPixelShaderByteCode);

	// create constant buffers
	SHADER_HELPERS::CreateConstantBuffer(_CBVertex);

	_simpleClipShaderReflection = new SimpleClipSceneShader();
	_simpleClipShaderRefraction = new SimpleClipSceneShader();
}

WaterShader::~WaterShader()
{
	_waterVertexShader->Release();
	_waterPixelShader->Release();

	_waterVertexShaderByteCode->Release();
	_waterPixelShaderByteCode->Release();

	_CBVertex->Release();

	delete _simpleClipShaderReflection;
	delete _simpleClipShaderRefraction;
}

void WaterShader::ShowDebugQuads()
{
	DebugQuadHandler* quadHandler = Systems::renderer->debugQuadHandler; 
	quadHandler->AddDebugQuad(_simpleClipShaderReflection->GetRenderSRV());
	quadHandler->AddDebugQuad(_simpleClipShaderRefraction->GetRenderSRV());
}

void WaterShader::Render(std::vector<Mesh*>& waterMeshes)
{
	if (waterMeshes.size() == 0)
		return;

	// get systems
	DXManager&     DXM = *Systems::dxManager;
	CameraManager& CM  = *Systems::cameraManager;
	LightManager&  LM  = *Systems::lightManager;
	Renderer& renderer = *Systems::renderer;

	// get device context
	ID3D11DeviceContext*& devCon = DXM.GetDeviceCon();

	// get game camera and shadow camera
	CameraComponent*& camera      = CM.currentCameraGame;
	CameraComponent*& cameraLight = CM.currentCameraDepthMap;

	TransformComponent* camTrans = camera->GetComponent<TransformComponent>();

	// get camera position
	XMFLOAT3 cameraPos = camTrans->GetPositionVal();
	XMFLOAT3 cameraRot = camTrans->GetRotationVal();

	// create constant buffer structures
	CBVertexWater constantVertex;
	XMStoreFloat3(&constantVertex.camPos, XMLoadFloat3(&cameraPos));

	// loop over all meshes that will project reflections onto itself
	size_t numMeshes = waterMeshes.size();
	for (int i = 0; i < numMeshes; i++)
	{
		// get the mesh that will project the reflections
		Mesh* mesh = waterMeshes[i];

		// get the clip plane of the water mesh so we dont render anything above this plane
		XMFLOAT4 clipPlaneRefract(0, -1, 0, mesh->GetPosition().y);

		// render the refractionmap
		_simpleClipShaderRefraction->RenderScene(renderer.GetMeshes(SHADER_TYPE::S_REFRACT_OPAQUE), renderer.GetMeshes(SHADER_TYPE::S_REFRACT_ALPHA), renderer.GetInstancedModels(INSTANCED_SHADER_TYPE::S_INSTANCED_REFRACT), clipPlaneRefract, true, false);

		// get the clip plane of the water mesh so we dont render anything below this plane
		XMFLOAT4 clipPlaneReflect(0, 1, 0, -mesh->GetPosition().y);

		// change the camera y position to render reflection
		XMFLOAT3 reflectPos = cameraPos;
		reflectPos.y -= (cameraPos.y - mesh->GetPosition().y) * 2.0f;

		// set reflection position/rotation and build matrices
		camTrans->SetRotation(XMFLOAT3(-cameraRot.x, cameraRot.y, cameraRot.z));
		camTrans->SetPosition(reflectPos);
		camTrans->UpdateWorldMatrix();
		camera->CalculateViewMatrix();

		// save the reflection viewprojmatrix
		XMFLOAT4X4 reflectMat = camera->viewProjMatrix;

		// render the reflectionmap
		_simpleClipShaderReflection->RenderScene(renderer.GetMeshes(SHADER_TYPE::S_CAST_REFLECTION_OPAQUE), renderer.GetMeshes(SHADER_TYPE::S_CAST_REFLECTION_ALPHA), renderer.GetInstancedModels(INSTANCED_SHADER_TYPE::S_INSTANCED_CAST_REFLECTION), clipPlaneReflect, true);

		// change back to original camera position/rotation
		camTrans->SetRotation(cameraRot);
		camTrans->SetPosition(cameraPos);
		camTrans->UpdateWorldMatrix();
		camera->CalculateViewMatrix();

		// set back to defualt render target and render the reflection mesh
		Systems::renderer->SetMainRenderTarget();

		// set shaders
		devCon->VSSetShader(_waterVertexShader, NULL, 0);
		devCon->PSSetShader(_waterPixelShader, NULL, 0);

		ID3D11Buffer* pointBuffer = LM.GetPointLightCB();
		ID3D11Buffer* ambDirBuffer = LM.GetAmbDirLightCB();

		// set the vertex constant buffer, the pixel ones is already set
		devCon->VSSetConstantBuffers(0, 1, &_CBVertex);
		devCon->PSSetConstantBuffers(0, 1, &ambDirBuffer);
		devCon->PSSetConstantBuffers(1, 1, &pointBuffer);

		// set to alpha blending
		DXM.BlendStates()->SetBlendState(BLEND_STATE::BLEND_ALPHA);

		// set vertex constant values
		const XMFLOAT4X4& worldMat = mesh->GetWorldMatrix();

		XMStoreFloat4x4(&constantVertex.world,                XMLoadFloat4x4(&mesh->GetWorldMatrixTrans()));
		XMStoreFloat4x4(&constantVertex.worldViewProj,        XMLoadFloat4x4(&MATH_HELPERS::MatrixMutiplyTrans(&worldMat, &camera->viewProjMatrix)));
		XMStoreFloat4x4(&constantVertex.worldViewProjLight,   XMLoadFloat4x4(&MATH_HELPERS::MatrixMutiplyTrans(&worldMat, &cameraLight->viewProjMatrix)));
		XMStoreFloat4x4(&constantVertex.worldViewProjReflect, XMLoadFloat4x4(&MATH_HELPERS::MatrixMutiplyTrans(&worldMat, &reflectMat)));
		XMStoreFloat2(&constantVertex.uvOffset,               XMLoadFloat2(&mesh->GetUvOffset()));

		// update vertex constant buffer
		SHADER_HELPERS::UpdateConstantBuffer((void*)&constantVertex, sizeof(CBVertexWater), _CBVertex);

		// get the texture array of mesh
		ID3D11ShaderResourceView** meshTextures = mesh->GetTextureArray();

		// fill texture array with all textures including the shadow map and reflection map
		ID3D11ShaderResourceView* t[9] = { meshTextures[1], meshTextures[2], mesh->GetDUDVMap(), cameraLight->renderTexture, _simpleClipShaderReflection->GetRenderSRV(), _simpleClipShaderRefraction->GetRenderSRV(), _simpleClipShaderRefraction->GetDepthSRV(), mesh->GetFoamMap(), mesh->GetNoiseMap() };

		// set SRV's
		devCon->PSSetShaderResources(0, 9, t);

		mesh->UploadBuffers();

		devCon->DrawIndexed(mesh->GetNumIndices(), 0, 0);

		// unbind so we can use resources as input in next stages
		ID3D11ShaderResourceView* nullSRV[8] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
		devCon->PSSetShaderResources(0, 8, nullSRV);
	}
}
