#pragma once
#include <DirectXMath.h>

using namespace DirectX;

namespace MATH_HELPERS
{
	static XMFLOAT4X4 MatrixMutiplyTrans(const XMFLOAT4X4* a, const XMFLOAT4X4* b, const XMFLOAT4X4* c)
	{
		XMFLOAT4X4 result;

		XMStoreFloat4x4(&result, XMMatrixMultiply(XMLoadFloat4x4(a),
			XMMatrixMultiply(XMLoadFloat4x4(b), XMLoadFloat4x4(c))));

		XMStoreFloat4x4(&result, XMMatrixTranspose(XMLoadFloat4x4(&result)));

		return result;
	}

	static XMFLOAT4X4 MatrixMutiplyTrans(const XMFLOAT4X4* a, const XMFLOAT4X4* b)
	{
		XMFLOAT4X4 result;

		XMStoreFloat4x4(&result, XMMatrixMultiply(XMLoadFloat4x4(a), XMLoadFloat4x4(b)));
		XMStoreFloat4x4(&result, XMMatrixTranspose(XMLoadFloat4x4(&result)));
			
		return result;
	}

	static XMFLOAT4X4 MatrixMutiply(const XMFLOAT4X4* a, const XMFLOAT4X4* b, const XMFLOAT4X4* c)
	{
		XMFLOAT4X4 result;

		XMStoreFloat4x4(&result, XMMatrixMultiply(XMLoadFloat4x4(a),
			XMMatrixMultiply(XMLoadFloat4x4(b), XMLoadFloat4x4(c))));

		return result;
	}

	static XMFLOAT4X4 MatrixMutiply(const XMFLOAT4X4* a, const XMFLOAT4X4* b)
	{
		XMFLOAT4X4 result;

		XMStoreFloat4x4(&result, XMMatrixMultiply(XMLoadFloat4x4(a), XMLoadFloat4x4(b)));

		return result;
	}
}
