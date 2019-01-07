#include "ForwardAlphaShader.h"
#include "ShaderHelpers.h"
#include "DXManager.h"
#include "DXBlendStates.h"
#include "DXDepthStencilStates.h"
#include "CameraManager.h"
#include "LightManager.h"
#include "Mesh.h"
#include "Systems.h"
#include "MathHelpers.h"

ForwardAlphaShader::ForwardAlphaShader()
{
	//create shaders
	SHADER_HELPERS::CreateVertexShader(L"shaders/vertexForward.vs",  _vertexShader, _vertexShaderByteCode);
	SHADER_HELPERS::CreatePixelShader(L"shaders/pixelForward.ps",    _pixelShader,  _pixelShaderByteCode);

	// create constant buffers
	SHADER_HELPERS::CreateConstantBuffer(_CBVertex);
	SHADER_HELPERS::CreateConstantBuffer(_CBPixelAmbDir);
	SHADER_HELPERS::CreateConstantBuffer(_CBPixelPoint);
}

ForwardAlphaShader::~ForwardAlphaShader()
{
	_vertexShader->Release();
	_pixelShader->Release();

	_vertexShaderByteCode->Release();
	_pixelShaderByteCode->Release();

	_CBPixelAmbDir->Release();
	_CBPixelPoint->Release();
	_CBVertex->Release();
}

void ForwardAlphaShader::RenderForward(std::vector<Mesh*>& meshes)
{
	if (meshes.size() == 0)
		return;

	// get systems
	DXManager& DXM    = *Systems::dxManager;
	CameraManager& CM = *Systems::cameraManager;
	LightManager& LM  = *Systems::lightManager;

	// get device context
	ID3D11DeviceContext*& devCon = DXM.GetDeviceCon();

	// get game camera and shadow camera
	CameraComponent*& camera      = CM.GetCurrentCameraGame();
	CameraComponent*& cameraLight = CM.GetCurrentCameraDepthMap();

	// get camera position
	const XMFLOAT3& cameraPos = camera->GetComponent<TransformComponent>()->GetPositionRef();

	// get directional light
	LightDirectionComponent*& directionalLight = LM.GetDirectionalLight();

	// set shaders
	devCon->VSSetShader(_vertexShader, NULL, 0);
	devCon->PSSetShader(_pixelShader, NULL, 0);

	// set constant buffers
	devCon->VSSetConstantBuffers(0, 1, &_CBVertex);
	devCon->PSSetConstantBuffers(0, 1, &_CBPixelAmbDir);
	devCon->PSSetConstantBuffers(1, 1, &_CBPixelPoint);

	// set to alpha blending
	DXM.BlendStates()->SetBlendState(BLEND_STATE::BLEND_ALPHA);

	// create constant buffer structures
	CBVertex constantVertex;
	CBAmbDir constantAmbDirPixel;

	// set ambient and directional light properties for pixel shader
	XMStoreFloat4(&constantAmbDirPixel.ambientColor,    XMLoadFloat4(&LM.GetAmbientColor()));
	XMStoreFloat4(&constantAmbDirPixel.dirDiffuseColor, XMLoadFloat4(&directionalLight->GetLightColor()));
	XMStoreFloat3(&constantAmbDirPixel.lightDir,        XMLoadFloat3(&directionalLight->GetLightDirectionInv()));

	// set camera matrices
	XMStoreFloat3(&constantVertex.camPos, XMLoadFloat3(&cameraPos));

	// get shadow map
	ID3D11ShaderResourceView* shadowMap = cameraLight->GetSRV();

	// update pixel shader constant buffers
	SHADER_HELPERS::UpdateConstantBuffer((void*)&constantAmbDirPixel, sizeof(CBAmbDir), _CBPixelAmbDir);
	SHADER_HELPERS::UpdateConstantBuffer((void*)LM.GetCBPointBuffer(),sizeof(CBPoint) * LM.GetNumPointLights(), _CBPixelPoint);

	// sort alpha meshes to render back to front
	SHADER_HELPERS::MeshSort(meshes, cameraPos, true);

	size_t numMeshes = meshes.size();
	for (int i = 0; i < numMeshes; i++)
	{
		// get mesh
		Mesh* mesh = meshes[i];

		// get mesh specific constant data
		const XMFLOAT4X4& worldMat = mesh->GetWorldMatrix();

		XMStoreFloat4x4(&constantVertex.world,              XMLoadFloat4x4(&mesh->GetWorldMatrixTrans()));
		XMStoreFloat4x4(&constantVertex.worldViewProj,      XMLoadFloat4x4(&MATH_HELPERS::MatrixMutiplyTrans(&worldMat, &camera->GetViewProjMatrix())));
		XMStoreFloat4x4(&constantVertex.worldViewProjLight, XMLoadFloat4x4(&MATH_HELPERS::MatrixMutiplyTrans(&worldMat, &cameraLight->GetViewProjMatrix())));
		XMStoreFloat2(&constantVertex.uvOffset,             XMLoadFloat2(&mesh->GetUvOffset()));

		// update vertex constant buffer
		SHADER_HELPERS::UpdateConstantBuffer((void*)&constantVertex, sizeof(CBVertex), _CBVertex);

		// get the texture array of mesh
		ID3D11ShaderResourceView** meshTextures = mesh->GetTextureArray();

		// fill texture array with all textures including the shadow map
		ID3D11ShaderResourceView* t[5] = { meshTextures[0], meshTextures[1], meshTextures[2], meshTextures[3], shadowMap };

		// set SRV's
		devCon->PSSetShaderResources(0, 5, t);

		mesh->UploadBuffers();

		devCon->DrawIndexed(mesh->GetNumIndices(), 0, 0);
	}
}
