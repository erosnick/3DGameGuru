// DirectDraw.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "DirectDraw.h"
#include <ddraw.h>
#include <stdint.h>
#include <timeapi.h>
#include <cstdio>

#pragma pack(1)

#define MAX_LOADSTRING 100

const int SCREEN_WIDTH = 1920;
const int SCREEN_HEIGHT = 1080;
const int SCREEN_BPP = 32;

int memoryPitch = 0;
uint32_t* videoBuffer = nullptr;
bool windowClosed = false;

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
LPDIRECTDRAWCLIPPER clipper = nullptr;

// a low tech bitmap that uses palette entry 1 for the color :)
UCHAR happyBitmap[256] = { 0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,
							0,0,0,1,1,0,0,0,0,0,0,1,1,0,0,0,
							0,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,
							0,1,0,0,0,1,1,0,0,1,1,0,0,0,1,0,
							0,1,0,0,0,1,1,0,0,1,1,0,0,0,1,0,
							1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
							1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
							1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
							1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
							1,0,0,1,0,0,0,0,0,0,0,0,1,0,0,1,
							1,0,0,0,1,0,0,0,0,0,0,1,0,0,0,1,
							0,1,0,0,0,1,1,1,1,1,1,0,0,0,1,0,
							0,1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,
							0,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,
							0,0,0,1,1,0,0,0,0,0,0,1,1,0,0,0,
							0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0 };

							
UCHAR sadBitmap[64] = { 0,0,0,0,0,0,0,0,
						0,0,1,1,1,1,0,0,
						0,1,0,1,1,0,1,0,
						0,1,1,1,1,1,1,0,
						0,1,1,0,0,1,1,0,
						0,1,0,1,1,0,1,0,
						0,0,1,1,1,1,0,0,
						0,0,0,0,0,0,0,0 };

struct HappyFace
{
	int x;
	int y;
	int vx;
	int vy;
};

struct Bitmap
{
	int width;
	int height;
	int bpp;
	int imageSize;
	int bytesPerLine;
	UCHAR* imageData;
};

Bitmap bitmap;

HappyFace happyFaces[100];

#define RGB32(a, r, g, b) ((a & 255) << 24) + ((r & 255) << 16) + ((g & 255) << 8) + (b & 255)

#define DDRAW_INIT_STRUCT(ddstruct) ZeroMemory(&ddstruct, sizeof(ddstruct)); ddstruct.dwSize = sizeof(ddstruct)

#define  RANDOM_COLOR() RGB(rand() % 256, rand() % 256, rand() % 256)

#define SAFE_RELEASE(pointer) pointer->Release(); pointer = nullptr

EXTERN_C ULONG64 myAdd(ULONG64 u1, ULONG64 u2);
EXTERN_C ULONG64 mySub(ULONG64 u1, ULONG64 u2);

EXTERN_C void Fill(uint32_t width, uint32_t color, uint32_t* videoBuffer);
//void Fill(uint32_t width, uint32_t color, uint32_t* videoBuffer)
//{
//	for (int i = 0; i < SCREEN_HEIGHT; i++)
//	{
//		for (int i = 0; i < width; i++)
//		{
//			videoBuffer[i] = color;
//		}
//
//		videoBuffer += memoryPitch;
//	}
//}

// x, y - Position to draw bitmap
// width, height - Size of bitmap in pixels
// bitmap - Pointer to bitmap data
// videoBuffer - Pointer to video buffer surface
// memoryPitch - Video pitch per line
void BlitClipped(int x, int y, int bitmapWidth, int bitmapHeight, UCHAR* bitmapBuffer, uint32_t* videoBuffer, int memoryPitch, RECT clipRect)
{
	int width = clipRect.right - clipRect.left;
	int height = clipRect.bottom - clipRect.top;

	// First do trivial rejections of bitmap, is it totally invisible?
	if ((x >= width) || (y >= height) ||
		 (x + bitmapWidth <= clipRect.left) || (y + bitmapHeight <= clipRect.top))
	{
		return;
	}

	// Clip source rectangle
	// Pre-compute the bounding rect to make life easy
	int x1 = x;
	int y1 = y;
	int x2 = x1 + bitmapWidth - 1;
	int y2 = y1 + bitmapHeight - 1;

	// Upper left hand corner first
	if (x1 < 0)
	{
		x1 = 0;
	}

	if (y1 < 0)
	{
		y1 = 0;
	}

	// Now lower left hand corner
	if (x2 >= width)
	{
		x2 = width - 1;
	}

	if (y2 >= height)
	{
		y2 = height - 1;
	}

	// Now we know to draw only the portions of
	// the bitmap from (x1, y1) to (x2, y2)
	// compute offsets into bitmap on x, y axes,
	// we need this to compute starting point
	// to rasterize from
	int xOffset = x1 - x;
	int yOffset = y1 - y;

	// Compute number of columns and rows to blit
	int dx = x2 - x1 + 1;
	int dy = y2 - y1 + 1;

	// Compute starting address in videoBuffer
	videoBuffer += (x1 + y1 * memoryPitch);

	// Compute starting address in bitmap to scan data from
	bitmapBuffer += (xOffset + yOffset * bitmapWidth);

	UCHAR pixel;

	// At this point bitmap is pointing to the first
	// pixel in the bitmap that needs to 
	// be blitted, and videoBuffer is pointing to
	// the memory location on the destination
	// buffer to put it, so now enter rasterizer loop
	for (int indexY = 0; indexY < dy; indexY++)
	{
		for (int indexX = 0; indexX < dx; indexX++)
		{
			// Read pixel from source bitmap
			// test for transparency and plot
			if ((pixel = bitmapBuffer[indexX]))
			{
				videoBuffer[indexX] = RGB32(255, 255, 255, 0);
			}
		}

		videoBuffer += memoryPitch;
		bitmapBuffer += bitmapWidth;
	}
}

LPDIRECTDRAWCLIPPER AttachClipper(LPDIRECTDRAWSURFACE7 surface, int numRects, LPRECT clipList)
{							// Looping variable
	LPDIRECTDRAWCLIPPER createdClipper = nullptr;
	LPRGNDATA regionData;

	if (FAILED(directDraw->CreateClipper(0, &createdClipper, nullptr)))
	{
		MessageBox(nullptr, L"CreateClipper failed.", L"Error", MB_OK);
		return nullptr;
	}

	size_t dataSize = sizeof(RECT) * numRects;
	
	regionData = (LPRGNDATA)(new UCHAR[sizeof(RGNDATAHEADER) + dataSize]);

	memcpy_s(regionData->Buffer, sizeof(RECT) * numRects, clipList, dataSize);

	regionData->rdh.dwSize = sizeof(RGNDATAHEADER);
	regionData->rdh.iType = RDH_RECTANGLES;
	regionData->rdh.nCount = numRects;
	regionData->rdh.nRgnSize = dataSize;
	regionData->rdh.rcBound.left = 0;
	regionData->rdh.rcBound.top = 0;
	regionData->rdh.rcBound.right = 10000;
	regionData->rdh.rcBound.bottom = 10000;

	RECT* clipBound = &regionData->rdh.rcBound;

	// Find bounds of all clipping regions
	for (size_t i = 0; i < numRects; i++)
	{
		// Test if the next rectangle unioned with
		// the current bound is larger
		if (clipList[i].left < clipBound->left)
		{
			clipBound->left = clipList[i].left;
		}

		if (clipList[i].right > clipBound->right)
		{
			clipBound->right = clipList[i].right;
		}

		if (clipList[i].top < clipBound->top)
		{
			clipBound->top = clipList[i].top;
		}

		if (clipList[i].bottom > clipBound->bottom)
		{
			clipBound->bottom = clipList[i].bottom;
		}
	}

	// Now we have computed the bounding rectangle region and set up the data
	// Let's set the clipping list
	if (FAILED(createdClipper->SetClipList(regionData, 0)))
	{
		delete[] regionData;
		return nullptr;
	}

	// Now attach the clipper to the surface
	if (FAILED(surface->SetClipper(createdClipper)))
	{
		delete[] regionData;
		return nullptr;
	}

	delete[] regionData;

	return createdClipper;
}

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
	surfaceDesc.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
	surfaceDesc.dwBackBufferCount = 1;
	surfaceDesc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_COMPLEX | DDSCAPS_FLIP;

	if (FAILED(directDraw->CreateSurface(&surfaceDesc, &primarySurface, nullptr)))
	{
		MessageBox(nullptr, L"CreateSurface failed.", L"Error", MB_OK);
		return false;
	}

	surfaceDesc.ddsCaps.dwCaps = DDSCAPS_BACKBUFFER;

	if (FAILED(primarySurface->GetAttachedSurface(&surfaceDesc.ddsCaps, &secondarySurface)))
	{
		MessageBox(nullptr, L"GetAttachedSurface failed.", L"Error", MB_OK);
		return false;
	}

	if (FAILED(directDraw->CreateClipper(0, &clipper, nullptr)))
	{
		MessageBox(nullptr, L"CreateClipper failed.", L"Error", MB_OK);
		return false;
	}

	RECT clipList[] = {0, 0, 100, 100};

	clipper = AttachClipper(secondarySurface, 1, clipList);

    return true;
}

void Lock(LPDIRECTDRAWSURFACE7 surface)
{
	DDSURFACEDESC2 surfaceDesc;

	ZeroMemory(&surfaceDesc, sizeof(DDSURFACEDESC2));

	surfaceDesc.dwSize = sizeof(DDSURFACEDESC2);

	if (FAILED(surface->Lock(nullptr, &surfaceDesc, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, nullptr)))
	{
		MessageBox(nullptr, L"Lock failed.", L"Error", MB_OK);
	}

	memoryPitch = surfaceDesc.lPitch >> 2;

	videoBuffer = (uint32_t*)surfaceDesc.lpSurface;
}

void Unlock(LPDIRECTDRAWSURFACE7 surface)
{
	if (FAILED(surface->Unlock(nullptr)))
	{
		MessageBox(nullptr, L"Lock failed.", L"Error", MB_OK);
	}
}

void MyLoadBitmap(const TCHAR* fileName, Bitmap* bitmap)
{
	// struct BITMAPFILEHEADER {
	//	WORD    bfType;
	//	DWORD   bfSize;
	//	WORD    bfReserved1;
	//	WORD    bfReserved2;
	//	DWORD   bfOffBits;
	//};
	BITMAPFILEHEADER bitmapHeader;

	size_t headerSize = sizeof(BITMAPFILEHEADER);

	ZeroMemory(&bitmapHeader, headerSize);

	FILE* file = nullptr;

	if (_wfopen_s(&file, fileName, L"rb") != 0)
	{
		TCHAR buffer[32];

		wsprintf(buffer, L"Open file %s failed!", fileName);

		MessageBox(nullptr, buffer, L"Error", MB_OK);

		return;
	}

	UCHAR* bitmapData = nullptr;

	if (file != nullptr)
	{
		fread_s(&bitmapHeader, headerSize, headerSize, 1, file);

		if (bitmapHeader.bfType != 0x4D42)
		{
			MessageBox(nullptr, L"Not bitmap file.", L"Error", MB_OK);

			return;
		}

		size_t bitmapInfoSize = sizeof(BITMAPINFO);

		BITMAPINFO bitmapInfo;

		ZeroMemory(&bitmapInfo, bitmapInfoSize);

		fread_s(&bitmapInfo, bitmapInfoSize, bitmapInfoSize, 1, file);

		bitmap->width = bitmapInfo.bmiHeader.biWidth;
		bitmap->height = bitmapInfo.bmiHeader.biHeight;
		bitmap->bpp = bitmapInfo.bmiHeader.biBitCount;
		bitmap->imageSize = bitmapInfo.bmiHeader.biSizeImage;

		int bytePerPixel = bitmap->bpp / 8;

		bitmap->bytesPerLine = bitmap->width * bytePerPixel;

		if (bitmap->imageSize != bitmap->width * bitmap->height * bytePerPixel)
		{
			// 24位位图，每个像素占3字节，每个颜色分量占8位
			// 例子：图像宽度为5像素，字节数为5 * 3 = 15(byte)，不能被4整除
			// (5 * 24 + 31) / 8 = 18(byte)
			// 18 / 4 * 4 = 16，正好可以被4整除。
			// 加31的作用是补3个字节(31 / 8 = 3)，1字节补3字节正好是4字节；
			// 2字节补3字节为5，可以通过5 / 4 * 4去掉多余的字节(整数除法丢弃结果小数部分)，以此类推。
			int dataSizePerLine = (bitmap->width * bitmap->bpp + 31) / 8;

			dataSizePerLine = dataSizePerLine / 4 * 4;

			bitmap->bytesPerLine = dataSizePerLine;

			bitmap->imageSize = bitmap->bytesPerLine * bitmap->height;
		}

		bitmap->imageData = new UCHAR[bitmap->imageSize];

		fseek(file, -bitmap->imageSize, SEEK_END);

		int read = fread_s(bitmap->imageData, bitmap->imageSize, bitmap->imageSize, 1, file);

		if (read == 0)
		{
			MessageBox(nullptr, L"Read file failed.", L"Error", MB_OK);
		}
	}
}

int FlipBitmap(UCHAR* image, int bytesPerLine, int height)
{
	// this function is used to flip bottom-up .BMP images
	int loop = height / 2;

	UCHAR* swapBuffer = (UCHAR*)(new UCHAR[bytesPerLine]);

	// Swap in place reduce copy count
	for (int i = 0; i < loop; i++)
	{
		memcpy_s(swapBuffer, bytesPerLine, &image[i * bytesPerLine], bytesPerLine);

		memcpy_s(&image[i * bytesPerLine], bytesPerLine, &image[bytesPerLine * height - (i + 1) * bytesPerLine], bytesPerLine);

		memcpy_s(&image[bytesPerLine * height - (i + 1) * bytesPerLine], bytesPerLine, swapBuffer, bytesPerLine);
	}

	delete[] swapBuffer;

	// return success
	return(1);

} // end Flip_Bitmap

bool GameInit()
{
    InitDirectDraw();

	srand(timeGetTime());

	for (size_t i = 0; i < 100; i++)
	{
		happyFaces[i].x = rand() % SCREEN_WIDTH;
		happyFaces[i].y = rand() % SCREEN_HEIGHT;
		happyFaces[i].vx = -5 + rand() % 8;
		happyFaces[i].vy = -5 + rand() % 10;
	}

	MyLoadBitmap(L"kitten.bmp", &bitmap);

	FlipBitmap(bitmap.imageData, bitmap.bytesPerLine, bitmap.height);

	return true;
}

void PlotPixel(int x, int y, uint32_t color)
{
	videoBuffer[y * memoryPitch + x] = color;
}

bool GameMain()
{
	if (windowClosed)
	{
		return true;
	}

	DDBLTFX ddbltfx;

	DDRAW_INIT_STRUCT(ddbltfx);

	ddbltfx.dwFillColor = 0;

	RECT destRect;

	destRect.left = 100;
	destRect.top = 0;
	destRect.right = 200;
	destRect.bottom = 100;

	RECT sourceRect;

	sourceRect.left = 0;
	sourceRect.top = 0;
	sourceRect.right = 100;
	sourceRect.bottom = 100;

	if (FAILED(secondarySurface->Blt(nullptr, nullptr, nullptr, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx)))
	{
		MessageBox(nullptr, L"Blt failed.", L"Error", MB_OK);
		return false;
	}

	//if (FAILED(primarySurface->Blt(&destRect, secondarySurface, &sourceRect, DDBLT_WAIT, nullptr)))
	//{
	//	MessageBox(nullptr, L"Blt failed.", L"Error", MB_OK);
	//	return false;
	//}

	int pixelOffset = bitmap.bpp / 8;

	Lock(secondarySurface);

	for (size_t y = 0; y < bitmap.height; y++)
	{
		for (size_t x = 0; x < bitmap.width; x++)
		{
			int index = y * bitmap.bytesPerLine  + x * pixelOffset;

			UCHAR b = bitmap.imageData[index];
			UCHAR g = bitmap.imageData[index + 1];
			UCHAR r = bitmap.imageData[index + 2];

			int color = RGB32(255, r, g, b);

			PlotPixel(x, y, color);
		}
	}

	//for (int i = 0; i < SCREEN_HEIGHT; i++)
	//{
	//	Fill(SCREEN_WIDTH, RANDOM_COLOR(), videoBuffer);

	//	videoBuffer += memoryPitch;
	//}

	//RECT clipRect = { 0, 0, 100, 100 };

	//for (size_t i = 0; i < 100; i++)
	//{
	//	BlitClipped(happyFaces[i].x, happyFaces[i].y , 16, 16, happyBitmap, videoBuffer, memoryPitch, clipRect);
	//}

	//for (size_t i = 0; i < 100; i++)
	//{
	//	// Move
	//	happyFaces[i].x += happyFaces[i].vx;
	//	happyFaces[i].y += happyFaces[i].vy;

	//	// Check for off screen, if so wrap
	//	if (happyFaces[i].x > SCREEN_WIDTH)
	//	{
	//		happyFaces[i].x = -8;
	//		happyFaces[i].vx = -5 + rand() % 8;
	//		happyFaces[i].vy = -5 + rand() % 10;
	//	}
	//	else if (happyFaces[i].x < -8)
	//	{
	//		happyFaces[i].x = SCREEN_WIDTH;
	//		happyFaces[i].vx = -5 + rand() % 8;
	//		happyFaces[i].vy = -5 + rand() % 10;
	//	}

	//	if (happyFaces[i].y > SCREEN_HEIGHT)
	//	{
	//		happyFaces[i].y = -8;
	//		happyFaces[i].vx = -5 + rand() % 8;
	//		happyFaces[i].vy = -5 + rand() % 10;
	//	}
	//	else if (happyFaces[i].y < -8)
	//	{
	//		happyFaces[i].y = SCREEN_HEIGHT;
	//		happyFaces[i].vx = -5 + rand() % 8;
	//		happyFaces[i].vy = -5 + rand() % 10;
	//	}
	//}

	Unlock(secondarySurface);

	if (FAILED(primarySurface->Flip(nullptr, DDFLIP_WAIT)))
	{
		MessageBox(nullptr, L"Flip failed.", L"Error", MB_OK);
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
			windowClosed = true;
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
