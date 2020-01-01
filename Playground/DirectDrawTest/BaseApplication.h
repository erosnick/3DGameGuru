#pragma once

#include "Resource.h"
#include <windows.h>
#include <d3d8.h>
#include <ddraw.h>

class BaseApplication
{
public:
	bool Initialize();

	int Run();

	void GameMain();

	void Shutdown();

	void DrawPoint(int x, int y, DWORD color);
private:

	ATOM                MyRegisterClass();
	BOOL                InitInstance();

	static LRESULT CALLBACK    WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	void InitDirectDraw();
	void CreatePrimarySurface();
	LPDIRECTDRAWSURFACE7 CreateSurface(int width, int height);

	void BeginDraw();

	void Flip();

	void EndDraw();

private:
	static const int  MAX_LOADSTRING = 256;

	HWND mainWindowHandle;
	HINSTANCE mainInstanceHandle;                                // 当前实例
	WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
	WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名
	LPDIRECTDRAW7 directDraw = nullptr;
	LPDIRECTDRAWPALETTE directDrawPalette = nullptr;
	LPDIRECTDRAWSURFACE7 primarySurface = nullptr;
	LPDIRECTDRAWSURFACE7 backSurface = nullptr;
	DDSURFACEDESC2 surfaceDesc;
	RECT clientRect;
	DWORD* primaryBuffer;
	UINT SCREEN_WIDTH = 800;
	UINT SCREEN_HEIGHT = 600;
	UINT SCREEN_BPP = 32;
	int pixelForamt = 0;
	int pitch;
};