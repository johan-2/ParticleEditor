#include "Renderer.h"
#include "ShaderManager.h"
#include "DXManager.h"
#include "TransformComponent.h"
#include "CameraComponent.h"
#include "CameraManager.h"
#include "Entity.h"
#include "RenderToTexture.h"
#include "QuadComponent.h"
#include "SkyBox.h"
#include "Mesh.h"
#include "SystemDefs.h"
#include <algorithm>
#include <DirectXMath.h>
#include "GBuffer.h"
#include "ScreenQuad.h"

using namespace DirectX;

Renderer* Renderer::_instance = nullptr;

Renderer& Renderer::GetInstance() 
{
	if (_instance == nullptr)
		_instance = new Renderer();

	return *_instance;
}

Renderer::Renderer()
{
}

Renderer::~Renderer()
{
	delete _depthMap;
	delete _gBuffer;
	delete _screenQuad;
	delete _skyBox;
}

void Renderer::CreateDepthMap() 
{
	// depthmap settings
	const float orthoSize = 80;
	const float res = 8192.0f;

	// create depthmap rendertexturepositionQuad
	_depthMap = new RenderToTexture(res, res, true);
	
	// create camera to render depth and give it a reference to the render texture it will use
	_cameraDepth = new Entity();
	_cameraDepth->AddComponent<TransformComponent>()->Init(XMFLOAT3(0.0f, 30.0f, -41.0f), XMFLOAT3(45.0f, 0.0f, 0));
	_cameraDepth->AddComponent<CameraComponent>()->Init2D(XMFLOAT2(orthoSize, orthoSize), XMFLOAT2(0.01f, 1000.0f));

	// give camera a reference to the shaderResource in depthmap
	_cameraDepth->GetComponent<CameraComponent>()->SetSRV(_depthMap->GetShaderResource());
	CameraManager::GetInstance().SetCurrentCameraDepthMap(_cameraDepth->GetComponent<CameraComponent>());

	// create gbuffer for deffered rendering
	_gBuffer = new GBuffer();

	// create fullscreenquad for deferred rendering
	_screenQuad = new ScreenQuad();

	// create skybox
	_skyBox = new SkyBox(L"Skyboxes/DarkCloudy.dds");

	// create quads that can render a preview of all render textures
#ifdef _DEBUG
	Entity* depthMapQuad = new Entity();
	depthMapQuad->AddComponent<QuadComponent>()->Init(XMFLOAT2(SCREEN_WIDTH * 0.06f, SCREEN_HEIGHT * 0.1f), XMFLOAT2(SCREEN_WIDTH * 0.08f, SCREEN_WIDTH * 0.08f), L"");
	depthMapQuad->GetComponent<QuadComponent>()->SetTexture(_depthMap->GetShaderResource());

	Entity* positionQuad = new Entity();
	positionQuad->AddComponent<QuadComponent>()->Init(XMFLOAT2(SCREEN_WIDTH * 0.18f, SCREEN_HEIGHT * 0.1f), XMFLOAT2(SCREEN_WIDTH * 0.1f, SCREEN_HEIGHT * 0.1f), L"");
	positionQuad->GetComponent<QuadComponent>()->SetTexture(_gBuffer->GetSrvArray()[0]);

	Entity* normalQuad = new Entity();
	normalQuad->AddComponent<QuadComponent>()->Init(XMFLOAT2(SCREEN_WIDTH * 0.30f, SCREEN_HEIGHT * 0.1f), XMFLOAT2(SCREEN_WIDTH * 0.1f, SCREEN_HEIGHT * 0.1f), L"");
	normalQuad->GetComponent<QuadComponent>()->SetTexture(_gBuffer->GetSrvArray()[1]);

	Entity* DiffuseQuad = new Entity();
	DiffuseQuad->AddComponent<QuadComponent>()->Init(XMFLOAT2(SCREEN_WIDTH * 0.42f, SCREEN_HEIGHT * 0.1f), XMFLOAT2(SCREEN_WIDTH * 0.1f, SCREEN_HEIGHT * 0.1f), L"");
	DiffuseQuad->GetComponent<QuadComponent>()->SetTexture(_gBuffer->GetSrvArray()[2]);

	Entity* specularQuad = new Entity();
	specularQuad->AddComponent<QuadComponent>()->Init(XMFLOAT2(SCREEN_WIDTH * 0.54f, SCREEN_HEIGHT * 0.1f), XMFLOAT2(SCREEN_WIDTH * 0.1f, SCREEN_HEIGHT * 0.1f), L"");
	specularQuad->GetComponent<QuadComponent>()->SetTexture(_gBuffer->GetSrvArray()[3]);

#endif	
}

void Renderer::AddToRenderer(Mesh* mesh, SHADER_TYPE type) 
{
	_meshes[type].push_back(mesh);
}

void Renderer::RemoveFromRenderer(Mesh* mesh, SHADER_TYPE type) 
{
	for (int i = 0; i < _meshes[type].size(); i++) 
	{
		if (mesh == _meshes[type][i]) 
		{
			Mesh* temp = _meshes[type].back();
			_meshes[type].back() = _meshes[type][i];
			_meshes[type][i] = temp;

			_meshes[type].pop_back();
		}
	}
}

void Renderer::AddParticleSystem(ParticleSystemComponent* emitter)
{
	_particleSystems.push_back(emitter);
}

void Renderer::RemoveParticleSystem(ParticleSystemComponent* emitter)
{
	for (int i = 0; i < _particleSystems.size(); i++)
	{
		if (emitter == _particleSystems[i])
		{
			ParticleSystemComponent* temp = _particleSystems.back();
			_particleSystems.back() = _particleSystems[i];
			_particleSystems[i] = temp;
			_particleSystems.pop_back();
		}
	}
}

void Renderer::AddQuadToUIRenderer(QuadComponent* quad) 
{
	_quads.push_back(quad);
}

void Renderer::RemoveQuadFromUIRenderer(QuadComponent* quad) 
{
	for (int i = 0; i < _quads.size(); i++) 
	{
		if (quad == _quads[i]) 
		{
			QuadComponent* temp = _quads.back();
			_quads.back() = _quads[i];
			_quads[i] = temp;
			_quads.pop_back();
		}
	}
}

void Renderer::Render() 
{	
	ShaderManager& SM = ShaderManager::GetInstance();

	// set input layout for 3dmeshes
	SM.SetInputLayout(INPUT_LAYOUT_TYPE::LAYOUT3D);
	
	// set defualt constantbuffers (everything uses these exept for the deferred rendering that has its own buffers) 
	SM.SetConstantBuffers();

	// render shadowmap
	RenderDepth();

	// render all geometry and then fullscreen quad with lightcalculations
	RenderDeferred();

	// set back regular constantbuffers and render alpha meshes with regular forward rendering
	SM.SetInputLayout(INPUT_LAYOUT_TYPE::LAYOUT3D);
	SM.SetConstantBuffers();
	RenderLightsAlpha();	

	// render skybox, will mask out all pixels that contains geometry in the fullscreen quad, leaving only the skybox rendered on "empty" pixels
	_skyBox->Render();
	
	// render particles (currently alpha meshes and particles cant be rendered perfect together, need to find solution for this)
	SM.SetInputLayout(INPUT_LAYOUT_TYPE::LAYOUTPARTICLE);
	RenderParticles();
	
	// set inputlayout for UI
	SM.SetInputLayout(INPUT_LAYOUT_TYPE::LAYOUT2D);
	RenderUI();		
}

void Renderer::RenderDeferred()
{
	ShaderManager& SM = ShaderManager::GetInstance();
	DXManager& dXM = DXManager::GetInstance();
		
	// set gbuffer and render the geometry data to the buffers		
	_gBuffer->SetRenderTargets();		
	SM.RenderGeometry(_meshes[S_DEFERRED]);

	SM.SetInputLayout(INPUT_LAYOUT_TYPE::LAYOUT2D);
	// set to defualt rendertarget with the depth buffer as read only so we still can use the depth texture as shader input
	// TODO: recontruct position from depth so we can remove the position render target from the g-buffer completely
	dXM.SetRenderTarget(nullptr, nullptr, true, true);

	// upload the vertices of the screensized quad
	_screenQuad->UploadBuffers();

	// render lights as 2d post processing
	SM.RenderLights(_gBuffer); 
}

void Renderer::RenderDepth() 
{
	if (_meshes[S_DEPTH].size() == 0)
		return;

	DXManager&    dXM = DXManager::GetInstance();
	ShaderManager& SM = ShaderManager::GetInstance();

	// set depth stuff
	_depthMap->ClearRenderTarget(0, 0, 0, 0, true);
	dXM.SetRenderTarget(nullptr, _depthMap->GetDepthStencil());
	dXM.SetViewport(_depthMap->GetViewport(), false);

	SM.RenderDepth(_meshes[S_DEPTH]);
}

void Renderer::RenderLightsAlpha() 
{
	if (_meshes[S_FORWARD_ALPHA].size() == 0)
		return;

	ShaderManager& SM = ShaderManager::GetInstance();

	AlphaSort();
	
}

void Renderer::RenderParticles()
{
	if (_particleSystems.size() == 0)
		return;

	ShaderManager& SM = ShaderManager::GetInstance();
	SM.RenderParticles(_particleSystems);
}

void Renderer::RenderUI() 
{
	if (_quads.size() == 0)
		return;

	ShaderManager& SM = ShaderManager::GetInstance();
	SM.RenderQuadUI(_quads);
}

void Renderer::AlphaSort() 
{
	// right now alpha meshes and particles are not sorted against each other, Need to find a solution for this
	const XMFLOAT3& camPos = CameraManager::GetInstance().GetCurrentCameraGame()->GetComponent<TransformComponent>()->GetPositionRef();
	for (int i = 0; i < _meshes[S_FORWARD_ALPHA].size(); i++)
	{
		XMFLOAT3 vec;
		float distance = 0;

		XMStoreFloat3(&vec, XMVectorSubtract(XMLoadFloat3(&_meshes[S_FORWARD_ALPHA][i]->GetPosition()), XMLoadFloat3(&camPos)));
		XMStoreFloat(&distance, XMVector3Length(XMLoadFloat3(&vec)));

		_meshes[S_FORWARD_ALPHA][i]->SetDistanceToCamera(distance);
	}

	std::sort(_meshes[S_FORWARD_ALPHA].begin(), _meshes[S_FORWARD_ALPHA].end(), [](Mesh* a, Mesh* b) -> bool {return a->GetDistanceFromCamera() > b->GetDistanceFromCamera(); });
}





