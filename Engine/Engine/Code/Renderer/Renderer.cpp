#include "Renderer.h"
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
#include "DepthShader.h"
#include "DXInputLayouts.h"
#include "DeferredShader.h"
#include "QuadShader.h"
#include "ParticleShader.h"
#include "ImGUIShader.h"
#include "ForwardAlphaShader.h"

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
	// create and compile shaders
	_depthShader        = new DepthShader();
	_deferredShader     = new DeferredShader();
	_quadShader         = new QuadShader();
	_particleShader     = new ParticleShader();
	_imGUIShader        = new ImGUIShader();
	_forwardAlphaShader = new ForwardAlphaShader();

	// create skybox
	_skyBox = new SkyBox(L"Skyboxes/DarkCloudy.dds");

	// create input layouts
	_inputLayouts = new DXInputLayouts();
	_inputLayouts->CreateInputLayout3D(_deferredShader->GetVertexGeometryShaderByteCode());
	_inputLayouts->CreateInputLayout2D(_quadShader->GetVertexShaderByteCode());
	_inputLayouts->CreateInputLayoutParticle(_particleShader->GetVertexShaderByteCode());
	_inputLayouts->CreateInputLayoutGUI(_imGUIShader->GetVertexShaderByteCode());
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

void Renderer::Render() 
{	
	// set input layout for 3dmeshes
	_inputLayouts->SetInputLayout(INPUT_LAYOUT_TYPE::LAYOUT3D);
	
	// render shadowmap
	RenderDepth();

	// render all geometry and then fullscreen quad with lightcalculations
	RenderDeferred();

	// set back to 3d layout after the deferred 2d lightning pass
	_inputLayouts->SetInputLayout(INPUT_LAYOUT_TYPE::LAYOUT3D);	
	
	// render alpha meshes if we have any
	if (_meshes[S_FORWARD_ALPHA].size() > 0)
	{
		AlphaSort();
		_forwardAlphaShader->RenderForward(_meshes[S_FORWARD_ALPHA]);
	}

	// render skybox, will mask out all pixels that contains geometry in the fullscreen quad, leaving only the skybox rendered on "empty" pixels
	_skyBox->Render();
	
	// render particles if we have any
	if (_particleSystems.size() > 0)
	{
		_inputLayouts->SetInputLayout(INPUT_LAYOUT_TYPE::LAYOUTPARTICLE);
		_particleShader->RenderParticles(_particleSystems);
	}
	
	// render 2D quads if we have any
	if (_quads.size() > 0)
	{
		_inputLayouts->SetInputLayout(INPUT_LAYOUT_TYPE::LAYOUT2D);
		_quadShader->RenderQuadUI(_quads);
	}

	_inputLayouts->SetInputLayout(INPUT_LAYOUT_TYPE::LAYOUTGUI);
	_imGUIShader->RenderGUI();
}

void Renderer::RenderDeferred()
{
	DXManager& dXM = DXManager::GetInstance();
		
	// set the rendertargets of the GBuffer active		
	_gBuffer->SetRenderTargets();		

	// render all geometry info to the render targets
	_deferredShader->RenderGeometry(_meshes[S_DEFERRED]);

	// set to 2D input Layout
	_inputLayouts->SetInputLayout(INPUT_LAYOUT_TYPE::LAYOUT2D);

	// set to defualt rendertarget with the depth buffer as read only 
	// so we still can use the depth texture as shader input during the lightning pass
	dXM.SetRenderTarget(nullptr, nullptr, true, true);

	// upload the vertices of the screensized quad
	_screenQuad->UploadBuffers();

	// render lights as 2d post processing
	_deferredShader->RenderLightning(_gBuffer);
}

void Renderer::RenderDepth() 
{
	if (_meshes[S_DEPTH].size() == 0)
		return;

	// get the DX manager
	DXManager& dXM = DXManager::GetInstance();

	// clear the depth map render texture to black
	_depthMap->ClearRenderTarget(0, 0, 0, 0, true);

	// set the depth stencil view active
	dXM.SetRenderTarget(nullptr, _depthMap->GetDepthStencil());

	// set the viewport of the camera that renders the depth
	dXM.SetViewport(_depthMap->GetViewport(), false);

	// render all meshes 
	_depthShader->RenderDepth(_meshes[S_DEPTH]);
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





