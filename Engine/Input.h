#pragma once
#include <dInput.h>
#include <DirectXMath.h>
using namespace DirectX;

#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dInput8.lib")

#define SAFE_RELEASE(p) if(p != nullptr){ p->Release(); p = nullptr;}

namespace Input
{
	void Initialize(HWND hWnd);
	void Update();
	bool IsKey(int keyCode);
	bool IsKeyDown(int keyCode);
	bool IsKeyUp(int keyCode);
	void Release();

	//スクリーン座標でのマウス位置取得・設定
	XMVECTOR GetMousePosition();
	void SetMousePosition(int x,int y);
	//ウィンドウ座標でのマウス位置取得・設定
	XMVECTOR GetMouseWindowPos();
	void SetMouseWindowPos(int x, int y);
	bool IsMouseButton(int btnCode);
	bool IsMouseButtonDown(int btnCode);
	bool IsMouseButtonUp(int btnCode);
};