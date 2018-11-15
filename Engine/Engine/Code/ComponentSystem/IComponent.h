#pragma once
#include "ComponentTypes.h"
#include "Entity.h"

// base class for all components
// have a pure virtual update and a pointer to 
// the entity for talking to other components
// this system is written with convenience in mind but
// should be rewritten at some point with a data oriented approch
class IComponent
{
public:
	IComponent(COMPONENT_TYPE type);
	virtual ~IComponent();

	// everyone have thier own update defined
	virtual void Update() = 0;

	// inactivate this component meaning update wont be called
	// can overide this from each component if some more functionality is wanted
	virtual void SetActive(bool active) { _isActive = active; }

	// is this component active
	bool IsActive() { return _isActive; }

	// get the type of component this is
	COMPONENT_TYPE Type() { return _type; }

	// get pointer to the entity
	Entity*& GetParent() { return _parent; }

	// set the entity this component belongs to
	void SetParent(Entity* parent) { _parent = parent; }

	// convenience method to get another component without having to get the parent first
	template <class T> T* GetComponent(){ return _parent->GetComponent<T>(); }	

protected:

	// type and pointer to entity
	COMPONENT_TYPE _type;
	Entity*        _parent;
	bool           _isActive;
};

