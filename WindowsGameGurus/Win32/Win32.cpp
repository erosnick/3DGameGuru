// Win32.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "Win32.h"
#include <timeapi.h>
#include <stdint.h>

typedef struct STAR_TYP
{
	int x, y;        // position of star
	int vel;         // horizontal velocity of star
	COLORREF col;    // color of star
} STAR, * STAR_PTR;

#define NUM_STARS 256

STAR stars[256];    // holds the starfield

#define MAX_LOADSTRING 100

#define  RANDOM_COLOR() RGB(rand() % 256, rand() % 256, rand() % 256)

// Global Variables:
HWND mainWindow;
HDC hdc;
HPEN redPen;
HBRUSH whiteBrush;
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int x1 = 0;
int y1 = 0;
int x2 = 0;
int y2 = 0;

int x1v = -4 + rand() % 8;
int y1v = -4 + rand() % 8;
int x2v = -4 + rand() % 8;
int y2v = -4 + rand() % 8;

int windowWidth = 0;
int windowHeight = 0;

int colorChangeCount = 0;

void DrawRandomRectangles()
{
	Rectangle(hdc, rand() % 400, rand() % 400, rand() % 400, rand() % 400);
}

void DrawRandomLines()
{
	MoveToEx(hdc, rand() % 400, rand() % 400, nullptr);

	LineTo(hdc, rand() % 400, rand() % 400);
}

void DrawLineScreenSaver()
{
    if (colorChangeCount++ >= 100)
    {
        colorChangeCount = 0;

        if (redPen != nullptr)
        {
            DeleteObject(redPen);
        }

        redPen = CreatePen(PS_SOLID, 1, RANDOM_COLOR());

        SelectObject(hdc, redPen);
    }

    // Move endpoints of line
    x1 += x1v;
    y1 += y1v;

    x2 += x2v;
    y2 += y2v;
   
    // Test if either end hit window edge
    if (x1 < 0 || x1 >= windowWidth)
    {
        // Invert velocity
        x1v = -x1v;

        // Bum endpoint back
        x1 += x1v;
    }

    if (y1 < 0 || y1 >= windowHeight)
    {
		// Invert velocity
		y1v = -y1v;

		// Bum endpoint back
		y1 += y1v;
    }

	// Test if either end hit window edge
	if (x2 < 0 || x2 >= windowWidth)
	{
		// Invert velocity
		x2v = -x2v;

		// Bum endpoint back
		x2 += x2v;
	}

	if (y2 < 0 || y2 >= windowHeight)
	{
		// Invert velocity
		y2v = -y2v;

		// Bum endpoint back
		y2 += y2v;
	}

    MoveToEx(hdc, x1, y1, nullptr);

    LineTo(hdc, x2, y2);
}

void Draw()
{
    static float x = 10.0f;
    static float y = 10.0f;

    //SetPixel(hdc, rand() % 400, rand() % 400, RGB(rand() % 256, rand() % 256, rand() % 256));

	redPen = CreatePen(PS_SOLID, 1, RANDOM_COLOR());

	SelectObject(hdc, redPen);

	HBRUSH brush = CreateSolidBrush(RANDOM_COLOR());

    SelectObject(hdc, brush);

    DrawRandomLines();

    DeleteObject(brush);

    DeleteObject(redPen);
}

void InitStars(void)
{
	// this function initializes all the stars

	for (int index = 0; index < NUM_STARS; index++)
	{
		// select random position
		stars[index].x = rand() % windowWidth;
		stars[index].y = rand() % windowHeight;

		// set random velocity   
		stars[index].vel = 1 + rand() % 16;

		// set intensity which is inversely prop to velocity for 3D effect
		// note, I am mixing equal amounts of RGB to make black -> bright white    
		int intensity = 15 * (17 - stars[index].vel);
		stars[index].col = RGB(intensity, intensity, intensity);

	} // end for index

} // end Init_Stars

////////////////////////////////////////////////////////////

void EraseStars(void)
{
	// this function erases all the stars
	for (int index = 0; index < NUM_STARS; index++)
		SetPixel(hdc, stars[index].x, stars[index].y, RGB(0, 0, 0));

} // end Erase_Stars

////////////////////////////////////////////////////////////

void DrawStars()
{
	// this function draws all the stars
	for (int index = 0; index < NUM_STARS; index++)
		SetPixel(hdc, stars[index].x, stars[index].y, stars[index].col);


} // end Draw_Stars

////////////////////////////////////////////////////////////

void MoveStars(void)
{
	// this function moves all the stars and wraps them around the 
	// screen boundaries
	for (int index = 0; index < NUM_STARS; index++)
	{
		// move the star and test for edge
		stars[index].x += stars[index].vel;

		if (stars[index].x >= windowWidth)
			stars[index].x -= windowHeight;

	} // end for index

} // end Move_Stars

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
    LoadStringW(hInstance, IDC_WIN32, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WIN32));

    hdc = GetDC(mainWindow);

    MSG msg;

    ZeroMemory(&msg, sizeof(MSG));

    RECT rect;

    GetClientRect(mainWindow, &rect);

    WindowWidth = rect.right - rect.left;
    WindowHeight = rect.bottom - rect.top;

	int x1 = rand() % WindowWidth;
	int y1 = rand() % WindowHeight;
	int x2 = rand() % WindowWidth;
	int y2 = rand() % WindowHeight;

    whiteBrush = (HBRUSH)GetStockObject(WHITE_BRUSH);

    srand(timeGetTime());

    int64_t simulatedTime = GetTickCount64();

    //  «∑Ò”– Û±Í
    GetSystemMetrics(SM_MOUSEPRESENT);

    LPTEXTMETRIC textMetric;

    GetTextMetrics(hdc, textMetric);

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

            //Draw();
            DrawLineScreenSaver();
        }

        //SetTextColor(hdc, RGB(rand() % 256, rand() % 256, rand() % 256));

        //SetBkColor(hdc, RGB(0, 0, 0));

        //SetBkMode(hdc, TRANSPARENT);

        //TextOut(hdc, rand() % width, rand() % height, L"GDI Text Demo!", strlen("GDI Text Demo!"));
    }

    ReleaseDC(mainWindow, hdc);

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

    wcex.style          = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON1));
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON1));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WIN32);
    wcex.lpszClassName  = szWindowClass;

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

   mainWindow = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

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
        case ID_FILE_OPEN:
            break;
        case ID_FILE_SAVE:
            break;
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

        HDC hdc = GetDC(hWnd);

        SetTextColor(hdc, RGB(rand() % 256, rand() % 256, rand() % 256));

        SetBkColor(hdc, RGB(0, 0, 0));

        SetBkMode(hdc, TRANSPARENT);

        TextOut(hdc, mouseX, mouseY, L"GDI Text Demo!", strlen("GDI Text Demo!"));

        ReleaseDC(hWnd, hdc);
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
