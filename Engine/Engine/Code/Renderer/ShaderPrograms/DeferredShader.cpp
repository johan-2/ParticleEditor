#include "DeferredShader.h"
#include "DXManager.h"
#include "CameraComponent.h"
#include "CameraManager.h"
#include "Mesh.h"
#include "ShaderHelpers.h"
#include "LightManager.h"
#include "DXBlendStates.h"
#include "DXDepthStencilStates.h"
#include "GBuffer.h"

DeferredShader::DeferredShader()
{
	//create deferred geometry shaders
	SHADER_HELPERS::CreateVertexShader(L"shaders/vertexDeferredGeometry.vs", _vertexGeometryShader, _vertexGeometryShaderByteCode);
	SHADER_HELPERS::CreatePixelShader(L"shaders/pixelDeferredGeometry.ps",   _pixelGeometryShader,  _pixelGeometryShaderByteCode);

	//create deferred lightning shaders
	SHADER_HELPERS::CreateVertexShader(L"shaders/vertexDeferredLightning.vs", _vertexLightShader, _vertexLightningShaderByteCode);
	SHADER_HELPERS::CreatePixelShader(L"shaders/pixelDeferredLightning.ps",   _pixelLightShader,    _pixelLightningShaderByteCode);

	// create constant buffers for the deferred passes
	SHADER_HELPERS::CreateConstantBuffer(_constantBufferGeometry);
	SHADER_HELPERS::CreateConstantBuffer(_constantBufferDefAmbient);
	SHADER_HELPERS::CreateConstantBuffer(_constantBufferDefDirectional);
	SHADER_HELPERS::CreateConstantBuffer(_constantBufferDefPoint);
}

DeferredShader::~DeferredShader()
{
	_vertexGeometryShaderByteCode->Release();
	_pixelGeometryShaderByteCode->Release();

	_vertexLightningShaderByteCode->Release();
	_pixelLightningShaderByteCode->Release();

	_vertexGeometryShader->Release();
	_pixelGeometryShader->Release();

	_vertexLightShader->Release();
	_pixelLightShader->Release();

	_constantBufferGeometry->Release();
	_constantBufferDefAmbient->Release();
	_constantBufferDefDirectional->Release();
	_constantBufferDefPoint->Release();
}

void DeferredShader::RenderGeometry(std::vector<Mesh*>& meshes)
{
	// get DXManager
	DXManager& DXM = DXManager::GetInstance();

	// get devicecontext
	ID3D11DeviceContext* devCon = DXM.GetDeviceCon();

	// get the game camera
	CameraComponent* camera = CameraManager::GetInstance().GetCurrentCameraGame();

	// render opaque objects here only	
	DXM.BlendStates()->SetBlendState(BLEND_STATE::BLEND_OPAQUE);

	// set shaders			
	devCon->VSSetShader(_vertexGeometryShader, NULL, 0);
	devCon->PSSetShader(_pixelGeometryShader,  NULL, 0);

	// set the vertex constant buffer
	devCon->VSSetConstantBuffers(0, 1, &_constantBufferGeometry);

	// get camera matrices
	const XMFLOAT4X4& viewMatrix       = camera->GetViewMatrix();
	const XMFLOAT4X4& projectionMatrix = camera->GetProjectionMatrix();
	
	// constantbuffer structure
	ConstantGeometryVertex vertexData;

	// stuff that need to be set per mesh
	unsigned int size = meshes.size();
	for (int i = 0; i < size; i++)
	{
		// upload vertex and indexbuffers
		meshes[i]->UploadBuffers();

		// get world matrix for the mesh
		const XMFLOAT4X4& worldMatrix = meshes[i]->GetWorldMatrix();

		//set and upload vertexconstantdata 
		vertexData.world      = worldMatrix;
		vertexData.view       = viewMatrix;
		vertexData.projection = projectionMatrix;
		vertexData.uvOffset   = meshes[i]->GetUvOffset();

		// update the constant buffer with the mesh data
		SHADER_HELPERS::UpdateConstantBuffer((void*)&vertexData, sizeof(ConstantGeometryVertex), _constantBufferGeometry);

		// set textures
		devCon->PSSetShaderResources(0, 3, meshes[i]->GetTextureArray());

		// draw
		devCon->DrawIndexed(meshes[i]->GetNumIndices(), 0, 0);
	}
}

void DeferredShader::RenderLightning(GBuffer*& gBuffer)
{
	// get DXManager
	DXManager& DXM = DXManager::GetInstance();

	// get devicecontext
	ID3D11DeviceContext* devCon = DXM.GetDeviceCon();

	// get cameras
	CameraComponent* camera      = CameraManager::GetInstance().GetCurrentCameraGame();
	CameraComponent* cameraLight = CameraManager::GetInstance().GetCurrentCameraDepthMap();

	// get point lights
	std::vector<LightPointComponent*>& pointLights = LightManager::GetInstance().GetPointLight();

	// set shaders			
	devCon->VSSetShader(_vertexLightShader, NULL, 0);
	devCon->PSSetShader(_pixelLightShader, NULL, 0);

	// render with mask so we dont preform lighting calculations on non geometry pixels
	DXM.DepthStencilStates()->SetDepthStencilState(DEPTH_STENCIL_STATE::MASKED_LIGHTNING);

	// constantbuffer structures
	ConstantDeferredAmbient     ambientLightData;
	ConstantDeferredDirectional directionalLightData;
	ConstantDeferredPoint       pointLightData[MAX_POINT_LIGHTS];

	// set the pointlight data
	const int size = pointLights.size();
	for (int i = 0; i < size; i++)
	{
		pointLightData[i].color          = pointLights[i]->GetLightColor();
		pointLightData[i].intensity      = pointLights[i]->GetIntensity();
		pointLightData[i].radius         = pointLights[i]->GetRadius();
		pointLightData[i].lightPosition  = pointLights[i]->GetComponent<TransformComponent>()->GetPositionRef();
		pointLightData[i].specularColor  = pointLights[i]->GetSpecularColor();
		pointLightData[i].specularPower  = pointLights[i]->GetSpecularPower();
		pointLightData[i].attConstant    = pointLights[i]->GetAttConstant();
		pointLightData[i].attLinear      = pointLights[i]->GetAttLinear();
		pointLightData[i].attExponential = pointLights[i]->GetAttExponential();
		pointLightData[i].numLights      = size;
	}

	// set ambientdata(camerapos is in this buffer aswell)
	ambientLightData.ambientColor = LightManager::GetInstance().GetAmbientColor();

	const XMFLOAT3& camPos = camera->GetComponent<TransformComponent>()->GetPositionRef();
	ambientLightData.cameraPosition = XMFLOAT4(camPos.x, camPos.y, camPos.z, 1.0f);

	// get directional light
	LightDirectionComponent*& directionalLight = LightManager::GetInstance().GetDirectionalLight();

	// get directionallight matrices for shadow calculations
	directionalLightData.lightView        =  cameraLight->GetViewMatrix();
	directionalLightData.lightProjection  = cameraLight->GetProjectionMatrix();

	// set the inversed light direction
	XMStoreFloat3(&directionalLightData.lightDirection, 
		XMVectorNegate(XMLoadFloat3(&directionalLight->GetLightDirection())));

	// set the light properties of directional light
	directionalLightData.lightColor         = directionalLight->GetLightColor();
	directionalLightData.lightSpecularColor = directionalLight->GetSpecularColor();
	directionalLightData.lightSpecularpower = directionalLight->GetSpecularPower();

	// update constantbuffers		
	SHADER_HELPERS::UpdateConstantBuffer((void*)&ambientLightData,     sizeof(ConstantDeferredAmbient),                    _constantBufferDefAmbient);
	SHADER_HELPERS::UpdateConstantBuffer((void*)&directionalLightData, sizeof(ConstantDeferredDirectional),                _constantBufferDefDirectional);
	SHADER_HELPERS::UpdateConstantBuffer((void*)&pointLightData,       sizeof(ConstantDeferredPoint) * pointLights.size(), _constantBufferDefPoint);

	// get Gbuffer textures
	ID3D11ShaderResourceView**& gBufferTextures = gBuffer->GetSrvArray();

	// get depth map texture for shadow calculations
	ID3D11ShaderResourceView* depthMap = cameraLight->GetSRV();

	// add all to one array
	ID3D11ShaderResourceView* textureArray[5] = { depthMap, gBufferTextures[0], gBufferTextures[1], gBufferTextures[2], gBufferTextures[3] };

	// set the textures
	devCon->PSSetShaderResources(0, 5, textureArray);

	// set all constant buffers
	devCon->PSSetConstantBuffers(0, 1, &_constantBufferDefAmbient);
	devCon->PSSetConstantBuffers(1, 1, &_constantBufferDefDirectional);
	devCon->PSSetConstantBuffers(2, 1, &_constantBufferDefPoint);

	// draw
	devCon->DrawIndexed(6, 0, 0);

	// unbind all textures
	ID3D11ShaderResourceView* nullTextureArray[5] = { nullptr, nullptr, nullptr, nullptr, nullptr };
	devCon->PSSetShaderResources(0, 5, nullTextureArray);

	// enable depth again
	DXM.DepthStencilStates()->SetDepthStencilState(DEPTH_STENCIL_STATE::ENABLED);
}