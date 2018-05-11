#pragma once
#include "IComponent.h"

class TransformComponent : public IComponent
{
public:
	TransformComponent();
	~TransformComponent();

	void Init(D3DXVECTOR3 position = D3DXVECTOR3(0, 0, 0), D3DXVECTOR3 rotation = D3DXVECTOR3(0, 0, 0), D3DXVECTOR3 scale = D3DXVECTOR3(1, 1, 1));
	
	D3DXMATRIX GetWorldMatrix();

	D3DXVECTOR3& GetPositionRef() { return _position; }
	D3DXVECTOR3  GetPositionVal() { return _position; }

	D3DXVECTOR3& GetRotationRef() { return _rotation; }
	D3DXVECTOR3  GetRotationVal() { return _rotation; }

	D3DXVECTOR3& GetScaleRef() { return _scale; }
	D3DXVECTOR3  GetScaleVal() { return _scale; }
	
	D3DXVECTOR3  GetRight() { return CalculateAxises(Axis::RIGHT); }
	D3DXVECTOR3  GetForward() { return CalculateAxises(Axis::FORWARD); }
	D3DXVECTOR3  GetUp() { return CalculateAxises(Axis::UP); }

	void GetAllAxis(D3DXVECTOR3& forward, D3DXVECTOR3& right, D3DXVECTOR3& up) { CalculateAxises(Axis::ALL, forward, right, up); }

	void Update();

	enum Axis
	{
		RIGHT,
		FORWARD,
		UP,
		ALL,
		NONE
	};

private:
		
	D3DXVECTOR3 CalculateAxises(Axis axis, D3DXVECTOR3& forward = D3DXVECTOR3(0,0,0), D3DXVECTOR3& right = D3DXVECTOR3(0, 0, 0), D3DXVECTOR3& up = D3DXVECTOR3(0, 0, 0));
	
	D3DXVECTOR3 _position;
	D3DXVECTOR3 _rotation;
	D3DXVECTOR3 _scale;
	
};

