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
#include "DXRasterizerStates.h"
#include "WireframeShader.h"
#include "Input.h"

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
	delete _depthShader;
	delete _deferredShader;
	delete _quadShader;
	delete _particleShader;
	delete _imGUIShader;
	delete _forwardAlphaShader;
	delete _inputLayouts;

	_cameraDepth->RemoveEntity();
}

void Renderer::Initailize()
{
	// create and compile shaders
	_depthShader        = new DepthShader();
	_deferredShader     = new DeferredShader();
	_quadShader         = new QuadShader();
	_particleShader     = new ParticleShader();
	_imGUIShader        = new ImGUIShader();
	_forwardAlphaShader = new ForwardAlphaShader();
	_wireframeShader    = new WireframeShader();

	// create skybox
	_skyBox = new SkyBox(L"Skyboxes/EmptySpace.dds");

	// create input layouts
	_inputLayouts = new DXInputLayouts();
	_inputLayouts->CreateInputLayout3D(_deferredShader->GetVertexGeometryShaderByteCode());
	_inputLayouts->CreateInputLayout2D(_quadShader->GetVertexShaderByteCode());
	_inputLayouts->CreateInputLayoutParticle(_particleShader->GetVertexShaderByteCode());
	_inputLayouts->CreateInputLayoutGUI(_imGUIShader->GetVertexShaderByteCode());

	// create gbuffer for deffered rendering
	_gBuffer = new GBuffer();

	// create fullscreenquad for deferred rendering
	_screenQuad = new ScreenQuad();

	CreateDepthMap();
}

void Renderer::CreateDepthMap() 
{
	CameraManager& CM = CameraManager::GetInstance();

	// depthmap settings
	const float orthoSize = 80;
	const float res = 8192.0f;

	// create depthmap render texture
	_depthMap = new RenderToTexture(res, res, true);

	// create camera entity with orthographic view for shadowmap rendering
	_cameraDepth = new Entity();
	_cameraDepth->AddComponent<TransformComponent>()->Init(XMFLOAT3(0.0f, 30.0f, -41.0f), XMFLOAT3(45.0f, 0.0f, 0));
	_cameraDepth->AddComponent<CameraComponent>()->Init2D(XMFLOAT2(orthoSize, orthoSize), XMFLOAT2(0.01f, 1000.0f));

	// get pointer to camera component
	CameraComponent* depthCamera = _cameraDepth->GetComponent<CameraComponent>();

	// give camera a reference to the SRV in depthMap render texture
	depthCamera->SetSRV(_depthMap->GetShaderResource());

	// set this camera to the active depth render camera
	CM.SetCurrentCameraDepthMap(depthCamera);
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

	// render skybox, will mask out all pixels that contains geometry in the fullscreen quad, leaving only the skybox rendered on "empty" pixels
	_skyBox->Render();

	// render debug wireframe meshes, these are forward rendered
	_wireframeShader->RenderWireFrame(_meshes[S_WIREFRAME]);

	// render particles
	_inputLayouts->SetInputLayout(INPUT_LAYOUT_TYPE::LAYOUTPARTICLE);
	_particleShader->RenderParticles(_particleSystems);
		
	// set to 3D layout and render alpha meshes
	_inputLayouts->SetInputLayout(INPUT_LAYOUT_TYPE::LAYOUT3D);
	_forwardAlphaShader->RenderForward(_meshes[S_FORWARD_ALPHA]);

	// render UI quads
	_inputLayouts->SetInputLayout(INPUT_LAYOUT_TYPE::LAYOUT2D);
	_quadShader->RenderQuadUI(_quads);
	
	// render IM GUI
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

	// set to defualt rendertarget 
	dXM.SetRenderTarget(nullptr, nullptr, true, false);

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






