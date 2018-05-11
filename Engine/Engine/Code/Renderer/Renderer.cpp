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
}

void Renderer::CreateDepthMap() 
{
	// depthmap settings
	const float orthoSize = 80;
	const float res = 8192.0f;

	// create depthmap rendertexture
	_depthMap = new RenderToTexture(res, res, true);

	// create camera to render depth and give it a reference to the render texture it will use
	_cameraDepth = new Entity();
	_cameraDepth->AddComponent<TransformComponent>()->Init(XMFLOAT3(0.0f, 30.0f, -41.0f), XMFLOAT3(45.0f, 0.0f, 0));
	CameraComponent* camDepth = _cameraDepth->AddComponent<CameraComponent>(); camDepth->Init2D(XMFLOAT2(orthoSize, orthoSize), XMFLOAT2(0.01f, 1000.0f));

	// give camera a reference to the shaderResource in depthmap
	camDepth->SetRenderTexture(_depthMap->GetShaderResource());
	CameraManager::GetInstance().SetCurrentCameraDepthMap(camDepth);

	Entity* UIQuad1 = new Entity();
	QuadComponent* q = UIQuad1->AddComponent<QuadComponent>(); q->Init(XMFLOAT2(SCREEN_WIDTH * 0.06f, SCREEN_HEIGHT * 0.1f), XMFLOAT2(SCREEN_WIDTH * 0.1f, SCREEN_WIDTH * 0.1f), L"");
	q->SetTexture(_depthMap->GetShaderResource());

	_skyBox = new SkyBox(L"Skyboxes/ThickCloudsWater.dds");
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

void Renderer::AddToAlphaMeshes(Mesh* mesh) 
{
	_meshesAlpha.push_back(mesh);
}

void Renderer::RemoveFromAlphaMeshes(Mesh* mesh) 
{
	for (int i = 0; i < _meshesAlpha.size(); i++)
	{
		if (mesh == _meshesAlpha[i])
		{
			Mesh* temp = _meshesAlpha.back();
			_meshesAlpha.back() = _meshesAlpha[i];
			_meshesAlpha[i] = temp;
			_meshesAlpha.pop_back();
		}
	}
}

void Renderer::AddParticleEmitter(ParticleEmitterComponent* emitter)
{
	_particleEmitters.push_back(emitter);
}

void Renderer::RemoveParticleEmitter(ParticleEmitterComponent* emitter)
{
	for (int i = 0; i < _particleEmitters.size(); i++)
	{
		if (emitter == _particleEmitters[i])
		{
			ParticleEmitterComponent* temp = _particleEmitters.back();
			_particleEmitters.back() = _particleEmitters[i];
			_particleEmitters[i] = temp;
			_particleEmitters.pop_back();
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

	// constantbuffers need to be set once per frame, set inputlayout for 3d meshes
	SM.SetConstantBuffers();
	SM.SetInputLayout(INPUT_LAYOUT_TYPE::LAYOUT3D);

	// render 3D
	_skyBox->Render();
	RenderDepth();
	RenderLights();		
	RenderLightsAlpha();

	SM.SetInputLayout(INPUT_LAYOUT_TYPE::LAYOUTPARTICLE);
	RenderParticles();

	// set inputlayout for UI
	SM.SetInputLayout(INPUT_LAYOUT_TYPE::LAYOUT2D);
	RenderUI();
}

void Renderer::RenderDepth() 
{
	if (_meshes[S_DEPTH].size() == 0)
		return;

	DXManager& dXM = DXManager::GetInstance();
	ShaderManager& SM = ShaderManager::GetInstance();

	// set depth stuff
	_depthMap->ClearRenderTarget(0, 0, 0, 0, true);
	dXM.SetRenderTarget(nullptr, _depthMap->GetDepthStencil());
	dXM.SetViewport(_depthMap->GetViewport(), false);

	SM.RenderDepth(_meshes[S_DEPTH]);
}

void Renderer::RenderLights() 
{
	DXManager& dXM = DXManager::GetInstance();
	ShaderManager& SM = ShaderManager::GetInstance();

	// set to defualt rendertargets and viewport
	dXM.SetRenderTarget(nullptr, nullptr, true);
	dXM.SetViewport(nullptr, true);

	if (_meshes[S_AMBIENT].size() >0)
	    SM.RenderAmbient(_meshes[S_AMBIENT]);

	if (_meshes[S_DIRECTIONAL].size() >0)
	    SM.RenderDirectional(_meshes[S_DIRECTIONAL]);

	if (_meshes[S_POINT].size() >0)
		SM.RenderPoint(_meshes[S_POINT]);

	if (_meshes[S_DIRECTIONAL_SHADOWS].size() >0)
		SM.RenderDirectionalShadows(_meshes[S_DIRECTIONAL_SHADOWS]);	
}

void Renderer::RenderLightsAlpha() 
{
	if (_meshesAlpha.size() == 0)
		return;

	ShaderManager& SM = ShaderManager::GetInstance();

	AlphaSort();

	for (int i = 0; i < _meshesAlpha.size(); i++) 
	{
		unsigned int flags = _meshesAlpha[i]->GetFlags();

		// very bad temporary solution, the renderfunction takes a list of all meshes for each shader and renders in a batch
		// but alpha meshes need to take one mesh and render it in all shaders before moving on to the next and doing the same thing(if not, shadows etc wont show throght alpha layers)
		// since the renderfunction expects a list just put one mesh in a list at a time and send it in, will fix a better solution for this later
		std::vector<Mesh*> temp;
		temp.push_back(_meshesAlpha[i]);

		if ((flags & LIGHTS_ALL) == LIGHTS_ALL) 
		{						
			SM.RenderAmbient(temp);
			SM.RenderDirectionalShadows(temp);
		}
		if ((flags & AMBIENT) == AMBIENT)		
			SM.RenderAmbient(temp);

		if ((flags & DIRECTIONAL) == DIRECTIONAL)
			SM.RenderDirectional(temp);

		if ((flags & RECIVE_SHADOW_DIR) == RECIVE_SHADOW_DIR)
			SM.RenderDirectionalShadows(temp);			
	}
}

void Renderer::RenderParticles()
{
	if (_particleEmitters.size() == 0)
		return;

	ShaderManager& SM = ShaderManager::GetInstance();
	SM.RenderParticles(_particleEmitters);
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
	XMFLOAT3 camPos = CameraManager::GetInstance().GetCurrentCameraGame()->GetComponent<TransformComponent>()->GetPositionVal();
	for (int i = 0; i < _meshesAlpha.size(); i++) 
	{
		XMFLOAT3 vec;

		XMStoreFloat3(&vec, XMVectorSubtract(XMLoadFloat3(&_meshesAlpha[i]->GetPosition()), XMLoadFloat3(&camPos)));
		XMStoreFloat(&_meshesAlpha[i]->_distance, XMVector3Length(XMLoadFloat3(&vec)));
	}

	std::sort(_meshesAlpha.begin(), _meshesAlpha.end(), [](Mesh* a, Mesh* b) -> bool {return a->_distance > b->_distance; });	
}





