#pragma once

#include <vector>

class Mesh;
class QuadComponent;
class RenderToTexture;
class Entity;
class SkyBox;
class ParticleEmitterComponent;

enum SHADER_TYPE
{
	S_AMBIENT,
	S_DIRECTIONAL,
	S_DEPTH,
	S_DIRECTIONAL_SHADOWS,
	S_POINT,
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

	void AddToAlphaMeshes(Mesh* mesh);
	void RemoveFromAlphaMeshes(Mesh* mesh);

	void AddParticleEmitter(ParticleEmitterComponent* emitter);
	void RemoveParticleEmitter(ParticleEmitterComponent* emitter);
	
	void Render();
	void CreateDepthMap();

	SkyBox* GetSkybox() { return _skyBox; }
	
private:

	static Renderer* _instance;

	void RenderDepth();
	void RenderLights();
	void RenderLightsAlpha();
	void RenderParticles();
	void RenderUI();

	void AlphaSort();	

	std::vector<Mesh*> _meshes[S_NUM_RENDER_TYPES];	
	std::vector<Mesh*> _meshesAlpha;
	std::vector<QuadComponent*> _quads;
	std::vector<ParticleEmitterComponent*> _particleEmitters;

	//depth stuff
	Entity* _cameraDepth;
	RenderToTexture* _depthMap;

	SkyBox* _skyBox;

};
