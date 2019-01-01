#pragma once
#include "IEditorWindow.h"
#include "MainEditorWindow.h"
#include "SkyDomeWindow.h"

class MasterEditor
{
public:
	MasterEditor();
	~MasterEditor();

	void Update();
	void RenderStatsWindow();
	void RenderEditorWindow();

	IEditorWindow*    activeWindow;
	MainEditorWindow* mainWindow;
	SkyDomeWindow*    skyDomeWindow;


private:

	bool _showEditorWindow;
	bool _showStatsWindow;
};

