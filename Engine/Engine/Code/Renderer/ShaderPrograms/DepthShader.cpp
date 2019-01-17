#include "DepthShader.h"
#include "DXManager.h"
#include "CameraComponent.h"
#include "CameraManager.h"
#include "Mesh.h"
#include "ShaderHelpers.h"
#include "Systems.h"
#include "DXBlendStates.h"
#include "MathHelpers.h"
#include "InstancedModel.h"

DepthShader::DepthShader()
{
	// create and compile shaders
	SHADER_HELPERS::CreateVertexShader(L"shaders/vertexDepth.vs", _vertexShader, _vertexShaderByteCode);
	SHADER_HELPERS::CreateVertexShader(L"shaders/vertexDepthInstanced.vs", _vertexShaderInstanced, _vertexShaderByteCodeInstanced);
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

	size_t size = meshes.size();
	for (int i = 0; i < size; i++)
	{
		// set world matrix of mesh
		XMStoreFloat4x4(&vertexData.worldViewProj, XMLoadFloat4x4(&MATH_HELPERS::MatrixMutiplyTrans(&meshes[i]->GetWorldMatrix(), &camera->GetViewProjMatrix())));

		// update the constant buffer with the vertexdata of this mesh
		SHADER_HELPERS::UpdateConstantBuffer((void*)&vertexData, sizeof(ConstantVertex), _constantBufferVertex);

		// set textures
		devCon->PSSetShaderResources(0, 1, meshes[i]->GetTextureArray());

		// upload vertices and indices
		meshes[i]->UploadBuffers();

		// draw
		devCon->DrawIndexed(meshes[i]->GetNumIndices(), 0, 0);
	}

	// unbind so we can use resources as input in next stages
	ID3D11ShaderResourceView* nullSRV[1] = { NULL };
	devCon->PSSetShaderResources(0, 1, nullSRV);
}

void DepthShader::RenderDepthInstanced(std::vector<InstancedModel*>& models)
{
	// get dx manager
	DXManager& DXM = *Systems::dxManager;
	CameraManager& CM = *Systems::cameraManager;

	DXM.BlendStates()->SetBlendState(BLEND_STATE::BLEND_ALPHA);

	// get devicecontext
	ID3D11DeviceContext* devCon = DXM.GetDeviceCon();

	// get the camera that will render the depthmap
	CameraComponent* camera = CM.GetCurrentCameraDepthMap();

	// set our shaders
	devCon->VSSetShader(_vertexShaderInstanced, NULL, 0);
	devCon->PSSetShader(_pixelShader, NULL, 0);

	// set the vertex constant buffer
	devCon->VSSetConstantBuffers(0, 1, &_constantBufferVertex);

	// set world matrix of mesh
	ConstantVertex vertexData;
	XMStoreFloat4x4(&vertexData.worldViewProj, XMLoadFloat4x4(&camera->GetViewProjMatrixTrans()));
	SHADER_HELPERS::UpdateConstantBuffer((void*)&vertexData, sizeof(ConstantVertex), _constantBufferVertex);

	size_t size = models.size();
	for (int i = 0; i < size; i++)
		models[i]->RenderInstances();

	ID3D11ShaderResourceView* nullSRV[4] = { NULL, NULL, NULL, NULL };
	devCon->PSSetShaderResources(0, 4, nullSRV);
}
