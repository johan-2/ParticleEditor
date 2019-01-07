#include "LightManager.h"
#include "ShaderHelpers.h"

LightManager::LightManager():
	_CBPointBuffer(nullptr)
{	
}

LightManager::~LightManager()
{
}

// set the pointer to the directional light to use
void LightManager::SetDirectionalLight(LightDirectionComponent* light)
{
	_directionalLight = light;
}

// add a new point light
void LightManager::AddPointLight(LightPointComponent* light)
{
	if (_pointLights.size() < MAX_POINT_LIGHTS)
	{
		_pointLights.push_back(light);
		return;
	}

	// delete entity and all its components if we have more lights then
	// our shader is set to be able to recive in one drawcall
	light->GetParent()->RemoveEntity();
	printf("Could not add light, alredy at maximum\n");
}

void LightManager::RemoveDirectionalLight()
{
	_directionalLight = nullptr;
}

void LightManager::UpdateLightBuffers()
{
	if (!_CBPointBuffer)
		SHADER_HELPERS::CreateConstantBuffer(_CBPointBuffer);

	// set the pointlight data
	size_t size = _pointLights.size();
	for (int i = 0; i < size; i++)
	{
		_pointData[i].color          = _pointLights[i]->GetLightColor();
		_pointData[i].intensity      = _pointLights[i]->GetIntensity();
		_pointData[i].radius         = _pointLights[i]->GetRadius();
		_pointData[i].lightPosition  = _pointLights[i]->GetComponent<TransformComponent>()->GetPositionRef();
		_pointData[i].attConstant    = _pointLights[i]->GetAttConstant();
		_pointData[i].attLinear      = _pointLights[i]->GetAttLinear();
		_pointData[i].attExponential = _pointLights[i]->GetAttExponential();
		_pointData[i].numLights      = size;
	}

	SHADER_HELPERS::UpdateConstantBuffer(&_pointData, sizeof(CBPoint) * size, _CBPointBuffer);	
}



