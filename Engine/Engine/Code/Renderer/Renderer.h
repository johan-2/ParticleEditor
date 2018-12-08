#pragma once
#include <vector>
#include "VectorHelpers.h"
#include "DXInputLayouts.h"

// forward declare types
class Mesh;
class QuadComponent;
class RenderToTexture;
class Entity;
class SkyBox;
class ParticleSystemComponent;
class GBuffer;
class ScreenQuad;
class DepthShader;
class DeferredShader;
class QuadShader;
class ParticleShader;
class ImGUIShader;
class ForwardAlphaShader;
class WireframeShader;
class PlanarReflectionShader;
class PostProcessingShader;

// the different render lists we have
enum SHADER_TYPE
{	
	S_DEFERRED,
	S_DEPTH,
	S_FORWARD_ALPHA,
	S_WIREFRAME,
	S_CAST_REFLECTION,
	S_ALPHA_REFLECTION,
	S_NUM_RENDER_TYPES,
};

class Renderer
{
public:
	Renderer();
	~Renderer();

	// add objects to respective renderers
	void AddMeshToRenderer(Mesh* mesh, SHADER_TYPE type)               { _meshes[type].push_back(mesh); }
	void AddQuadToRenderer(QuadComponent* quad)                        { _quads.push_back(quad); }
	void AddParticleSystemToRenderer(ParticleSystemComponent* emitter) { _particleSystems.push_back(emitter); }

	// remove objects from respective renderers
	void RemoveMeshFromRenderer(Mesh* mesh, SHADER_TYPE type)               { VECTOR_HELPERS::RemoveItemFromVector(_meshes[type], mesh); }
	void RemoveQuadFromRenderer(QuadComponent* quad)                        { VECTOR_HELPERS::RemoveItemFromVector(_quads, quad); }
	void RemoveParticleSystemFromRenderer(ParticleSystemComponent* emitter) { VECTOR_HELPERS::RemoveItemFromVector(_particleSystems, emitter); }
	
	// returns skybox
	SkyBox* GetSkybox() { return _skyBox; }

	void SetClearColor(float r, float g, float b, float a) { _clearColor[0] = r; _clearColor[1] = g; _clearColor[2] = b; _clearColor[3] = a;  }

	void SetMainRenderTarget(); 

	// initialize everything
	void Initailize();
	
	// will render everything
	void Render();

	// set a input layout
	void SetInputLayout(INPUT_LAYOUT_TYPE type) { _inputLayouts->SetInputLayout(type); }
	
private:

	// setupFunctions
	void CreateDepthMap();
	void CreateDebugImages();

	// render functions
	void RenderDeferred();
	void RenderDepth();

	// shader "programs" that will handle all preperations
	// for rendering with a specific shader
	DepthShader*            _depthShader;
	DeferredShader*         _deferredShader;
	QuadShader*             _quadShader;
	ParticleShader*         _particleShader;
	ImGUIShader*            _imGUIShader;
	ForwardAlphaShader*     _forwardAlphaShader;
	WireframeShader*        _wireframeShader;
	PlanarReflectionShader* _planarReflectionShader;
	PostProcessingShader*   _PostProcessingShader;

	// skybox class with all rendering built in
	SkyBox* _skyBox;

	// contains all input layouts
	DXInputLayouts* _inputLayouts;

	// list of meshes for each shader type
	// one mesh can be added to several shader types
	std::vector<Mesh*> _meshes[S_NUM_RENDER_TYPES];	

	// all 2D UI quads
	std::vector<QuadComponent*> _quads;

	// all particle systems
	std::vector<ParticleSystemComponent*> _particleSystems;

	// the camera entity that renders the depth map for shadows
	Entity* _cameraDepth;

	// the render texure that the depth camera renders to
	RenderToTexture* _depthMap;
	RenderToTexture* _mainRendertarget;

	// the Gbuffer for deferred rendering
	// and the fullscreen quad where we will 
	// project the deferred lightningpass
	GBuffer*    _gBuffer;
	ScreenQuad* _fullScreenQuad;

	float _clearColor[4];
};

