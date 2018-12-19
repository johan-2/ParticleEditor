#pragma once
#include <DirectXMath.h>
#include "LightDirectionComponent.h"
#include "LightPointComponent.h"

using namespace DirectX;

#define MAX_POINT_LIGHTS 1024

// constant data for point lightning in the lightning pass
struct CBPoint
{
	XMFLOAT3 lightPosition;
	float    radius;
	XMFLOAT3 color;
	float    intensity;
	float attConstant;
	float attLinear;
	float attExponential;
	int   numLights;
};

class LightManager
{
public:
	LightManager();
	~LightManager();				

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

	// updates the data for light constant buffers
	void UpdateLightBuffers();

	// get pointer to point light data
	CBPoint* GetCBPointBuffer() { return _pointCBBuffer; }

	// get num of pointlights active
	int GetNumPointLights() { return (int)_pointLights.size(); }
	
private:
	
	// color for ambient light
	XMFLOAT4 _ambientColor;

	// pointer to directional light
	LightDirectionComponent* _directionalLight;
	
	// list of all point lights
	std::vector<LightPointComponent*> _pointLights;

	// holds the properties of all point lights
	CBPoint _pointCBBuffer[MAX_POINT_LIGHTS];
};

