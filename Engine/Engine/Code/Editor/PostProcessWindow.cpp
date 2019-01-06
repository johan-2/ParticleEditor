#include "PostProcessWindow.h"
#include "MasterEditor.h"
#include "GUI.h"
#include "SystemDefs.h"
#include "Systems.h"
#include "PostProcessingShader.h"
PostProcessWindow::PostProcessWindow(MasterEditor* masterEditor) : IEditorWindow(masterEditor)
{
}

PostProcessWindow::~PostProcessWindow()
{
}

void PostProcessWindow::Render()
{
	GUI::BeginWindow("Post Process Settings", 0.6f, SCREEN_WIDTH * 0.22f, SCREEN_HEIGHT * 0.5f, 0, 0, 0, 0);

	// BLOOM
	GUI::Text(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "BLOOM");
	GUI::CheckBox("1", "Apply Bloom",   "if bloom will be applied to final image",          &PostProcessing::APPLY_BLOOM);
	GUI::CheckBox("2", "Two Pass Blur", "if we will do a two pass blur for better quality", &PostProcessing::BLOOM_USE_TWO_PASS_BLUR);

	GUI::SetItemWidth(SCREEN_WIDTH * 0.10f);
	GUI::FloatSlider("3", "Intensity", "How strong the bloom effect will be", 0.1f, 10.0f, &PostProcessing::BLOOM_INTENSITY);

	GUI::FloatSlider("4", "Res Scale Down P1", "How much we will lower the res output blurred image on first pass",  1.0f, 10.0f, &PostProcessing::BLOOM_BLUR_SCALE_DOWN_PASS_1);
	GUI::SameLine(); if (GUI::Button("Apply1")) Systems::renderer->GetPostProcessShader()->CreateBlurRenderTextures();
	GUI::FloatSlider("5", "Res Scale Down P2", "How much we will lower the res output blurred image on second pass", 1.0f, 10.0f, &PostProcessing::BLOOM_BLUR_SCALE_DOWN_PASS_2);
	GUI::SameLine(); if (GUI::Button("Apply2")) Systems::renderer->GetPostProcessShader()->CreateBlurRenderTextures();
	GUI::ClearItemWidth();

	// DEPTH OF FIELD
	//
	//


	// serialize settings
	if (GUI::Button("Save Settings"))
	{
		std::string file = GUI::FindFileFromDirectory(".json", "save post processing settings");
		if (file != "")
			PostProcessing::WriteSettings(file.c_str());
	}

	// deserialize settings
	if (GUI::Button("Load Settings"))
	{
		std::string file = GUI::FindFileFromDirectory(".json", "save post processing settings");
		if (file != "")
			PostProcessing::ReadSettings(file.c_str());
	}

	if (GUI::Button("Back"))
		GoToMain();

	GUI::EndWindow();
}