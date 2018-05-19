#include "ShaderManager.h"
#include "CameraManager.h"
#include "DXManager.h"
#include "LightManager.h"
#include "Mesh.h"
#include "QuadComponent.h"
#include "ParticleEmitterComponent.h"
#include <iostream>
#include "GuiManager.h"
#include "GBuffer.h"
#include <d3dcompiler.h>


ShaderManager* ShaderManager::_instance = 0;

ShaderManager& ShaderManager::GetInstance()
{
	if (_instance == 0)
		_instance = new ShaderManager;

	return *_instance;

}

ShaderManager::ShaderManager()
{
	_vertexDirectionalShader = nullptr;
	_pixelDirectionalShader = nullptr;
	_inputlayout3D = nullptr;	
	// initialize all missing things
}

ShaderManager::~ShaderManager()
{
}

void ShaderManager::Initialize()
{	
	CreateShaders();		
}

void ShaderManager::Shutdown() 
{
	if (_inputlayout3D)
	{
		_inputlayout3D->Release();
		_inputlayout3D = 0;
	}
	if (_inputlayout2D)
	{
		_inputlayout2D->Release();
		_inputlayout2D = 0;
	}
	if (_pixelDirectionalShader)
	{
		_pixelDirectionalShader->Release();
		_pixelDirectionalShader = 0;
	}
	if (_vertexDirectionalShader)
	{
		_vertexDirectionalShader->Release();
		_vertexDirectionalShader = 0;
	}
	if (_pixelAmbientShader)
	{
		_pixelAmbientShader->Release();
		_pixelAmbientShader = 0;
	}
	if (_vertexAmbientShader)
	{
		_vertexAmbientShader->Release();
		_vertexAmbientShader = 0;
	}
	if (_pixelSpriteShader)
	{
		_pixelSpriteShader->Release();
		_pixelSpriteShader = 0;
	}
	if (_vertexSpriteShader)
	{
		_vertexSpriteShader->Release();
		_vertexSpriteShader = 0;
	}
}

void ShaderManager::CreateShaders() 
{
	HRESULT result;

	ID3D11Device* device = DXManager::GetInstance().GetDevice();

	// the blob objects will load in the data from the shader files, contains functions to get the size and data of buffer
	ID3D10Blob* vertexShaderBufferDirectional = nullptr; 
	ID3D10Blob* pixelShaderBufferDirectional = nullptr;
	ID3D10Blob* vertexShaderBufferPoint = nullptr;
	ID3D10Blob* pixelShaderBufferPoint = nullptr;
	ID3D10Blob* vertexShaderBufferAmbient = nullptr;
	ID3D10Blob* pixelShaderBufferAmbient = nullptr;
	ID3D10Blob* vertexShaderBufferSprite = nullptr;
	ID3D10Blob* pixelShaderBufferSprite = nullptr;
	ID3D10Blob* vertexShaderBufferDepth = nullptr;
	ID3D10Blob* pixelShaderBufferDepth = nullptr;
	ID3D10Blob* vertexShaderBufferSkyBox = nullptr;
	ID3D10Blob* pixelShaderBufferSkyBox = nullptr;
	ID3D10Blob* vertexShaderBufferDirectionalShadows = nullptr;
	ID3D10Blob* pixelShaderBufferDirectionalShadows = nullptr;
	ID3D10Blob* vertexShaderBufferParticle = nullptr;
	ID3D10Blob* pixelShaderBufferParticle = nullptr;
	ID3D10Blob* vertexShaderBufferGUI = nullptr;
	ID3D10Blob* pixelShaderBufferGUI = nullptr;
	ID3D10Blob* vertexShaderBufferGeometry = nullptr;
	ID3D10Blob* pixelShaderBufferGeometry = nullptr;
	ID3D10Blob* vertexShaderBufferLight = nullptr;
	ID3D10Blob* pixelShaderBufferLight = nullptr;
				
	//COMPILE AND CREATE SHADERS
	////create ambient shaders
	CreateVertexShader(L"shaders/vertexAmbient.vs", &_vertexAmbientShader, &vertexShaderBufferAmbient);
	CreatePixelShader(L"shaders/pixelAmbient.ps", &_pixelAmbientShader, &pixelShaderBufferAmbient);

	//create directional shaders
	CreateVertexShader(L"shaders/vertexDirectional.vs",  &_vertexDirectionalShader, &vertexShaderBufferDirectional);
	CreatePixelShader(L"shaders/pixelDirectional.ps",  &_pixelDirectionalShader, &pixelShaderBufferDirectional);

	//create Point shaders
	CreateVertexShader(L"shaders/vertexPoint.vs", &_vertexPointShader, &vertexShaderBufferPoint);
	CreatePixelShader(L"shaders/pixelPoint.ps", &_pixelPointShader, &pixelShaderBufferPoint);
	
	//create sprite shaders
	CreateVertexShader(L"shaders/vertexSprite.vs", &_vertexSpriteShader, &vertexShaderBufferSprite);
	CreatePixelShader(L"shaders/pixelSprite.ps", &_pixelSpriteShader, &pixelShaderBufferSprite);	
	
	//create depth shaders
	CreateVertexShader(L"shaders/vertexDepth.vs", &_vertexDepthShader, &vertexShaderBufferDepth);
	CreatePixelShader(L"shaders/pixelDepth.ps", &_pixelDepthShader, &pixelShaderBufferDepth);

	//create skybox shaders
	CreateVertexShader(L"shaders/vertexSkyBox.vs", &_vertexSkyBoxShader, &vertexShaderBufferSkyBox);
	CreatePixelShader(L"shaders/pixelSkyBox.ps", &_pixelSkyBoxShader, &pixelShaderBufferSkyBox);

	//create directional shadows shaders
	CreateVertexShader(L"shaders/vertexDirectionalShadows.vs", &_vertexDirectionalShadowsShader, &vertexShaderBufferDirectionalShadows);
	CreatePixelShader(L"shaders/pixelDirectionalShadows.ps", &_pixelDirectionalShadowsShader, &pixelShaderBufferDirectionalShadows);

	//create particle shaders
	CreateVertexShader(L"shaders/vertexParticle.vs", &_vertexParticleShader, &vertexShaderBufferParticle);
	CreatePixelShader(L"shaders/pixelParticle.ps", &_pixelParticleShader, &pixelShaderBufferParticle);

	//create gui shaders
	CreateVertexShader(L"shaders/vertexImGui.vs", &_vertexGUIShader, &vertexShaderBufferGUI);
	CreatePixelShader(L"shaders/pixelImGui.ps", &_pixelGUIShader, &pixelShaderBufferGUI);

	//create geometry shaders
	CreateVertexShader(L"shaders/vertexGeometry.vs", &_vertexGeometryShader, &vertexShaderBufferGeometry);
	CreatePixelShader(L"shaders/pixelGeometry.ps", &_pixelGeometryShader, &pixelShaderBufferGeometry);

	//create geometry shaders
	CreateVertexShader(L"shaders/vertexLightning.vs", &_vertexLightShader, &vertexShaderBufferLight);
	CreatePixelShader(L"shaders/pixelLightning.ps", &_pixelLightShader, &pixelShaderBufferLight);
	
	// create input layouts
	D3D11_INPUT_ELEMENT_DESC inputLayout3D[5]
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },		
	};

	D3D11_INPUT_ELEMENT_DESC inputLayout2D[2]
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	
	D3D11_INPUT_ELEMENT_DESC inputLayoutParticle[8]
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0,                            0, D3D11_INPUT_PER_VERTEX_DATA,   0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,   0 },
		{ "TEXCOORD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1,                            0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "TEXCOORD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "TEXCOORD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "TEXCOORD", 4, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "COLOR"   , 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "TEXCOORD", 5, DXGI_FORMAT_R32G32_FLOAT,       1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
	};

	D3D11_INPUT_ELEMENT_DESC inputLayoutGui[3] 
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT,   0, (size_t)(&((ImDrawVert*)0)->pos), D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,   0, (size_t)(&((ImDrawVert*)0)->uv),  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, (size_t)(&((ImDrawVert*)0)->col), D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
			

	// create the inputLayout from the description and the vertexshaderbuffer
	result = device->CreateInputLayout(inputLayout3D, sizeof(inputLayout3D) / sizeof(inputLayout3D[0]),  vertexShaderBufferDirectional->GetBufferPointer(), vertexShaderBufferDirectional->GetBufferSize(), &_inputlayout3D);
	if (FAILED(result)) 
		printf("failed to create Inputlayout\n");

	result = device->CreateInputLayout(inputLayout2D, sizeof(inputLayout2D) / sizeof(inputLayout2D[0]), vertexShaderBufferSprite->GetBufferPointer(), vertexShaderBufferSprite->GetBufferSize(), &_inputlayout2D);
	if (FAILED(result)) 
		printf("failed to create Inputlayout2d\n");

	result = device->CreateInputLayout(inputLayoutParticle, sizeof(inputLayoutParticle) / sizeof(inputLayoutParticle[0]), vertexShaderBufferParticle->GetBufferPointer(), vertexShaderBufferParticle->GetBufferSize(), &_inputlayoutParticle);
	if (FAILED(result))
		printf("failed to create InputlayoutParticle\n");

	result = device->CreateInputLayout(inputLayoutGui, 3, vertexShaderBufferGUI->GetBufferPointer(), vertexShaderBufferGUI->GetBufferSize(), &_inputlayoutGUI);
	if (FAILED(result))
		printf("failed to create InputlayoutGUI\n");
		
	//create 2 dynamic constantbuffers, one for vertexshader and one for pixelshader
	D3D11_BUFFER_DESC constantBufferdesc;
	constantBufferdesc.Usage = D3D11_USAGE_DYNAMIC;
	constantBufferdesc.ByteWidth = 16;
	constantBufferdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBufferdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constantBufferdesc.MiscFlags = 0;
	constantBufferdesc.StructureByteStride = 0;

	result = device->CreateBuffer(&constantBufferdesc, NULL, &_constantBufferVertex);
	if (FAILED(result))
		printf("failed to create vertex constantbuffer\n");

	result = device->CreateBuffer(&constantBufferdesc, NULL, &_constantBufferPixel);
	if (FAILED(result))
		printf("Failed to create constantpixelbuffer\n");

	result = device->CreateBuffer(&constantBufferdesc, NULL, &_constantBufferDefAmbient);
	if (FAILED(result))
		printf("Failed to create constantpixelbuffer\n");

	result = device->CreateBuffer(&constantBufferdesc, NULL, &_constantBufferDefDirectional);
	if (FAILED(result))
		printf("Failed to create constantpixelbuffer\n");

	result = device->CreateBuffer(&constantBufferdesc, NULL, &_constantBufferDefPoint);
	if (FAILED(result))
		printf("Failed to create constantpixelbuffer\n");

	// release all blobs, all data is now in shader objects
	vertexShaderBufferDirectional->Release();
	pixelShaderBufferDirectional->Release();
	vertexShaderBufferAmbient->Release();
	pixelShaderBufferAmbient->Release();
	vertexShaderBufferSprite->Release();
	pixelShaderBufferSprite->Release();
	vertexShaderBufferDepth->Release();
	pixelShaderBufferDepth->Release();
	vertexShaderBufferSkyBox->Release();
	pixelShaderBufferSkyBox->Release();
	vertexShaderBufferDirectionalShadows->Release();
	pixelShaderBufferDirectionalShadows->Release();
	vertexShaderBufferParticle->Release();
	pixelShaderBufferParticle->Release();
	vertexShaderBufferGUI->Release();
	pixelShaderBufferGUI->Release();
	vertexShaderBufferPoint->Release();
	pixelShaderBufferPoint->Release();
}

void ShaderManager::SetConstantBuffers() 
{
	ID3D11DeviceContext* devCon = DXManager::GetInstance().GetDeviceCon();
	devCon->VSSetConstantBuffers(0, 1, &_constantBufferVertex);
	devCon->PSSetConstantBuffers(0, 1, &_constantBufferPixel);
}

void ShaderManager::SetInputLayout(INPUT_LAYOUT_TYPE type) 
{
	ID3D11DeviceContext* devCon = DXManager::GetInstance().GetDeviceCon();
	switch (type)
	{
	case LAYOUT3D:
		devCon->IASetInputLayout(_inputlayout3D);
		break;
	case LAYOUT2D:
		devCon->IASetInputLayout(_inputlayout2D);
		break;
	case LAYOUTPARTICLE:
		devCon->IASetInputLayout(_inputlayoutParticle);
		break;
	default:
		break;
	}
}

void ShaderManager::RenderGeometry(const std::vector<Mesh*>& meshes)
{
	// get devicecontext
	DXManager& DXM = DXManager::GetInstance();
	ID3D11DeviceContext* devCon = DXM.GetDeviceCon();

	// constantbuffer structures
	ConstantGeometryVertex vertexData;

	CameraComponent* camera = CameraManager::GetInstance().GetCurrentCameraGame();

	//render with alpha blending	
	DXM.SetBlendState(BLEND_STATE::BLEND_OPAQUE);

	// set shaders			
	devCon->VSSetShader(_vertexGeometryShader, NULL, 0);
	devCon->PSSetShader(_pixelGeometryShader, NULL, 0);

	// get and transpose camera matrices
	XMFLOAT4X4 viewMatrix = camera->GetViewMatrix();
	XMFLOAT4X4 projectionMatrix = camera->GetProjectionMatrix();

	XMStoreFloat4x4(&viewMatrix, XMMatrixTranspose(XMLoadFloat4x4(&viewMatrix)));
	XMStoreFloat4x4(&projectionMatrix, XMMatrixTranspose(XMLoadFloat4x4(&projectionMatrix)));

	// stuff that need to be set per mesh
	for (int i = 0; i < meshes.size(); i++)
	{
		// upload vertex and indexbuffers
		meshes[i]->UploadBuffers();

		// get and transpose the world matrix for the mesh
		XMFLOAT4X4 worldMatrix = meshes[i]->GetWorldMatrix();
		XMStoreFloat4x4(&worldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&worldMatrix)));

		//set and upload vertexconstantdata 
		vertexData.world = worldMatrix;
		vertexData.view = viewMatrix;
		vertexData.projection = projectionMatrix;
		vertexData.uvOffset = meshes[i]->GetUvOffset();
		UpdateConstantBuffer((void*)&vertexData, sizeof(ConstantGeometryVertex), _constantBufferVertex);

		// set textures
		devCon->PSSetShaderResources(0, 3, meshes[i]->GetTextureArray());

		// draw
		devCon->DrawIndexed(meshes[i]->GetNumIndices(), 0, 0);
	}
}

void ShaderManager::RenderLights(GBuffer*& gBuffer)
{
	// get devicecontext
	DXManager& DXM = DXManager::GetInstance();
	ID3D11DeviceContext* devCon = DXM.GetDeviceCon();

	// render with depth read/write Off
	DXM.SetZBuffer(DEPTH_STATE::DISABLED);

	// get camera 
	CameraComponent* camera = CameraManager::GetInstance().GetCurrentCameraGame();
	CameraComponent* cameraLight = CameraManager::GetInstance().GetCurrentCameraDepthMap();

	// set shaders			
	devCon->VSSetShader(_vertexLightShader, NULL, 0);
	devCon->PSSetShader(_pixelLightShader, NULL, 0);

	// get point lights
	std::vector<LightPointComponent*>& pointLights = LightManager::GetInstance().GetPointLight();
	const int size = pointLights.size();
	
	// constantbuffer structures
	ConstantDeferredAmbient ambientLightData;
	ConstantDeferredDirectional directionalLightData;
	ConstantDeferredPoint pointLightData[MAX_POINT_LIGHTS];
	
	// set the pointlight data
	for (int i = 0; i< pointLights.size(); i++)
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
		pointLightData[i].numLights      = pointLights.size();
	}

	// set ambientdata(camerapos is in this buffer aswell)
	ambientLightData.ambientColor = LightManager::GetInstance().GetAmbientColor();
	XMFLOAT3 camPos = camera->GetComponent<TransformComponent>()->GetPositionVal();
	ambientLightData.cameraPosition = XMFLOAT4(camPos.x, camPos.y, camPos.z, 1.0f);

	// get directional light
	LightDirectionComponent*& directionalLight = LightManager::GetInstance().GetDirectionalLight();

	// get directionallight matrices for shadow calculations
	XMFLOAT4X4 viewMatrixLight       = cameraLight->GetViewMatrix();
	XMFLOAT4X4 projectionMatrixLight = cameraLight->GetProjectionMatrix();

	// set the light data
	XMStoreFloat4x4(&directionalLightData.lightViewProj, XMMatrixMultiplyTranspose(XMLoadFloat4x4(&viewMatrixLight), XMLoadFloat4x4(&projectionMatrixLight)));
	XMStoreFloat3(&directionalLightData.lightDirection, XMVectorNegate(XMLoadFloat3(&directionalLight->GetLightDirection())));
	directionalLightData.lightColor         = directionalLight->GetLightColor();
	directionalLightData.lightSpecularColor = directionalLight->GetSpecularColor();
	directionalLightData.lightSpecularpower = directionalLight->GetSpecularPower();

	// update constantbuffers		
	UpdateConstantBuffer((void*)&ambientLightData,     sizeof(ConstantDeferredAmbient),                    _constantBufferDefAmbient);
	UpdateConstantBuffer((void*)&directionalLightData, sizeof(ConstantDeferredDirectional),                _constantBufferDefDirectional);
	UpdateConstantBuffer((void*)&pointLightData,       sizeof(ConstantDeferredPoint) * pointLights.size(), _constantBufferDefPoint);					

	// set textures
	ID3D11ShaderResourceView**& gBufferTextures = gBuffer->GetSrvArray();
	ID3D11ShaderResourceView* depthMap = cameraLight->GetRSV();
	ID3D11ShaderResourceView* textureArray[5] = { depthMap, gBufferTextures[0], gBufferTextures[1], gBufferTextures[2], gBufferTextures[3] };	
	devCon->PSSetShaderResources(0, 5, textureArray);

	// set constantbuffers
	/*ID3D11Buffer* ba[3] = { _constantBufferDefAmbient, _constantBufferDefDirectional, _constantBufferDefPoint };
	devCon->PSSetConstantBuffers(0, 3, ba);*/

	devCon->PSSetConstantBuffers(0, 1, &_constantBufferDefAmbient);
	devCon->PSSetConstantBuffers(1, 1, &_constantBufferDefDirectional);
	devCon->PSSetConstantBuffers(2, 1, &_constantBufferDefPoint);

	// draw
	devCon->DrawIndexed(6, 0, 0);

	ID3D11ShaderResourceView* nullTextureArray[5] = { nullptr, nullptr, nullptr, nullptr, nullptr };
	devCon->PSSetShaderResources(0, 5, nullTextureArray);
	
	DXM.SetZBuffer(DEPTH_STATE::ENABLED);

}

void ShaderManager::RenderDirectional(const std::vector<Mesh*>& meshes)
{			
	//// get devicecontext
	DXManager& DXM = DXManager::GetInstance();
	ID3D11DeviceContext* devCon = DXM.GetDeviceCon();

	// create constantbuffer structures
	ConstantDirectionalVertex vertexData;
	ConstantDirectionalPixel pixelData;

	CameraComponent* camera = CameraManager::GetInstance().GetCurrentCameraGame();
	
	// get directional lights
	LightDirectionComponent*& directionalLight = LightManager::GetInstance().GetDirectionalLight();
	
	// get camera position
	XMFLOAT3 cameraPos = camera->GetComponent<TransformComponent>()->GetPositionRef();

	// render with additive blend state
	DXM.SetBlendState(BLEND_STATE::BLEND_ADDITIVE);

	// set shaders	
	devCon->VSSetShader(_vertexDirectionalShader, NULL, 0);
	devCon->PSSetShader(_pixelDirectionalShader, NULL, 0);
	
	// get and transpose the camera matrices
	XMFLOAT4X4 viewMatrix = camera->GetViewMatrix();
	XMFLOAT4X4 projectionMatrix = camera->GetProjectionMatrix();

	XMStoreFloat4x4(&viewMatrix, XMMatrixTranspose(XMLoadFloat4x4(&viewMatrix)));
	XMStoreFloat4x4(&projectionMatrix, XMMatrixTranspose(XMLoadFloat4x4(&projectionMatrix)));
	
	// set the light data
	pixelData.diffuseColor = directionalLight->GetLightColor();
	XMStoreFloat3(&pixelData.lightDir, XMVectorNegate(XMLoadFloat3(&directionalLight->GetLightDirection())));
	pixelData.specularColor = directionalLight->GetSpecularColor();
	pixelData.specularPower = directionalLight->GetSpecularPower();

	// upload new light data to pixelshader		
	UpdateConstantBuffer((void*)&pixelData, sizeof(ConstantDirectionalPixel), _constantBufferPixel);
	
	for(int i =0; i< meshes.size(); i++)
	{		
		// get and transpose the world matrix for the mesh
		XMFLOAT4X4 worldMatrix = meshes[i]->GetWorldMatrix();
		XMStoreFloat4x4(&worldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&worldMatrix)));
		
		// set vertex constants data and upload to gpu
		vertexData.world = worldMatrix;
		vertexData.view = viewMatrix;
		vertexData.projection = projectionMatrix;
		vertexData.camPos = cameraPos;
		vertexData.uvOffset = meshes[i]->GetUvOffset();
		UpdateConstantBuffer((void*)&vertexData, sizeof(ConstantDirectionalVertex), _constantBufferVertex);
		
		// set textures
		devCon->PSSetShaderResources(0, 3, meshes[i]->GetTextureArray());

		//upload vertex and index buffers for this mesh
		meshes[i]->UploadBuffers();
				
		// draw the mesh additivly for each light
		devCon->DrawIndexed(meshes[i]->GetNumIndices(), 0, 0);
		
	}						
}

void ShaderManager::RenderDirectionalShadows(const std::vector<Mesh*>& meshes) 
{
	// get devicecontext
	DXManager& DXM = DXManager::GetInstance();
	ID3D11DeviceContext* devCon = DXM.GetDeviceCon();

	// create constantbuffer structures
	ConstantDirectionalShadowVertex vertexData;
	ConstantDirectionalShadowPixel pixelData;

	CameraComponent* camera = CameraManager::GetInstance().GetCurrentCameraGame();
	CameraComponent* cameraLight = CameraManager::GetInstance().GetCurrentCameraDepthMap();

	// get directional lights
	LightDirectionComponent*& directionalLight = LightManager::GetInstance().GetDirectionalLight();

	// get camera position
	XMFLOAT3 cameraPos = camera->GetComponent<TransformComponent>()->GetPositionRef();

	// render with additive blend state
	DXM.SetBlendState(BLEND_STATE::BLEND_ADDITIVE);

	// set shaders	
	devCon->VSSetShader(_vertexDirectionalShadowsShader, NULL, 0);
	devCon->PSSetShader(_pixelDirectionalShadowsShader, NULL, 0);

	// get and transpose the camera matrices
	XMFLOAT4X4 viewMatrix = camera->GetViewMatrix();
	XMFLOAT4X4 projectionMatrix = camera->GetProjectionMatrix();

	XMStoreFloat4x4(&viewMatrix, XMMatrixTranspose(XMLoadFloat4x4(&viewMatrix)));
	XMStoreFloat4x4(&projectionMatrix, XMMatrixTranspose(XMLoadFloat4x4(&projectionMatrix)));

	XMFLOAT4X4 viewMatrixLight = cameraLight->GetViewMatrix();
	XMFLOAT4X4 projectionMatrixLight = cameraLight->GetProjectionMatrix();

	XMStoreFloat4x4(&viewMatrixLight, XMMatrixTranspose(XMLoadFloat4x4(&viewMatrixLight)));
	XMStoreFloat4x4(&projectionMatrixLight, XMMatrixTranspose(XMLoadFloat4x4(&projectionMatrixLight)));

	// set the light data
	pixelData.diffuseColor = directionalLight->GetLightColor();
	XMStoreFloat3(&pixelData.lightDir, XMVectorNegate(XMLoadFloat3(&directionalLight->GetLightDirection())));
	pixelData.specularColor = directionalLight->GetSpecularColor();
	pixelData.specularPower = directionalLight->GetSpecularPower();

	// upload new light data to pixelshader		
	UpdateConstantBuffer((void*)&pixelData, sizeof(ConstantDirectionalShadowPixel), _constantBufferPixel);

	ID3D11ShaderResourceView* shadowMap = cameraLight->GetRSV();
	
	for (int i = 0; i< meshes.size(); i++)
	{
		// get and transpose the world matrix for the mesh
		XMFLOAT4X4 worldMatrix = meshes[i]->GetWorldMatrix();
		XMStoreFloat4x4(&worldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&worldMatrix)));

		// set vertex constants data and upload to gpu
		vertexData.world = worldMatrix;
		vertexData.view = viewMatrix;
		vertexData.projection = projectionMatrix;
		vertexData.lightView = viewMatrixLight;
		vertexData.lightProjection = projectionMatrixLight;
		vertexData.camPos = cameraPos;
		vertexData.uvOffset = meshes[i]->GetUvOffset();
		UpdateConstantBuffer((void*)&vertexData, sizeof(ConstantDirectionalShadowVertex), _constantBufferVertex);
		
		ID3D11ShaderResourceView** meshTextures = meshes[i]->GetTextureArray();
		ID3D11ShaderResourceView* t[4] = { meshTextures[0], meshTextures[1], meshTextures[2], shadowMap };

		// set textures
		devCon->PSSetShaderResources(0, 4, t);

		//upload vertex and index buffers for this mesh
		meshes[i]->UploadBuffers();
					
		// draw the mesh additivly for the light
		devCon->DrawIndexed(meshes[i]->GetNumIndices(), 0, 0);		
	}

}

void ShaderManager::RenderPoint(const std::vector<Mesh*>& meshes)
{
	//// get devicecontext
	DXManager& DXM = DXManager::GetInstance();
	ID3D11DeviceContext* devCon = DXM.GetDeviceCon();
	
	CameraComponent* camera = CameraManager::GetInstance().GetCurrentCameraGame();

	// get point lights
	std::vector<LightPointComponent*>& pointLights = LightManager::GetInstance().GetPointLight();
	const int size = pointLights.size();
	
	ConstantPointVertex vertexData;
	ConstantPointPixel pixelData[MAX_POINT_LIGHTS];

	// get camera position
	XMFLOAT3 cameraPos = camera->GetComponent<TransformComponent>()->GetPositionRef();

	// render with additive blend state
	DXM.SetBlendState(BLEND_STATE::BLEND_ADDITIVE);

	// set shaders	
	devCon->VSSetShader(_vertexPointShader, NULL, 0);
	devCon->PSSetShader(_pixelPointShader, NULL, 0);

	// get and transpose the camera matrices
	XMFLOAT4X4 viewMatrix = camera->GetViewMatrix();
	XMFLOAT4X4 projectionMatrix = camera->GetProjectionMatrix();

	XMStoreFloat4x4(&viewMatrix, XMMatrixTranspose(XMLoadFloat4x4(&viewMatrix)));
	XMStoreFloat4x4(&projectionMatrix, XMMatrixTranspose(XMLoadFloat4x4(&projectionMatrix)));


	// set the light data
	for(int i =0; i< pointLights.size(); i++)
	{
		pixelData[i].color          = pointLights[i]->GetLightColor();
		pixelData[i].intensity      = pointLights[i]->GetIntensity();
		pixelData[i].radius         = pointLights[i]->GetRadius();
		pixelData[i].lightPosition  = pointLights[i]->GetComponent<TransformComponent>()->GetPositionRef();
		pixelData[i].specularColor  = pointLights[i]->GetSpecularColor();
		pixelData[i].specularPower  = pointLights[i]->GetSpecularPower();
		pixelData[i].attConstant    = pointLights[i]->GetAttConstant();
		pixelData[i].attLinear      = pointLights[i]->GetAttLinear();
		pixelData[i].attExponential = pointLights[i]->GetAttExponential();
		pixelData[i].numLights		= pointLights.size();
	}

	// upload new light data to pixelshader		
	UpdateConstantBuffer((void*)&pixelData, sizeof(ConstantPointPixel) * pointLights.size(), _constantBufferPixel);

	for (int y = 0; y< meshes.size(); y++)
	{
		// get and transpose the world matrix for the mesh
		XMFLOAT4X4 worldMatrix = meshes[y]->GetWorldMatrix();
		XMStoreFloat4x4(&worldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&worldMatrix)));

		// set vertex constants data and upload to gpu
		vertexData.world = worldMatrix;
		vertexData.view = viewMatrix;
		vertexData.projection = projectionMatrix;
		vertexData.camPos = cameraPos;
		vertexData.uvOffset = meshes[y]->GetUvOffset();
		UpdateConstantBuffer((void*)&vertexData, sizeof(ConstantPointVertex), _constantBufferVertex);

		// set textures
		devCon->PSSetShaderResources(0, 3, meshes[y]->GetTextureArray());

		//upload vertex and index buffers for this mesh
		meshes[y]->UploadBuffers();

		// draw the mesh additivly for each light
		devCon->DrawIndexed(meshes[y]->GetNumIndices(), 0, 0);
	}	
}

void ShaderManager::RenderAmbient(const std::vector<Mesh*>& meshes)
{		
	// get devicecontext
	DXManager& DXM = DXManager::GetInstance();
	ID3D11DeviceContext* devCon = DXM.GetDeviceCon();

	// constantbuffer structures
	ConstantAmbientVertex vertexData;
	ConstantAmbientPixel pixelData;		

	CameraComponent* camera = CameraManager::GetInstance().GetCurrentCameraGame();	

	//render with alpha blending	
	DXM.SetBlendState(BLEND_STATE::BLEND_ALPHA);

	// set shaders			
	devCon->VSSetShader(_vertexAmbientShader, NULL, 0);
	devCon->PSSetShader(_pixelAmbientShader, NULL, 0);
	
	// get ambient light
	const XMFLOAT4& ambientColor = LightManager::GetInstance().GetAmbientColor();	
	
	// get and transpose camera matrices
	XMFLOAT4X4 viewMatrix = camera->GetViewMatrix();
	XMFLOAT4X4 projectionMatrix = camera->GetProjectionMatrix();

	XMStoreFloat4x4(&viewMatrix, XMMatrixTranspose(XMLoadFloat4x4(&viewMatrix)));
	XMStoreFloat4x4(&projectionMatrix, XMMatrixTranspose(XMLoadFloat4x4(&projectionMatrix)));

	// set and update pixelconstants, only light color for now
	pixelData.color = ambientColor;
	UpdateConstantBuffer((void*)&pixelData, sizeof(ConstantAmbientPixel), _constantBufferPixel);

	// stuff that need to be set per mesh
	for (int i = 0; i < meshes.size(); i++) 
	{
		// upload vertex and indexbuffers
		meshes[i]->UploadBuffers();

		// get the world matrix and transpose it
		// get and transpose the world matrix for the mesh
		XMFLOAT4X4 worldMatrix = meshes[i]->GetWorldMatrix();
		XMStoreFloat4x4(&worldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&worldMatrix)));
		
		//set and upload vertexconstantdata 
		vertexData.world = worldMatrix;
		vertexData.view = viewMatrix;
		vertexData.projection = projectionMatrix;
		vertexData.uvOffset = meshes[i]->GetUvOffset();
		UpdateConstantBuffer((void*)&vertexData, sizeof(ConstantAmbientVertex), _constantBufferVertex);
						
		// set textures
		devCon->PSSetShaderResources(0, 1, meshes[i]->GetTextureArray());

		// draw
		devCon->DrawIndexed(meshes[i]->GetNumIndices(), 0, 0);
	}
	
}

void ShaderManager::RenderQuadUI(const std::vector<QuadComponent*>& quads)
{	
	DXManager& DXM = DXManager::GetInstance();
	ID3D11DeviceContext* devCon = DXM.GetDeviceCon();

	ConstantQuadUIVertex vertexData;
	ConstantQuadUIPixel pixelData;

	// set shaders			
	devCon->VSSetShader(_vertexSpriteShader, NULL, 0);
	devCon->PSSetShader(_pixelSpriteShader, NULL, 0);

	//disable the zbuffer for the 2d rendering
	DXM.SetBlendState(BLEND_STATE::BLEND_ALPHA);
	DXM.SetZBuffer(DEPTH_STATE::DISABLED);
	
	// get matrices 
	XMFLOAT4X4 viewMatrix = CameraManager::GetInstance().GetCurrentCameraUI()->GetViewMatrix(); 
	XMFLOAT4X4 projectionMatrix = CameraManager::GetInstance().GetCurrentCameraUI()->GetProjectionMatrix();

	// Transpose the matrices 	
	XMStoreFloat4x4(&viewMatrix, XMMatrixTranspose(XMLoadFloat4x4(&viewMatrix)));
	XMStoreFloat4x4(&projectionMatrix, XMMatrixTranspose(XMLoadFloat4x4(&projectionMatrix)));

	//FILL ALL CBUFFERS WITH DATA
	vertexData.projection = projectionMatrix;
	vertexData.view = viewMatrix;

	// update vertexconstantbuffers, only needs to be done once for all quads
	UpdateConstantBuffer((void*)&vertexData, sizeof(ConstantQuadUIVertex), _constantBufferVertex);

	for (int i = 0; i < quads.size(); i++) 
	{
		// set constantbuffer pixel data, needs to be set and updated for each quad
		pixelData.color = quads[i]->GetColor();
		UpdateConstantBuffer((void*)&pixelData, sizeof(ConstantQuadUIPixel), _constantBufferPixel);

		ID3D11ShaderResourceView* texture = quads[i]->GetTexture();
		devCon->PSSetShaderResources(0, 1, &texture);

		quads[i]->UploadBuffers();

		devCon->DrawIndexed(6, 0, 0);
	}
		
	// change back the z buffer
	DXM.SetZBuffer(DEPTH_STATE::ENABLED);

}

void ShaderManager::RenderDepth(const std::vector<Mesh*>& meshes)
{
	//// get devicecontext
	ID3D11DeviceContext* devCon = DXManager::GetInstance().GetDeviceCon();

	// constantbuffer structures
	ConstantAmbientVertex vertexData;

	CameraComponent* camera = CameraManager::GetInstance().GetCurrentCameraDepthMap();

	// only set vertexshader TODO: create pixelshader to handle depthmapping alpha meshes	
	devCon->VSSetShader(_vertexDepthShader, NULL, 0);
	devCon->PSSetShader(_pixelDepthShader, NULL, 0);
					
	// get and transpose camera matrices
	XMFLOAT4X4 viewMatrix = camera->GetViewMatrix();
	XMFLOAT4X4 projectionMatrix = camera->GetProjectionMatrix();

	XMStoreFloat4x4(&viewMatrix, XMMatrixTranspose(XMLoadFloat4x4(&viewMatrix)));
	XMStoreFloat4x4(&projectionMatrix, XMMatrixTranspose(XMLoadFloat4x4(&projectionMatrix)));

	for (int i = 0; i < meshes.size(); i++) 
	{		
		// get and transpose worldmatrix
		XMFLOAT4X4 worldMatrix = meshes[i]->GetWorldMatrix();		
		XMStoreFloat4x4(&worldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&worldMatrix)));

		//set and upload vertexconstantdata 
		vertexData.projection = projectionMatrix;
		vertexData.view = viewMatrix;
		vertexData.world = worldMatrix;
		UpdateConstantBuffer((void*)&vertexData, sizeof(ConstantDepthVertex), _constantBufferVertex);

		devCon->PSSetShaderResources(0, 1, meshes[i]->GetTextureArray());

		meshes[i]->UploadBuffers();

		devCon->DrawIndexed(meshes[i]->GetNumIndices(), 0, 0);
	}
	
}

void ShaderManager::RenderSkyBox(XMFLOAT4X4 worldMatrix) 
{
	//// get devicecontext
	DXManager& DXM = DXManager::GetInstance();
	ID3D11DeviceContext* devCon = DXM.GetDeviceCon();

	// constantbuffer structures
	ConstantSkyBoxVertex vertexData;

	CameraComponent* camera = CameraManager::GetInstance().GetCurrentCameraGame();

	// set shaders			
	devCon->VSSetShader(_vertexSkyBoxShader, NULL, 0);
	devCon->PSSetShader(_pixelSkyBoxShader, NULL, 0);
		
	//render with alpha blending
	DXM.SetBlendState(BLEND_STATE::BLEND_OPAQUE);
	DXM.SetRasterizerState(RASTERIZER_STATE::NOCULL);
	DXM.SetZBuffer(DEPTH_STATE::MASKED_SKYBOX);

	// get and transpose camera matrices
	XMFLOAT4X4 viewMatrix = camera->GetViewMatrix();
	XMFLOAT4X4 projectionMatrix = camera->GetProjectionMatrix();

	XMStoreFloat4x4(&viewMatrix, XMMatrixTranspose(XMLoadFloat4x4(&viewMatrix)));
	XMStoreFloat4x4(&projectionMatrix, XMMatrixTranspose(XMLoadFloat4x4(&projectionMatrix)));
	XMStoreFloat4x4(&worldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&worldMatrix)));

	//set and upload vertexconstantdata 
	vertexData.world = worldMatrix;
	vertexData.view = viewMatrix;
	vertexData.projection = projectionMatrix;		
	UpdateConstantBuffer((void*)&vertexData, sizeof(ConstantSkyBoxVertex), _constantBufferVertex);
	
	// draw
	devCon->DrawIndexed(36, 0, 0);

	DXM.SetZBuffer(DEPTH_STATE::ENABLED);
	DXM.SetRasterizerState(RASTERIZER_STATE::BACKCULL);
	
}

void ShaderManager::RenderParticles(const std::vector<ParticleEmitterComponent*>& emitters) 
{
	//// get devicecontext
	DXManager& DXM = DXManager::GetInstance();
	ID3D11DeviceContext* devCon = DXM.GetDeviceCon();

	// constantbuffer structures
	ConstantParticleVertex vertexData;
	
	CameraComponent* camera = CameraManager::GetInstance().GetCurrentCameraGame();

	DXM.SetZBuffer(DEPTH_STATE::READ_ONLY);
		
	// set shaders			
	devCon->VSSetShader(_vertexParticleShader, NULL, 0);
	devCon->PSSetShader(_pixelParticleShader, NULL, 0);
	
	// get and transpose camera matrices
	XMFLOAT4X4 viewMatrix = camera->GetViewMatrix();
	XMFLOAT4X4 projectionMatrix = camera->GetProjectionMatrix();

	XMStoreFloat4x4(&viewMatrix, XMMatrixTranspose(XMLoadFloat4x4(&viewMatrix)));
	XMStoreFloat4x4(&projectionMatrix, XMMatrixTranspose(XMLoadFloat4x4(&projectionMatrix)));

	vertexData.view = viewMatrix;
	vertexData.projection = projectionMatrix;
	UpdateConstantBuffer((void*)&vertexData, sizeof(ConstantParticleVertex), _constantBufferVertex);
		
	// stuff that need to be set per emitter
	for (int i = 0; i < emitters.size(); i++)
	{
		for(int y =0; y < emitters[i]->GetNumEmitters(); y++)
		{
			// upload vertex and indexbuffers
			emitters[i]->UploadBuffers(y);
			DXM.SetBlendState(emitters[i]->GetBlendState(y));

			// set texture
			ID3D11ShaderResourceView* texture = emitters[i]->GetTexture(y);
			devCon->PSSetShaderResources(0, 1, &texture);

			// draw		
			devCon->DrawIndexedInstanced(6, emitters[i]->GetNumParticles(y), 0, 0, 0);
		}		
	}
	DXM.SetZBuffer(DEPTH_STATE::ENABLED);
}

void ShaderManager::RenderGUI(ImDrawData* draw_data)
{
	ID3D11DeviceContext* devCon = DXManager::GetInstance().GetDeviceCon();
	ID3D11Device* device = DXManager::GetInstance().GetDevice();

	ID3D11Buffer*& vertexBuffer = GuiManager::GetInstance().GetVertexBuffer();
	ID3D11Buffer*& indexBuffer = GuiManager::GetInstance().GetIndexBuffer();

	HRESULT result;

	DXManager& DXM = DXManager::GetInstance();
	DXM.SetBlendState(BLEND_STATE::BLEND_ALPHA);
	DXM.SetZBuffer(DEPTH_STATE::DISABLED);

	// get current size of vertex and index buffers
	D3D11_BUFFER_DESC currentSizevertex;
	D3D11_BUFFER_DESC currentSizeIndex;
	vertexBuffer->GetDesc(&currentSizevertex);
	indexBuffer->GetDesc(&currentSizeIndex);

	// increase size of vertexbuffer if needed
	if(currentSizevertex.ByteWidth < draw_data->TotalVtxCount * sizeof(ImDrawVert))
	{
		vertexBuffer->Release(); 
		vertexBuffer = nullptr; 
				
		D3D11_BUFFER_DESC desc;
		memset(&desc, 0, sizeof(D3D11_BUFFER_DESC));
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.ByteWidth = currentSizevertex.ByteWidth + (5000 * sizeof(ImDrawVert));
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		result = device->CreateBuffer(&desc, NULL, &vertexBuffer);
		if (FAILED(result))
			printf("failed to update GUI vertexbuffersize\n");	
	}
	
	// increase size of indexbuffer if needed
	if (currentSizeIndex.ByteWidth < draw_data->TotalIdxCount * sizeof(ImDrawIdx))
	{
		indexBuffer->Release();
		indexBuffer = nullptr;

		D3D11_BUFFER_DESC desc;
		memset(&desc, 0, sizeof(D3D11_BUFFER_DESC));
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.ByteWidth = currentSizeIndex.ByteWidth + (10000 * sizeof(ImDrawVert));
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		result = device->CreateBuffer(&desc, NULL, &indexBuffer);
		if (FAILED(result))
			printf("failed to update GUI indexbuffersize\n");
	}

	// map vertex and indexbuffer
	D3D11_MAPPED_SUBRESOURCE vtx_resource, idx_resource;
	result = devCon->Map(vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &vtx_resource);
	if (FAILED(result))
		printf("failed to map gui vertexbuffer\n");

	result = devCon->Map(indexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &idx_resource);
	if (FAILED(result))
		printf("failed to map gui indexbuffer\n");

	// loop over all individual buffers and upload the data to one buffer
	ImDrawVert* vtx_dst = (ImDrawVert*)vtx_resource.pData;
	ImDrawIdx*  idx_dst = (ImDrawIdx*)idx_resource.pData;
	for (int i = 0; i < draw_data->CmdListsCount; i++)
	{
		const ImDrawList* cmd_list = draw_data->CmdLists[i];
		memcpy(vtx_dst, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
		memcpy(idx_dst, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
		vtx_dst += cmd_list->VtxBuffer.Size;
		idx_dst += cmd_list->IdxBuffer.Size;
	}
	devCon->Unmap(vertexBuffer, 0);
	devCon->Unmap(indexBuffer, 0);

	// create worldViewProjection
	float L = 0.0f;
	float R = ImGui::GetIO().DisplaySize.x;
	float B = ImGui::GetIO().DisplaySize.y;
	float T = 0.0f;
	float wvp[4][4] =
	{
		{ 2.0f / (R - L),    0.0f,              0.0f,       0.0f },
		{ 0.0f,              2.0f / (T - B),    0.0f,       0.0f },
		{ 0.0f,              0.0f,              0.5f,       0.0f },
		{ (R + L) / (L - R), (T + B) / (B - T), 0.5f,       1.0f },
	};
	
	// update the vertexconstantbuffer
	ConstantGUIVertex constantVertex;	
	memcpy(&constantVertex.projection, wvp, sizeof(wvp));
	UpdateConstantBuffer(&constantVertex, sizeof(ConstantGUIVertex), _constantBufferVertex);

	// Bind shader and vertex buffers
	unsigned int stride = sizeof(ImDrawVert);
	unsigned int offset = 0;
	devCon->IASetInputLayout(_inputlayoutGUI);
	devCon->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	devCon->IASetIndexBuffer(indexBuffer, sizeof(ImDrawIdx) == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT, 0);	
	devCon->VSSetShader(_vertexGUIShader, NULL, 0);	
	devCon->PSSetShader(_pixelGUIShader, NULL, 0);
	
	// Render command lists
	int vtx_offset = 0;
	int idx_offset = 0;
	for (int n = 0; n < draw_data->CmdListsCount; n++)
	{
		const ImDrawList* cmd_list = draw_data->CmdLists[n];
		for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
		{
			const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
			if (pcmd->UserCallback)
			{
				pcmd->UserCallback(cmd_list, pcmd);
			}
			else
			{
				const D3D11_RECT r = { (LONG)pcmd->ClipRect.x, (LONG)pcmd->ClipRect.y, (LONG)pcmd->ClipRect.z, (LONG)pcmd->ClipRect.w };
				devCon->PSSetShaderResources(0, 1, (ID3D11ShaderResourceView**)&pcmd->TextureId);
				devCon->RSSetScissorRects(1, &r);
				devCon->DrawIndexed(pcmd->ElemCount, idx_offset, vtx_offset);
			}
			idx_offset += pcmd->ElemCount;
		}
		vtx_offset += cmd_list->VtxBuffer.Size;
	}	

	DXM.SetZBuffer(DEPTH_STATE::ENABLED);
}

void ShaderManager::UpdateConstantBuffer(void* data, unsigned int size, ID3D11Buffer*& buffer)
{
	
	ID3D11DeviceContext* devCon = DXManager::GetInstance().GetDeviceCon();
	ID3D11Device* device = DXManager::GetInstance().GetDevice();

	//get desc of the current constant buffer in use
	D3D11_BUFFER_DESC old;
	buffer->GetDesc(&old);

	// if smaller then the new data we create a new bigger one and remove the old
    if (old.ByteWidth < size)
	{
		buffer->Release();
		buffer = nullptr;

		HRESULT result = 0;
		D3D11_BUFFER_DESC constVertexBufferDesc;

		constVertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC; 
		constVertexBufferDesc.ByteWidth = size;
		constVertexBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER; 
		constVertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; 
		constVertexBufferDesc.MiscFlags = 0; 
		constVertexBufferDesc.StructureByteStride = 0;

		result = device->CreateBuffer(&constVertexBufferDesc, NULL, &buffer);
		if (FAILED(result))
			printf("failed to create new ConstantBuffer\n");
	}

	//uppdate the buffer with the new data
	void* destination;
	D3D11_MAPPED_SUBRESOURCE subResource;
	devCon->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);

	destination = subResource.pData;
	memcpy(destination, data, size);

	devCon->Unmap(buffer, 0);	
}

void ShaderManager::CreateVertexShader(LPCWSTR filePath, ID3D11VertexShader** shader, ID3D10Blob** buffer)
{
	ID3D11Device* device = DXManager::GetInstance().GetDevice();
	HRESULT result;
	ID3D10Blob* errorMessage; 
			
	//compile the vertex shader code from the text file into temporeral buffers
	result = D3DCompileFromFile(filePath, NULL, NULL, "Main", "vs_5_0", 0, 0, buffer, &errorMessage);
	if (FAILED(result))
	{
		if (errorMessage) 
		{
			printf("failed to compile Vertexshadercode\n");
			printf("%s", (char*)errorMessage->GetBufferPointer());
		}
							
		else
			printf("failed to find Vertexshaderfile\n");					
	}
	//once the code have been compiled into the buffer we can create the shader objects themselfs from the blob objects
	result = device->CreateVertexShader((*buffer)->GetBufferPointer(), (*buffer)->GetBufferSize(), NULL, shader);
	if (FAILED(result)) 
		printf("failed to create the vertexShader from vertexbuffer\n");
	
}

void ShaderManager::CreatePixelShader(LPCWSTR filePath, ID3D11PixelShader** shader, ID3D10Blob** buffer)
{
	ID3D11Device* device = DXManager::GetInstance().GetDevice();
	HRESULT result;
	ID3D10Blob* errorMessage;
		
	result = D3DCompileFromFile(filePath, NULL, NULL, "Main", "ps_5_0", 0, 0, buffer, &errorMessage);
	if (FAILED(result))
	{
		if (errorMessage) 
		{
			printf("failed to compile Pixelshadercode\n");
			printf("%s", (char*)errorMessage->GetBufferPointer());
		}								
		else 
			printf("failed to find Pixelshaderfile\n");					
	}

	result = device->CreatePixelShader((*buffer)->GetBufferPointer(), (*buffer)->GetBufferSize(), NULL, shader);
	if (FAILED(result)) 
		printf("failed to create the pixelShader from pixelbuffer\n");
				
}
