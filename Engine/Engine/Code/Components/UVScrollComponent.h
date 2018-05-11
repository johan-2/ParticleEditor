#pragma once

#include "IComponent.h"

class Entity;
class ModelComponent;

class UVScrollComponent : public IComponent
{
public:
	UVScrollComponent();
	~UVScrollComponent();

	void Init(D3DXVECTOR2 speed);

	void Update();
private:

	ModelComponent* _modelComponent;

	D3DXVECTOR2 _scrollSpeed;
	D3DXVECTOR2 _uvOffset;
};

