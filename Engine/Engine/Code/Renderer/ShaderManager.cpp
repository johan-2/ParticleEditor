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












void ShaderManager::RenderGUI(ImDrawData* draw_data)
{
	//ID3D11DeviceContext* devCon = DXManager::GetInstance().GetDeviceCon();
	//ID3D11Device* device = DXManager::GetInstance().GetDevice();

	//ID3D11Buffer*& vertexBuffer = GuiManager::GetInstance().GetVertexBuffer();
	//ID3D11Buffer*& indexBuffer = GuiManager::GetInstance().GetIndexBuffer();

	//HRESULT result;

	//DXManager& DXM = DXManager::GetInstance();
	//DXM.BlendStates()->SetBlendState(BLEND_STATE::BLEND_ALPHA);
	//DXM.DepthStencilStates()->SetDepthStencilState(DEPTH_STENCIL_STATE::DISABLED);

	//// get current size of vertex and index buffers
	//D3D11_BUFFER_DESC currentSizevertex;
	//D3D11_BUFFER_DESC currentSizeIndex;
	//vertexBuffer->GetDesc(&currentSizevertex);
	//indexBuffer->GetDesc(&currentSizeIndex);

	//// increase size of vertexbuffer if needed
	//if(currentSizevertex.ByteWidth < draw_data->TotalVtxCount * sizeof(ImDrawVert))
	//{
	//	vertexBuffer->Release(); 
	//	vertexBuffer = nullptr; 
	//			
	//	D3D11_BUFFER_DESC desc;
	//	memset(&desc, 0, sizeof(D3D11_BUFFER_DESC));
	//	desc.Usage = D3D11_USAGE_DYNAMIC;
	//	desc.ByteWidth = currentSizevertex.ByteWidth + (5000 * sizeof(ImDrawVert));
	//	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	//	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	//	desc.MiscFlags = 0;
	//	result = device->CreateBuffer(&desc, NULL, &vertexBuffer);
	//	if (FAILED(result))
	//		printf("failed to update GUI vertexbuffersize\n");	
	//}
	//
	//// increase size of indexbuffer if needed
	//if (currentSizeIndex.ByteWidth < draw_data->TotalIdxCount * sizeof(ImDrawIdx))
	//{
	//	indexBuffer->Release();
	//	indexBuffer = nullptr;

	//	D3D11_BUFFER_DESC desc;
	//	memset(&desc, 0, sizeof(D3D11_BUFFER_DESC));
	//	desc.Usage = D3D11_USAGE_DYNAMIC;
	//	desc.ByteWidth = currentSizeIndex.ByteWidth + (10000 * sizeof(ImDrawVert));
	//	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	//	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	//	desc.MiscFlags = 0;
	//	result = device->CreateBuffer(&desc, NULL, &indexBuffer);
	//	if (FAILED(result))
	//		printf("failed to update GUI indexbuffersize\n");
	//}

	//// map vertex and indexbuffer
	//D3D11_MAPPED_SUBRESOURCE vtx_resource, idx_resource;
	//result = devCon->Map(vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &vtx_resource);
	//if (FAILED(result))
	//	printf("failed to map gui vertexbuffer\n");

	//result = devCon->Map(indexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &idx_resource);
	//if (FAILED(result))
	//	printf("failed to map gui indexbuffer\n");

	//// loop over all individual buffers and upload the data to one buffer
	//ImDrawVert* vtx_dst = (ImDrawVert*)vtx_resource.pData;
	//ImDrawIdx*  idx_dst = (ImDrawIdx*)idx_resource.pData;
	//for (int i = 0; i < draw_data->CmdListsCount; i++)
	//{
	//	const ImDrawList* cmd_list = draw_data->CmdLists[i];
	//	memcpy(vtx_dst, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
	//	memcpy(idx_dst, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
	//	vtx_dst += cmd_list->VtxBuffer.Size;
	//	idx_dst += cmd_list->IdxBuffer.Size;
	//}
	//devCon->Unmap(vertexBuffer, 0);
	//devCon->Unmap(indexBuffer, 0);

	//// create worldViewProjection
	//float L = 0.0f;
	//float R = ImGui::GetIO().DisplaySize.x;
	//float B = ImGui::GetIO().DisplaySize.y;
	//float T = 0.0f;
	//float wvp[4][4] =
	//{
	//	{ 2.0f / (R - L),    0.0f,              0.0f,       0.0f },
	//	{ 0.0f,              2.0f / (T - B),    0.0f,       0.0f },
	//	{ 0.0f,              0.0f,              0.5f,       0.0f },
	//	{ (R + L) / (L - R), (T + B) / (B - T), 0.5f,       1.0f },
	//};
	//
	//// update the vertexconstantbuffer
	//ConstantGUIVertex constantVertex;	
	//memcpy(&constantVertex.projection, wvp, sizeof(wvp));
	//UpdateConstantBuffer(&constantVertex, sizeof(ConstantGUIVertex), _constantBufferVertex);

	//// Bind shader and vertex buffers
	//unsigned int stride = sizeof(ImDrawVert);
	//unsigned int offset = 0;
	//
	//devCon->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	//devCon->IASetIndexBuffer(indexBuffer, sizeof(ImDrawIdx) == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT, 0);	
	//devCon->VSSetShader(_vertexGUIShader, NULL, 0);	
	//devCon->PSSetShader(_pixelGUIShader, NULL, 0);
	//
	//// Render command lists
	//int vtx_offset = 0;
	//int idx_offset = 0;
	//for (int n = 0; n < draw_data->CmdListsCount; n++)
	//{
	//	const ImDrawList* cmd_list = draw_data->CmdLists[n];
	//	for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
	//	{
	//		const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
	//		if (pcmd->UserCallback)
	//		{
	//			pcmd->UserCallback(cmd_list, pcmd);
	//		}
	//		else
	//		{
	//			const D3D11_RECT r = { (LONG)pcmd->ClipRect.x, (LONG)pcmd->ClipRect.y, (LONG)pcmd->ClipRect.z, (LONG)pcmd->ClipRect.w };
	//			devCon->PSSetShaderResources(0, 1, (ID3D11ShaderResourceView**)&pcmd->TextureId);
	//			devCon->RSSetScissorRects(1, &r);
	//			devCon->DrawIndexed(pcmd->ElemCount, idx_offset, vtx_offset);
	//		}
	//		idx_offset += pcmd->ElemCount;
	//	}
	//	vtx_offset += cmd_list->VtxBuffer.Size;
	//}	

	//DXM.DepthStencilStates()->SetDepthStencilState(DEPTH_STENCIL_STATE::ENABLED);
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

