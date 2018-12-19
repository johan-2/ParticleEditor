#include "DepthShader.h"
#include "DXManager.h"
#include "CameraComponent.h"
#include "CameraManager.h"
#include "Mesh.h"
#include "ShaderHelpers.h"
#include "Systems.h"
#include "DXBlendStates.h"

DepthShader::DepthShader()
{
	// create and compile shaders
	SHADER_HELPERS::CreateVertexShader(L"shaders/vertexDepth.vs", _vertexShader, _vertexShaderByteCode);
	SHADER_HELPERS::CreatePixelShader(L"shaders/pixelDepth.ps",   _pixelShader,  _pixelShaderByteCode);

	// create constant buffer for vertex shader
	SHADER_HELPERS::CreateConstantBuffer(_constantBufferVertex);
}

DepthShader::~DepthShader()
{
	_vertexShaderByteCode->Release();
	_pixelShaderByteCode->Release();

	_vertexShader->Release();
	_pixelShader->Release();

	_constantBufferVertex->Release();
}

void DepthShader::RenderDepth(std::vector<Mesh*>& meshes)
{
	// get dx manager
	DXManager& DXM    = *Systems::dxManager;
	CameraManager& CM = *Systems::cameraManager;

	DXM.BlendStates()->SetBlendState(BLEND_STATE::BLEND_ALPHA);

	// get devicecontext
	ID3D11DeviceContext* devCon = DXM.GetDeviceCon();

	// constantbuffer structure for vertex data
	ConstantVertex vertexData;

	// get the camera that will render the depthmap
	CameraComponent* camera = CM.GetCurrentCameraDepthMap();

	// set our shaders
	devCon->VSSetShader(_vertexShader, NULL, 0);
	devCon->PSSetShader(_pixelShader, NULL, 0);

	// set the vertex constant buffer
	devCon->VSSetConstantBuffers(0, 1, &_constantBufferVertex);

	// set constant data that is shared between all meshes
	XMStoreFloat4x4(&vertexData.view,       XMLoadFloat4x4(&camera->GetViewMatrix()));
	XMStoreFloat4x4(&vertexData.projection, XMLoadFloat4x4(&camera->GetProjectionMatrix()));

	unsigned int size = meshes.size();
	for (int i = 0; i < size; i++)
	{
		// set world matrix of mesh
		XMStoreFloat4x4(&vertexData.world, XMLoadFloat4x4(&meshes[i]->GetWorldMatrix()));

		// update the constant buffer with the vertexdata of this mesh
		SHADER_HELPERS::UpdateConstantBuffer((void*)&vertexData, sizeof(ConstantVertex), _constantBufferVertex);

		// set textures
		devCon->PSSetShaderResources(0, 1, meshes[i]->GetTextureArray());

		// upload vertices and indices
		meshes[i]->UploadBuffers();

		// draw
		devCon->DrawIndexed(meshes[i]->GetNumIndices(), 0, 0);
	}
}
