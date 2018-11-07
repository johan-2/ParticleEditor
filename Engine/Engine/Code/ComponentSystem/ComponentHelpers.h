#pragma once
#include <vector>
#include "IComponent.h"

static void RemoveComponentFromList(std::vector<IComponent*>& components, IComponent*& componentToRemove)
{
	for (int i =0; i< components.size(); i++)
	{
		if (components[i] == componentToRemove)
		{
			// switch place on the component last in list with the component we want to remove
			IComponent* temp  = components.back();
			components.back() = components[i];
			components[i]     = temp;

			// remove the last component
			components.pop_back();
			return;
		}
	}
}