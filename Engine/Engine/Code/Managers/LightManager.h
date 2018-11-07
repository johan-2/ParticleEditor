#pragma once
#include <DirectXMath.h>
#include "LightDirectionComponent.h"
#include "LightPointComponent.h"

using namespace DirectX;

#define MAX_POINT_LIGHTS 1024

class LightManager
{
public:
	LightManager();
	~LightManager();				
	
	static LightManager& GetInstance();

	// set the color of our ambient light
	void SetAmbientColor(XMFLOAT4 color) { _ambientColor = color; } 
	
	// set the directional light to use for rendering
	void SetDirectionalLight(LightDirectionComponent* light);

	// add a pointlight to the list of pointlights
	void AddPointLight(LightPointComponent* light);

	// get all lights
	const XMFLOAT4&                     GetAmbientColor()     { return _ambientColor; }
	LightDirectionComponent*&           GetDirectionalLight() { return _directionalLight; }
	std::vector<LightPointComponent*>&  GetPointLight()       { return _pointLights; }
		
	// remove the current directional light
	void RemoveDirectionalLight();

private:
	
	// color for ambient light
	XMFLOAT4 _ambientColor;

	// pointer to directional light
	LightDirectionComponent* _directionalLight;
	
	// list of all point lights
	std::vector<LightPointComponent*> _pointLights;
	
	static LightManager* _instance;
};

