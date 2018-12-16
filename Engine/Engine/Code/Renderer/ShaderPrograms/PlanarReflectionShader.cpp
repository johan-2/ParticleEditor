#include "PlanarReflectionShader.h"
#include "ShaderHelpers.h"
#include "DXManager.h"
#include "DXBlendStates.h"
#include "DXDepthStencilStates.h"
#include "CameraManager.h"
#include "LightManager.h"
#include "Mesh.h"
#include "Systems.h"
#include "RenderToTexture.h"
#include "SystemDefs.h"
#include "Entity.h"
#include "QuadComponent.h"
#include "ParticleShader.h"
#include "SkyDome.h"
#include "ReflectionMapShader.h"

PlanarReflectionShader::PlanarReflectionShader()
{
	//create shaders
	SHADER_HELPERS::CreateVertexShader(L"shaders/vertexForwardPlanar.vs", _planarVertexShader, _planarVertexShaderByteCode);
	SHADER_HELPERS::CreatePixelShader(L"shaders/pixelForwardPlanar.ps",   _planarPixelShader, _planarPixelShaderByteCode);

	// create constant buffers
	SHADER_HELPERS::CreateConstantBuffer(_constantBufferPlanarVertex);
	SHADER_HELPERS::CreateConstantBuffer(_constantBufferPlanarPixelAmbDir);
	SHADER_HELPERS::CreateConstantBuffer(_constantBufferPlanarPixelPoint);		
}

PlanarReflectionShader::~PlanarReflectionShader()
{
	_planarVertexShader->Release();
	_planarPixelShader->Release();	

	_planarVertexShaderByteCode->Release();
	_planarPixelShaderByteCode->Release();
	
	_constantBufferPlanarPixelAmbDir->Release();
	_constantBufferPlanarPixelPoint->Release();
	_constantBufferPlanarVertex->Release();	
}

void PlanarReflectionShader::Render(std::vector<Mesh*>& reflectionMeshes,
    std::vector<Mesh*>& reflectiveOpaqueMeshes, 
    std::vector<Mesh*>& reflectiveAlphaMeshes,
    std::vector<ParticleSystemComponent*>& reflectiveParticles,
    ParticleShader*& particleShader, 
    DXInputLayouts*& inputLayouts, 
	ReflectionMapShader*& reflectionMapShader)
{
	if (reflectionMeshes.size() == 0)
		return;

	// get systems
	DXManager&     DXM = *Systems::dxManager;
	CameraManager& CM  = *Systems::cameraManager;
	LightManager&  LM  = *Systems::lightManager;

	// get device context
	ID3D11DeviceContext*& devCon = DXM.GetDeviceCon();

	// get game camera and shadow camera
	CameraComponent* camera      = CM.GetCurrentCameraGame();
	CameraComponent* cameraLight = CM.GetCurrentCameraDepthMap();

	// get directional light
	LightDirectionComponent*& directionalLight = LM.GetDirectionalLight();

	// get point lights
	std::vector<LightPointComponent*>& pointLights = LM.GetPointLight();

	// create constant buffer structures
	ConstantVertexPlanar                  constantVertex;
	ConstantAmbientDirectionalPixelPlanar constantAmbDirPixel;
	ConstantPointPixelPlanar              constantPointPixel[MAX_POINT_LIGHTS];

	// set ambient and directional light properties for pixel shader
	constantAmbDirPixel.ambientColor    = LM.GetAmbientColor();
	constantAmbDirPixel.dirDiffuseColor = directionalLight->GetLightColor();
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
	const XMFLOAT4X4& viewMatrix       = camera->GetViewMatrix();
	const XMFLOAT4X4& projectionMatrix = camera->GetProjectionMatrix();

	// get light matrices
	const XMFLOAT4X4& lightViewMatrix       = cameraLight->GetViewMatrix();
	const XMFLOAT4X4& lightProjectionMatrix = cameraLight->GetProjectionMatrix();

	// get camera position
	const XMFLOAT3& cameraPos = camera->GetComponent<TransformComponent>()->GetPositionRef();

	// get shadow map 
	ID3D11ShaderResourceView* shadowMap = cameraLight->GetSRV();

	// update pixel shader constant buffer fro point lights
	SHADER_HELPERS::UpdateConstantBuffer((void*)&constantPointPixel, sizeof(ConstantPointPixelPlanar) * size,        _constantBufferPlanarPixelPoint);
	SHADER_HELPERS::UpdateConstantBuffer((void*)&constantAmbDirPixel, sizeof(ConstantAmbientDirectionalPixelPlanar), _constantBufferPlanarPixelAmbDir);

	// loop over all meshes that will project reflections onto itself
	const int numMeshes = reflectionMeshes.size();
	for (int i = 0; i < numMeshes; i++)
	{	
		// get the mesh that will project the reflections
		Mesh* mesh = reflectionMeshes[i];

		// get the clip plane of the reflection mesh so we dont render anything below this plane
		XMFLOAT4 clipPlane(0, 1, 0, -mesh->GetPosition().y);

		// render the reflectionmap
		reflectionMapShader->GenerateReflectionMap(reflectiveOpaqueMeshes, reflectiveAlphaMeshes, reflectiveParticles, particleShader, inputLayouts, mesh, clipPlane);

		// set back to defualt render target and render the reflection mesh
		Systems::renderer->SetMainRenderTarget();

		// set shaders
		devCon->VSSetShader(_planarVertexShader, NULL, 0);
		devCon->PSSetShader(_planarPixelShader, NULL, 0);

		// set the vertex constant buffer, the pixel ones is already set
		devCon->VSSetConstantBuffers(0, 1, &_constantBufferPlanarVertex);
		devCon->PSSetConstantBuffers(0, 1, &_constantBufferPlanarPixelAmbDir);
		devCon->PSSetConstantBuffers(1, 1, &_constantBufferPlanarPixelPoint);

		// set to alpha blending
		DXM.BlendStates()->SetBlendState(BLEND_STATE::BLEND_ALPHA);			

		// set vertex constant values
		constantVertex.world           = mesh->GetWorldMatrix();
		constantVertex.view            = viewMatrix;
		constantVertex.projection      = projectionMatrix;
		constantVertex.lightView       = lightViewMatrix;
		constantVertex.lightProjection = lightProjectionMatrix;
		constantVertex.camPos          = cameraPos;
		constantVertex.uvOffset        = mesh->GetUvOffset();
		constantVertex.reflectionView  = camera->GetReflectionViewMatrix(mesh->GetPosition().y);

		// set the fraction of the reflection blending with the texture color
		constantAmbDirPixel.reflectiveFraction = mesh->GetReflectiveData().reflectiveFraction;

		// update vertex constant buffer
		SHADER_HELPERS::UpdateConstantBuffer((void*)&constantVertex,      sizeof(ConstantVertexPlanar),                  _constantBufferPlanarVertex);
		SHADER_HELPERS::UpdateConstantBuffer((void*)&constantAmbDirPixel, sizeof(ConstantAmbientDirectionalPixelPlanar), _constantBufferPlanarPixelAmbDir);

		// get the texture array of mesh
		ID3D11ShaderResourceView** meshTextures = mesh->GetTextureArray();

		// fill texture array with all textures including the shadow map and reflection map
		ID3D11ShaderResourceView* t[6] = { meshTextures[0], meshTextures[1], meshTextures[2], meshTextures[3], shadowMap, reflectionMapShader->GetReflectionMap() };

		// set SRV's
		devCon->PSSetShaderResources(0, 6, t);

		mesh->UploadBuffers();

		devCon->DrawIndexed(mesh->GetNumIndices(), 0, 0);
	}
}
