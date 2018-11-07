#include "IComponent.h"
#include "World.h"

// add this component to world by type
IComponent::IComponent(COMPONENT_TYPE type):
	_type(type)
{
	World::GetInstance().AddComponent(type, this);
}

IComponent::~IComponent()
{	
}
