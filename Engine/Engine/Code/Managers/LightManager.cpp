#include "LightManager.h"

LightManager::LightManager()
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
	// set the pointlight data
	const int size = _pointLights.size();
	for (int i = 0; i < size; i++)
	{
		_pointCBBuffer[i].color          = _pointLights[i]->GetLightColor();
		_pointCBBuffer[i].intensity      = _pointLights[i]->GetIntensity();
		_pointCBBuffer[i].radius         = _pointLights[i]->GetRadius();
		_pointCBBuffer[i].lightPosition  = _pointLights[i]->GetComponent<TransformComponent>()->GetPositionRef();
		_pointCBBuffer[i].attConstant    = _pointLights[i]->GetAttConstant();
		_pointCBBuffer[i].attLinear      = _pointLights[i]->GetAttLinear();
		_pointCBBuffer[i].attExponential = _pointLights[i]->GetAttExponential();
		_pointCBBuffer[i].numLights      = size;
	}
}



