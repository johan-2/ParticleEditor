#pragma once
#include "CameraComponent.h"

class CameraManager
{
public:

	static CameraManager& GetInstance();
	CameraManager();
	~CameraManager();
	
	CameraComponent*& GetCurrentCameraGame() { return _currentCameraGame; }
	void SetCurrentCameraGame(CameraComponent* camera) { _currentCameraGame = camera; }

	CameraComponent*& GetCurrentCameraUI() { return _currentCameraUI; }
	void SetCurrentCameraUI(CameraComponent* camera) { _currentCameraUI = camera; }

	CameraComponent*& GetCurrentCameraDepthMap() { return _currentCameraDepthMap; }
	void SetCurrentCameraDepthMap(CameraComponent* camera) { _currentCameraDepthMap = camera; }

private:

	static CameraManager* _instance;
	CameraComponent* _currentCameraGame;
	CameraComponent* _currentCameraUI;
	CameraComponent* _currentCameraDepthMap;
};

