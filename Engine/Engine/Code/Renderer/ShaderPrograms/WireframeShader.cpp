#include "WireframeShader.h"
#include "DXManager.h"
#include "CameraComponent.h"
#include "CameraManager.h"
#include "Mesh.h"
#include "ShaderHelpers.h"
#include "DXRasterizerStates.h"

WireframeShader::WireframeShader()
{
	// create and compile shaders
	SHADER_HELPERS::CreateVertexShader(L"shaders/vertexWireframe.vs", _vertexShader, _vertexShaderByteCode);
	SHADER_HELPERS::CreatePixelShader(L"shaders/pixelWireframe.ps", _pixelShader, _pixelShaderByteCode);

	// create constant buffer for vertex shader
	SHADER_HELPERS::CreateConstantBuffer(_constantBufferVertex);
}

WireframeShader::~WireframeShader()
{
	_vertexShaderByteCode->Release();
	_pixelShaderByteCode->Release();

	_vertexShader->Release();
	_pixelShader->Release();

	_constantBufferVertex->Release();
}

void WireframeShader::RenderWireFrame(std::vector<Mesh*>& meshes)
{
	if (meshes.size() == 0)
		return;

	// get dx manager
	DXManager& DXM = DXManager::GetInstance();

	// get devicecontext
	ID3D11DeviceContext* devCon = DXM.GetDeviceCon();

	// constantbuffer structure for vertex data
	ConstantVertex vertexData;

	// get the camera that will render the depthmap
	CameraComponent* camera = CameraManager::GetInstance().GetCurrentCameraGame();

	// set our shaders
	devCon->VSSetShader(_vertexShader, NULL, 0);
	devCon->PSSetShader(_pixelShader, NULL, 0);

	// set the vertex constant buffer
	devCon->VSSetConstantBuffers(0, 1, &_constantBufferVertex);

	DXM.RasterizerStates()->SetRasterizerState(RASTERIZER_STATE::WIREFRAME);

	// get camera matrices
	const XMFLOAT4X4& viewMatrix = camera->GetViewMatrix();
	const XMFLOAT4X4& projectionMatrix = camera->GetProjectionMatrix();

	unsigned int size = meshes.size();
	for (int i = 0; i < size; i++)
	{
		// get and transpose worldmatrix
		const XMFLOAT4X4& worldMatrix = meshes[i]->GetWorldMatrix();

		//set and upload vertexconstantdata 
		vertexData.projection = projectionMatrix;
		vertexData.view       = viewMatrix;
		vertexData.world      = worldMatrix;

		// update the constant buffer with the vertexdata of this mesh
		SHADER_HELPERS::UpdateConstantBuffer((void*)&vertexData, sizeof(ConstantVertex), _constantBufferVertex);

		// upload vertices and indices
		meshes[i]->UploadBuffers();

		// draw
		devCon->DrawIndexed(meshes[i]->GetNumIndices(), 0, 0);
	}

	DXM.RasterizerStates()->SetRasterizerState(RASTERIZER_STATE::BACKCULL);
}
