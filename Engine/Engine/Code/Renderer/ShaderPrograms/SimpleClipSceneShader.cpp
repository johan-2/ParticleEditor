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

SimpleClipSceneShader::SimpleClipSceneShader(bool debugQuad)
{
	// create shaders
	SHADER_HELPERS::CreateVertexShader(L"shaders/vertexSimpleClip.vs", _vertexShader, _vertexShaderByteCode);
	SHADER_HELPERS::CreatePixelShader(L"shaders/pixelSimpleClip.ps",   _pixelShader,  _pixelShaderByteCodeOpaque);

	// create constant buffers
	SHADER_HELPERS::CreateConstantBuffer(_CBVertex);

	// create render texture
	_renderTexture = new RenderToTexture((unsigned int)SystemSettings::SCREEN_WIDTH, (unsigned int)SystemSettings::SCREEN_HEIGHT, false, SystemSettings::USE_HDR);

	if (debugQuad)
	{
		// create debug quad
		Entity* reflectionQuad = new Entity();
		reflectionQuad->AddComponent<QuadComponent>()->Init(XMFLOAT2(SystemSettings::SCREEN_WIDTH * 0.66f, SystemSettings::SCREEN_HEIGHT * 0.1f), XMFLOAT2(SystemSettings::SCREEN_WIDTH * 0.1f, SystemSettings::SCREEN_HEIGHT * 0.1f), L"");
		reflectionQuad->GetComponent<QuadComponent>()->SetTexture(_renderTexture->GetRenderTargetSRV());
	}
}

SimpleClipSceneShader::~SimpleClipSceneShader()
{
	_vertexShader->Release();
	_pixelShader->Release();

	_vertexShaderByteCode->Release();
	_pixelShaderByteCodeOpaque->Release();

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

	// set vertex data that all meshes share
	CBVertex constantVertex;
	XMStoreFloat4(&constantVertex.clipingPlane, XMLoadFloat4(&clipPlane));
	
	// clear our reflection map render texture and set it to active
	_renderTexture->ClearRenderTarget(0, 0, 0, 1, false);
	_renderTexture->SetRendertarget(false, false);

	// render skybox first
	if (includeSkyBox)
		Systems::renderer->GetSkyDome()->Render(true);

	// set opaque blend state
	DXM.BlendStates()->SetBlendState(BLEND_STATE::BLEND_OPAQUE);

	// set shaders that will handle rednering opaque meshes to reflectionmap
	devCon->VSSetShader(_vertexShader, NULL, 0);
	devCon->PSSetShader(_pixelShader, NULL, 0);

	ID3D11Buffer* pointBuffer = LM.GetPointLightCB();
	ID3D11Buffer* ambDirBuffer = LM.GetAmbDirLightCB();

	// set constant buffer for the vertex and pixel shader
	devCon->VSSetConstantBuffers(0, 1, &_CBVertex);
	devCon->PSSetConstantBuffers(0, 1, &ambDirBuffer);
	devCon->PSSetConstantBuffers(1, 1, &pointBuffer);

	// loop over all opaque meshes that is set to cast reflections
	size_t size = opaqueMeshes.size();
	for (int y = 0; y < size; y++)
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

	// unbind so we can use resources as input in next stages
	ID3D11ShaderResourceView* nullSRV[3] = { NULL, NULL, NULL };
	devCon->PSSetShaderResources(0, 3, nullSRV);

	// set shader and blend state if we have any alpha meshes
	// that is being reflected, also sort these from the pos
	// of the camera reflection matrix
	size = alphaMeshes.size();
	if (size > 0)
	{
		DXM.BlendStates()->SetBlendState(BLEND_STATE::BLEND_ALPHA);
		SHADER_HELPERS::MeshSort(alphaMeshes, camera->GetComponent<TransformComponent>()->GetPositionVal(), true);
	}

	// loop over all alpha meshes that is set to cast reflections
	for (int y = 0; y < size; y++)
	{
		// get world matrix of mesh and update the buffer
		XMStoreFloat4x4(&constantVertex.world,         XMLoadFloat4x4(&alphaMeshes[y]->GetWorldMatrixTrans()));
		XMStoreFloat4x4(&constantVertex.worldViewProj, XMLoadFloat4x4(&MATH_HELPERS::MatrixMutiplyTrans(&alphaMeshes[y]->GetWorldMatrix(), &camera->GetViewProjMatrix())));
		SHADER_HELPERS::UpdateConstantBuffer((void*)&constantVertex, sizeof(CBVertex), _CBVertex);

		// get the texture array of mesh
		ID3D11ShaderResourceView** meshTextures = alphaMeshes[y]->GetTextureArray();

		// specular map is not sent to this shader tho this is not calculated
		ID3D11ShaderResourceView* texArray[3] = { meshTextures[0], meshTextures[1], meshTextures[3] };
		devCon->PSSetShaderResources(0, 3, texArray);

		// upload and draw the mesh
		alphaMeshes[y]->UploadBuffers();

		devCon->DrawIndexed(alphaMeshes[y]->GetNumIndices(), 0, 0);
	}

	devCon->PSSetShaderResources(0, 3, nullSRV);

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
