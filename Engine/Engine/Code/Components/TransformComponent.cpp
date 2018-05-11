#include "TransformComponent.h"

TransformComponent::TransformComponent() : IComponent(COMPONENT_TYPE::TRANSFORM_COMPONENT)
{	
}

TransformComponent::~TransformComponent()
{	
}

void TransformComponent::Init(D3DXVECTOR3 position, D3DXVECTOR3 rotation, D3DXVECTOR3 scale)
{
	_position = position;
	_rotation = rotation;
	_scale = scale;
}

D3DXMATRIX TransformComponent::GetWorldMatrix() 
{
	// find out if we are leaking memory here, ruturns a pointer and takes an outparameter? 
	D3DXMATRIX matrixPosition = *D3DXMatrixIdentity(&matrixPosition);	
	D3DXMATRIX matrixScale = *D3DXMatrixIdentity(&matrixScale);
	D3DXMATRIX matrixRotation = *D3DXMatrixIdentity(&matrixRotation);
	
	// translate and scale to our matrices
	D3DXMatrixTranslation(&matrixPosition, _position.x, _position.y, _position.z);
	D3DXMatrixScaling(&matrixScale, _scale.x, _scale.y, _scale.z);

	// apply rotation to matrix(convert to radians)	
	D3DXMatrixRotationYawPitchRoll(&matrixRotation, D3DXToRadian( _rotation.y) , D3DXToRadian(_rotation.x), D3DXToRadian(_rotation.z));
	
	// return all matrices multiplied
	return matrixScale * matrixRotation * matrixPosition;
	
}

D3DXVECTOR3 TransformComponent::CalculateAxises(Axis axis, D3DXVECTOR3& forward, D3DXVECTOR3& right, D3DXVECTOR3& up)
{
	// get rotation matrix
	D3DXMATRIX matrixRotation = *D3DXMatrixIdentity(&matrixRotation);
	D3DXMatrixRotationYawPitchRoll(&matrixRotation, D3DXToRadian(_rotation.y), D3DXToRadian(_rotation.x), D3DXToRadian(_rotation.z));

	D3DXVECTOR3 f = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	D3DXVECTOR3 r = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	D3DXVECTOR3 u = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	
	// transform axis by our rotation
	D3DXVec3TransformCoord(&f, &f, &matrixRotation);
	D3DXVec3TransformCoord(&r, &r, &matrixRotation);
	D3DXVec3TransformCoord(&u, &u, &matrixRotation);

	switch (axis)
	{
	case TransformComponent::FORWARD:
		return *D3DXVec3Normalize(&f, &f);
		break;
	case TransformComponent::RIGHT:
		return *D3DXVec3Normalize(&r, &r);
		break;
	case TransformComponent::UP:
		return *D3DXVec3Normalize(&u, &u);
		break;	
	case TransformComponent::ALL:
		forward = f;
		right = r;
		up = u;
		break;
	}

	return D3DXVECTOR3(0,0,0);
}

void TransformComponent::Update() 
{

}

