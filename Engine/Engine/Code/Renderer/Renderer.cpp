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
#include "DeferredShader.h"
#include "QuadShader.h"
#include "ParticleShader.h"
#include "ImGUIShader.h"
#include "ForwardAlphaShader.h"
#include "DXRasterizerStates.h"
#include "WireframeShader.h"
#include "Input.h"
#include "Systems.h"
#include "FreeMoveComponent.h"
#include "PlanarReflectionShader.h"
#include "PostProcessingShader.h"

using namespace DirectX;

Renderer::Renderer()
{
}

Renderer::~Renderer()
{
	delete _depthMap;
	delete _gBuffer;
	delete _fullScreenQuad;
	delete _skyBox;
	delete _depthShader;
	delete _deferredShader;
	delete _quadShader;
	delete _particleShader;
	delete _imGUIShader;
	delete _forwardAlphaShader;
	delete _inputLayouts;
	delete _planarReflectionShader;
}

void Renderer::Initailize()
{
	// set clear color
	SetClearColor(0, 0, 0, 1);

	// create and compile shaders
	_depthShader            = new DepthShader();
	_deferredShader         = new DeferredShader();
	_quadShader             = new QuadShader();
	_particleShader         = new ParticleShader();
	_imGUIShader            = new ImGUIShader();
	_forwardAlphaShader     = new ForwardAlphaShader();
	_wireframeShader        = new WireframeShader();
	_planarReflectionShader = new PlanarReflectionShader();
	_PostProcessingShader            = new PostProcessingShader();

	// create skybox
	_skyBox = new SkyBox(L"Skyboxes/ThickCloudsWater.dds", SKY_DOME_RENDER_MODE::CUBEMAP_COLOR_BLEND);

	// create input layouts
	_inputLayouts = new DXInputLayouts();
	_inputLayouts->CreateInputLayout3D(_deferredShader->GetVertexGeometryShaderByteCode());
	_inputLayouts->CreateInputLayout2D(_quadShader->GetVertexShaderByteCode());
	_inputLayouts->CreateInputLayoutParticle(_particleShader->GetVertexShaderByteCode());
	_inputLayouts->CreateInputLayoutGUI(_imGUIShader->GetVertexShaderByteCode());

	// create gbuffer for deffered rendering
	_gBuffer = new GBuffer();

	// create fullscreenquad for deferred rendering 
	// and to project the final scene image after post processing
	_fullScreenQuad = new ScreenQuad();

	_mainRendertarget = new RenderToTexture(SCREEN_WIDTH, SCREEN_HEIGHT, false);

	CreateDepthMap();

	_skyBox->SetSunDirectionTransformPtr(_cameraDepth->GetComponent<TransformComponent>());
	_skyBox->SetSunDistance(5.0f);
	_skyBox->SetThreeLayerColorBlendSettings(XMFLOAT4(10,  10,  10,  30), XMFLOAT4(238, 105, 49,  40), XMFLOAT4(21,  90,  251, 50));
	_skyBox->SetCubeMapColorBlendSettings(XMFLOAT4(21, 90, 251, 255), XMFLOAT4(199, 176, 135, 255), 40, 55, 80, false);

#ifdef _DEBUG				   
	CreateDebugImages();
#endif
}

void Renderer::CreateDepthMap() 
{
	// depthmap settings
	const float orthoSize = 250;
	const float res       = 8192.0f;

	// create depthmap render texture
	_depthMap = new RenderToTexture(res, res, true);

	// create camera entity with orthographic view for shadowmap rendering
	_cameraDepth = new Entity();
	_cameraDepth->AddComponent<TransformComponent>()->Init(XMFLOAT3(-6, 325, 9), XMFLOAT3(85.0f, -90.0f, 0));
	_cameraDepth->AddComponent<CameraComponent>()->Init2D(XMFLOAT2(orthoSize, orthoSize), XMFLOAT2(0.01f, 5000.0f));
	_cameraDepth->AddComponent<FreeMoveComponent>()->init(80, 0.1f);

	// start the free moce component of shadow camera inactive
	_cameraDepth->GetComponent<FreeMoveComponent>()->SetActive(false);

	// get pointer to camera component
	CameraComponent* depthCamera = _cameraDepth->GetComponent<CameraComponent>();

	// give camera a reference to the SRV in depthMap render texture
	depthCamera->SetSRV(_depthMap->GetDepthStencilSRV());

	// set this camera to the active depth render camera
	Systems::cameraManager->SetCurrentCameraDepthMap(depthCamera);
}

void Renderer::CreateDebugImages()
{
	// create debug images to show each texture in the G buffer and the depth map
	Entity* shadowMapQuad = new Entity();
	shadowMapQuad->AddComponent<QuadComponent>()->Init(XMFLOAT2(SCREEN_WIDTH * 0.06f, SCREEN_HEIGHT * 0.1f), XMFLOAT2(SCREEN_WIDTH * 0.08f, SCREEN_WIDTH * 0.08f), L"");
	shadowMapQuad->GetComponent<QuadComponent>()->SetTexture(_depthMap->GetDepthStencilSRV());

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
}

void Renderer::Render() 
{	
	// get dx manager
	DXManager& dXM = *Systems::dxManager;

	// clear the backbuffer rendertarget
	dXM.ClearRenderTarget(_clearColor[0], _clearColor[1], _clearColor[2], _clearColor[3]);
	_mainRendertarget->ClearRenderTarget(_clearColor[0], _clearColor[1], _clearColor[2], _clearColor[3], false);

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

	// render planar reflections forward
	_planarReflectionShader->Render(_meshes[S_ALPHA_REFLECTION], _meshes[S_CAST_REFLECTION], _particleSystems, _particleShader, _inputLayouts);

	// TODO: alpha meshes and particles is not sorted against each other
	// either sort them and send one object at a time to the shaders or
	// research to see if there are a more elegant solution to this problem

	// render particles
	_inputLayouts->SetInputLayout(INPUT_LAYOUT_TYPE::LAYOUTPARTICLE);
	_particleShader->RenderParticles(_particleSystems);
		
	// set to 3D layout and render alpha meshes forward
	_inputLayouts->SetInputLayout(INPUT_LAYOUT_TYPE::LAYOUT3D);
	_forwardAlphaShader->RenderForward(_meshes[S_FORWARD_ALPHA]);

	// render the final 2d stuff 
	_inputLayouts->SetInputLayout(INPUT_LAYOUT_TYPE::LAYOUT2D);

	// Render post processing 
	_PostProcessingShader->Render(_fullScreenQuad, _mainRendertarget->GetRenderTargetSRV());

	// render UI
	_quadShader->RenderQuadUI(_quads);
	
	// render IM GUI
	_inputLayouts->SetInputLayout(INPUT_LAYOUT_TYPE::LAYOUTGUI);
	_imGUIShader->RenderGUI();
}

void Renderer::RenderDeferred()
{
	DXManager& dXM = *Systems::dxManager;
		
	// set the rendertargets of the GBuffer active		
	_gBuffer->SetRenderTargets(_mainRendertarget->GetDepthStencil());		

	// render all geometry info to the render targets
	_deferredShader->RenderGeometry(_meshes[S_DEFERRED]);

	// set to 2D input Layout
	_inputLayouts->SetInputLayout(INPUT_LAYOUT_TYPE::LAYOUT2D);

	// set to defualt rendertarget 
	_mainRendertarget->SetRendertarget(false, false);

	// upload the vertices of the screensized quad
	_fullScreenQuad->UploadBuffers();

	// render lights as 2d post processing
	_deferredShader->RenderLightning(_gBuffer);
}

void Renderer::RenderDepth() 
{
	if (_meshes[S_DEPTH].size() == 0)
		return;

	// get the DX manager
	DXManager& dXM = *Systems::dxManager;

	// clear the depth map render texture to black
	_depthMap->ClearRenderTarget(0, 0, 0, 0, true);

	_depthMap->SetRendertarget(true, false);

	// set the viewport of the camera that renders the depth
	dXM.SetViewport(_depthMap->GetViewport(), false);

	// render all meshes 
	_depthShader->RenderDepth(_meshes[S_DEPTH]);
}

void Renderer::SetMainRenderTarget()
{
	_mainRendertarget->SetRendertarget(false, false); 
}