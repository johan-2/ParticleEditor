#pragma once

#include <D3D11.h>
#include <DirectXMath.h>
#include "IComponent.h"

using namespace DirectX;

class TransformComponent;

class CameraComponent : public IComponent
{
public:
	CameraComponent();

	void Init3D(const float& fov);
	void Init2D(const XMFLOAT2& size, const XMFLOAT2& nearfar);
	
	~CameraComponent();
	
	void Update();
		
	const XMFLOAT4X4& GetViewMatrix()       { return _viewMatrix; }
	const XMFLOAT4X4& GetProjectionMatrix() { return _projectionMatrix; }

	ID3D11ShaderResourceView* GetSRV()         { return _renderTexture;}
	void SetSRV(ID3D11ShaderResourceView* tex) { _renderTexture = tex; }
	
private:
	
	void CalculateViewMatrix();

	TransformComponent* _transform;
	XMFLOAT4X4 _viewMatrix;
	XMFLOAT4X4 _projectionMatrix;

	ID3D11ShaderResourceView* _renderTexture;		
};

