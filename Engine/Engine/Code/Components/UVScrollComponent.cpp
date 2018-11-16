#include "UVScrollComponent.h"
#include "Entity.h"
#include "ModelComponent.h"
#include "ComponentTypes.h"
#include "Time.h"
#include "Mesh.h"
#include "Systems.h"

UVScrollComponent::UVScrollComponent() : IComponent(COMPONENT_TYPE::UVSCROLL_COMPONENT)
{
}

UVScrollComponent::~UVScrollComponent()
{
}

void UVScrollComponent::Init(XMFLOAT2 speed)
{
	_uvOffset = XMFLOAT2(0, 0);
	_scrollSpeed = speed;

	_modelComponent = GetComponent<ModelComponent>();

}

void UVScrollComponent::Update(const float& delta)
{
	XMFLOAT2 translation;
	translation.x = _scrollSpeed.x * delta;
	translation.y = _scrollSpeed.y * delta;

	_uvOffset.x += translation.x;
	_uvOffset.y += translation.y;

	const std::vector<Mesh*>& meshes = _modelComponent->GetMeshes(); 
	unsigned int count = meshes.size();

	for (int i = 0; i < count; i++)
		meshes[i]->SetUvOffset(_uvOffset);
}