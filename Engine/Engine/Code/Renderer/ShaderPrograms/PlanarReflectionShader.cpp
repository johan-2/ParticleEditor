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
#include "SimpleClipSceneShader.h"
#include "MathHelpers.h"
#include "Renderer.h"
#include "DebugQuadHandler.h"

PlanarReflectionShader::PlanarReflectionShader()
{
	//create shaders
	SHADER_HELPERS::CreateVertexShader(L"shaders/vertexForwardPlanar.vs", _planarVertexShader, _planarVertexShaderByteCode);
	SHADER_HELPERS::CreatePixelShader(L"shaders/pixelForwardPlanar.ps",   _planarPixelShader, _planarPixelShaderByteCode);

	// create constant buffers
	SHADER_HELPERS::CreateConstantBuffer(_CBVertex);
	SHADER_HELPERS::CreateConstantBuffer(_CBReflect);

	_simpleClipShaderReflection = new SimpleClipSceneShader();
}

PlanarReflectionShader::~PlanarReflectionShader()
{
	_planarVertexShader->Release();
	_planarPixelShader->Release();

	_planarVertexShaderByteCode->Release();
	_planarPixelShaderByteCode->Release();

	_CBReflect->Release();
	_CBVertex->Release();

	delete _simpleClipShaderReflection;
}

void PlanarReflectionShader::ShowDebugQuads()
{
	Systems::renderer->GetDebugQuadHandler()->AddDebugQuad(_simpleClipShaderReflection->GetRenderSRV());
}

void PlanarReflectionShader::Render(std::vector<Mesh*>& reflectionMeshes)
{
	if (reflectionMeshes.size() == 0)
		return;

	// get systems
	DXManager&     DXM = *Systems::dxManager;
	CameraManager& CM  = *Systems::cameraManager;
	LightManager&  LM  = *Systems::lightManager;
	Renderer& renderer = *Systems::renderer;

	// get device context
	ID3D11DeviceContext*& devCon = DXM.GetDeviceCon();

	// get game camera and shadow camera
	CameraComponent*& camera      = CM.GetCurrentCameraGame();
	CameraComponent*& cameraLight = CM.GetCurrentCameraDepthMap();

	TransformComponent* camTrans = camera->GetComponent<TransformComponent>();

	// get camera position
	XMFLOAT3 cameraPos = camTrans->GetPositionVal();
	XMFLOAT3 cameraRot = camTrans->GetRotationVal();

	// create constant buffer structures
	CBVertex  cbVertex;
	CBReflect cbReflect;

	XMStoreFloat3(&cbVertex.camPos, XMLoadFloat3(&cameraPos));

	// get shadow map
	ID3D11ShaderResourceView* shadowMap = cameraLight->GetSRV();	

	// loop over all meshes that will project reflections onto itself
	size_t numMeshes = reflectionMeshes.size();
	for (int i = 0; i < numMeshes; i++)
	{
		// get the mesh that will project the reflections
		Mesh* mesh = reflectionMeshes[i];

		// get the clip plane of the reflection mesh so we dont render anything below this plane
		XMFLOAT4 clipPlane(0, 1, 0, -mesh->GetPosition().y);

		// change the camera y position to render reflection
		XMFLOAT3 reflectPos = cameraPos;
		reflectPos.y -= (cameraPos.y - mesh->GetPosition().y) * 2.0f;

		// set reflection position/rotation and build matrices
		camTrans->SetRotation(XMFLOAT3(-cameraRot.x, cameraRot.y, cameraRot.z));
		camTrans->SetPosition(reflectPos);
		camTrans->UpdateWorldMatrix();
		camera->CalculateViewMatrix();

		// save the reflection viewprojmatrix
		XMFLOAT4X4 reflectMat = camera->GetViewProjMatrix();

		// render the reflectionmap
		_simpleClipShaderReflection->RenderScene(renderer.GetMeshes(SHADER_TYPE::S_CAST_REFLECTION_OPAQUE), renderer.GetMeshes(SHADER_TYPE::S_CAST_REFLECTION_ALPHA), renderer.GetInstancedModels(INSTANCED_SHADER_TYPE::S_INSTANCED_CAST_REFLECTION), clipPlane, true);

		// change back to original camera position/rotation
		camTrans->SetRotation(cameraRot);
		camTrans->SetPosition(cameraPos);
		camTrans->UpdateWorldMatrix();
		camera->CalculateViewMatrix();

		// set back to defualt render target and render the reflection mesh
		Systems::renderer->SetMainRenderTarget();

		// set shaders
		devCon->VSSetShader(_planarVertexShader, NULL, 0);
		devCon->PSSetShader(_planarPixelShader, NULL, 0);

		ID3D11Buffer* pointBuffer  = LM.GetPointLightCB();
		ID3D11Buffer* ambDirBuffer = LM.GetAmbDirLightCB();

		// set the vertex constant buffer, the pixel ones is already set
		devCon->VSSetConstantBuffers(0, 1, &_CBVertex);
		devCon->PSSetConstantBuffers(0, 1, &ambDirBuffer);
		devCon->PSSetConstantBuffers(1, 1, &pointBuffer);
		devCon->PSSetConstantBuffers(2, 1, &_CBReflect);

		// set to alpha blending
		DXM.BlendStates()->SetBlendState(BLEND_STATE::BLEND_ALPHA);

		// set vertex constant values
		const XMFLOAT4X4& worldMat = mesh->GetWorldMatrix();

		XMStoreFloat4x4(&cbVertex.world,                XMLoadFloat4x4(&mesh->GetWorldMatrixTrans()));
		XMStoreFloat4x4(&cbVertex.worldViewProj,        XMLoadFloat4x4(&MATH_HELPERS::MatrixMutiplyTrans(&worldMat, &camera->GetViewProjMatrix())));
		XMStoreFloat4x4(&cbVertex.worldViewProjLight,   XMLoadFloat4x4(&MATH_HELPERS::MatrixMutiplyTrans(&worldMat, &cameraLight->GetViewProjMatrix())));
		XMStoreFloat4x4(&cbVertex.worldViewProjReflect, XMLoadFloat4x4(&MATH_HELPERS::MatrixMutiplyTrans(&worldMat, &reflectMat)));
		XMStoreFloat2(&cbVertex.uvOffset,               XMLoadFloat2(&mesh->GetUvOffset()));

		// set the fraction of the reflection blending with the texture color
		cbReflect.reflectiveFraction = mesh->GetReflectiveData().reflectiveFraction;

		// update vertex constant buffer
		SHADER_HELPERS::UpdateConstantBuffer((void*)&cbVertex,  sizeof(CBVertex),  _CBVertex);
		SHADER_HELPERS::UpdateConstantBuffer((void*)&cbReflect, sizeof(CBReflect), _CBReflect);

		// get the texture array of mesh
		ID3D11ShaderResourceView** meshTextures = mesh->GetTextureArray();

		// fill texture array with all textures including the shadow map and reflection map
		ID3D11ShaderResourceView* t[6] = { meshTextures[0], meshTextures[1], meshTextures[2], meshTextures[3], shadowMap, _simpleClipShaderReflection->GetRenderSRV() };

		// set SRV's
		devCon->PSSetShaderResources(0, 6, t);

		mesh->UploadBuffers();

		devCon->DrawIndexed(mesh->GetNumIndices(), 0, 0);
	}

	// unbind so we can use resources as input in next stages
	ID3D11ShaderResourceView* nullSRV[6] = { NULL, NULL, NULL, NULL, NULL, NULL };
	devCon->PSSetShaderResources(0, 6, nullSRV);
}
