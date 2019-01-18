#pragma once
#include <vector>
#include "VectorHelpers.h"
#include "DXInputLayouts.h"
#include <DirectXMath.h>
#include "SkyDome.h"

// forward declare types
class Mesh;
class QuadComponent;
class RenderToTexture;
class Entity;
class SkyDome;
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
class SimpleClipSceneShader;
class WaterShader;
class DebugQuadHandler;
class InstancedModel;

// the different render lists we have
enum SHADER_TYPE
{	
	S_DEFERRED,
	S_DEPTH,
	S_FORWARD_ALPHA,
	S_WIREFRAME,
	S_CAST_REFLECTION_OPAQUE,
	S_CAST_REFLECTION_ALPHA,
	S_REFRACT_OPAQUE,
	S_REFRACT_ALPHA,
	S_ALPHA_REFLECTION,
	S_ALPHA_WATER,
	S_NUM_RENDER_TYPES,
};

enum INSTANCED_SHADER_TYPE
{
	S_INSTANCED_DEFERRED,
	S_INSTANCED_DEPTH,
	S_INSTANCED_CAST_REFLECTION,
	S_INSTANCED_REFRACT,
	S_NUM_INSTANCED_RENDER_TYPES
};

class Renderer
{
public:
	Renderer();
	~Renderer();

	void Initialize();

	// add objects to respective renderers
	void AddMeshToRenderer(Mesh* mesh, SHADER_TYPE type)                                { _meshes[type].push_back(mesh); }
	void AddInstancedModelToRenderer(InstancedModel* model, INSTANCED_SHADER_TYPE type) { _instancedModels[type].push_back(model); }
	void AddQuadToRenderer(QuadComponent* quad)                                         { _quads.push_back(quad); }
	void AddParticleSystemToRenderer(ParticleSystemComponent* emitter)                  { _particleSystems.push_back(emitter); }

	// remove objects from respective renderers
	void RemoveMeshFromRenderer(Mesh* mesh, SHADER_TYPE type)                                { VECTOR_HELPERS::RemoveItemFromVector(_meshes[type], mesh); }
	void RemoveInstancedModelFromRenderer(InstancedModel* model, INSTANCED_SHADER_TYPE type) { VECTOR_HELPERS::RemoveItemFromVector(_instancedModels[type], model); }
	void RemoveQuadFromRenderer(QuadComponent* quad)                                         { VECTOR_HELPERS::RemoveItemFromVector(_quads, quad); }
	void RemoveParticleSystemFromRenderer(ParticleSystemComponent* emitter)                  { VECTOR_HELPERS::RemoveItemFromVector(_particleSystems, emitter); }
	
	// get meshes and particles
	std::vector<Mesh*>& GetMeshes(SHADER_TYPE type)                              { return _meshes[type]; }
	std::vector<InstancedModel*>& GetInstancedModels(INSTANCED_SHADER_TYPE type) { return _instancedModels[type]; }
	std::vector<ParticleSystemComponent*>& GetParticles()                        { return _particleSystems; }

	// get input layouts
	DXInputLayouts* GetInputLayouts()       { return _inputLayouts; } 

	// get the debug quad handler
	DebugQuadHandler* GetDebugQuadHandler() { return _debugQuadHandler; }

	// get shaders
	ParticleShader*         GetParticleShader()    { return _particleShader; }
	PostProcessingShader*   GetPostProcessShader() { return _PostProcessingShader; }
	WaterShader*            GetWaterShader()       { return _waterShader; }
	PlanarReflectionShader* GetPlanarShader()      { return _planarReflectionShader; }

	// create/ get skydome
	SkyDome* GetSkyDome() { return _skyDome; }
	SkyDome* CreateSkyDome(const char* file);

	// create the shadowmap
	Entity* CreateShadowMap(float orthoSize, float resolution, XMFLOAT3 position, XMFLOAT3 rotation, bool debugQuad);

	// create debug images that show each component of the G-Buffer
	void ShowGBufferDebugImages();

	// set the clear color
	void SetClearColor(float r, float g, float b, float a) { _clearColor[0] = r; _clearColor[1] = g; _clearColor[2] = b; _clearColor[3] = a; }

	// set the main render target active
	void SetMainRenderTarget(); 
	
	// will render everything
	void Render();

	// set a input layout
	void SetInputLayout(INPUT_LAYOUT_TYPE type) { _inputLayouts->SetInputLayout(type); }
	
private:

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
	WaterShader*            _waterShader;

	// skybox class with all rendering built in
	SkyDome* _skyDome;

	// contains all input layouts
	DXInputLayouts* _inputLayouts;

	// list of meshes for each shader type
	// one mesh can be added to several shader types
	std::vector<Mesh*> _meshes[S_NUM_RENDER_TYPES];	

	// list of all instanced models
	std::vector<InstancedModel*> _instancedModels[S_NUM_INSTANCED_RENDER_TYPES];

	// all 2D UI quads
	std::vector<QuadComponent*> _quads;

	// all particle systems
	std::vector<ParticleSystemComponent*> _particleSystems;

	// the camera entity that renders the depth map for shadows
	Entity* _cameraDepth;

	// render targets
	RenderToTexture* _depthMap;
	RenderToTexture* _mainRendertarget;

	// class that handles creating and aligning debug render quads
	DebugQuadHandler* _debugQuadHandler;

	// the Gbuffer for deferred rendering
	// and the fullscreen quad where we will 
	// project the deferred lightningpass
	GBuffer*    _gBuffer;
	ScreenQuad* _fullScreenQuad;

	float _clearColor[4];
};

