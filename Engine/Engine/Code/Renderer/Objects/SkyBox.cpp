#include "SkyBox.h"
#include "CameraManager.h"
#include "DXManager.h"
#include "TransformComponent.h"
#include "DDSTextureLoader/DDSTextureLoader.h"
#include "DXErrorHandler.h"
#include "ShaderHelpers.h"
#include "DXBlendstates.h"
#include "DXRasterizerStates.h"
#include "DXDepthStencilStates.h"
#include "Systems.h"
#include "ModelLoader.h"
#include "Mesh.h"

SkyBox::SkyBox(const wchar_t* textureFile):
	_isActive(true)
{
	// create mesh and cubemap
	CreateBox();
	LoadCubemap(textureFile);

	// create shaders
	SHADER_HELPERS::CreateVertexShader(L"shaders/vertexSkyBox.vs",    _vertexDomeShader, _vertexDomeShaderByteCode);
	SHADER_HELPERS::CreatePixelShader(L"shaders/pixelSkyBox.ps",      _pixelDomeShader,  _pixelDomeShaderByteCode);
	SHADER_HELPERS::CreateVertexShader(L"shaders/vertexSkyBoxSun.vs", _vertexSunShader,  _vertexSunShaderByteCode);
	SHADER_HELPERS::CreatePixelShader(L"shaders/pixelSkyBoxSun.ps",   _pixelSunShader,   _pixelSunShaderByteCode);

	// create constant buffer
	SHADER_HELPERS::CreateConstantBuffer(_constantBufferVertex);

	_sun.position     = XMFLOAT3(0, 0, 0);
	_sun.rotation     = XMFLOAT3(0, 0, 0);
	_sun.scale        = XMFLOAT3(1, 1, 1);
	_sun.direction    = XMFLOAT3(1, 0, 0);
	_sun.distance     = XMFLOAT3(20, 20, 20);
	_sun.directionPtr = nullptr;

	// calculate the matrices that wont ever change
	XMStoreFloat4x4(&_sun.rotationMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&_sun.scaleMatrix,    XMMatrixScalingFromVector(XMLoadFloat3(&_sun.scale)));
}

SkyBox::~SkyBox()
{
	_vertexDomeShaderByteCode->Release();
	_pixelDomeShaderByteCode->Release();

	_vertexDomeShader->Release();
	_pixelDomeShader->Release();

	_constantBufferVertex->Release();
}

void SkyBox::CreateBox() 
{
	_domeMesh = ModelLoader::CreateSphere(0, L"", L"", L"", L"", 1.0f, nullptr);
	_sun.mesh = ModelLoader::CreateSphere(0, L"Textures/SunTest.dds", L"", L"", L"", 1.0f, nullptr);
}

void SkyBox::LoadCubemap(const wchar_t* file) 
{
	// remove the old cubemap texture if one exist
	if (_cubeMap != nullptr)
		_cubeMap->Release();

	// create cubemap from file
	HRESULT result = DirectX::CreateDDSTextureFromFile(Systems::dxManager->GetDevice(), file, NULL, &_cubeMap);

	// get and print error message if failed
	if (FAILED(result))						
		DX_ERROR::PrintError(result, (std::string("failed to create cubemap with filename ") + DX_ERROR::ConvertFromWString(file)).c_str());
}

void SkyBox::Render(bool useReflectViewMatrix)
{
	if (!_isActive)
		return;

	// RENDER SKYDOME SPHERE
	// get DX manager
	DXManager& DXM = *Systems::dxManager;

	// get device context
	ID3D11DeviceContext* devCon = DXM.GetDeviceCon();

	// get game camera
	CameraComponent* camera = Systems::cameraManager->GetCurrentCameraGame();

	// set shaders			
	devCon->VSSetShader(_vertexDomeShader, NULL, 0);
	devCon->PSSetShader(_pixelDomeShader, NULL, 0);

	// set constant buffer
	devCon->VSSetConstantBuffers(0, 1, &_constantBufferVertex);

	// set texture
	devCon->PSSetShaderResources(0, 1, &_cubeMap);

	// render with opaque blend
	DXM.BlendStates()->SetBlendState(BLEND_STATE::BLEND_OPAQUE);

	// use no culling
	DXM.RasterizerStates()->SetRasterizerState(RASTERIZER_STATE::NOCULL);

	// only render non geometry pixels
	DXM.DepthStencilStates()->SetDepthStencilState(DEPTH_STENCIL_STATE::MASKED_SKYBOX);

	// constantbuffer structure
	ConstantVertex vertexData;
	
	// get and transpose the position matrix of camera transform
	XMFLOAT4X4 matrixPosition = camera->GetComponent<TransformComponent>()->GetPositionMatrix();	
	XMStoreFloat4x4(&matrixPosition, XMMatrixTranspose(XMLoadFloat4x4(&matrixPosition)));
		
	// set vertices
	vertexData.world      = matrixPosition;
	vertexData.view       = camera->GetViewMatrix();
	vertexData.projection = camera->GetProjectionMatrix();

	// if the skybox is being rendered to a reflection map
	if (useReflectViewMatrix)
	{
		vertexData.view = camera->GetReflectionViewMatrix(camera->GetComponent<TransformComponent>()->GetPositionVal().y);
		DXM.DepthStencilStates()->SetDepthStencilState(DEPTH_STENCIL_STATE::DISABLED);
	}

	// update constant buffer
	SHADER_HELPERS::UpdateConstantBuffer((void*)&vertexData, sizeof(ConstantVertex), _constantBufferVertex);

	// upload and draw dome mesh
	_domeMesh->UploadBuffers();	
	devCon->DrawIndexed(_domeMesh->GetNumIndices(), 0, 0);

	// RENDER SUN
	XMFLOAT3 cameraPos = camera->GetComponent<TransformComponent>()->GetPositionVal();

	CaluclateSunMatrix(cameraPos);

	// set world matrix
	vertexData.world = _sun.worldMatrix;

	// update constant buffer
	SHADER_HELPERS::UpdateConstantBuffer((void*)&vertexData, sizeof(ConstantVertex), _constantBufferVertex);

	// set shaders			
	devCon->VSSetShader(_vertexSunShader, NULL, 0);
	devCon->PSSetShader(_pixelSunShader, NULL, 0);

	// set texture
	devCon->PSSetShaderResources(0, 1, &_sun.mesh->GetTextureArray()[0]);

	// upload and draw sun
	_sun.mesh->UploadBuffers();
	devCon->DrawIndexed(_sun.mesh->GetNumIndices(), 0, 0);

	// enable rendering of all pixels
	DXM.DepthStencilStates()->SetDepthStencilState(DEPTH_STENCIL_STATE::ENABLED);

	// set back to backcull
	DXM.RasterizerStates()->SetRasterizerState(RASTERIZER_STATE::BACKCULL);
}

void SkyBox::CaluclateSunMatrix(XMFLOAT3 cameraPosition)
{
	XMFLOAT3 offset;
	XMFLOAT3 sunDirection = _sun.directionPtr != nullptr ? _sun.directionPtr->GetForward() : _sun.direction;
	
	XMStoreFloat3(&offset,        XMVectorMultiply(XMLoadFloat3(&sunDirection),   XMLoadFloat3(&_sun.distance)));
	XMStoreFloat3(&_sun.position, XMVectorSubtract(XMLoadFloat3(&cameraPosition), XMLoadFloat3(&offset)));

	// create new position matrix from the new sun position
	XMStoreFloat4x4(&_sun.positionMatrix, XMMatrixTranslationFromVector(XMLoadFloat3(&_sun.position)));
	
	// multiply matrices for the final world matrix
	XMStoreFloat4x4(&_sun.worldMatrix, XMMatrixMultiply(XMLoadFloat4x4(&_sun.scaleMatrix), XMLoadFloat4x4(&_sun.rotationMatrix)));
	XMStoreFloat4x4(&_sun.worldMatrix, XMMatrixMultiply(XMLoadFloat4x4(&_sun.worldMatrix), XMLoadFloat4x4(&_sun.positionMatrix)));
	XMStoreFloat4x4(&_sun.worldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&_sun.worldMatrix)));
}