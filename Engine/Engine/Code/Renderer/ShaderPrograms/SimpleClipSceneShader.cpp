#include "SimpleClipSceneShader.h"
#include "ShaderHelpers.h"
#include "DXManager.h"
#include "DXBlendStates.h"
#include "DXDepthStencilStates.h"
#include "CameraManager.h"
#include "LightManager.h"
#include "Mesh.h"
#include "Systems.h"
#include "SystemDefs.h"
#include "Entity.h"
#include "QuadComponent.h"
#include "ParticleShader.h"
#include "SkyDome.h"
#include "MathHelpers.h"

SimpleClipSceneShader::SimpleClipSceneShader(bool debugQuad, XMFLOAT2 pos)
{
	// create shaders
	SHADER_HELPERS::CreateVertexShader(L"shaders/vertexSimpleClip.vs",     _reflectionMapVertexShader,      _reflectionMapVertexShaderByteCode);
	SHADER_HELPERS::CreatePixelShader(L"shaders/pixelSimpleClipOpaque.ps", _reflectionMapPixelShaderOpaque, _reflectionMapPixelShaderByteCodeOpaque);
	SHADER_HELPERS::CreatePixelShader(L"shaders/pixelSimpleClipAlpha.ps",  _reflectionMapPixelShaderAlpha,  _reflectionMapPixelShaderByteCodeAlpha);

	// create constant buffers
	SHADER_HELPERS::CreateConstantBuffer(_CBVertex);
	SHADER_HELPERS::CreateConstantBuffer(_CBPixelAmbDir);
	SHADER_HELPERS::CreateConstantBuffer(_CBPixelPoint);

	// create render texture
	_srv = new RenderToTexture(SCREEN_WIDTH, SCREEN_HEIGHT, false);

	if (debugQuad)
	{
		// create debug quad
		Entity* reflectionQuad = new Entity();
		reflectionQuad->AddComponent<QuadComponent>()->Init(XMFLOAT2(SCREEN_WIDTH * 0.66f, SCREEN_HEIGHT * 0.1f), XMFLOAT2(SCREEN_WIDTH * 0.1f, SCREEN_HEIGHT * 0.1f), L"");
		reflectionQuad->GetComponent<QuadComponent>()->SetTexture(_srv->GetRenderTargetSRV());
	}
}

SimpleClipSceneShader::~SimpleClipSceneShader()
{
	_reflectionMapVertexShader->Release();
	_reflectionMapPixelShaderOpaque->Release();
	_reflectionMapPixelShaderAlpha->Release();

	_reflectionMapVertexShaderByteCode->Release();
	_reflectionMapPixelShaderByteCodeOpaque->Release();
	_reflectionMapPixelShaderByteCodeAlpha->Release();

	_CBVertex->Release();
}

void SimpleClipSceneShader::RenderScene(std::vector<Mesh*>& opaqueMeshes, std::vector<Mesh*>& alphaMeshes, XMFLOAT4 clipPlane, bool includeSkyBox, bool includeParticles)
{
	// get systems
	DXManager&     DXM = *Systems::dxManager;
	CameraManager& CM  = *Systems::cameraManager;
	LightManager&  LM  = *Systems::lightManager;

	// get device context
	ID3D11DeviceContext*& devCon = DXM.GetDeviceCon();

	// get game camera and shadow camera
	CameraComponent*& camera      = CM.GetCurrentCameraGame();
	CameraComponent*& cameraLight = CM.GetCurrentCameraDepthMap();

	// get direction light
	LightDirectionComponent*& directionalLight = LM.GetDirectionalLight();

	// create constant buffer structures
	CBAmbDirPixel constantAmbDirPixel;
	CBVertex      constantVertex;

	// set ambient and directional light properties for pixel shader
	XMStoreFloat4(&constantAmbDirPixel.ambientColor,    XMLoadFloat4(&LM.GetAmbientColor()));
	XMStoreFloat4(&constantAmbDirPixel.dirDiffuseColor, XMLoadFloat4(&directionalLight->GetLightColor()));
	XMStoreFloat3(&constantAmbDirPixel.lightDir,        XMLoadFloat3(&directionalLight->GetLightDirectionInv()));

	// update pixel shader constant buffer fro point lights
	SHADER_HELPERS::UpdateConstantBuffer((void*)LM.GetCBPointBuffer(), sizeof(CBPoint) * LM.GetNumPointLights(), _CBPixelPoint);
	SHADER_HELPERS::UpdateConstantBuffer((void*)&constantAmbDirPixel, sizeof(CBAmbDirPixel), _CBPixelAmbDir);

	// set vertex data that all meshes share
	XMStoreFloat4(&constantVertex.clipingPlane, XMLoadFloat4(&clipPlane));
	
	// clear our reflection map render texture and set it to active
	_srv->ClearRenderTarget(0, 0, 0, 1, false);
	_srv->SetRendertarget(false, false);

	// render skybox first
	if (includeSkyBox)
		Systems::renderer->GetSkybox()->Render(true);

	// set opaque blend state
	DXM.BlendStates()->SetBlendState(BLEND_STATE::BLEND_OPAQUE);

	// set shaders that will handle rednering opaque meshes to reflectionmap
	devCon->VSSetShader(_reflectionMapVertexShader, NULL, 0);
	devCon->PSSetShader(_reflectionMapPixelShaderOpaque, NULL, 0);

	// set constant buffer for the vertex and pixel shader
	devCon->VSSetConstantBuffers(0, 1, &_CBVertex);
	devCon->PSSetConstantBuffers(0, 1, &_CBPixelAmbDir);
	devCon->PSSetConstantBuffers(1, 1, &_CBPixelPoint);

	// loop over all opaque meshes that is set to cast reflections
	for (int y = 0; y < opaqueMeshes.size(); y++)
	{
		// get world matrix of mesh and update the buffer
		XMStoreFloat4x4(&constantVertex.world, XMLoadFloat4x4(&opaqueMeshes[y]->GetWorldMatrixTrans()));
		XMStoreFloat4x4(&constantVertex.worldViewProj, XMLoadFloat4x4(&MATH_HELPERS::MatrixMutiplyTrans(&opaqueMeshes[y]->GetWorldMatrix(), &camera->GetViewProjMatrix())));
		SHADER_HELPERS::UpdateConstantBuffer((void*)&constantVertex, sizeof(CBVertex), _CBVertex);

		// get the texture array of mesh
		ID3D11ShaderResourceView** meshTextures = opaqueMeshes[y]->GetTextureArray();

		// specular map is not sent to this shader tho this is not calculated
		ID3D11ShaderResourceView* texArray[3] = { meshTextures[0], meshTextures[1], meshTextures[3] };
		devCon->PSSetShaderResources(0, 3, texArray);

		// upload and draw the mesh
		opaqueMeshes[y]->UploadBuffers();

		devCon->DrawIndexed(opaqueMeshes[y]->GetNumIndices(), 0, 0);
	}

	// set shader and blend state if we have any alpha meshes
	// that is being reflected, also sort these from the pos
	// of the camera reflection matrix
	if (alphaMeshes.size() > 0)
	{
		DXM.BlendStates()->SetBlendState(BLEND_STATE::BLEND_ALPHA);
		devCon->PSSetShader(_reflectionMapPixelShaderAlpha, NULL, 0);
		SHADER_HELPERS::MeshSort(alphaMeshes, camera->GetComponent<TransformComponent>()->GetPositionVal(), true);
	}

	// loop over all alpha meshes that is set to cast reflections
	for (int y = 0; y < alphaMeshes.size(); y++)
	{
		// get world matrix of mesh and update the buffer
		XMStoreFloat4x4(&constantVertex.world,         XMLoadFloat4x4(&alphaMeshes[y]->GetWorldMatrixTrans()));
		XMStoreFloat4x4(&constantVertex.worldViewProj, XMLoadFloat4x4(&MATH_HELPERS::MatrixMutiplyTrans(&alphaMeshes[y]->GetWorldMatrix(), &camera->GetViewProjMatrix())));
		SHADER_HELPERS::UpdateConstantBuffer((void*)&constantVertex, sizeof(CBVertex), _CBVertex);

		// get the texture array of mesh
		ID3D11ShaderResourceView** meshTextures = alphaMeshes[y]->GetTextureArray();

		// specular map is not sent to this shader tho this is not calculated
		ID3D11ShaderResourceView* texArray[2] = { meshTextures[0], meshTextures[1] };
		devCon->PSSetShaderResources(0, 2, texArray);

		// upload and draw the mesh
		alphaMeshes[y]->UploadBuffers();

		devCon->DrawIndexed(alphaMeshes[y]->GetNumIndices(), 0, 0);
	}

	// render all particles to the texture 
	if (includeParticles)
	{
		Renderer& renderer = *Systems::renderer;
		std::vector<ParticleSystemComponent*>& particles = renderer.GetParticles();

		if (particles.size() > 0)
		{
			renderer.GetInputLayouts()->SetInputLayout(INPUT_LAYOUT_TYPE::LAYOUTPARTICLE);
			renderer.GetParticleShader()->RenderParticles(particles);
			renderer.GetInputLayouts()->SetInputLayout(INPUT_LAYOUT_TYPE::LAYOUT3D);
		}
	}
}
