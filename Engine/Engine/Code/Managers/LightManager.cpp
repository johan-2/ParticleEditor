#include "LightManager.h"

LightManager* LightManager::_instance = 0;

LightManager& LightManager::GetInstance()
{
	if (_instance == 0)
		_instance = new LightManager;

	return *_instance;
}

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
	if(_pointLights.size() < MAX_POINT_LIGHTS)
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



