#pragma once

#define MAX_POINT_LIGHTS 1024

#include <DirectXMath.h>
#include "LightDirectionComponent.h"
#include "LightPointComponent.h"

using namespace DirectX;

class LightManager
{
public:
	
	static LightManager& GetInstance();

	LightManager();
	~LightManager();				
	
	void SetDirectionalLight(LightDirectionComponent* light);
	void AddPointLight(LightPointComponent* light);
	void SetAmbientColor(XMFLOAT4 color) { _ambientColor = color; }
	const XMFLOAT4& GetAmbientColor()    { return _ambientColor; }
		
	void RemoveDirectionalLight();
	
	LightDirectionComponent*& GetDirectionalLight()     { return _directionalLight; }
	std::vector<LightPointComponent*>&  GetPointLight() { return _pointLights; }

private:
	
	XMFLOAT4 _ambientColor;
	LightDirectionComponent* _directionalLight;
	
	std::vector<LightPointComponent*> _pointLights;
	
	static LightManager* _instance;
};

