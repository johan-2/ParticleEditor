#pragma once

#include <D3D11.h>
#include "imgui.h"

class GuiManager
{
public:
	GuiManager();
	~GuiManager();

	static GuiManager& GetInstance();

	void Update();
	void Initialize(HWND hwnd);

	ID3D11Buffer*& GetVertexBuffer() { return _vertexBuffer; }
	ID3D11Buffer*& GetIndexBuffer() { return _indexBuffer; }

	void UpdateMouseCursor();
		
private:
	
	void CreateBuffers();
	void CreateTexture();

	

	ID3D11ShaderResourceView* _texture;
	ID3D11ShaderResourceView* _fontTex;
	ID3D11Buffer* _vertexBuffer;
	ID3D11Buffer* _indexBuffer;

	HWND _hwnd;

	ImGuiMouseCursor _lastMouseCursor = ImGuiMouseCursor_COUNT;

	static GuiManager* _instance;
};

