#pragma once

#include "ComponentTypes.h"
#include <vector>

class IComponent;
class ModelComponent;
class TransformationComponent;
class FreeMoveComponent;
class CameraComponent;
class QuadComponent;
class UVScrollComponent;
class ParticleSystemComponent;
class LightDirectionComponent;
class LightPointComponent;

class Entity
{
public:
	Entity();
	~Entity();	

	template <class T> T* AddComponent()
	{
		T* component = new T();
		_components.push_back(component);
		_components.back()->Setparent(this);

		return component;
	}

	template <class T> T* GetComponent()
	{
		COMPONENT_TYPE type = GetTypeFromTemplate<T>();
		for(int i =0; i< _components.size(); i++)
		{
			if(_components[i]->Type() == type)
			{
				return (T*)_components[i];
			}
		}

		return nullptr;
	}

	void RemoveComponent(IComponent* component);
	void RemoveEntity();

private:

	std::vector<IComponent*> _components;

	template <class T> COMPONENT_TYPE GetTypeFromTemplate()
	{
		if (std::is_same<T, TransformComponent>::value)       { return TRANSFORM_COMPONENT; }
													   
		if (std::is_same<T, ModelComponent>::value)           { return MODEL_COMPONENT; }
															   
		if (std::is_same<T, TransformationComponent>::value)  { return TRANSFORMATION_COMPONENT; }
													   
		if (std::is_same<T, FreeMoveComponent>::value)        { return FREE_MOVE_COMPONENT; }
													   
		if (std::is_same<T, CameraComponent>::value)          { return CAMERA_COMPONENT; }
													   
		if (std::is_same<T, QuadComponent>::value)            { return QUAD_COMPONENT; }
													   
		if (std::is_same<T, LightDirectionComponent>::value)  { return DIR_LIGHT_COMPONENT; }
													   
		if (std::is_same<T, LightPointComponent>::value)      { return POINT_LIGHT_COMPONENT; }
													   
		if (std::is_same<T, UVScrollComponent>::value)        { return UVSCROLL_COMPONENT; }

		if (std::is_same<T, ParticleSystemComponent>::value) { return PARTICLE_COMPONENT; }

		return NUM_COMPONENT_TYPES;		
	}

};

