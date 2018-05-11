#include "ParticleEmitterComponent.h"
#include "Entity.h"
#include "DXManager.h"
#include "Renderer.h"
#include "TexturePool.h"
#include "Time.h"
#include "CameraManager.h"
#include "TransformComponent.h"
#include <algorithm>
#include <random>
#include <chrono>
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"
#include <fstream>
#include <string>


ParticleEmitterComponent::ParticleEmitterComponent() : IComponent(PARTICLE_COMPONENT)
{

}

void ParticleEmitterComponent::Init(char* particleFile)
{

	ParsefromJson(particleFile);

	// allocate memory for all settings
	_currentParticlesLifetime = new float[_numEmitters];
	_particleSpawnRatio = new float[_numEmitters];
	_spawnTimer = new float[_numEmitters];
	_numSpawnedParticles = new unsigned int[_numEmitters];

	_vertexBuffer = new ID3D11Buffer*[_numEmitters];
	_indexBuffer = new ID3D11Buffer*[_numEmitters];
	_instanceBuffer = new ID3D11Buffer*[_numEmitters];

	_particleInstanceData = new ParticleInstanceType*[_numEmitters];
	_particleData = new ParticleData*[_numEmitters];
	_sortedData = new ParticleData**[_numEmitters];

	_texture = new ID3D11ShaderResourceView*[_numEmitters];

	for (int i = 0; i< _numEmitters; i++)
	{
		_particleInstanceData[i] = new ParticleInstanceType[_settings[i].numParticles];
		_particleData[i] = new ParticleData[_settings[i].numParticles];
		_sortedData[i] = new ParticleData*[_settings[i].numParticles];
	}

	SetUp();

}

void ParticleEmitterComponent::SetUp()
{
	//_hasLifetime = true;

	//// add to renderer and get transform reference
	//Renderer::GetInstance().AddParticleEmitter(this);
	//_transform = GetComponent<TransformComponent>();

	//for(int i =0; i < _numEmitters; i++)
	//{				
	//	_currentParticlesLifetime[i] = _settings[i].particleLifetime; // if in burst mode we use one timer for every particle per emitter
	//	_numSpawnedParticles[i] = 0; // init to zero

	//	// get texture
	//	std::string tp = "Textures/";
	//	tp.append(_settings[i].texturePath.c_str());
	//	_texture[i] = _settings[i].texturePath.c_str() != "" ? TexturePool::GetInstance().GetTexture(tp.c_str()) : TexturePool::GetInstance().GetTexture("textures/defaultDiffuse.dds");

	//	// check if emitter have a limited lifetime or not(0 == magic number for no)
	//	if (_settings[i].emitterLifetime == 0)
	//		_hasLifetime = false;
	//	
	//	// get a pointer to the data of each particle, these pointers will be sorted instead of the actual particle data so we dont have to move around big data structures
	//	for (int y = 0; y < _settings[i].numParticles; y++)
	//		_sortedData[i][y] = &_particleData[i][y];

	//	// create all D3D11 buffers and two triangles for rendering
	//	CreateBuffers(_settings[i].startSize, i);

	//	if (_settings[i].burst)
	//		SpawnAllParticles(i);
	//	else
	//	{
	//		// start all particles inactive and set scale to 0 so inactive particles wont be visible
	//		for (int y = 0; y < _settings[i].numParticles; y++)
	//		{
	//			_particleData[i][y].active = false;
	//			_particleData[i][y].scale = XMFLOAT3(0, 0, 0);
	//		}

	//		// get spawnratio based on num particles and lifetimr per particle
	//		_particleSpawnRatio[i] = _settings[i].particleLifetime / _settings[i].numParticles;
	//		_spawnTimer[i] = _particleSpawnRatio[i];
	//	}		
	//}
	//
	//// get current entity position for use next frame
	//_previousPosition = _transform->GetPositionVal();
}

ParticleEmitterComponent::~ParticleEmitterComponent()
{
	delete[] _particleInstanceData;
	delete[] _particleData;
	delete[] _sortedData;
	delete[] _currentParticlesLifetime;
	delete[] _particleSpawnRatio;
	delete[] _spawnTimer;
	delete[] _settings;

	for(int i=0; i< _numEmitters; i++)
	{
		_vertexBuffer[i]->Release();
		_indexBuffer[i]->Release();
		_instanceBuffer[i]->Release();
	}
	
	delete[] _vertexBuffer;
	delete[] _indexBuffer;
	delete[] _instanceBuffer;

	Renderer::GetInstance().RemoveParticleEmitter(this);
}

void ParticleEmitterComponent::CreateBuffers(XMFLOAT2 size, unsigned int index)
{
	
	float halfX = size.x * 0.5f;
	float halfY = size.y * 0.5f;

	ParticleVertexType* vertices = new ParticleVertexType[4];

	vertices[0].position = XMFLOAT3(-halfX, halfY, 0.0f);
	vertices[0].texCoords = XMFLOAT2(0.0f, 0.0f);

	vertices[1].position = XMFLOAT3(halfX, halfY, 0.0f);
	vertices[1].texCoords = XMFLOAT2(1.0f, 0.0f);

	vertices[2].position = XMFLOAT3(-halfX, -halfY, 0.0f);
	vertices[2].texCoords = XMFLOAT2(0.0f, 1.0f);

	vertices[3].position = XMFLOAT3(halfX, -halfY, 0.0f);
	vertices[3].texCoords = XMFLOAT2(1.0f, 1.0f);

	unsigned long* indices = new unsigned long[6]{ 0,1,2,2,1,3 };

	ID3D11Device* device = DXManager::GetInstance().GetDevice();

	// create the descriptions and rasourcedata to buffers
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc, instanceBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData, instanceData;

	// Set up the description of the vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(ParticleVertexType) * 4;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Set up the description of the index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * 6;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Set up the description of the instance buffer.
	instanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	instanceBufferDesc.ByteWidth = sizeof(ParticleInstanceType) * _settings[index].numParticles;
	instanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	instanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	instanceBufferDesc.MiscFlags = 0;
	instanceBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	instanceData.pSysMem = _particleInstanceData[index];
	instanceData.SysMemPitch = 0;
	instanceData.SysMemSlicePitch = 0;

	HRESULT result;

	// create vertex, index and instance buffers
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &_vertexBuffer[index]);
	if (FAILED(result))
		printf("failed to create vertexbuffer for particlesystem\n");

	result = device->CreateBuffer(&indexBufferDesc, &indexData, &_indexBuffer[index]);
	if (FAILED(result))
		printf("failed to create indexbuffer for particlesystem\n");

	result = device->CreateBuffer(&instanceBufferDesc, &instanceData, &_instanceBuffer[index]);
	if (FAILED(result))
		printf("failed to create instancebuffer for particlesystem\n");

	delete[] vertices;
	delete[] indices;
}

void ParticleEmitterComponent::SpawnAllParticles(unsigned int index)
{
	for (int i = 0; i < _settings[index].numParticles; i++)
	{
		SpawnParticle(_particleData[index][i], index);
	}
	_currentParticlesLifetime[index] = _settings[index].particleLifetime;
}

void ParticleEmitterComponent::SpawnParticle(ParticleData& particle, unsigned int index)
{
	// get minmax spawn position values
	//const XMFLOAT3& emitterPos = _transform->GetPositionRef();

	//XMFLOAT2 spawnMinMaxX = XMFLOAT2(emitterPos.x - (_settings[index].spawnRadius * 0.5f), emitterPos.x + (_settings[index].spawnRadius * 0.5f));
	//XMFLOAT2 spawnMinMaxY = XMFLOAT2(emitterPos.y - (_settings[index].spawnRadius * 0.5f), emitterPos.y + (_settings[index].spawnRadius * 0.5f));
	//XMFLOAT2 spawnMinMaxZ = XMFLOAT2(emitterPos.z - (_settings[index].spawnRadius * 0.5f), emitterPos.z + (_settings[index].spawnRadius * 0.5f));

	//XMFLOAT2 directionMinMaxX = XMFLOAT2(_settings[index].direction.x - (_settings[index].velocitySpread.x * 0.5f), _settings[index].direction.x + (_settings[index].velocitySpread.x * 0.5f));
	//XMFLOAT2 directionMinMaxY = XMFLOAT2(_settings[index].direction.y - (_settings[index].velocitySpread.y * 0.5f), _settings[index].direction.y + (_settings[index].velocitySpread.y * 0.5f));
	//XMFLOAT2 directionMinMaxZ = XMFLOAT2(_settings[index].direction.z - (_settings[index].velocitySpread.z * 0.5f), _settings[index].direction.z + (_settings[index].velocitySpread.z * 0.5f));

	//// get velocityDirection;
	//XMFLOAT3 direction = XMFLOAT3(GetRandomFloat(directionMinMaxX.x, directionMinMaxX.y), GetRandomFloat(directionMinMaxY.x, directionMinMaxY.y), GetRandomFloat(directionMinMaxZ.x, directionMinMaxZ.y));
	//D3DXVec3Normalize(&direction, &direction);
	//if (_settings[index].LocalSpace)
	//	direction = GetDirectionLocal(direction);

	//// get random color values
	//XMFLOAT3 startColorMultiplier = XMFLOAT3
	//								(GetRandomFloat(_settings[index].startColorMultiplierRGBMin.x, _settings[index].startColorMultiplierRGBMax.x), 
	//								 GetRandomFloat(_settings[index].startColorMultiplierRGBMin.y, _settings[index].startColorMultiplierRGBMax.y),
	//								 GetRandomFloat(_settings[index].startColorMultiplierRGBMin.z, _settings[index].startColorMultiplierRGBMax.z));

	//XMFLOAT3 endColorMultiplier = XMFLOAT3
	//								(GetRandomFloat(_settings[index].endColorMultiplierRGBMin.x, _settings[index].endColorMultiplierRGBMax.x),
	//								 GetRandomFloat(_settings[index].endColorMultiplierRGBMin.y, _settings[index].endColorMultiplierRGBMax.y),
	//								 GetRandomFloat(_settings[index].endColorMultiplierRGBMin.z, _settings[index].endColorMultiplierRGBMax.z));

	//// get random speed
	//float speed = GetRandomFloat(_settings[index].minMaxSpeed.x, _settings[index].minMaxSpeed.y);

	//float startScale = GetRandomFloat(_settings[index].startScaleMinMax.x, _settings[index].startScaleMinMax.y);
	//float endScale = GetRandomFloat(_settings[index].endScaleMinMax.x, _settings[index].endScaleMinMax.y);

	//float rotationSpeed = GetRandomFloat(_settings[index].rotationPerSecMinMax.x, _settings[index].rotationPerSecMinMax.y);
	//XMFLOAT2 uvScrollSpeed(GetRandomFloat(_settings[index].uvScrollXMinMax.x, _settings[index].uvScrollXMinMax.y), GetRandomFloat(_settings[index].uvScrollYMinMax.x, _settings[index].uvScrollYMinMax.y));

	//// set start values in particledatastruct
	//particle.position = XMFLOAT3(GetRandomFloat(spawnMinMaxX.x, spawnMinMaxX.y), GetRandomFloat(spawnMinMaxY.x, spawnMinMaxY.y), GetRandomFloat(spawnMinMaxZ.x, spawnMinMaxZ.y)) + _settings[index].spawnOffset;
	//particle.zRotation = 0;
	//particle.scale = XMFLOAT3(1, 1, 1);
	//particle.direction = direction;
	//particle.speed.x = speed;
	//particle.speed.y = speed;
	//particle.speed.z = speed;
	//particle.dragMultiplier = 1.0f;
	//particle.lifeTime = _settings[index].particleLifetime;
	//particle.active = true;
	//particle.currentColorMultiplier = _settings[index].startColorMultiplierRGBMin;
	//particle.currentAlpha = _settings[index].startAlpha;
	//particle.fraction = 0;
	//particle.startColorMultiplierRGB = startColorMultiplier;
	//particle.endColorMultiplierRGB = endColorMultiplier;
	//particle.startScale = startScale;
	//particle.endScale = endScale;
	//particle.zRotationSpeed = rotationSpeed;
	//particle.uvOffset = XMFLOAT2(0, 0);
	//particle.uvOffsetSpeed = uvScrollSpeed;
	
}

void ParticleEmitterComponent::Update() 
{
	const float& delta = Time::GetInstance().GetDeltaTime();
	
	UpdateLifeTime(delta);
	UpdateVelocity(delta);
	UpdateLerps(delta);
	UpdateRotations(delta);

	for(int i =0; i< _numEmitters; i++)
		if(_settings[i].BLEND == BLEND_STATE::BLEND_ALPHA)
		   SortParticles(i);

	UpdateBuffer();
		
}

void ParticleEmitterComponent::UpdateVelocity(const float& delta)
{
	//XMFLOAT3 EmitterPos = _transform->GetPositionVal();

	//for (int i = 0; i < _numEmitters; i++)
	//{
	//	for(int y =0; y < _settings[i].numParticles; y++)
	//	{
	//		if (!_particleData[i][y].active)
	//			continue;

	//		// save last frame position before we update velocity (used to calculate velocity direction of particle for rotation) 
	//		_particleData[i][y].previousPosition = _particleData[i][y].position;

	//		// manipulate speed based on gravity and direction
	//		// if direction is negative we have to take -gravity resulting in the speed decreasing with a positive gravity and increasing with a negative gravity (becuase of doubble negative)
	//		// positive direction works normally, will add negative gravity resulting in speed slowing down or add positive gravity that will increase speed
	//		if (_particleData[i][y].direction.x < 0.0f)
	//			_particleData[i][y].speed.x -= _settings[i].gravity.x * delta;
	//		else
	//			_particleData[i][y].speed.x += _settings[i].gravity.x * delta;

	//		if (_particleData[i][y].direction.y < 0.0f)
	//			_particleData[i][y].speed.y -= _settings[i].gravity.y * delta;
	//		else
	//			_particleData[i][y].speed.y += _settings[i].gravity.y * delta;

	//		if (_particleData[i][y].direction.z < 0.0f)
	//			_particleData[i][y].speed.z -= _settings[i].gravity.z * delta;
	//		else
	//			_particleData[i][y].speed.z += _settings[i].gravity.z * delta;

	//		// update the drag
	//		_particleData[i][y].dragMultiplier -= _settings[i].drag * delta;
	//		if (_particleData[i][y].dragMultiplier < 0.0f)
	//			_particleData[i][y].dragMultiplier = 0;

	//		// update the position based on direction and speed
	//		_particleData[i][y].position.x += (_particleData[i][y].direction.x * _particleData[i][y].speed.x * _particleData[i][y].dragMultiplier * delta) + (EmitterPos - _previousPosition).x * _settings[i].inheritVelocityScale.x;
	//		_particleData[i][y].position.y += (_particleData[i][y].direction.y * _particleData[i][y].speed.y * _particleData[i][y].dragMultiplier * delta) + (EmitterPos - _previousPosition).y * _settings[i].inheritVelocityScale.y;
	//		_particleData[i][y].position.z += (_particleData[i][y].direction.z * _particleData[i][y].speed.z * _particleData[i][y].dragMultiplier * delta) + (EmitterPos - _previousPosition).z * _settings[i].inheritVelocityScale.z;

	//		// add on gravity
	//		_particleData[i][y].position.x += _settings[i].gravity.x * _particleData[i][y].dragMultiplier * delta;
	//		_particleData[i][y].position.y += _settings[i].gravity.y * _particleData[i][y].dragMultiplier * delta;
	//		_particleData[i][y].position.z += _settings[i].gravity.z * _particleData[i][y].dragMultiplier * delta;

	//		// add on the movement of entity from last to current frame 
	//		if (_settings[i].followEmitter)
	//			_particleData[i][y].position += (EmitterPos - _previousPosition);

	//		_particleData[i][y].uvOffset.x += _particleData[i][y].uvOffsetSpeed.x * delta;
	//		_particleData[i][y].uvOffset.y += _particleData[i][y].uvOffsetSpeed.y * delta;
	//	}		
	//}

	//_previousPosition = EmitterPos;

}

void ParticleEmitterComponent::UpdateLerps(const float& delta)
{
	//for(int i = 0; i < _numEmitters; i++)
	//{
	//	for(int y =0; y < _settings[i].numParticles; y++)
	//	{
	//		if (!_particleData[i][y].active)
	//			continue;

	//		// add on to fraction
	//		_particleData[i][y].fraction += delta / _settings[i].particleLifetime;

	//		// color and alpha
	//		D3DXVec3Lerp(&_particleData[i][y].currentColorMultiplier, &_particleData[i][y].startColorMultiplierRGB, &_particleData[i][y].endColorMultiplierRGB, _particleData[i][y].fraction);
	//		_particleData[i][y].currentAlpha = LerpFloat(_settings[i].startAlpha, _settings[i].endAlpha, _particleData[i][y].fraction);

	//		//scale
	//		_particleData[i][y].scale = XMFLOAT3(1, 1, 1) * LerpFloat(_particleData[i][y].startScale, _particleData[i][y].endScale, _particleData[i][y].fraction);
	//	}		
	//}
}

void ParticleEmitterComponent::UpdateLifeTime(const float& delta)
{
	//for(int i =0; i < _numEmitters; i++)
	//{
	//	// if has a lifetime, countdown and remove entity
	//	if (_hasLifetime)
	//	{
	//		_settings[i].emitterLifetime -= delta;
	//		if (_settings[i].emitterLifetime <= 0)
	//			_parent->RemoveEntity();
	//	}

	//	// if in burst mode only update the timer that all particles share
	//	if (_settings[i].burst)
	//	{
	//		_currentParticlesLifetime[i] -= delta;
	//		if (_currentParticlesLifetime[i] <= 0)
	//			SpawnAllParticles(i);
	//		continue;
	//	}

	//	// if flow mode update the lifespan of all active particles
	//	for (int y = 0; y < _settings[i].numParticles; y++)
	//	{
	//		if (_particleData[i][y].active)
	//		{
	//			_particleData[i][y].lifeTime -= delta;
	//			if (_particleData[i][y].lifeTime <= 0)
	//				SpawnParticle(_particleData[i][y], i);
	//		}
	//	}

	//	// check if all particles is alredy active and spawned
	//	if (_numSpawnedParticles[i] == _settings[i].numParticles)
	//		continue;

	//	// spawn new particle based on number of particles in emitter and lifespan of 1 particle to get a nice flow
	//	_spawnTimer[i] += delta;
	//	if (_spawnTimer[i] >= _particleSpawnRatio[i])
	//	{
	//		_spawnTimer[i] = 0;
	//		SpawnParticle(_particleData[i][_numSpawnedParticles[i]],i);
	//		_numSpawnedParticles[i]++;
	//	}
	//}	

}

void ParticleEmitterComponent::SortParticles(unsigned int index)
{
	//XMFLOAT3 camPos = CameraManager::GetInstance().GetCurrentCameraGame()->GetComponent<TransformComponent>()->GetPositionVal();
	//XMFLOAT3 vec;

	//// get distance of particle from camera
	//for (int i = 0; i < _settings[index].numParticles; i++)
	//{
	//	vec = _particleData[index][i].position - camPos;
	//	_particleData[index][i].distance = D3DXVec3Length(&vec);
	//}
	//		
	//std::sort(_sortedData[index], _sortedData[index] + _settings[index].numParticles, [&](ParticleData* a, ParticleData* b) -> bool {return a->distance > b->distance; });
}

float ParticleEmitterComponent::GetRandomFloat(float min, float max)
{
	unsigned int seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::mt19937 generator(seed);
	std::uniform_real_distribution<float> distribution(min, max);

	return distribution(generator);
}

float ParticleEmitterComponent::LerpFloat(float a, float b, float f)
{
	return (a * (1.0f - f)) + (b * f);
}

XMFLOAT3 ParticleEmitterComponent::GetDirectionLocal(XMFLOAT3 direction)
{
	//const XMFLOAT3& emitterRotation = _transform->GetRotationRef();
	//D3DXMATRIX matrixRotation = *D3DXMatrixIdentity(&matrixRotation);
	//D3DXMatrixRotationYawPitchRoll(&matrixRotation, D3DXToRadian(emitterRotation.y), D3DXToRadian(emitterRotation.x), D3DXToRadian(emitterRotation.z));
	//	
	//// transform direction by our rotation
	//D3DXVec3TransformCoord(&direction, &direction, &matrixRotation);
	//		
	//return *D3DXVec3Normalize(&direction, &direction);

	return XMFLOAT3(0, 0, 0);
}

void ParticleEmitterComponent::UpdateRotations(const float& delta)
{

	//XMFLOAT3 forward, up, right;
	//CameraManager::GetInstance().GetCurrentCameraGame()->GetComponent<TransformComponent>()->GetAllAxis(forward, right, up);

	// XMFLOAT4X4 rotationMatrix = *D3DXMatrixIdentity(&rotationMatrix);

	// for(int i =0; i < _numEmitters; i++)
	// {
	//	 if (_settings[i].rotationByVelocity) // billboarding with z rotation by velocityvector
	//	 {
	//		 for (int y = 0; y < _settings[i].numParticles; y++)
	//		 {
	//			 // get the direction of velocity
	//			 XMFLOAT3 dir = _particleData[i][y].position - _particleData[i][y].previousPosition;
	//			 D3DXVec3Normalize(&dir, &dir);

	//			 // get perpendicular vector from camera forward and inverted velocity vector
	//			 XMFLOAT3 byVelocityRight(0, 0, 0);
	//			 D3DXVec3Cross(&byVelocityRight, &forward, &-dir);
	//			 D3DXVec3Normalize(&byVelocityRight, &byVelocityRight);

	//			 // get the perpendicular vector from camera forward and the newly calculated rightvector 
	//			 XMFLOAT3 byVelocityUp(0, 0, 0);
	//			 D3DXVec3Cross(&byVelocityUp, &forward, &byVelocityRight);
	//			 D3DXVec3Normalize(&byVelocityUp, &byVelocityUp);

	//			 rotationMatrix._11 = byVelocityRight.x;
	//			 rotationMatrix._21 = byVelocityRight.y;
	//			 rotationMatrix._31 = byVelocityRight.z;

	//			 rotationMatrix._12 = byVelocityUp.x;
	//			 rotationMatrix._22 = byVelocityUp.y;
	//			 rotationMatrix._32 = byVelocityUp.z;

	//			 rotationMatrix._13 = forward.x;
	//			 rotationMatrix._23 = forward.y;
	//			 rotationMatrix._33 = forward.z;

	//			 D3DXMatrixTranspose(&rotationMatrix, &rotationMatrix);

	//			 _particleData[i][y].rotationMatrix = rotationMatrix;
	//		 }
	//	 }
	//	 else // billboarding with z rotation by speed
	//	 {
	//		 rotationMatrix._11 = right.x;
	//		 rotationMatrix._21 = right.y;
	//		 rotationMatrix._31 = right.z;

	//		 rotationMatrix._12 = up.x;
	//		 rotationMatrix._22 = up.y;
	//		 rotationMatrix._32 = up.z;

	//		 rotationMatrix._13 = forward.x;
	//		 rotationMatrix._23 = forward.y;
	//		 rotationMatrix._33 = forward.z;

	//		 D3DXMATRIX zRotationMatrix = *D3DXMatrixIdentity(&zRotationMatrix);
	//		 D3DXMATRIX result = *D3DXMatrixIdentity(&result);

	//		 for (int y = 0; y < _settings[i].numParticles; y++)
	//		 {
	//			 // add on rotation for each particle and caculate the rotationmatrix for the z rotation
	//			 _particleData[i][y].zRotation += _particleData[i][y].zRotationSpeed * delta;
	//			 D3DXMatrixRotationZ(&zRotationMatrix, D3DXToRadian(_particleData[i][y].zRotation));

	//			 // multiply the billboard rotationmatrix with the z rotationmatrix and transpose the result
	//			 D3DXMatrixMultiply(&result, &rotationMatrix, &zRotationMatrix);
	//			 D3DXMatrixTranspose(&result, &result);

	//			 _particleData[i][y].rotationMatrix = result;
	//		 }
	//	 }
	// }					
}

void ParticleEmitterComponent::UpdateBuffer() 
{
	//D3DXMATRIX matrixPosition = *D3DXMatrixIdentity(&matrixPosition);
	//D3DXMATRIX matrixScale	  = *D3DXMatrixIdentity(&matrixScale);
	//D3DXMATRIX matrixRotation = *D3DXMatrixIdentity(&matrixRotation);

	//XMFLOAT3 color;
	//float alpha;
	//		
	//for(int i =0; i < _numEmitters; i++)
	//{
	//	for (int y = 0; y < _settings[i].numParticles; y++)
	//	{
	//		// calculate all matrices from the sorted particle values
	//		D3DXMatrixTranslation(&matrixPosition, _sortedData[i][y]->position.x, _sortedData[i][y]->position.y, _sortedData[i][y]->position.z);
	//		D3DXMatrixScaling(&matrixScale, _sortedData[i][y]->scale.x, _sortedData[i][y]->scale.y, _sortedData[i][y]->scale.z);
	//		matrixRotation = _sortedData[i][y]->rotationMatrix;

	//		// set the values for each instace
	//		_particleInstanceData[i][y].worldMatrix = matrixScale * matrixRotation * matrixPosition;

	//		color = _sortedData[i][y]->currentColorMultiplier;
	//		alpha = _sortedData[i][y]->currentAlpha;

	//		// multiply all colors with alpha value, black == transparent
	//		if (_settings[i].BLEND == BLEND_STATE::BLEND_ADDITIVE || _settings[i].BLEND == BLEND_STATE::BLEND_SUBTRACTIVE)
	//			_particleInstanceData[i][y].color = D3DXVECTOR4(color.x, color.y, color.z, alpha) * alpha;
	//		// send in the color normaly, alpha is set in shader
	//		else if (_settings[i].BLEND == BLEND_STATE::BLEND_ALPHA)
	//			_particleInstanceData[i][y].color = D3DXVECTOR4(color.x, color.y, color.z, alpha);

	//		_particleInstanceData[i][y].uvOffset = _sortedData[i][y]->uvOffset;
	//	}

	//	ID3D11DeviceContext* devCon = DXManager::GetInstance().GetDeviceCon();
	//	D3D11_MAPPED_SUBRESOURCE data;

	//	// map instancebuffer
	//	HRESULT result = devCon->Map(_instanceBuffer[i], 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
	//	if (FAILED(result))
	//		printf("failed to map instancebuffer in particlesystem\n");

	//	// copy the instancedata over to the instancebuffer
	//	memcpy(data.pData, (void*)_particleInstanceData[i], sizeof(ParticleInstanceType) * _settings[i].numParticles);

	//	//unmap
	//	devCon->Unmap(_instanceBuffer[i], 0);
	//}
	
}


void ParticleEmitterComponent::UploadBuffers(unsigned int index)
{
	ID3D11DeviceContext* devCon = DXManager::GetInstance().GetDeviceCon();

	unsigned int strides[2];
	unsigned int offsets[2];
	ID3D11Buffer* bufferptrs[2];

	// Set vertex buffer stride and offset.
	strides[0] = sizeof(ParticleVertexType);
	strides[1] = sizeof(ParticleInstanceType);

	offsets[0] = 0;
	offsets[1] = 0;

	bufferptrs[0] = _vertexBuffer[index];
	bufferptrs[1] = _instanceBuffer[index];

	// Set the vertex buffer to active in the input assembler so it can be rendered, the vertices is sent to the pipeline before the go trought the shader for rendering
	devCon->IASetVertexBuffers(0, 2, bufferptrs, strides, offsets);
	// Set the index buffer to active in the input assembler so it can be rendered.
	devCon->IASetIndexBuffer(_indexBuffer[index], DXGI_FORMAT_R32_UINT, 0);
}

void ParticleEmitterComponent::ParsefromJson(char* file)
{
	FILE* fp; fopen_s(&fp, file, "rb");
	char readBuffer[65536];
	rapidjson::FileReadStream inStream(fp, readBuffer, sizeof(readBuffer));

	rapidjson::Document d;
	d.ParseStream(inStream);
	fclose(fp);

	assert(d.IsObject());

	assert(d["numEmitters"].IsInt());
	_numEmitters = d["numEmitters"].GetInt();

	_settings = new ParticleSettings[_numEmitters];
	
	std::string key = "";

	for(int i =0; i < _numEmitters; i++)
	{
		std::string index = std::to_string(i);

		key = "numParticles";
		key.append(index.c_str());
		assert(d[key.c_str()].IsInt());
		_settings[i].numParticles = d[key.c_str()].GetInt();

		key = "texture";
		key.append(index.c_str());
		assert(d[key.c_str()].IsString());
		_settings[i].texturePath = std::string(d[key.c_str()].GetString(), 100);

		key = "startSize";
		key.append(index.c_str());
		assert(d[key.c_str()].IsArray());
		rapidjson::Value& a = d[key.c_str()];
		_settings[i].startSize = XMFLOAT2(a[0].GetFloat(), a[1].GetFloat());

		key = "direction";
		key.append(index.c_str());
		assert(d[key.c_str()].IsArray());
		a = d[key.c_str()];
		_settings[i].direction = XMFLOAT3(a[0].GetFloat(), a[1].GetFloat(), a[2].GetFloat());

		key = "minMaxSpeed";
		key.append(index.c_str());
		assert(d[key.c_str()].IsArray());
		a = d[key.c_str()];
		_settings[i].minMaxSpeed = XMFLOAT2(a[0].GetFloat(), a[1].GetFloat());

		key = "gravity";
		key.append(index.c_str());
		assert(d[key.c_str()].IsArray());
		a = d[key.c_str()];
		_settings[i].gravity = XMFLOAT3(a[0].GetFloat(), a[1].GetFloat(), a[2].GetFloat());

		key = "drag";
		key.append(index.c_str());
		assert(d[key.c_str()].IsFloat());
		_settings[i].drag = d[key.c_str()].GetFloat();

		key = "velocitySpread";
		key.append(index.c_str());
		assert(d[key.c_str()].IsArray());
		a = d[key.c_str()];
		_settings[i].velocitySpread = XMFLOAT3(a[0].GetFloat(), a[1].GetFloat(), a[2].GetFloat());

		key = "emitterLifetime";
		key.append(index.c_str());
		assert(d[key.c_str()].IsFloat());
		_settings[i].emitterLifetime = d[key.c_str()].GetFloat();

		key = "particleLifetime";
		key.append(index.c_str());
		assert(d[key.c_str()].IsFloat());
		_settings[i].particleLifetime = d[key.c_str()].GetFloat();

		key = "spawnRadius";
		key.append(index.c_str());
		assert(d[key.c_str()].IsFloat());
		_settings[i].spawnRadius = d[key.c_str()].GetFloat();

		key = "burst";
		key.append(index.c_str());
		assert(d[key.c_str()].IsBool());
		_settings[i].burst = d[key.c_str()].GetBool();

		key = "followEmitter";
		key.append(index.c_str());
		assert(d[key.c_str()].IsBool());
		_settings[i].followEmitter = d[key.c_str()].GetBool();

		key = "startColorMultiplierRGBMin";
		key.append(index.c_str());
		assert(d[key.c_str()].IsArray());
		a = d[key.c_str()];
		_settings[i].startColorMultiplierRGBMin = XMFLOAT3(a[0].GetFloat(), a[1].GetFloat(), a[2].GetFloat());

		key = "startColorMultiplierRGBMax";
		key.append(index.c_str());
		assert(d[key.c_str()].IsArray());
		a = d[key.c_str()];
		_settings[i].startColorMultiplierRGBMax = XMFLOAT3(a[0].GetFloat(), a[1].GetFloat(), a[2].GetFloat());

		key = "endColorMultiplierRGBMin";
		key.append(index.c_str());
		assert(d[key.c_str()].IsArray());
		a = d[key.c_str()];
		_settings[i].endColorMultiplierRGBMin = XMFLOAT3(a[0].GetFloat(), a[1].GetFloat(), a[2].GetFloat());

		key = "endColorMultiplierRGBMax";
		key.append(index.c_str());
		assert(d[key.c_str()].IsArray());
		a = d[key.c_str()];
		_settings[i].endColorMultiplierRGBMax = XMFLOAT3(a[0].GetFloat(), a[1].GetFloat(), a[2].GetFloat());

		key = "startAlpha";
		key.append(index.c_str());
		assert(d[key.c_str()].IsFloat());
		_settings[i].startAlpha = d[key.c_str()].GetFloat();

		key = "endAlpha";
		key.append(index.c_str());
		assert(d[key.c_str()].IsFloat());
		_settings[i].endAlpha = d[key.c_str()].GetFloat();

		key = "BLEND";
		key.append(index.c_str());
		assert(d[key.c_str()].IsInt());
		_settings[i].BLEND = (BLEND_STATE)d[key.c_str()].GetInt();

		key = "spawnOffset";
		key.append(index.c_str());
		assert(d[key.c_str()].IsArray());
		a = d[key.c_str()];
		_settings[i].spawnOffset = XMFLOAT3(a[0].GetFloat(), a[1].GetFloat(), a[2].GetFloat());

		key = "localSpace";
		key.append(index.c_str());
		assert(d[key.c_str()].IsBool());
		_settings[i].LocalSpace = d[key.c_str()].GetBool();

		key = "startScaleMinMax";
		key.append(index.c_str());
		assert(d[key.c_str()].IsArray());
		a = d[key.c_str()];
		_settings[i].startScaleMinMax = XMFLOAT2(a[0].GetFloat(), a[1].GetFloat());

		key = "endScaleMinMax";
		key.append(index.c_str());
		assert(d[key.c_str()].IsArray());
		a = d[key.c_str()];
		_settings[i].endScaleMinMax = XMFLOAT2(a[0].GetFloat(), a[1].GetFloat());

		key = "rotationByVelocity";
		key.append(index.c_str());
		assert(d[key.c_str()].IsBool());
		_settings[i].rotationByVelocity = d[key.c_str()].GetBool();

		key = "rotationPerSecMinMax";
		key.append(index.c_str());
		assert(d[key.c_str()].IsArray());
		a = d[key.c_str()];
		_settings[i].rotationPerSecMinMax = XMFLOAT2(a[0].GetFloat(), a[1].GetFloat());

		key = "uvScrollXMinMax";
		key.append(index.c_str());
		assert(d[key.c_str()].IsArray());
		a = d[key.c_str()];
		_settings[i].uvScrollXMinMax = XMFLOAT2(a[0].GetFloat(), a[1].GetFloat());

		key = "uvScrollYMinMax";
		key.append(index.c_str());
		assert(d[key.c_str()].IsArray());
		a = d[key.c_str()];
		_settings[i].uvScrollYMinMax = XMFLOAT2(a[0].GetFloat(), a[1].GetFloat());

		key = "inheritVelocityScale";
		key.append(index.c_str());
		assert(d[key.c_str()].IsArray());
		a = d[key.c_str()];
		_settings[i].inheritVelocityScale = XMFLOAT3(a[0].GetFloat(), a[1].GetFloat(), a[2].GetFloat());
	}

	
		
}