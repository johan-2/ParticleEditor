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
#include "Systems.h"
#include "MathHelpers.h"
#include "InstancedModel.h"

DeferredShader::DeferredShader()
{
	//create deferred geometry shaders
	SHADER_HELPERS::CreateVertexShader(L"shaders/vertexDeferredGeometry.vs", _vertexGeometryShader, _vertexGeometryShaderByteCode);
	SHADER_HELPERS::CreateVertexShader(L"shaders/vertexDeferredGeometryInstanced.vs", _vertexGeometryShaderInstanced, _vertexGeometryShaderByteCodeInstanced);
	SHADER_HELPERS::CreatePixelShader(L"shaders/pixelDeferredGeometry.ps",   _pixelGeometryShader,  _pixelGeometryShaderByteCode);

	//create deferred lightning shaders
	SHADER_HELPERS::CreateVertexShader(L"shaders/vertexDeferredLightning.vs", _vertexLightShader, _vertexLightningShaderByteCode);
	SHADER_HELPERS::CreatePixelShader(L"shaders/pixelDeferredLightning.ps",   _pixelLightShader,    _pixelLightningShaderByteCode);

	// create constant buffers for the deferred passes
	SHADER_HELPERS::CreateConstantBuffer(_CBGeometryVertex);
	SHADER_HELPERS::CreateConstantBuffer(_CBGeometryVertexInstanced);
	SHADER_HELPERS::CreateConstantBuffer(_CBMisc);
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

	_CBGeometryVertex->Release();
	_CBMisc->Release();
}

void DeferredShader::RenderGeometry(std::vector<Mesh*>& meshes)
{
	// get DXManager
	DXManager& DXM    = *Systems::dxManager;
	CameraManager& CM = *Systems::cameraManager;

	// render opaque objects here only
	DXM.BlendStates()->SetBlendState(BLEND_STATE::BLEND_OPAQUE);

	// get devicecontext
	ID3D11DeviceContext* devCon = DXM.GetDeviceCon();

	// get the game camera
	CameraComponent* camera = CM.GetCurrentCameraGame();

	// set shaders
	devCon->VSSetShader(_vertexGeometryShader, NULL, 0);
	devCon->PSSetShader(_pixelGeometryShader,  NULL, 0);

	// set the vertex constant buffer
	devCon->VSSetConstantBuffers(0, 1, &_CBGeometryVertex);

	// constantbuffer structure
	CBGeometryVertex vertexData;

	// stuff that need to be set per mesh
	size_t size = meshes.size();
	for (int i = 0; i < size; i++)
	{
		// upload vertex and indexbuffers
		meshes[i]->UploadBuffers();

		//set and upload vertex constantdata
		XMStoreFloat4x4(&vertexData.world,         XMLoadFloat4x4(&meshes[i]->GetWorldMatrixTrans()));
		XMStoreFloat4x4(&vertexData.worldViewProj, XMLoadFloat4x4(&MATH_HELPERS::MatrixMutiplyTrans(&meshes[i]->GetWorldMatrix(), &camera->GetViewProjMatrix())));
		XMStoreFloat2(&vertexData.uvOffset,        XMLoadFloat2(&meshes[i]->GetUvOffset()));

		// update the constant buffer with the mesh data
		SHADER_HELPERS::UpdateConstantBuffer((void*)&vertexData, sizeof(CBGeometryVertex), _CBGeometryVertex);

		// set textures
		devCon->PSSetShaderResources(0, 4, meshes[i]->GetTextureArray());

		// draw
		devCon->DrawIndexed(meshes[i]->GetNumIndices(), 0, 0);
	}

	// unbind so we can use resources as input in next stages
	ID3D11ShaderResourceView* nullSRV[4] = { NULL, NULL, NULL, NULL };
	devCon->PSSetShaderResources(0, 4, nullSRV);
}

void DeferredShader::renderGeometryInstanced(std::vector<InstancedModel*> models)
{
	// get DXManager
	DXManager& DXM = *Systems::dxManager;
	CameraManager& CM = *Systems::cameraManager;

	// render opaque objects here only
	DXM.BlendStates()->SetBlendState(BLEND_STATE::BLEND_OPAQUE);

	// get devicecontext
	ID3D11DeviceContext* devCon = DXM.GetDeviceCon();

	// get the game camera
	CameraComponent* camera = CM.GetCurrentCameraGame();

	// set shaders
	devCon->VSSetShader(_vertexGeometryShaderInstanced, NULL, 0);
	devCon->PSSetShader(_pixelGeometryShader, NULL, 0);

	// set the vertex constant buffer
	devCon->VSSetConstantBuffers(0, 1, &_CBGeometryVertexInstanced);

	// constantbuffer structure
	CBGeometryVertexInstanced vertexData;	
	XMStoreFloat4x4(&vertexData.ViewProj, XMLoadFloat4x4(&camera->GetViewProjMatrixTrans()));
	SHADER_HELPERS::UpdateConstantBuffer((void*)&vertexData, sizeof(CBGeometryVertexInstanced), _CBGeometryVertexInstanced);

	// stuff that need to be set per mesh
	size_t size = models.size();
	for (int i = 0; i < size; i++)	
		models[i]->RenderInstances();							

	// unbind so we can use resources as input in next stages
	ID3D11ShaderResourceView* nullSRV[4] = { NULL, NULL, NULL, NULL };
	devCon->PSSetShaderResources(0, 4, nullSRV);
}

void DeferredShader::RenderLightning(GBuffer*& gBuffer)
{
	// get DXManager
	DXManager& DXM    = *Systems::dxManager;
	LightManager& LM  = *Systems::lightManager;
	CameraManager& CM = *Systems::cameraManager;

	// get devicecontext
	ID3D11DeviceContext*& devCon = DXM.GetDeviceCon();

	// get cameras
	CameraComponent*& camera      = CM.GetCurrentCameraGame();
	CameraComponent*& cameraLight = CM.GetCurrentCameraDepthMap();
	const XMFLOAT3& camPos        = camera->GetComponent<TransformComponent>()->GetPositionRef();

	// set shaders
	devCon->VSSetShader(_vertexLightShader, NULL, 0);
	devCon->PSSetShader(_pixelLightShader, NULL, 0);

	// render with mask so we dont preform lighting calculations on non geometry pixels
	DXM.DepthStencilStates()->SetDepthStencilState(DEPTH_STENCIL_STATE::MASKED_LIGHTNING);

	// fill misc buffer with camera position and light worldviewproj
	CBMiscPixel miscPixel;
	miscPixel.cameraPosition = XMFLOAT4(camPos.x, camPos.y, camPos.z, 1.0f);	
	XMStoreFloat4x4(&miscPixel.lightViewProj, XMLoadFloat4x4(&cameraLight->GetViewProjMatrixTrans()));

	// update buffer
	SHADER_HELPERS::UpdateConstantBuffer((void*)&miscPixel, sizeof(CBMiscPixel), _CBMisc);

	// get Gbuffer textures and depthmap
	ID3D11ShaderResourceView**& gBufferTextures = gBuffer->GetSrvArray();
	ID3D11ShaderResourceView* depthMap = cameraLight->GetSRV();

	// add all to one array
	ID3D11ShaderResourceView* textureArray[5] = { depthMap, gBufferTextures[0], gBufferTextures[1], gBufferTextures[2], gBufferTextures[3] };

	// set the textures
	devCon->PSSetShaderResources(0, 5, textureArray);

	ID3D11Buffer* pointBuffer  = LM.GetPointLightCB();
	ID3D11Buffer* ambDirBuffer = LM.GetAmbDirLightCB();

	// set all constant buffers
	devCon->PSSetConstantBuffers(0, 1, &ambDirBuffer);
	devCon->PSSetConstantBuffers(1, 1, &pointBuffer);
	devCon->PSSetConstantBuffers(2, 1, &_CBMisc);

	// draw
	devCon->DrawIndexed(6, 0, 0);

	// unbind all textures
	ID3D11ShaderResourceView* nullTextureArray[5] = { nullptr, nullptr, nullptr, nullptr, nullptr };
	devCon->PSSetShaderResources(0, 5, nullTextureArray);

	// enable depth again
	DXM.DepthStencilStates()->SetDepthStencilState(DEPTH_STENCIL_STATE::ENABLED);
}
