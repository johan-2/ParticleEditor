#pragma once
#include <vector>
#include "VectorHelpers.h"

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
class DXInputLayouts;

enum SHADER_TYPE
{	
	S_DEFERRED,
	S_DEPTH,
	S_FORWARD_ALPHA,
	S_NUM_RENDER_TYPES,
};

class Renderer
{
public:
	Renderer();
	~Renderer();

	static Renderer& GetInstance();

	// add objects to respective renderers
	void AddMeshToRenderer(Mesh* mesh, SHADER_TYPE type)               { _meshes[type].push_back(mesh); }
	void AddQuadToRenderer(QuadComponent* quad)                        { _quads.push_back(quad); }
	void AddParticleSystemToRenderer(ParticleSystemComponent* emitter) { _particleSystems.push_back(emitter); }

	// remove objects from respective renderers
	void RemoveMeshFromRenderer(Mesh* mesh, SHADER_TYPE type)               { VECTOR_HELPERS::RemoveItemFromVector(_meshes[type], mesh); }
	void RemoveQuadFromRenderer(QuadComponent* quad)                        { VECTOR_HELPERS::RemoveItemFromVector(_quads, quad); }
	void RemoveParticleSystemFromRenderer(ParticleSystemComponent* emitter) { VECTOR_HELPERS::RemoveItemFromVector(_particleSystems, emitter); }
	
	void Render();
	void CreateDepthMap();
	
private:

	DepthShader*    _depthShader;
	DeferredShader* _deferredShader;
	QuadShader*     _quadShader;
	ParticleShader* _particleShader;
	DXInputLayouts* _inputLayouts;

	static Renderer* _instance;

	void RenderDeferred();
	void RenderDepth();
	void RenderLightsAlpha();
	void RenderParticles();
	void RenderQuads();

	void AlphaSort();	

	// list of meshes for each shader type
	// one mesh can be added to several shader types
	std::vector<Mesh*> _meshes[S_NUM_RENDER_TYPES];	

	// all 2D UI quads
	std::vector<QuadComponent*>           _quads;

	// all particle systems
	std::vector<ParticleSystemComponent*> _particleSystems;

	Entity* _cameraDepth;
	RenderToTexture* _depthMap;

	GBuffer* _gBuffer;
	ScreenQuad* _screenQuad;
	SkyBox* _skyBox;

};

