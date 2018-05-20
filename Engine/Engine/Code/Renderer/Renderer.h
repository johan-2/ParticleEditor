#pragma once

#include <vector>

class Mesh;
class QuadComponent;
class RenderToTexture;
class Entity;
class SkyBox;
class ParticleSystemComponent;
class GBuffer;
class ScreenQuad;

enum SHADER_TYPE
{	
	S_DEFERRED,
	S_DEPTH,
	S_FORWARD_ALPHA,
	S_NUM_RENDER_TYPES
};

class Renderer
{
public:
	Renderer();
	~Renderer();

	static Renderer& GetInstance();

	void AddToRenderer(Mesh* mesh, SHADER_TYPE type);
	void RemoveFromRenderer(Mesh* mesh, SHADER_TYPE type);

	void AddQuadToUIRenderer(QuadComponent* quad);
	void RemoveQuadFromUIRenderer(QuadComponent* quad);

	void AddParticleSystem(ParticleSystemComponent* emitter);
	void RemoveParticleSystem(ParticleSystemComponent* emitter);
	
	void Render();
	void CreateDepthMap();
	
private:

	static Renderer* _instance;

	void RenderDeferred();
	void RenderDepth();
	void RenderLightsAlpha();
	void RenderParticles();
	void RenderUI();

	void AlphaSort();	

	std::vector<Mesh*> _meshes[S_NUM_RENDER_TYPES];	
	std::vector<QuadComponent*> _quads;
	std::vector<ParticleSystemComponent*> _particleSystems;

	Entity* _cameraDepth;
	RenderToTexture* _depthMap;

	GBuffer* _gBuffer;
	ScreenQuad* _screenQuad;
	SkyBox* _skyBox;

};

