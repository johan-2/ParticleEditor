#pragma once

#include <d3d11.h>
#include <map>

class TexturePool
{
public:
	TexturePool();
	~TexturePool();

	static TexturePool& GetInstance();

	ID3D11ShaderResourceView* GetTexture(const wchar_t* name);

private:

	std::map<std::wstring, ID3D11ShaderResourceView*> _textures;

	static TexturePool* _instance;
};

