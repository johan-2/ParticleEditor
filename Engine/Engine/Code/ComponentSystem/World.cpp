#include "World.h"
#include "Entity.h"
#include "IComponent.h"

World* World::_instance = nullptr;

World& World::GetInstance() 
{
	if (_instance == nullptr)
		_instance = new World();

	return *_instance;
}

World::World():
	_isComponentsDirty(false),
	_isEntitiesDirty(false)
{
}

World::~World()
{
}

void World::Update()
{
	// update all components
	for (int i = 0; i < NUM_COMPONENT_TYPES; i++) 
	{
		unsigned int size = _components[i].size();

		for (int y = 0; y < size; y++)
			_components[i][y]->Update();
	}

	// remove and delete all components/entities that got flagged dirty during the update
	if (_isComponentsDirty)
		HandleDirtyComponents();
	
	if (_isEntitiesDirty)
		HandleDirtyEntities();
		
}

void World::AddComponent(COMPONENT_TYPE type, IComponent* component)
{
	_components[type].push_back(component);
}

void World::AddEntity(Entity* entity)
{
	_entities.push_back(entity);
}

void World::RemoveComponent(COMPONENT_TYPE type, IComponent* component) 
{	// add to dirtylist
	_componentsToRemove[type].push_back(component);
	_isComponentsDirty = true;
}

void World::RemoveEntity(Entity* entity) 
{	// add to dirtylist
	_entitiesToRemove.push_back(entity);
	_isEntitiesDirty = true;
}

void World::DeleteComponent(COMPONENT_TYPE type, IComponent* component)
{
	unsigned int size = _components[type].size();

	for (int i = 0; i < size; i++)
	{
		if (_components[type][i] == component)
		{	// delete and remove component from vector
			IComponent* temp = _components[type].back();
			_components[type].back() = _components[type][i];
			_components[type][i] = temp;
			delete _components[type].back();
			_components[type].pop_back();
			break;
		}
	}
}

void World::DeleteEntity(Entity* entity)
{
	unsigned int size = _entities.size();

	for (int i = 0; i < size; i++)
	{	// delete and remove entity from vector
		if (_entities[i] == entity)
		{
			Entity* temp = _entities.back();
			_entities.back() = _entities[i];
			_entities[i] = temp;
			delete _entities.back();
			_entities.pop_back();
			break;
		}
	}
}

void World::HandleDirtyComponents() 
{
	// delete and remove from list
	for (int i = 0; i < NUM_COMPONENT_TYPES; i++)
	{
		unsigned int size = _componentsToRemove[i].size();

		for (int y = 0; y < size; y++)
			DeleteComponent((COMPONENT_TYPE)i, _componentsToRemove[i][y]);

		_componentsToRemove[i].clear();
	}
	_isComponentsDirty = false;
}

void World::HandleDirtyEntities()
{
	// delete and remove from list
	unsigned int size = _entitiesToRemove.size();

	for (int i = 0; i < size; i++)
		DeleteEntity(_entitiesToRemove[i]);

	_entitiesToRemove.clear();
	_isEntitiesDirty = false;
}

void World::Empty() 
{
	// delete all components and entitys in the world
	for (int i = 0; i < NUM_COMPONENT_TYPES; i++) 
	{
		unsigned int size = _components[i].size();

		for (int y = 0; y < size; y++)
			delete _components[i][y];

		_components[i].clear();
		_componentsToRemove[i].clear();
	}

	unsigned int size = _entities.size();

	for (int i = 0; i < size; i++)
		delete _entities[i];

	_entities.clear();
	_entitiesToRemove.clear();

}