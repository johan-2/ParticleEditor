#include "IComponent.h"
#include "World.h"


IComponent::IComponent(COMPONENT_TYPE type):
	_type(type)
{
	World::GetInstance().AddComponent(type, this);
}

IComponent::~IComponent()
{	
}
