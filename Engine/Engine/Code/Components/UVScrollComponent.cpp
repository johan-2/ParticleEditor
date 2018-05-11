#include "UVScrollComponent.h"
#include "Entity.h"
#include "ModelComponent.h"
#include "ComponentTypes.h"
#include "Time.h"
#include "Mesh.h"


UVScrollComponent::UVScrollComponent() : IComponent(COMPONENT_TYPE::UVSCROLL_COMPONENT)
{
	
}

UVScrollComponent::~UVScrollComponent()
{
}

void UVScrollComponent::Init(D3DXVECTOR2 speed)
{
	_uvOffset = D3DXVECTOR2(0, 0);
	_scrollSpeed = speed;

	_modelComponent = GetComponent<ModelComponent>();

}

void UVScrollComponent::Update() 
{
	const float& delta = Time::GetInstance().GetDeltaTime();

	_uvOffset += _scrollSpeed * delta;

	_modelComponent->GetMesh()->SetUvOffset(_uvOffset);
	
}