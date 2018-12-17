#pragma once
#include "CameraComponent.h"

class CameraManager
{
public:
	CameraManager();
	~CameraManager();
	
	// set and get the current game camera that is in use
	CameraComponent*& GetCurrentCameraGame() { return _currentCameraGame; }
	void SetCurrentCameraGame(CameraComponent* camera) { _currentCameraGame = camera; }

	// set and get the current UI camera that is in use
	CameraComponent*& GetCurrentCameraUI() { return _currentCameraUI; }
	void SetCurrentCameraUI(CameraComponent* camera) { _currentCameraUI = camera; }

	// set and get the current camera that renders the depth map for shadows
	CameraComponent*& GetCurrentCameraDepthMap() { return _currentCameraDepthMap; }
	void SetCurrentCameraDepthMap(CameraComponent* camera) { _currentCameraDepthMap = camera; }

private:

	// camera component pointer
	CameraComponent* _currentCameraGame;
	CameraComponent* _currentCameraUI;
	CameraComponent* _currentCameraDepthMap;
};

