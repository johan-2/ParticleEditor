#include "TexturePool.h"
#include "DXManager.h"
#include "DDSTextureLoader/DDSTextureLoader.h"
#include <comdef.h>
#include "Systems.h"
#include "DXErrorHandler.h"

TexturePool::TexturePool()
{
}

TexturePool::~TexturePool()
{
	for (std::map<std::wstring, ID3D11ShaderResourceView*>::iterator it = _textures.begin(); it != _textures.end(); it++)	
		it->second->Release();
		
	_textures.clear();
}

ID3D11ShaderResourceView* TexturePool::GetTexture(const wchar_t* name)
{
	if (name == L"")
		return nullptr;

	// get wstring
	std::wstring key(name);

	// see if the texture already exists in map 
	std::map<std::wstring, ID3D11ShaderResourceView*>::iterator it = _textures.find(key);

	// return pointer to existing texture
	if (it != _textures.end()) 
	{
		printf("Returning existing texture : %ls\n", key.c_str());
		return it->second;
	}
	else 
	{
		ID3D11ShaderResourceView* texture;

		// create new texture
		HRESULT result = DirectX::CreateDDSTextureFromFile(Systems::dxManager->GetDevice(), name, NULL, &texture);

		if (FAILED(result))
		{
			DX_ERROR::PrintError(result, "failed to create texture");
			return nullptr;
		}
		else
		{
			printf("Create texture : %ls\n", key.c_str());

			// insert texure in map
			_textures.insert(std::pair<std::wstring, ID3D11ShaderResourceView*>(key, texture));

			return texture;
		}
	}	 			
}
