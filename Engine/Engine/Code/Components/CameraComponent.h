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
	~CameraComponent();

	// get camera matrices
	XMFLOAT4X4& GetViewMatrix()          { return _viewMatrix; }
	XMFLOAT4X4& GetProjectionMatrix()    { return _projectionMatrix; }
	XMFLOAT4X4& GetViewProjMatrix()      { return _viewProjMatrix; }
	XMFLOAT4X4& GetViewProjMatrixTrans() { return _viewProjMatrixTrans; }

	XMFLOAT4X4 GetReflectionViewProj(float yPosition, bool viewOnly = false);

	// get and set an associated render texture with the camera
	ID3D11ShaderResourceView* GetSRV()         { return _renderTexture;}
	void SetSRV(ID3D11ShaderResourceView* tex) { _renderTexture = tex; }

	// init for perspective and orthographic
	void Init3D(const float& fov);
	void Init2D(const XMFLOAT2& size, const XMFLOAT2& nearfar);
	
	// update from pure virtual in IComponent
	void Update(const float& delta);
	
private:
	
	// calculates the camera viewmatrix
	void CalculateViewMatrix();

	// cached pointer to transform component
	TransformComponent* _transform;

	// camera matrices
	XMFLOAT4X4 _viewMatrix;
	XMFLOAT4X4 _projectionMatrix;
	XMFLOAT4X4 _viewProjMatrix;
	XMFLOAT4X4 _viewProjMatrixTrans;

	// pointer to a render texture
	ID3D11ShaderResourceView* _renderTexture;
};

