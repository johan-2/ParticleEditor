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
	
	// get the matrices
	// world matrix is always returned transposed, the others is not
	const XMFLOAT4X4& GetWorldMatrix()    { return _worldMatrix; }
	const XMFLOAT4X4& GetPositionMatrix() { return _positionMatrix; }
	const XMFLOAT4X4& GetRotationMatrix() { return _rotationMatrix; }
	const XMFLOAT4X4& GetScaleMatrix()    { return _scaleMatrix; }

	// get transform properties by constant references
	const XMFLOAT3& GetPositionRef() { return _position; }
	const XMFLOAT3& GetRotationRef() { return _rotation; }
	const XMFLOAT3& GetScaleRef()    { return _scale; }

	// get transform properties by value
	XMFLOAT3  GetPositionVal() { return _position; }
	XMFLOAT3  GetRotationVal() { return _rotation; }
	XMFLOAT3  GetScaleVal()    { return _scale; }

	// set porperties
	void SetPosition(XMFLOAT3 position) { _position = position; }
	void SetRotation(XMFLOAT3 rotation) { _rotation = rotation; }
	void SetScale(XMFLOAT3 scale)       { _scale    = scale; }

	// get individual directions from transform
	XMFLOAT3  GetRight();
	XMFLOAT3  GetForward();
	XMFLOAT3  GetUp();

	// get all directions of transform
	void GetAllAxis(XMFLOAT3& forward, XMFLOAT3& right, XMFLOAT3& up);

	// add amount to position, rotation and scale
	void AddTranslation(XMFLOAT3& amount);
	void AddRotation(XMFLOAT3& amount);
	void AddScale(XMFLOAT3& amount);

	// updates the entire world matrix from all other matrices
	void UpdateWorldMatrix();

	// pure virtual from IComponent
	void Update(const float& delta);

private:
	
	// vector3 representations
	XMFLOAT3 _position;
	XMFLOAT3 _rotation;
	XMFLOAT3 _scale;

	// matrix representations
	XMFLOAT4X4 _positionMatrix;
	XMFLOAT4X4 _rotationMatrix;
	XMFLOAT4X4 _scaleMatrix;
	XMFLOAT4X4 _worldMatrix;
	
};

