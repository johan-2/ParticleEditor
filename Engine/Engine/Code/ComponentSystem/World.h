#pragma once

#include "IComponent.h"
#include <vector>

class Entity;
class IComponent;

class World
{
public:
	World();
	~World();

	static World& GetInstance();

	void Update();
	
	
	void AddComponent(COMPONENT_TYPE type, IComponent* component);
	void AddEntity(Entity* entity);
	
	void RemoveComponent(COMPONENT_TYPE type, IComponent* component);
	void RemoveEntity(Entity* entity);

	void Empty();

	void SetComponentsDirty() { _isComponentsDirty = true; }
	void SetEntitiesDirty() { _isEntitiesDirty = true; }

private:

	
	void HandleDirtyComponents();
	void HandleDirtyEntities();
	void DeleteComponent(COMPONENT_TYPE type, IComponent* component);
	void DeleteEntity(Entity* entity);

	static World* _instance;

	std::vector<IComponent*> _components[NUM_COMPONENT_TYPES];
	std::vector<Entity*> _entities;

	std::vector<IComponent*> _componentsToRemove[NUM_COMPONENT_TYPES];
	std::vector<Entity*> _entitiesToRemove;

	bool _isComponentsDirty;
	bool _isEntitiesDirty;

};

