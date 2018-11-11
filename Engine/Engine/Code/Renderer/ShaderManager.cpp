#include "ShaderManager.h"
#include "CameraManager.h"
#include "DXManager.h"
#include "LightManager.h"
#include "Mesh.h"
#include "QuadComponent.h"
#include "ParticleSystemComponent.h"
#include <iostream>
#include "GuiManager.h"
#include "GBuffer.h"
#include <d3dcompiler.h>
#include "DXBlendStates.h"
#include "DXRasterizerStates.h"
#include "DXDepthStencilStates.h"


ShaderManager* ShaderManager::_instance = 0;

ShaderManager& ShaderManager::GetInstance()
{
	if (_instance == 0)
		_instance = new ShaderManager;

	return *_instance;
}

ShaderManager::ShaderManager()
{
}

ShaderManager::~ShaderManager()
{
}

void ShaderManager::Initialize() 
{
				
	//COMPILE AND CREATE SHADERS
	////create ambient shaders
	//CreateVertexShader(L"shaders/vertexAmbient.vs", _vertexAmbientShader, vertexShaderBufferAmbient);
	//CreatePixelShader(L"shaders/pixelAmbient.ps", _pixelAmbientShader, pixelShaderBufferAmbient);

	////create directional shaders
	//CreateVertexShader(L"shaders/vertexDirectional.vs",  _vertexDirectionalShader, vertexShaderBufferDirectional);
	//CreatePixelShader(L"shaders/pixelDirectional.ps",  _pixelDirectionalShader, pixelShaderBufferDirectional);

	////create Point shaders
	//CreateVertexShader(L"shaders/vertexPoint.vs", _vertexPointShader, vertexShaderBufferPoint);
	//CreatePixelShader(L"shaders/pixelPoint.ps", _pixelPointShader, pixelShaderBufferPoint);
	//
	////create sprite shaders
	//CreateVertexShader(L"shaders/vertexSprite.vs", _vertexSpriteShader, vertexShaderBufferSprite);
	//CreatePixelShader(L"shaders/pixelSprite.ps", _pixelSpriteShader, pixelShaderBufferSprite);	
	//
	////create depth shaders
	//

	////create skybox shaders
	//CreateVertexShader(L"shaders/vertexSkyBox.vs", _vertexSkyBoxShader, vertexShaderBufferSkyBox);
	//CreatePixelShader(L"shaders/pixelSkyBox.ps", _pixelSkyBoxShader, pixelShaderBufferSkyBox);

	////create directional shadows shaders
	//CreateVertexShader(L"shaders/vertexDirectionalShadows.vs", _vertexDirectionalShadowsShader, vertexShaderBufferDirectionalShadows);
	//CreatePixelShader(L"shaders/pixelDirectionalShadows.ps", _pixelDirectionalShadowsShader, pixelShaderBufferDirectionalShadows);

	////create particle shaders
	//CreateVertexShader(L"shaders/vertexParticle.vs", _vertexParticleShader, vertexShaderBufferParticle);
	//CreatePixelShader(L"shaders/ pixelParticle.ps", _pixelParticleShader, pixelShaderBufferParticle);

	////create gui shaders
	//CreateVertexShader(L"shaders/vertexImGui.vs", _vertexGUIShader, vertexShaderBufferGUI);
	//CreatePixelShader(L"shaders/pixelImGui.ps", _pixelGUIShader, pixelShaderBufferGUI);

	

	
	
	

	
			
	
}














//void ShaderManager::RenderDirectionalAlpha(Mesh*& mesh)
//{
//	//// get devicecontext
//	DXManager& DXM = DXManager::GetInstance();
//	ID3D11DeviceContext* devCon = DXM.GetDeviceCon();
//
//	// create constantbuffer structures
//	ConstantDirectionalVertex vertexData;
//	ConstantDirectionalPixel pixelData;
//
//	CameraComponent* camera = CameraManager::GetInstance().GetCurrentCameraGame();
//
//	// get directional lights
//	LightDirectionComponent*& directionalLight = LightManager::GetInstance().GetDirectionalLight();
//
//	// get camera position
//	XMFLOAT3 cameraPos = camera->GetComponent<TransformComponent>()->GetPositionRef();
//
//	// render with additive blend state
//	DXM.BlendStates()->SetBlendState(BLEND_STATE::BLEND_ALPHA);
//
//	// set shaders	
//	devCon->VSSetShader(_vertexDirectionalShader, NULL, 0);
//	devCon->PSSetShader(_pixelDirectionalShader, NULL, 0);
//
//	// get and transpose the camera matrices
//	XMFLOAT4X4 viewMatrix = camera->GetViewMatrix();
//	XMFLOAT4X4 projectionMatrix = camera->GetProjectionMatrix();
//
//	XMStoreFloat4x4(&viewMatrix, XMMatrixTranspose(XMLoadFloat4x4(&viewMatrix)));
//	XMStoreFloat4x4(&projectionMatrix, XMMatrixTranspose(XMLoadFloat4x4(&projectionMatrix)));
//
//	// set the light data
//	pixelData.diffuseColor = directionalLight->GetLightColor();
//	XMStoreFloat3(&pixelData.lightDir, XMVectorNegate(XMLoadFloat3(&directionalLight->GetLightDirection())));
//	pixelData.specularColor = directionalLight->GetSpecularColor();
//	pixelData.specularPower = directionalLight->GetSpecularPower();
//
//	// upload new light data to pixelshader		
//	UpdateConstantBuffer((void*)&pixelData, sizeof(ConstantDirectionalPixel), _constantBufferPixel);
//
//	// get and transpose the world matrix for the mesh
//	XMFLOAT4X4 worldMatrix = mesh->GetWorldMatrix();
//	XMStoreFloat4x4(&worldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&worldMatrix)));
//
//	// set vertex constants data and upload to gpu
//	vertexData.world = worldMatrix;
//	vertexData.view = viewMatrix;
//	vertexData.projection = projectionMatrix;
//	vertexData.camPos = cameraPos;
//	vertexData.uvOffset = mesh->GetUvOffset();
//	UpdateConstantBuffer((void*)&vertexData, sizeof(ConstantDirectionalVertex), _constantBufferVertex);
//
//	// set textures
//	devCon->PSSetShaderResources(0, 3, mesh->GetTextureArray());
//
//	//upload vertex and index buffers for this mesh
//	mesh->UploadBuffers();
//
//	// draw the mesh additivly for each light
//	devCon->DrawIndexed(mesh->GetNumIndices(), 0, 0);
//
//}
//
//void ShaderManager::RenderDirectionalShadowsAlpha(Mesh*& mesh)
//{
//	// get devicecontext
//	DXManager& DXM = DXManager::GetInstance();
//	ID3D11DeviceContext* devCon = DXM.GetDeviceCon();
//
//	// create constantbuffer structures
//	ConstantDirectionalShadowVertex vertexData;
//	ConstantDirectionalShadowPixel pixelData;
//
//	CameraComponent* camera = CameraManager::GetInstance().GetCurrentCameraGame();
//	CameraComponent* cameraLight = CameraManager::GetInstance().GetCurrentCameraDepthMap();
//
//	// get directional lights
//	LightDirectionComponent*& directionalLight = LightManager::GetInstance().GetDirectionalLight();
//
//	// get camera position
//	XMFLOAT3 cameraPos = camera->GetComponent<TransformComponent>()->GetPositionRef();
//
//	// render with additive blend state
//	DXM.BlendStates()->SetBlendState(BLEND_STATE::BLEND_ADDITIVE);
//
//	// set shaders	
//	devCon->VSSetShader(_vertexDirectionalShadowsShader, NULL, 0);
//	devCon->PSSetShader(_pixelDirectionalShadowsShader, NULL, 0);
//
//	// get and transpose the camera matrices
//	XMFLOAT4X4 viewMatrix = camera->GetViewMatrix();
//	XMFLOAT4X4 projectionMatrix = camera->GetProjectionMatrix();
//
//	XMStoreFloat4x4(&viewMatrix, XMMatrixTranspose(XMLoadFloat4x4(&viewMatrix)));
//	XMStoreFloat4x4(&projectionMatrix, XMMatrixTranspose(XMLoadFloat4x4(&projectionMatrix)));
//
//	XMFLOAT4X4 viewMatrixLight = cameraLight->GetViewMatrix();
//	XMFLOAT4X4 projectionMatrixLight = cameraLight->GetProjectionMatrix();
//
//	XMStoreFloat4x4(&viewMatrixLight, XMMatrixTranspose(XMLoadFloat4x4(&viewMatrixLight)));
//	XMStoreFloat4x4(&projectionMatrixLight, XMMatrixTranspose(XMLoadFloat4x4(&projectionMatrixLight)));
//
//	// set the light data
//	pixelData.diffuseColor = directionalLight->GetLightColor();
//	XMStoreFloat3(&pixelData.lightDir, XMVectorNegate(XMLoadFloat3(&directionalLight->GetLightDirection())));
//	pixelData.specularColor = directionalLight->GetSpecularColor();
//	pixelData.specularPower = directionalLight->GetSpecularPower();
//
//	// upload new light data to pixelshader		
//	UpdateConstantBuffer((void*)&pixelData, sizeof(ConstantDirectionalShadowPixel), _constantBufferPixel);
//
//	ID3D11ShaderResourceView* shadowMap = cameraLight->GetSRV();
//
//	// get and transpose the world matrix for the mesh
//	XMFLOAT4X4 worldMatrix = mesh->GetWorldMatrix();
//	XMStoreFloat4x4(&worldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&worldMatrix)));
//
//	// set vertex constants data and upload to gpu
//	vertexData.world = worldMatrix;
//	vertexData.view = viewMatrix;
//	vertexData.projection = projectionMatrix;
//	vertexData.lightView = viewMatrixLight;
//	vertexData.lightProjection = projectionMatrixLight;
//	vertexData.camPos = cameraPos;
//	vertexData.uvOffset = mesh->GetUvOffset();
//	UpdateConstantBuffer((void*)&vertexData, sizeof(ConstantDirectionalShadowVertex), _constantBufferVertex);
//
//	ID3D11ShaderResourceView** meshTextures = mesh->GetTextureArray();
//	ID3D11ShaderResourceView* t[4] = { meshTextures[0], meshTextures[1], meshTextures[2], shadowMap };
//
//	// set textures
//	devCon->PSSetShaderResources(0, 4, t);
//
//	//upload vertex and index buffers for this mesh
//	mesh->UploadBuffers();
//
//	// draw the mesh additivly for the light
//	devCon->DrawIndexed(mesh->GetNumIndices(), 0, 0);
//
//}
//
//void ShaderManager::RenderPointAlpha(Mesh*& mesh)
//{
//	//// get devicecontext
//	DXManager& DXM = DXManager::GetInstance();
//	ID3D11DeviceContext* devCon = DXM.GetDeviceCon();
//
//	CameraComponent* camera = CameraManager::GetInstance().GetCurrentCameraGame();
//
//	// get point lights
//	std::vector<LightPointComponent*>& pointLights = LightManager::GetInstance().GetPointLight();
//	const int size = pointLights.size();
//
//	ConstantPointVertex vertexData;
//	ConstantPointPixel pixelData[MAX_POINT_LIGHTS];
//
//	// get camera position
//	XMFLOAT3 cameraPos = camera->GetComponent<TransformComponent>()->GetPositionRef();
//
//	// render with additive blend state
//	DXM.BlendStates()->SetBlendState(BLEND_STATE::BLEND_ADDITIVE);
//
//	// set shaders	
//	devCon->VSSetShader(_vertexPointShader, NULL, 0);
//	devCon->PSSetShader(_pixelPointShader, NULL, 0);
//
//	// get and transpose the camera matrices
//	XMFLOAT4X4 viewMatrix = camera->GetViewMatrix();
//	XMFLOAT4X4 projectionMatrix = camera->GetProjectionMatrix();
//
//	XMStoreFloat4x4(&viewMatrix, XMMatrixTranspose(XMLoadFloat4x4(&viewMatrix)));
//	XMStoreFloat4x4(&projectionMatrix, XMMatrixTranspose(XMLoadFloat4x4(&projectionMatrix)));
//
//	// set the light data
//	for (int i = 0; i< pointLights.size(); i++)
//	{
//		pixelData[i].color          = pointLights[i]->GetLightColor();
//		pixelData[i].intensity      = pointLights[i]->GetIntensity();
//		pixelData[i].radius         = pointLights[i]->GetRadius();
//		pixelData[i].lightPosition  = pointLights[i]->GetComponent<TransformComponent>()->GetPositionRef();
//		pixelData[i].specularColor  = pointLights[i]->GetSpecularColor();
//		pixelData[i].specularPower  = pointLights[i]->GetSpecularPower();
//		pixelData[i].attConstant    = pointLights[i]->GetAttConstant();
//		pixelData[i].attLinear      = pointLights[i]->GetAttLinear();
//		pixelData[i].attExponential = pointLights[i]->GetAttExponential();
//		pixelData[i].numLights      = pointLights.size();
//	}
//
//	// upload new light data to pixelshader		
//	UpdateConstantBuffer((void*)&pixelData, sizeof(ConstantPointPixel) * pointLights.size(), _constantBufferPixel);
//
//	// get and transpose the world matrix for the mesh
//	XMFLOAT4X4 worldMatrix = mesh->GetWorldMatrix();
//	XMStoreFloat4x4(&worldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&worldMatrix)));
//
//	// set vertex constants data and upload to gpu
//	vertexData.world = worldMatrix;
//	vertexData.view = viewMatrix;
//	vertexData.projection = projectionMatrix;
//	vertexData.camPos = cameraPos;
//	vertexData.uvOffset = mesh->GetUvOffset();
//	UpdateConstantBuffer((void*)&vertexData, sizeof(ConstantPointVertex), _constantBufferVertex);
//
//	// set textures
//	devCon->PSSetShaderResources(0, 3, mesh->GetTextureArray());
//
//	//upload vertex and index buffers for this mesh
//	mesh->UploadBuffers();
//
//	devCon->DrawIndexed(mesh->GetNumIndices(), 0, 0);
//}
//
//void ShaderManager::RenderAmbientAlpha(Mesh*& mesh)
//{
//	// get devicecontext
//	DXManager& DXM = DXManager::GetInstance();
//	ID3D11DeviceContext* devCon = DXM.GetDeviceCon();
//
//	// constantbuffer structures
//	ConstantAmbientVertex vertexData;
//	ConstantAmbientPixel pixelData;
//
//	CameraComponent* camera = CameraManager::GetInstance().GetCurrentCameraGame();
//
//	//render with alpha blending	
//	DXM.BlendStates()->SetBlendState(BLEND_STATE::BLEND_ALPHA);
//
//	// set shaders			
//	devCon->VSSetShader(_vertexAmbientShader, NULL, 0);
//	devCon->PSSetShader(_pixelAmbientShader, NULL, 0);
//
//	// get ambient light
//	const XMFLOAT4& ambientColor = LightManager::GetInstance().GetAmbientColor();
//
//	// get and transpose camera matrices
//	XMFLOAT4X4 viewMatrix = camera->GetViewMatrix();
//	XMFLOAT4X4 projectionMatrix = camera->GetProjectionMatrix();
//
//	XMStoreFloat4x4(&viewMatrix, XMMatrixTranspose(XMLoadFloat4x4(&viewMatrix)));
//	XMStoreFloat4x4(&projectionMatrix, XMMatrixTranspose(XMLoadFloat4x4(&projectionMatrix)));
//
//	// set and update pixelconstants, only light color for now
//	pixelData.color = ambientColor;
//	UpdateConstantBuffer((void*)&pixelData, sizeof(ConstantAmbientPixel), _constantBufferPixel);
//
//	// upload vertex and indexbuffers
//	mesh->UploadBuffers();
//
//	// get the world matrix and transpose it
//	// get and transpose the world matrix for the mesh
//	XMFLOAT4X4 worldMatrix = mesh->GetWorldMatrix();
//	XMStoreFloat4x4(&worldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&worldMatrix)));
//
//	//set and upload vertexconstantdata 
//	vertexData.world = worldMatrix;
//	vertexData.view = viewMatrix;
//	vertexData.projection = projectionMatrix;
//	vertexData.uvOffset = mesh->GetUvOffset();
//	UpdateConstantBuffer((void*)&vertexData, sizeof(ConstantAmbientVertex), _constantBufferVertex);
//
//	// set textures
//	devCon->PSSetShaderResources(0, 1, mesh->GetTextureArray());
//
//	// draw
//	devCon->DrawIndexed(mesh->GetNumIndices(), 0, 0);
//}

