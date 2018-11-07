#include "Entity.h"
#include "IComponent.h"
#include "World.h"
#include "ComponentHelpers.h"

// adds this entity to the world on creation
Entity::Entity()
{
	World::GetInstance().AddEntity(this);
}

Entity::~Entity()
{
}

// removes a component from the component list in entity
// aswell as in the component list in world
void Entity::RemoveComponent(IComponent*& component) 
{
	RemoveComponentFromList(_components, component);
	World::GetInstance().RemoveComponent(component->Type(), component);
}

// removes all components owned by this entity from world
// aswell as removing the entity from entity list in world
void Entity::RemoveEntity() 
{
	World& world = World::GetInstance();

	for (int i = 0; i < _components.size(); i++)
		world.RemoveComponent(_components[i]->Type(), _components[i]);

	world.RemoveEntity(this);
}