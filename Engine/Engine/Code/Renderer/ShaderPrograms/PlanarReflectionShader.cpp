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
#include "SkyBox.h"

PlanarReflectionShader::PlanarReflectionShader()
{
	//create shaders
	SHADER_HELPERS::CreateVertexShader(L"shaders/vertexForwardPlanar.vs", _planarVertexShader, _planarVertexShaderByteCode);
	SHADER_HELPERS::CreatePixelShader(L"shaders/pixelForwardPlanar.ps",   _planarPixelShader, _planarPixelShaderByteCode);

	SHADER_HELPERS::CreateVertexShader(L"shaders/vertexReflectionMap.vs", _reflectionMapVertexShader, _reflectionMapVertexShaderByteCode);
	SHADER_HELPERS::CreatePixelShader(L"shaders/pixelReflectionMap.ps",   _reflectionMapPixelShader,  _reflectionMapPixelShaderByteCode);

	// create constant buffers
	SHADER_HELPERS::CreateConstantBuffer(_constantBufferPlanarVertex);
	SHADER_HELPERS::CreateConstantBuffer(_constantBufferPlanarPixelAmbDir);
	SHADER_HELPERS::CreateConstantBuffer(_constantBufferPlanarPixelPoint);

	SHADER_HELPERS::CreateConstantBuffer(_constantBufferReflectionMapVertex);

	_reflectionMap = new RenderToTexture(SCREEN_WIDTH, SCREEN_HEIGHT, false);

	Entity* reflectionQuad = new Entity();
	reflectionQuad->AddComponent<QuadComponent>()->Init(XMFLOAT2(SCREEN_WIDTH * 0.66f, SCREEN_HEIGHT * 0.1f), XMFLOAT2(SCREEN_WIDTH * 0.1f, SCREEN_HEIGHT * 0.1f), L"");
	reflectionQuad->GetComponent<QuadComponent>()->SetTexture(_reflectionMap->GetShaderResource());
}

PlanarReflectionShader::~PlanarReflectionShader()
{
	_planarVertexShader->Release();
	_planarPixelShader->Release();
	_reflectionMapVertexShader->Release();
	_reflectionMapPixelShader->Release();

	_planarVertexShaderByteCode->Release();
	_planarPixelShaderByteCode->Release();
	_reflectionMapVertexShaderByteCode->Release();
	_reflectionMapPixelShaderByteCode->Release();

	_constantBufferPlanarPixelAmbDir->Release();
	_constantBufferPlanarPixelPoint->Release();
	_constantBufferPlanarVertex->Release();
	_constantBufferReflectionMapVertex->Release();
}

void PlanarReflectionShader::Render(std::vector<Mesh*> reflectionMeshes,
	                                 std::vector<Mesh*> reflectiveMeshes, 
	                                 std::vector<ParticleSystemComponent*> reflectiveParticles,
	                                 ParticleShader* particleShader, 
	                                 DXInputLayouts* inputLayouts)
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
	SHADER_HELPERS::UpdateConstantBuffer((void*)&constantPointPixel, sizeof(ConstantPointPixelPlanar) * size, _constantBufferPlanarPixelPoint);

	// loop over all meshes that will project reflections onto itself
	const int numMeshes = reflectionMeshes.size();
	for (int i = 0; i < numMeshes; i++)
	{	
		// get the mesh that will project the reflections
		Mesh* mesh = reflectionMeshes[i];

		Mesh::ReflectiveData reflectData = mesh->GetReflectiveData();

		// clear our reflection map render texture and set i t active
		_reflectionMap->ClearRenderTarget(0,0,0,1, false);
		_reflectionMap->SetRendertarget();

		// get the clip plane of the reflection mesh so we dont render anything below this plane
		XMFLOAT4 clipPlane(0, 1, 0, mesh->GetPosition().y);

		// get the reflection view matrix based on moving the camera under the mesh by the distance between mesh and camera y position
		// this also invert the x rotation so we will look up instead of down
		XMFLOAT4X4 reflectionViewMatrix = camera->GetReflectionViewMatrix(mesh->GetPosition().y);

		// fill the constant buffer that the meshes that will be projected to this meshs surface need
		ConstantVertexReflectionMap constantVertexReflection;
		constantVertexReflection.clipingPlane = clipPlane;
		constantVertexReflection.view         = reflectionViewMatrix;
		constantVertexReflection.projection   = projectionMatrix;

		// render skybox first to the textures if it is set to reflect
		if (reflectData.reflectSkybox)
			Systems::renderer->GetSkybox()->Render(true);

		// set shaders that will render the reflection map this mesh will use
		devCon->VSSetShader(_reflectionMapVertexShader, NULL, 0);
		devCon->PSSetShader(_reflectionMapPixelShader, NULL, 0);

		// set constant buffer for the vertex shader
		devCon->VSSetConstantBuffers(0, 1, &_constantBufferReflectionMapVertex);

		// loop over all meshes that is set to cast reflections
		for (int y =0; y< reflectiveMeshes.size(); y++)
		{
			// get world matrix of mesh and update the buffer
			constantVertexReflection.world = reflectiveMeshes[y]->GetWorldMatrix();
			SHADER_HELPERS::UpdateConstantBuffer((void*)&constantVertexReflection, sizeof(ConstantVertexReflectionMap), _constantBufferReflectionMapVertex);

			// get the texture array of mesh
			ID3D11ShaderResourceView** meshTextures = reflectiveMeshes[y]->GetTextureArray();

			// the shader that renders to the reflection map is super simplified and only renders a sample from the diffusemap
			// more detail then this is a bit overkill but depending on if we want to render mirror clear reflections we might want 
			// to add a optional shader that maybe calculates lightning aswell
			devCon->PSSetShaderResources(0, 1, &meshTextures[0]);

			// upload and draw the mesh
			reflectiveMeshes[y]->UploadBuffers();

			devCon->DrawIndexed(reflectiveMeshes[y]->GetNumIndices(), 0, 0);
		}

		// render all particles to the texture if enabled
		if (reflectData.reflectParticles)
		{
			inputLayouts->SetInputLayout(INPUT_LAYOUT_TYPE::LAYOUTPARTICLE);
			particleShader->RenderParticles(reflectiveParticles, true);
			inputLayouts->SetInputLayout(INPUT_LAYOUT_TYPE::LAYOUT3D);
		}
		
		// set back to defualt render target and render the reflection mesh
		DXM.SetRenderTarget(nullptr, nullptr, true, false);

		// set shaders
		devCon->VSSetShader(_planarVertexShader, NULL, 0);
		devCon->PSSetShader(_planarPixelShader, NULL, 0);

		// set constant buffers
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
		constantVertex.reflectionView  = reflectionViewMatrix;

		// set the fraction of the reflection blending with the texture color
		constantAmbDirPixel.reflectiveFraction = reflectData.reflectiveFraction;

		// update vertex constant buffer
		SHADER_HELPERS::UpdateConstantBuffer((void*)&constantVertex, sizeof(ConstantVertexPlanar), _constantBufferPlanarVertex);
		SHADER_HELPERS::UpdateConstantBuffer((void*)&constantAmbDirPixel, sizeof(ConstantAmbientDirectionalPixelPlanar), _constantBufferPlanarPixelAmbDir);

		// get the texture array of mesh
		ID3D11ShaderResourceView** meshTextures = mesh->GetTextureArray();

		// fill texture array with all textures including the shadow map and reflection map
		ID3D11ShaderResourceView* t[5] = { meshTextures[0], meshTextures[1], meshTextures[2], shadowMap, _reflectionMap->GetShaderResource() };

		// set SRV's
		devCon->PSSetShaderResources(0, 5, t);

		mesh->UploadBuffers();

		devCon->DrawIndexed(mesh->GetNumIndices(), 0, 0);
	}
}
