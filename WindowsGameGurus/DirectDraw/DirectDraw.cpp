// DirectDraw.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "DirectDraw.h"
#include <ddraw.h>
#include <stdint.h>

#define MAX_LOADSTRING 100

const int SCREEN_WIDTH = 1920;
const int SCREEN_HEIGHT = 1080;
const int SCREEN_BPP = 32;

// Global Variables:
HWND mainWindow;
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

LPDIRECTDRAW7 directDraw = nullptr;
LPDIRECTDRAWSURFACE7 primarySurface = nullptr;
LPDIRECTDRAWSURFACE7 secondarySurface = nullptr;

#define RGB24(r, g, b) ((r) << 16) + ((g) << 8) + b

#define SAFE_RELEASE(pointer) pointer->Release(); pointer = nullptr

bool InitDirectDraw()
{
    if (FAILED(DirectDrawCreateEx(nullptr, (void**)&directDraw, IID_IDirectDraw7, nullptr)))
    {
        MessageBox(nullptr, L"DirectDrawCreateEx failed.", L"Error", MB_OK);

        return false;
    }

	if (FAILED(directDraw->SetCooperativeLevel(mainWindow, DDSCL_FULLSCREEN | DDSCL_ALLOWMODEX |
														   DDSCL_EXCLUSIVE | DDSCL_ALLOWREBOOT)))
	{
		MessageBox(nullptr, L"SetCooperativeLevel failed.", L"Error", MB_OK);
		return false;
	}

	if (FAILED(directDraw->SetDisplayMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, 0, 0)))
	{
		MessageBox(nullptr, L"SetDisplayMode failed.", L"Error", MB_OK);
		return false;
	}

	DDSURFACEDESC2 surfaceDesc;

	ZeroMemory(&surfaceDesc, sizeof(DDSURFACEDESC2));

	surfaceDesc.dwSize = sizeof(DDSURFACEDESC2);
	surfaceDesc.dwBackBufferCount = 1;
	surfaceDesc.dwFlags = DDSD_CAPS;
	surfaceDesc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

	if (FAILED(directDraw->CreateSurface(&surfaceDesc, &primarySurface, nullptr)))
	{
		MessageBox(nullptr, L"CreateSurface failed.", L"Error", MB_OK);
		return false;
	}

    return true;
}

bool GameInit()
{
    InitDirectDraw();

	return true;
}

bool GameMain()
{
	DDSURFACEDESC2 surfaceDesc;

	ZeroMemory(&surfaceDesc, sizeof(DDSURFACEDESC2));

	surfaceDesc.dwSize = sizeof(DDSURFACEDESC2);

	if (FAILED(primarySurface->Lock(nullptr, &surfaceDesc, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, nullptr)))
	{
		MessageBox(nullptr, L"Lock failed.", L"Error", MB_OK);

		return false;
	}

	int memoryPitch = surfaceDesc.lPitch >> 2;

	uint32_t* videoBuffer = (uint32_t*)surfaceDesc.lpSurface;

	for (size_t i = 0; i < 10000; i++)
	{
		int x = rand() % SCREEN_WIDTH;
		int y = rand() % SCREEN_HEIGHT;

		videoBuffer[y * memoryPitch + x] = RGB24(255, 0, 0);
	}

	if (FAILED(primarySurface->Unlock(nullptr)))
	{
		MessageBox(nullptr, L"Lock failed.", L"Error", MB_OK);

		return false;
	}

	return true;
}

void GameShutdown()
{
	SAFE_RELEASE(primarySurface);
	SAFE_RELEASE(directDraw);
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_DIRECTDRAW, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_DIRECTDRAW));

	MSG msg;

	ZeroMemory(&msg, sizeof(MSG));

	GameInit();

	int64_t simulatedTime = GetTickCount64();

	// 是否有鼠标
	GetSystemMetrics(SM_MOUSEPRESENT);

	// Main message loop:
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}

		if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
		{
			PostMessage(mainWindow, WM_CLOSE, 0, 0);
		}

		int64_t realTime = GetTickCount64();

		if (simulatedTime < realTime)
		{
			simulatedTime += 16;

			GameMain();
		}
	}

	GameShutdown();

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS | CS_OWNDC;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DIRECTDRAW));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= nullptr;// MAKEINTRESOURCEW(IDC_DIRECTDRAW);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   mainWindow = CreateWindowW(szWindowClass, szTitle, WS_POPUP,
      CW_USEDEFAULT, 0, SCREEN_WIDTH, SCREEN_HEIGHT, nullptr, nullptr, hInstance, nullptr);

   if (!mainWindow)
   {
      return FALSE;
   }

   ShowWindow(mainWindow, nCmdShow);
   UpdateWindow(mainWindow);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code that uses hdc here...
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_ACTIVATE:

		if (LOWORD(wParam) != WA_INACTIVE)
		{
			// Application is being activated

		}
		else
		{
			// Application i s being deactivated
		}

		break;
	case WM_CLOSE:
	{
		int result = MessageBox(hWnd, L"Are you sure?", L"Notice", MB_YESNO);

		if (result == IDYES)
		{
			DestroyWindow(hWnd);
		}
	}

	break;
	case WM_SIZE:
	{
		int width = LOWORD(lParam);
		int height = HIWORD(lParam);

		auto sizeType = wParam;

		switch (sizeType)
		{
		case SIZE_MINIMIZED:
			break;
		case SIZE_MAXIMIZED:
			break;
		default:
			break;
		}
	}

	break;
	case WM_MOVE:
	{
		int positionX = (int)LOWORD(lParam);
		int positionY = (int)HIWORD(lParam);
	}

	break;

	case WM_CHAR:
		break;

	case WM_KEYDOWN:
	{
		int keyCode = (int)wParam;
	}

	break;

	case WM_MOUSEMOVE:
	{
		int mouseX = (int)LOWORD(lParam);
		int mouseY = (int)HIWORD(lParam);

		int buttons = (int)wParam;

		if (buttons & MK_LBUTTON)
		{

		}
		else if (buttons & MK_RBUTTON)
		{

		}
		else if (buttons & MK_MBUTTON)
		{

		}
	}

	break;

	// Needs CS_DBLCLKS
	case WM_LBUTTONDBLCLK:
	{
		int mouseX = (int)LOWORD(lParam);
		int mouseY = (int)HIWORD(lParam);
	}

	case WM_LBUTTONDOWN:
	{
		int mouseX = (int)LOWORD(lParam);
		int mouseY = (int)HIWORD(lParam);
	}

	break;

	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
