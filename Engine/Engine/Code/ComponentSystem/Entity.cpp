#include "Entity.h"
#include "IComponent.h"
#include "World.h"


Entity::Entity()
{
	World::GetInstance().AddEntity(this);
}


Entity::~Entity()
{
	
}

void Entity::RemoveComponent(IComponent* component) 
{
	for (int i = 0; i < _components.size(); i++)
	{
		if (_components[i] == component)
		{
			// remove component from vector, memory is deleted in WORLD
			IComponent* temp = _components.back();			
			_components.back() = _components[i];
			_components[i] = temp;
			
			World::GetInstance().RemoveComponent(_components.back()->Type(), _components.back());
			_components.pop_back();

			break;
		}
	}
}

void Entity::RemoveEntity() 
{
	// memory will be deleted in WORLD
	World& world = World::GetInstance();

	for (int i = 0; i < _components.size(); i++)
		world.RemoveComponent(_components[i]->Type(), _components[i]);

	world.RemoveEntity(this);
}