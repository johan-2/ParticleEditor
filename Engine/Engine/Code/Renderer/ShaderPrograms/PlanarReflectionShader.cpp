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

PlanarReflectionShader::PlanarReflectionShader()
{
	//create shaders
	SHADER_HELPERS::CreateVertexShader(L"shaders/vertexForwardPlanar.vs", _planarVertexShader, _planarVertexShaderByteCode);
	SHADER_HELPERS::CreatePixelShader(L"shaders/pixelForwardPlanar.ps",   _planarPixelShader, _planarPixelShaderByteCode);

	// create constant buffers
	SHADER_HELPERS::CreateConstantBuffer(_CBVertex);
	SHADER_HELPERS::CreateConstantBuffer(_CBPixelAmbDir);
	SHADER_HELPERS::CreateConstantBuffer(_CBPixelPoint);

	_simpleClipShaderReflection = new SimpleClipSceneShader();
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

	delete _simpleClipShaderReflection;
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

	// get directional light
	LightDirectionComponent*& directionalLight = LM.GetDirectionalLight();

	// create constant buffer structures
	CBVertexPlanar      constantVertex;
	CBAmbDirPixelPlanar constantAmbDirPixel;

	// set ambient and directional light properties for pixel shader
	XMStoreFloat4(&constantAmbDirPixel.ambientColor,    XMLoadFloat4(&LM.GetAmbientColor()));
	XMStoreFloat4(&constantAmbDirPixel.dirDiffuseColor, XMLoadFloat4(&directionalLight->GetLightColor()));
	XMStoreFloat3(&constantAmbDirPixel.lightDir,        XMLoadFloat3(&directionalLight->GetLightDirectionInv()));

	XMStoreFloat3(&constantVertex.camPos, XMLoadFloat3(&cameraPos));

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
		_simpleClipShaderReflection->RenderScene(renderer.GetMeshes(SHADER_TYPE::S_CAST_REFLECTION_OPAQUE), renderer.GetMeshes(SHADER_TYPE::S_CAST_REFLECTION_ALPHA), clipPlane, true);

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

		// set the vertex constant buffer, the pixel ones is already set
		devCon->VSSetConstantBuffers(0, 1, &_CBVertex);
		devCon->PSSetConstantBuffers(0, 1, &_CBPixelAmbDir);
		devCon->PSSetConstantBuffers(1, 1, &_CBPixelPoint);

		// set to alpha blending
		DXM.BlendStates()->SetBlendState(BLEND_STATE::BLEND_ALPHA);

		// set vertex constant values
		const XMFLOAT4X4& worldMat = mesh->GetWorldMatrix();

		XMStoreFloat4x4(&constantVertex.world,                XMLoadFloat4x4(&mesh->GetWorldMatrixTrans()));
		XMStoreFloat4x4(&constantVertex.worldViewProj,        XMLoadFloat4x4(&MATH_HELPERS::MatrixMutiplyTrans(&worldMat, &camera->GetViewProjMatrix())));
		XMStoreFloat4x4(&constantVertex.worldViewProjLight,   XMLoadFloat4x4(&MATH_HELPERS::MatrixMutiplyTrans(&worldMat, &cameraLight->GetViewProjMatrix())));
		XMStoreFloat4x4(&constantVertex.worldViewProjReflect, XMLoadFloat4x4(&MATH_HELPERS::MatrixMutiplyTrans(&worldMat, &reflectMat)));
		XMStoreFloat2(&constantVertex.uvOffset,               XMLoadFloat2(&mesh->GetUvOffset()));

		// set the fraction of the reflection blending with the texture color
		constantAmbDirPixel.reflectiveFraction = mesh->GetReflectiveData().reflectiveFraction;

		// update vertex constant buffer
		SHADER_HELPERS::UpdateConstantBuffer((void*)&constantVertex,      sizeof(CBVertexPlanar),      _CBVertex);
		SHADER_HELPERS::UpdateConstantBuffer((void*)&constantAmbDirPixel, sizeof(CBAmbDirPixelPlanar), _CBPixelAmbDir);

		// get the texture array of mesh
		ID3D11ShaderResourceView** meshTextures = mesh->GetTextureArray();

		// fill texture array with all textures including the shadow map and reflection map
		ID3D11ShaderResourceView* t[6] = { meshTextures[0], meshTextures[1], meshTextures[2], meshTextures[3], shadowMap, _simpleClipShaderReflection->GetSRV() };

		// set SRV's
		devCon->PSSetShaderResources(0, 6, t);

		mesh->UploadBuffers();

		devCon->DrawIndexed(mesh->GetNumIndices(), 0, 0);
	}
}
