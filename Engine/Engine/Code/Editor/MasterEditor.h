#pragma once
#include "IEditorWindow.h"
#include "MainEditorWindow.h"
#include "SkyDomeWindow.h"
#include "PostProcessWindow.h"

class MasterEditor
{
public:
	MasterEditor();
	~MasterEditor();

	void Update();
	void RenderStatsWindow();
	void RenderEditorWindow();

	IEditorWindow*     activeWindow;
	MainEditorWindow*  mainWindow;
	SkyDomeWindow*     skyDomeWindow;
	PostProcessWindow* postProcessWindow;

private:

	bool _showEditorWindow;
	bool _showStatsWindow;
};

