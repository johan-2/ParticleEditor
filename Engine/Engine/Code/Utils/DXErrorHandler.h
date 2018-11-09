#pragma once
#include <d3d11.h>
#include <comdef.h>
#include <iostream>

namespace DX_ERROR
{
	// prints an error message from HRESULT with a message from the user
	static void PrintError(HRESULT result, const char* userMessage)
	{
		_com_error err(result);
		LPCTSTR msg = err.ErrorMessage();
		printf("****************\nWARNING DIRECT X ERROR:\nUser message  - %s\nError message - %s\n*****************\n", userMessage, msg);
	}

	// convert from wide string to string
	static std::string ConvertFromWString(wchar_t* wide)
	{
		_bstr_t conversion (wide);
		const char * text = conversion;

		return text;
	}
}