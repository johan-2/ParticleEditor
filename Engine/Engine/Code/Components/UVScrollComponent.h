#pragma once

#include "IComponent.h"
#include <DirectXMath.h>

using namespace DirectX;

class Entity;
class ModelComponent;

class UVScrollComponent : public IComponent
{
public:
	UVScrollComponent();
	~UVScrollComponent();

	void Init(XMFLOAT2 speed);

	void Update();
private:

	ModelComponent* _modelComponent;

	XMFLOAT2 _scrollSpeed;
	XMFLOAT2 _uvOffset;
};

