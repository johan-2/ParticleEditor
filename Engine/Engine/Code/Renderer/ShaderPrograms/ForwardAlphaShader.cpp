#include "ForwardAlphaShader.h"
#include "ShaderHelpers.h"
#include "DXManager.h"
#include "DXBlendStates.h"
#include "DXDepthStencilStates.h"
#include "CameraManager.h"
#include "LightManager.h"
#include "Mesh.h"
#include "Systems.h"

ForwardAlphaShader::ForwardAlphaShader()
{
	//create shaders
	SHADER_HELPERS::CreateVertexShader(L"shaders/vertexForward.vs",  _vertexShader, _vertexShaderByteCode);
	SHADER_HELPERS::CreatePixelShader(L"shaders/pixelForward.ps",    _pixelShader,  _pixelShaderByteCode);

	// create constant buffers
	SHADER_HELPERS::CreateConstantBuffer(_constantBufferVertex);
	SHADER_HELPERS::CreateConstantBuffer(_constantBufferPixelAmbDir);
	SHADER_HELPERS::CreateConstantBuffer(_constantBufferPixelPoint);
}

ForwardAlphaShader::~ForwardAlphaShader()
{
	_vertexShader->Release();
	_pixelShader->Release();

	_vertexShaderByteCode->Release();
	_pixelShaderByteCode->Release();

	_constantBufferPixelAmbDir->Release();
	_constantBufferPixelPoint->Release();
	_constantBufferVertex->Release();
}

void ForwardAlphaShader::RenderForward(std::vector<Mesh*>& meshes)
{
	if (meshes.size() == 0)
		return;

	// get systems
	DXManager& DXM   = *Systems::dxManager;
	CameraManager CM = *Systems::cameraManager;
	LightManager LM  = *Systems::lightManager;

	// get device context
	ID3D11DeviceContext*& devCon = DXM.GetDeviceCon();

	// get game camera and shadow camera
	CameraComponent* camera      = CM.GetCurrentCameraGame();
	CameraComponent* cameraLight = CM.GetCurrentCameraDepthMap();

	// get directional light
	LightDirectionComponent*& directionalLight = LM.GetDirectionalLight();

	// get point lights
	std::vector<LightPointComponent*>& pointLights = LM.GetPointLight();

	// set shaders
	devCon->VSSetShader(_vertexShader, NULL, 0);
	devCon->PSSetShader(_pixelShader, NULL, 0);

	// set constant buffers
	devCon->VSSetConstantBuffers(0, 1, &_constantBufferVertex);
	devCon->PSSetConstantBuffers(0, 1, &_constantBufferPixelAmbDir);
	devCon->PSSetConstantBuffers(1, 1, &_constantBufferPixelPoint);

	// set to alpha blending
	DXM.BlendStates()->SetBlendState(BLEND_STATE::BLEND_ALPHA);

	// create constant buffer structures
	ConstantVertex                  constantVertex;
	ConstantAmbientDirectionalPixel constantAmbDirPixel;
	ConstantPointPixel              constantPointPixel[MAX_POINT_LIGHTS];

	// set ambient and directional light properties for pixel shader
	constantAmbDirPixel.ambientColor     = LM.GetAmbientColor();
	constantAmbDirPixel.dirDiffuseColor  = directionalLight->GetLightColor();
	XMStoreFloat3(&constantAmbDirPixel.lightDir, XMVectorNegate(XMLoadFloat3(&directionalLight->GetLightDirection())));

	// set point light properties for pixel shader
	const int size = pointLights.size();
	for (int i = 0; i < size; i++)
	{
		constantPointPixel[i].color          = pointLights[i]->GetLightColor();
		constantPointPixel[i].intensity      = pointLights[i]->GetIntensity();
		constantPointPixel[i].radius         = pointLights[i]->GetRadius();
		constantPointPixel[i].lightPosition  = pointLights[i]->GetComponent<TransformComponent>()->GetPositionRef();
		constantPointPixel[i].attConstant    = pointLights[i]->GetAttConstant();
		constantPointPixel[i].attLinear      = pointLights[i]->GetAttLinear();
		constantPointPixel[i].attExponential = pointLights[i]->GetAttExponential();
		constantPointPixel[i].numLights      = size;
	}

	// get camera matrices
	const XMFLOAT4X4& viewMatrix            = camera->GetViewMatrix();
	const XMFLOAT4X4& projectionMatrix      = camera->GetProjectionMatrix();

	// get light matrices
	const XMFLOAT4X4& lightViewMatrix       = cameraLight->GetViewMatrix();
	const XMFLOAT4X4& lightProjectionMatrix = cameraLight->GetProjectionMatrix();

	// get camera position
	const XMFLOAT3& cameraPos = camera->GetComponent<TransformComponent>()->GetPositionRef();

	// get shadow map 
	ID3D11ShaderResourceView* shadowMap = cameraLight->GetSRV();

	// update pixel shader constant buffers
	SHADER_HELPERS::UpdateConstantBuffer((void*)&constantAmbDirPixel, sizeof(ConstantAmbientDirectionalPixel), _constantBufferPixelAmbDir);
	SHADER_HELPERS::UpdateConstantBuffer((void*)&constantPointPixel,  sizeof(ConstantPointPixel) * size,       _constantBufferPixelPoint);

	// sort alpha meshes to render back to front
	SHADER_HELPERS::MeshSort(meshes, cameraPos, true);

	const int numMeshes = meshes.size();
	for (int i =0; i < numMeshes; i++)
	{
		// get mesh
		Mesh* mesh = meshes[i];

		// set vertex constant values
		constantVertex.world           = mesh->GetWorldMatrix();
		constantVertex.view            = viewMatrix;
		constantVertex.projection      = projectionMatrix;
		constantVertex.lightView       = lightViewMatrix;
		constantVertex.lightProjection = lightProjectionMatrix;
		constantVertex.camPos          = cameraPos;
		constantVertex.uvOffset        = mesh->GetUvOffset();

		// update vertex constant buffer
		SHADER_HELPERS::UpdateConstantBuffer((void*)&constantVertex, sizeof(ConstantVertex), _constantBufferVertex);

		// get the texture array of mesh
		ID3D11ShaderResourceView** meshTextures = mesh->GetTextureArray();

		// fill texture array with all textures including the shadow map
		ID3D11ShaderResourceView* t[4] = { meshTextures[0], meshTextures[1], meshTextures[2], shadowMap };

		// set SRV's
		devCon->PSSetShaderResources(0, 4, t);

		mesh->UploadBuffers();

		devCon->DrawIndexed(mesh->GetNumIndices(), 0, 0);
	}

}