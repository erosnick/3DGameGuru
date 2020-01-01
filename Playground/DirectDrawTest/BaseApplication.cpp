#include "stdafx.h"
#include "BaseApplication.h"
#include "Utility.h"

bool BaseApplication::Initialize()
{
	mainInstanceHandle = GetModuleHandle(nullptr);

	// 初始化全局字符串
	LoadStringW(mainInstanceHandle, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(mainInstanceHandle, IDC_DIRECTDRAWTEST, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass();

	// 执行应用程序初始化:
	if (!InitInstance())
	{
		return false;
	}

	HACCEL hAccelTable = LoadAccelerators(mainInstanceHandle, MAKEINTRESOURCE(IDC_DIRECTDRAWTEST));

	return true;
}

int BaseApplication::Run()
{
	MSG msg;

	// 主消息循环:
	while (true)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				break;
			}

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			GameMain();
		}
	}

	Shutdown();

	return (int)msg.wParam;
}

void BaseApplication::GameMain()
{
	BeginDraw();

	if (pixelForamt == 32)
	{
		for (int i = 0; i < 10; i++)
		{
			int x = rand() % (clientRect.right - clientRect.left) + clientRect.left;
			int y = rand() % (clientRect.bottom - clientRect.top) + clientRect.top;

			DWORD color = RGB32BIT(0, rand() % 256, rand() % 256, rand() % 256);

			//*((DWORD *)(primaryBuffer + x * 4 + y * ddsd.lPitch)) = color;
			DrawPoint(x, y, color);
		}
	}

	EndDraw();
}

void BaseApplication::Shutdown()
{
	if (primarySurface != nullptr)
	{
		primarySurface->Release();
		primarySurface = nullptr;
	}

	if (directDraw != nullptr)
	{
		directDraw->Release();
		directDraw = nullptr;
	}
}

void BaseApplication::DrawPoint(int x, int y, DWORD color)
{
	primaryBuffer[x + y * (pitch >> 2)] = color;
}

ATOM BaseApplication::MyRegisterClass()
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = mainInstanceHandle;
	wcex.hIcon = LoadIcon(mainInstanceHandle, MAKEINTRESOURCE(IDI_DIRECTDRAWTEST));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_DIRECTDRAWTEST);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

BOOL BaseApplication::InitInstance()
{
	mainWindowHandle = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, SCREEN_WIDTH, SCREEN_HEIGHT, nullptr, nullptr, mainInstanceHandle, nullptr);

	if (!mainWindowHandle)
	{
		return FALSE;
	}

	ShowWindow(mainWindowHandle,SW_SHOW);
	UpdateWindow(mainWindowHandle);

	InitDirectDraw();
	CreatePrimarySurface();

	backSurface = CreateSurface(SCREEN_WIDTH, SCREEN_HEIGHT);

	return TRUE;
}

LRESULT CALLBACK BaseApplication::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
		{
			PostQuitMessage(0);
		}
		break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: 在此处添加使用 hdc 的任何绘图代码...
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void BaseApplication::InitDirectDraw()
{
	if (FAILED(DirectDrawCreateEx(NULL, (void**)&directDraw, IID_IDirectDraw7, nullptr)))
	{
		MessageBox(mainWindowHandle, L"Initialize DirectDraw failed!", L"Error", MB_OK);
	}

	if (FAILED(directDraw->SetCooperativeLevel(mainWindowHandle, DDSCL_NORMAL)))
	{
		MessageBox(mainWindowHandle, L"SetCooperativeLevel failed!", L"Error", MB_OK);
	}
}

void BaseApplication::CreatePrimarySurface()
{
	DDRAW_INIT_STRUCT(surfaceDesc);

	surfaceDesc.dwFlags = DDSD_CAPS;
	surfaceDesc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

	if (HRESULT hr; FAILED(hr = directDraw->CreateSurface(&surfaceDesc, &primarySurface, nullptr)))
	{
		MessageBox(mainWindowHandle, L"CreatePrimarySurface failed!", L"Error", MB_OK);
	}

	DDPIXELFORMAT ddpixelFormat;
	DDRAW_INIT_STRUCT(ddpixelFormat);

	primarySurface->GetPixelFormat(&ddpixelFormat);

	pixelForamt = ddpixelFormat.dwRGBBitCount;
}

LPDIRECTDRAWSURFACE7 BaseApplication::CreateSurface(int width, int height)
{
	DDSURFACEDESC2 ddsd;

	LPDIRECTDRAWSURFACE7 lpdds;

	ZeroMemory(&ddsd, sizeof(DDSURFACEDESC2));

	ddsd.dwSize = sizeof(DDSURFACEDESC2);
	ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;

	ddsd.dwWidth = width;
	ddsd.dwHeight = height;

	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;

	if (FAILED(directDraw->CreateSurface(&ddsd, &lpdds, nullptr)))
	{
		MessageBox(mainWindowHandle, L"CreateSurface failed!", L"Error", MB_OK);
	}

	return lpdds;
}

void BaseApplication::BeginDraw()
{
	GetWindowRect(mainWindowHandle, &clientRect);

	backSurface->Lock(nullptr, &surfaceDesc, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, nullptr);

	primaryBuffer = (DWORD*)surfaceDesc.lpSurface;
	pitch = surfaceDesc.lPitch;
}

void BaseApplication::Flip()
{

}

void BaseApplication::EndDraw()
{
	if (FAILED(backSurface->Unlock(nullptr)))
	{
		MessageBox(mainWindowHandle, L"Unlock failed!", L"Error", MB_OK);
	}

	primarySurface->Flip(nullptr, DDFLIP_WAIT);
}
