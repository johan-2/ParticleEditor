#pragma once
#include "IComponent.h"

#include <DirectXMath.h>

using namespace DirectX;

class TransformComponent : public IComponent
{
public:
	TransformComponent();
	~TransformComponent();

	void Init(XMFLOAT3 position = XMFLOAT3(0, 0, 0), XMFLOAT3 rotation = XMFLOAT3(0, 0, 0), XMFLOAT3 scale = XMFLOAT3(1, 1, 1));
	
	XMFLOAT4X4 GetWorldMatrix();

	XMFLOAT3& GetPositionRef() { return _position; }
	XMFLOAT3  GetPositionVal() { return _position; }

	XMFLOAT3& GetRotationRef() { return _rotation; }
	XMFLOAT3  GetRotationVal() { return _rotation; }

	XMFLOAT3& GetScaleRef() { return _scale; }
	XMFLOAT3  GetScaleVal() { return _scale; }
	
	XMFLOAT3  GetRight() { return CalculateAxises(Axis::RIGHT); }
	XMFLOAT3  GetForward() { return CalculateAxises(Axis::FORWARD); }
	XMFLOAT3  GetUp() { return CalculateAxises(Axis::UP); }

	void GetAllAxis(XMFLOAT3& forward, XMFLOAT3& right, XMFLOAT3& up) { CalculateAxises(Axis::ALL, forward, right, up); }

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
		
	XMFLOAT3 CalculateAxises(Axis axis, XMFLOAT3& forward = XMFLOAT3(0,0,0), XMFLOAT3& right = XMFLOAT3(0, 0, 0), XMFLOAT3& up = XMFLOAT3(0, 0, 0));
	
	XMFLOAT3 _position;
	XMFLOAT3 _rotation;
	XMFLOAT3 _scale;
	
};

