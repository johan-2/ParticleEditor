#pragma once
#include "ComponentTypes.h"
#include "Entity.h"

class IComponent
{
public:
	IComponent(COMPONENT_TYPE type);
	virtual ~IComponent();

	virtual void Update() = 0;

	COMPONENT_TYPE Type() { return _type; }
	Entity* GetParent() { return _parent; }
	void Setparent(Entity* parent) { _parent = parent; }

	template <class T> T* GetComponent(){ return _parent->GetComponent<T>(); }	

protected:

	COMPONENT_TYPE _type;
	Entity* _parent;
};

