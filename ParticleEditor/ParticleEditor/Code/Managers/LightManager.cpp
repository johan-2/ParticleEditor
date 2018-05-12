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

void LightManager::SetDirectionalLight(LightDirectionComponent* light)
{
	_directionalLight = light;
}

void LightManager::AddPointLight(LightPointComponent* light)
{
	if(_pointLights.size() < MAX_POINT_LIGHTS)
	{
		_pointLights.push_back(light);
		return;
	}
	// delete light if we have max lights alredy
	light->GetParent()->RemoveEntity();
	printf("Could not add light, alredy at maximum\n");
}


void LightManager::RemoveDirectionalLight()
{
	_directionalLight = nullptr;
}



