#pragma once

#include <DirectXMath.h>
#include "IComponent.h"
#include "DXManager.h"

using namespace DirectX;

class Entity;
class TransformComponent;

struct ParticleInstanceType
{
	XMFLOAT4X4 worldMatrix;
	XMFLOAT4 color;
	XMFLOAT2 uvOffset;
};

struct ParticleVertexType
{
	XMFLOAT3 position;
	XMFLOAT2 texCoords;
};

struct ParticleData
{
	XMFLOAT3 position;
	float zRotation;
	XMFLOAT3 scale;
	XMFLOAT3 direction;
	XMFLOAT3 speed;
	XMFLOAT3	startColorMultiplierRGB;
	XMFLOAT3	endColorMultiplierRGB;
	XMFLOAT3 currentColorMultiplier;
	float currentAlpha;
	float fraction;
	float lifeTime;
	float dragMultiplier;
	bool active;
	float distance;
	float startScale;
	float endScale;
	XMFLOAT3 previousPosition;
	XMFLOAT4X4 rotationMatrix;
	float zRotationSpeed;
	XMFLOAT2 uvOffset;
	XMFLOAT2 uvOffsetSpeed;

};

struct ParticleSettings
{
	unsigned int numParticles; //
	std::string  texturePath; //
	XMFLOAT2  startSize;   //
	XMFLOAT3  direction; //
	XMFLOAT2  minMaxSpeed;
	XMFLOAT3  gravity; //
	float        drag; //
	XMFLOAT3  velocitySpread; //
	float        emitterLifetime; //
	float        particleLifetime; //
	float        spawnRadius; //
	bool         burst; //
	bool		 followEmitter;
	XMFLOAT3	 startColorMultiplierRGBMin; //
	XMFLOAT3	 startColorMultiplierRGBMax; //
	XMFLOAT3	 endColorMultiplierRGBMin; //
	XMFLOAT3	 endColorMultiplierRGBMax; //
	float        startAlpha; //
	float        endAlpha; //
	BLEND_STATE  BLEND;
	XMFLOAT3  spawnOffset; //
	bool		 LocalSpace;
	XMFLOAT2  startScaleMinMax; //
	XMFLOAT2  endScaleMinMax; //
	bool         rotationByVelocity;
	XMFLOAT2  rotationPerSecMinMax;//
	XMFLOAT2  uvScrollXMinMax;//
	XMFLOAT2  uvScrollYMinMax;//
	XMFLOAT3  inheritVelocityScale;
};

class ParticleEmitterComponent : public IComponent
{
public:
	
	ParticleEmitterComponent();	
	~ParticleEmitterComponent();

	void Init(std::vector<ParticleSettings> settings, unsigned int numEmitters);
	void Init(char* particleFile);

	void Update();
	void UploadBuffers(unsigned int index);
	BLEND_STATE GetBlendState(int index) { return _settings[index].BLEND; }

	ID3D11ShaderResourceView* GetTexture(int index) { return _texture[index]; }
	unsigned int GetNumParticles(int index) { return _settings[index].numParticles; }
	unsigned int GetNumEmitters() { return _numEmitters; }

	ParticleSettings GetSettings(int index){return _settings[index];}
	ID3D11ShaderResourceView** _texture;

private:

	void SetUp();
	void CreateBuffers(XMFLOAT2 size, unsigned int index);
	void UpdateBuffer();

	void SpawnAllParticles(unsigned int index);
	void SpawnParticle(ParticleData& particle, unsigned int index);

	void UpdateVelocity(const float& delta);
	void UpdateLifeTime(const float& delta);
	void UpdateLerps(const float& delta);
	void UpdateRotations(const float& delta);
	void SortParticles(unsigned int index);

	void ParsefromJson(char* file);

	float GetRandomFloat(float min, float max);
	float LerpFloat(float a, float b, float f);
	XMFLOAT3 GetDirectionLocal(XMFLOAT3 direction);

	float* _currentParticlesLifetime;
	float* _particleSpawnRatio;
	float* _spawnTimer;

	unsigned int* _numSpawnedParticles;
	unsigned int _numEmitters;
		
	bool _hasLifetime;

	ParticleSettings* _settings;

	ID3D11Buffer** _vertexBuffer;
	ID3D11Buffer** _indexBuffer;
	ID3D11Buffer** _instanceBuffer;

	ParticleInstanceType** _particleInstanceData;
	ParticleData** _particleData;
	ParticleData*** _sortedData;

	XMFLOAT3 _previousPosition;
	TransformComponent* _transform;	

};

