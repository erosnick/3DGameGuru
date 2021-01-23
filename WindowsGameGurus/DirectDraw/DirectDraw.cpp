// DirectDraw.cpp : Defines the entry point32_t for the application.
//

#include "framework.h"
#include "DirectDraw.h"
#include <ddraw.h>
#include <stdint.h>
#include <timeapi.h>
#include <cstdio>
#include <vector>
#include <memory>
#include <windowsx.h>

#include "Utils.h"

#pragma warning(disable : 4244)
#pragma warning(disable : 4389)
#pragma warning(disable : 4018)
#pragma warning(disable : 4267)
#pragma warning(disable : 4838)

#define MAX_LOADSTRING 100

const int32_t SCREEN_WIDTH = 1920;
const int32_t SCREEN_HEIGHT = 1080;
const int32_t WINDOW_WIDTH = 1280;
const int32_t WINDOW_HEIGHT = 720;
const int32_t SCREEN_BPP = 32;

RECT windowRect = { 0, 0, 1280, 720 };

bool fullScreen = false;

float positionX = 0;

bool windowClosed = false;
bool windowMinimized = false;

// Global Variables:
HWND mainWindow;
HINSTANCE hInst;                                // current instance
TCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int32_t);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

LPDIRECTDRAW7 directDraw = nullptr;
LPDIRECTDRAWSURFACE7 primarySurface = nullptr;
LPDIRECTDRAWSURFACE7 secondarySurface = nullptr;
LPDIRECTDRAWCLIPPER clipper = nullptr;
LPDIRECTDRAWSURFACE7 offScreenSurface = nullptr;

LPDIRECTDRAWSURFACE7 CreateSurface(int32_t width, int32_t height, int32_t memoryFlags = DDSCAPS_VIDEOMEMORY);

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
	int32_t x;
	int32_t y;
	int32_t vx;
	int32_t vy;
};

struct Bitmap
{
	Bitmap()
		: width(0),
		  height(0),
		  bpp(0),
		  imageSize(0),
		  bytesPerLine(0),
		  imageData(nullptr)
	{}

	Bitmap(const Bitmap& bitmap) 
	{
		Assign(bitmap.width, bitmap.height, bitmap.bpp, bitmap.imageSize, bitmap.bytesPerLine);

		imageData = new UCHAR[imageSize];

		memcpy_s(imageData, imageSize, bitmap.imageData, bitmap.imageSize);
	}

	Bitmap& operator=(const Bitmap& bitmap)
	{
		Assign(bitmap.width, bitmap.height, bitmap.bpp, bitmap.imageSize, bitmap.bytesPerLine);

		imageData = new UCHAR[imageSize];

		memcpy_s(imageData, imageSize, bitmap.imageData, bitmap.imageSize);

		return *this;
	}

	Bitmap& operator=(Bitmap&& bitmap) noexcept
	{
		Assign(bitmap.width, bitmap.height, bitmap.bpp, bitmap.imageSize, bitmap.bytesPerLine);

		imageData = bitmap.imageData;

		bitmap.imageData = nullptr;

		return *this;
	}

	Bitmap(Bitmap&& bitmap) noexcept
	{
		Assign(bitmap.width, bitmap.height, bitmap.bpp, bitmap.imageSize, bitmap.bytesPerLine);

		imageData = bitmap.imageData;

		bitmap.imageData = nullptr;
	}

	~Bitmap() { delete[] imageData; }

	int32_t width;
	int32_t height;
	int32_t bpp;
	int32_t imageSize;
	int32_t bytesPerLine;
	UCHAR* imageData;

private:

	void Assign(int32_t inWidth, int32_t inHeight, int32_t inBpp, 
				int32_t inImageSize, int32_t inBytesPerLine)
	{
		width = inWidth;
		height = inHeight;
		bpp = inBpp;
		imageSize = inImageSize;
		bytesPerLine = inBytesPerLine;
	}
};

struct Sprite
{
	Sprite() 
	  : scale(1.0f),
	    surface(nullptr)
	{}

	Sprite(const Sprite& sprite)
	{
		scale = sprite.scale;
		bitmap = sprite.bitmap;

		surface = CreateSurface(bitmap.width, bitmap.height);
	}

	Sprite(Sprite&& sprite) noexcept
	{
		scale = sprite.scale;
		bitmap = sprite.bitmap;

		surface = sprite.surface;

		sprite.surface = nullptr;
	}

	Sprite& operator=(const Sprite& sprite)
	{
		scale = sprite.scale;
		bitmap = sprite.bitmap;

		surface = CreateSurface(bitmap.width, bitmap.height);

		return *this;
	}

	Sprite& operator=(Sprite&& sprite) noexcept
	{
		scale = sprite.scale;
		bitmap = sprite.bitmap;

		surface = sprite.surface;

		sprite.surface = nullptr;

		return *this;
	}
	
	~Sprite() { SAFE_RELEASE(surface); }

	float scale;
	Bitmap bitmap;
	LPDIRECTDRAWSURFACE7 surface;
};

class Renderable
{

};

class Animator
{
public:

	Animator()
		: tileX(0),
		  tileY(0),
		  tileSizeX(0),
		  tileSizeY(0),
		  frames(0),
		  animationTime(0.0f),
		  animationFPS(15.0f)
	{}

	Animator(int32_t inTileX, int32_t inTileY, int32_t inTileSizeX, int32_t inTileSizeY, int32_t inFrames)
		: tileX(inTileX),
		tileY(inTileY),
		tileSizeX(inTileSizeX),
		tileSizeY(inTileSizeY),
		frames(inFrames),
		animationTime(0.0f),
		animationFPS(15.0f)
	{

	}

	void Update(float deltaTime)
	{
		animationTime += deltaTime;

		if (animationTime >= 1.0f / animationFPS)
		{
			animationTime = 0.0f;

			tileX++;

			if (tileX >= 8)
			{
				tileX = 0;
			}
		}
	}

	int32_t tileX;
	int32_t tileY;
	int32_t tileSizeX;
	int32_t tileSizeY;
	int32_t frames;
	float animationTime;
	float animationFPS;
};

class Actor :public Renderable
{
public:

	Actor()
	  : x(0),
		y(0),
		sprite(nullptr)
	{}

	Actor(const Actor& actor) = delete;
	
	~Actor() {}

	void SetPosition(int newX, int newY)
	{
		x = newX;
		y = newY;
	}

	void SetSprite(std::shared_ptr<Sprite>& newSprite)
	{
		sprite = newSprite;
	}

	void SetAnimator(const Animator& newAnimator)
	{
		animator = newAnimator;
	}

	void UpdateAnimator(float deltaTime)
	{
		animator.Update(deltaTime);
	}

	const std::shared_ptr<Sprite>& GetSprite() const
	{
		return sprite;
	}

	const Animator& GetAnimator() const
	{
		return animator;
	}

public:

	int x;
	int y;

private:

	std::shared_ptr<Sprite> sprite;
	Animator animator;
};

struct Vertex2D
{
	float x;
	float y;
};

struct Polygon2D
{
	Polygon2D() {}
	~Polygon2D() { delete[] vertices; }

	int32_t startX = 0;					// Initial position of center of polygon
	int32_t startY = 0;
	int32_t x0 = 0;					// Position of center of polygon
	int32_t y0 = 0;
	int32_t xv = 0;					// Initial velocity
	int32_t yv = 0;
	int32_t state = 1;				// State of polygon
	int32_t numVertices = 1;		// Number of vertices
	uint32_t color = Color::Blue;

	Vertex2D* vertices = nullptr;	// Pointer to vertex list
};

std::vector<std::shared_ptr<Actor>> renderables;

std::shared_ptr<Polygon2D> triangle;

Polygon2D triangles[100];

int32_t triangleCount = 5;

std::shared_ptr<Sprite> sprite;

struct VideoMemory
{
	int32_t memoryPitch = 0;
	uint32_t* videoBuffer = nullptr;
};

Bitmap bitmap;

HappyFace happyFaces[100];

bool CheckSurfaces()
{
	// Check the primary surface
	if (primarySurface) {
		if (primarySurface->IsLost() == DDERR_SURFACELOST) {
			directDraw->RestoreAllSurfaces();
			return false;
		}
	}

	return true;
}

LPDIRECTDRAWCLIPPER AttachClipper(LPDIRECTDRAWSURFACE7 surface, int32_t numRects, LPRECT clipList);

EXTERN_C ULONG64 myAdd(ULONG64 u1, ULONG64 u2);
EXTERN_C ULONG64 mySub(ULONG64 u1, ULONG64 u2);

int32_t FlipBitmap(UCHAR* image, int32_t bytesPerLine, int32_t height);

void DrawBitmap(const Bitmap& bitmap, const VideoMemory& videoMemory, bool isBackbuffer = false);

void DrawSprite(const std::shared_ptr<Sprite>& sprite, int32_t x, int32_t y);

void DrawSprite(const std::shared_ptr<Sprite>& spriteSheet, int32_t x, int32_t y, int32_t tileX, int32_t tileY, RECT destRect, RECT sourceRect);

void DrawRenderables(const std::vector<std::shared_ptr<Actor>>& renderables);

void DrawClipLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t color, uint32_t* videoBuffer, int32_t pitch, RECT clipRect);

bool ClipLine(int32_t& x1, int32_t& y1, int32_t& x2, int32_t& y2, RECT clipRect);

void DrawPolygon2D(Polygon2D* polygon, uint32_t* videoBuffer, int32_t pitch);

void TranslatePolygon2D(Polygon2D* polygon, int32_t dx, int32_t dy);

void RotatePolygon2D(Polygon2D* polygon, float angle);

void ScalePolygon2D(Polygon2D* polygon, float scaleX, float scaleY);

void InitializeTriangles();

void MoveTriangles();

void RotateTriangles();

void DrawTriangles();

void Clear(uint32_t color);

bool Present();

bool GameInit();
bool GameMain();
void GameShutdown();

EXTERN_C void Fill(uint32_t width, uint32_t color, uint32_t* videoBuffer);
//void Fill(uint32_t width, uint32_t color, uint32_t* videoBuffer)
//{
//	for (int32_t i = 0; i < SCREEN_HEIGHT; i++)
//	{
//		for (int32_t i = 0; i < width; i++)
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
void BlitClipped(int32_t x, int32_t y, int32_t bitmapWidth, int32_t bitmapHeight, 
UCHAR* bitmapBuffer, uint32_t* videoBuffer, int32_t memoryPitch, RECT clipRect)
{
	int32_t width = clipRect.right - clipRect.left;
	int32_t height = clipRect.bottom - clipRect.top;

	// First do trivial rejections of bitmap, is it totally invisible?
	if ((x >= width) || (y >= height) ||
		 (x + bitmapWidth <= clipRect.left) || (y + bitmapHeight <= clipRect.top))
	{
		return;
	}

	// Clip source rectangle
	// Pre-compute the bounding rect to make life easy
	int32_t x1 = x;
	int32_t y1 = y;
	int32_t x2 = x1 + bitmapWidth - 1;
	int32_t y2 = y1 + bitmapHeight - 1;

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
	int32_t xOffset = x1 - x;
	int32_t yOffset = y1 - y;

	// Compute number of columns and rows to blit
	int32_t dx = x2 - x1 + 1;
	int32_t dy = y2 - y1 + 1;

	// Compute starting address in videoBuffer
	videoBuffer += (x1 + y1 * memoryPitch);

	// Compute starting address in bitmap to scan data from
	bitmapBuffer += (xOffset + yOffset * bitmapWidth);

	UCHAR pixel;

	// At this point32_t bitmap is pointing to the first
	// pixel in the bitmap that needs to 
	// be blitted, and videoBuffer is pointing to
	// the memory location on the destination
	// buffer to put it, so now enter rasterizer loop
	for (int32_t indexY = 0; indexY < dy; indexY++)
	{
		for (int32_t indexX = 0; indexX < dx; indexX++)
		{
			// Read pixel from source bitmap
			// test for transparency and plot
			if ((pixel = bitmapBuffer[indexX]))
			{
				videoBuffer[indexX] = Color::Yellow;
			}
		}

		videoBuffer += memoryPitch;
		bitmapBuffer += bitmapWidth;
	}
}

LPDIRECTDRAWSURFACE7 CreateSurface(int32_t width, int32_t height, int32_t memoryFlags)
{
	DDSURFACEDESC2 surfaceDesc;
	LPDIRECTDRAWSURFACE7 surface = nullptr;

	DDRAW_INIT_STRUCT(surfaceDesc);

	// Set to access caps, width, and height
	surfaceDesc.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT | DDSD_CKSRCBLT;

	// Set dimensions of the new bitmap surface
	surfaceDesc.dwWidth = width;
	surfaceDesc.dwHeight = height;

	// Set surface to offscreen plain
	surfaceDesc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | memoryFlags;

	//surfaceDesc.ddckCKSrcBlt.dwColorSpaceLowValue = RGB32(255, 255, 255, 255);
	//surfaceDesc.ddckCKSrcBlt.dwColorSpaceHighValue = RGB32(255, 255, 255, 255);

	surfaceDesc.ddckCKSrcBlt.dwColorSpaceLowValue = 0;
	surfaceDesc.ddckCKSrcBlt.dwColorSpaceHighValue = 0;

	// Create the surface
	if (DDFailedCheck(directDraw->CreateSurface(&surfaceDesc, &surface, nullptr), L"CreateSurface failed"))
	{
		return nullptr;
	}

	return surface;
}

LPDIRECTDRAWCLIPPER AttachClipper(LPDIRECTDRAWSURFACE7 surface, int32_t numRects, LPRECT clipList)
{							// Looping variable
	LPDIRECTDRAWCLIPPER createdClipper = nullptr;
	LPRGNDATA regionData;

	if (DDFailedCheck(directDraw->CreateClipper(0, &createdClipper, nullptr), L"CreateClipper failed."))
	{
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
	if (DDFailedCheck(createdClipper->SetClipList(regionData, 0), L"SetClipList failed"))
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
    if (DDFailedCheck(DirectDrawCreateEx(nullptr, (void**)&directDraw, IID_IDirectDraw7, nullptr), L"DirectDrawCreateEx failed"))
    {
        return false;
    }

	if (fullScreen)
	{
		if (DDFailedCheck(directDraw->SetCooperativeLevel(mainWindow, DDSCL_FULLSCREEN | DDSCL_EXCLUSIVE | DDSCL_ALLOWREBOOT), L"SetCooperativeLevel failed"))
		{
			return false;
		}
	}
	else
	{
		if (DDFailedCheck(directDraw->SetCooperativeLevel(mainWindow, DDSCL_NORMAL), L"SetCooperativeLevel failed"))
		{
			return false;
		}
	}

	//if (DDFailedCheck(directDraw->SetDisplayMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, 0, 0), L"SetDisplayMode failed"))
	//{
	//	return false;
	//}

	DDSURFACEDESC2 surfaceDesc;

	DDRAW_INIT_STRUCT(surfaceDesc);

	if (fullScreen)
	{
		surfaceDesc.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
		surfaceDesc.dwBackBufferCount = 1;
		surfaceDesc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_COMPLEX | DDSCAPS_FLIP;
	}
	else
	{
		surfaceDesc.dwFlags = DDSD_CAPS;
		surfaceDesc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
	}

	if (DDFailedCheck(directDraw->CreateSurface(&surfaceDesc, &primarySurface, nullptr), L"CreateSurface failed"))
	{
		return false;
	}

	if (fullScreen)
	{
		surfaceDesc.ddsCaps.dwCaps = DDSCAPS_BACKBUFFER;

		if (DDFailedCheck(primarySurface->GetAttachedSurface(&surfaceDesc.ddsCaps, &secondarySurface), L"GetAttachedSurface failed"))
		{
			return false;
		}
	}

	offScreenSurface = CreateSurface(SCREEN_WIDTH, SCREEN_HEIGHT);

	if (fullScreen)
	{
		RECT clipList[] = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };

		clipper = AttachClipper(offScreenSurface, 1, clipList);
	}
	else
	{
		DDFailedCheck(directDraw->CreateClipper(0, &clipper, nullptr), L"CreateClipper failed");
		clipper->SetHWnd(0, mainWindow);

		DDFailedCheck(primarySurface->SetClipper(clipper), L"SetClipper failed");
	}

    return true;
}

VideoMemory Lock(LPDIRECTDRAWSURFACE7 surface)
{
	DDSURFACEDESC2 surfaceDesc;

	DDRAW_INIT_STRUCT(surfaceDesc);

	DDFailedCheck(surface->Lock(nullptr, &surfaceDesc, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, nullptr), L"Lock failed");

	VideoMemory videoMemory;

	videoMemory.memoryPitch = surfaceDesc.lPitch >> 2;

	videoMemory.videoBuffer = (uint32_t*)surfaceDesc.lpSurface;

	return videoMemory;
}

void Unlock(LPDIRECTDRAWSURFACE7 surface)
{
	DDFailedCheck(surface->Unlock(nullptr), L"Lock failed");
}

int32_t FlipBitmap(UCHAR* image, int32_t bytesPerLine, int32_t height)
{
	// this function is used to flip bottom-up .BMP images
	int32_t loop = height / 2;

	UCHAR* swapBuffer = (UCHAR*)(new UCHAR[bytesPerLine]);

	// Swap in place reduce copy count
	for (int32_t i = 0; i < loop; i++)
	{
		memcpy_s(swapBuffer, bytesPerLine, &image[i * bytesPerLine], bytesPerLine);

		memcpy_s(&image[i * bytesPerLine], bytesPerLine, &image[bytesPerLine * height - (i + 1) * bytesPerLine], bytesPerLine);

		memcpy_s(&image[bytesPerLine * height - (i + 1) * bytesPerLine], bytesPerLine, swapBuffer, bytesPerLine);
	}

	delete[] swapBuffer;

	// return success
	return(1);

} // end Flip_Bitmap

void MyLoadBitmap(const TCHAR* fileName, Bitmap* bitmap, bool flipVertical = true)
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

		int32_t bytePerPixel = bitmap->bpp / 8;

		bitmap->bytesPerLine = bitmap->width * bytePerPixel;

		// 不知道为何被PS编辑过的文件会多出2字节？
		// https://en.wikipedia.org/wiki/Talk%3ABMP_file_format
		// As for Adobe, they do have a slightly incorrect BMP image file. Specifically, 
		// Adobe appends two extra 0x00 bytes to the end of the file. 
		// For example a 640x480 32bpp image should have 1,228,800 bytes, 
		// but Photoshop's output for this same image has 1,228,802 bytes.
		if (bitmap->imageSize > bitmap->width * bitmap->height * bytePerPixel + 2)
		{
			// 24位位图，每个像素占3字节，每个颜色分量占8位
			// 例子：图像宽度为5像素，字节数为5 * 3 = 15(byte)，不能被4整除
			// (5 * 24 + 31) / 8 = 18(byte)
			// 18 / 4 * 4 = 16，正好可以被4整除。
			// 加31的作用是补3个字节(31 / 8 = 3)，1字节补3字节正好是4字节；
			// 2字节补3字节为5，可以通过5 / 4 * 4去掉多余的字节(整数除法丢弃结果小数部分)，以此类推。
			int32_t dataSizePerLine = (bitmap->width * bitmap->bpp + 31) / 8;

			dataSizePerLine = dataSizePerLine / 4 * 4;

			bitmap->bytesPerLine = dataSizePerLine;

			bitmap->imageSize = bitmap->bytesPerLine * bitmap->height;
		}

		bitmap->imageData = new UCHAR[bitmap->imageSize];

		fseek(file, -bitmap->imageSize, SEEK_END);

		size_t read = fread_s(bitmap->imageData, bitmap->imageSize, bitmap->imageSize, 1, file);

		if (read == 0)
		{
			MessageBox(nullptr, L"Read file failed.", L"Error", MB_OK);
		}
	}

	if (flipVertical)
	{
		FlipBitmap(bitmap->imageData, bitmap->bytesPerLine, bitmap->height);
	}
}

void PlotPixel(int32_t x, int32_t y, uint32_t color, const VideoMemory& videoMemory)
{
	videoMemory.videoBuffer[y * videoMemory.memoryPitch + x] = color;
}

Sprite* CreateSprite(const TCHAR* fileName)
{
	Sprite* sprite = new Sprite();

	MyLoadBitmap(fileName, &sprite->bitmap);

	sprite->surface = CreateSurface(sprite->bitmap.width, sprite->bitmap.height);

	VideoMemory videoMemory = Lock(sprite->surface);

	DrawBitmap(sprite->bitmap, videoMemory);

	Unlock(sprite->surface);

	return sprite;
}

void DrawBitmap(const Bitmap& bitmap, const VideoMemory& videoMemory, bool isBackbuffer)
{
	int32_t pixelOffset = bitmap.bpp / 8;

	for (size_t y = 0; y < bitmap.height; y++)
	{
		for (size_t x = 0; x < bitmap.width; x++)
		{
			int32_t index = y * bitmap.bytesPerLine + x * pixelOffset;

			UCHAR b = bitmap.imageData[index];
			UCHAR g = bitmap.imageData[index + 1];
			UCHAR r = bitmap.imageData[index + 2];

			int32_t color = RGB32(255, r, g, b);

			if (isBackbuffer)
			{
				PlotPixel(x + windowRect.left, y + windowRect.top, color, videoMemory);
			}
			else
			{
				PlotPixel(x, y, color, videoMemory);
			}
		}
	}
}

void DrawSprite(const std::shared_ptr<Sprite>& sprite, int32_t x, int32_t y)
{
	RECT destRect = { x, y, destRect.left + sprite->bitmap.width * sprite->scale, destRect.top + sprite->bitmap.height * sprite->scale };

	RECT sourceRect = { 0, 0, sourceRect.left + sprite->bitmap.width, sourceRect.top + sprite->bitmap.height };

	DDFailedCheck(offScreenSurface->Blt(&destRect, sprite->surface, &sourceRect, DDBLT_WAIT | DDBLT_KEYSRC, nullptr), L"Blt failed");
}

void DrawSprite(const std::shared_ptr<Sprite>& spriteSheet, int32_t x, int32_t y, int32_t tileX, int32_t tileY, RECT destRect, RECT sourceRect)
{
	DDFailedCheck(offScreenSurface->Blt(&destRect, spriteSheet->surface, &sourceRect, DDBLT_WAIT | DDBLT_KEYSRC, nullptr), L"Blt failed");
}

void DrawRenderables(const std::vector<std::shared_ptr<Actor>>& renderables)
{
	for (const auto renderable : renderables)
	{
		Animator animator = renderable->GetAnimator();

		RECT destRect = { renderable->x, renderable->y, destRect.left + animator.tileSizeX * renderable->GetSprite()->scale,
														destRect.top + animator.tileSizeY * renderable->GetSprite()->scale};

		RECT sourceRect = { animator.tileX * animator.tileSizeX, animator.tileY * animator.tileSizeY, 
							sourceRect.left + animator.tileSizeX, sourceRect.top + animator.tileSizeY };

		DrawSprite(renderable->GetSprite(), renderable->x, renderable->y, animator.tileX, animator.tileY, destRect, sourceRect);
	}
}

void DrawClipLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t color, uint32_t* videoBuffer, int32_t pitch, RECT clipRect)
{
	// This function draws a line from x0, y0 to x1, y1
	// Using differential error term (based on Bresenham's work)
	int32_t dx;				// Difference in x's
	int32_t dy;				// Difference in y's
	int32_t dx2;			// dx * 2
	int32_t dy2;			// dy * 2
	int32_t xIncreament;	// Amount in pixel space to move during drawings
	int32_t yIncreament;	// Amount in pixel space to move during drawings
	int32_t error;			// The discriminant i.e. error i.e. decision variable
	int32_t index;			// Used for looping

	ClipLine(x0, y0, x1, y1, clipRect);

	// Precompute first pixel address in video buffer
	videoBuffer = videoBuffer + x0 + y0 * pitch;

	// Compute horizontal and vertical deltas
	dx = x1 - x0;
	dy = y1 - y0;

	// Test which direction the line is going in i.e. slope angle

	// Line is moving right
	if (dx >= 0)
	{

		xIncreament = 1;
	}
	// Line is moving left
	else
	{
		xIncreament = -1;
		dx = -dx;	// Need absolute value
	}

	// Test y component of slope

	// Line is moving down
	if (dy >= 0)
	{
		yIncreament = pitch;
	}
	// Line is moving up
	else
	{
		yIncreament = -pitch;
		dy = -dy;	// Need absolute value
	}
	
	// Compute (dx, dy) * 2
	dx2 = dx << 1;
	dy2 = dy << 1;

	// Now need on which delta is greater we can draw the line

	// |slope <= 1|
	if (dx > dy)
	{
		// Initialize error term
		error = dy2 - dx;

		// Draw the line
		for (index = 0; index <= dx; index++)
		{
			// Set the pixel
			*videoBuffer = color;

			// Test if error has overflowed
			if (error >= 0)
			{
				error -= dx2;

				// Move to next line
				videoBuffer += yIncreament;
			}

			// Adjust the error term
			error += dy2;

			// Move to the next pixel
			videoBuffer += xIncreament;
		}
	}
	// |slope > 1|
	else
	{
		// Initialize error term
		error = dx2 - dy;

		// Draw the line
		for (index = 0; index <= dy; index++)
		{
			// Set the pixel
			*videoBuffer = color;

			// Test if error overflowed
			if (error >= 0)
			{
				error -= dy2;

				// Move to next line
				videoBuffer += xIncreament;
			}

			// Adjust the error term
			error += dx2;

			// Move to the next pixel
			videoBuffer += yIncreament;
		}
	}
}

bool ClipLine(int32_t& x1, int32_t& y1, int32_t& x2, int32_t& y2, RECT clipRect)
{
	// Internal clipping codes
	const uint32_t CLIP_CODE_C =	 0x0000;
	const uint32_t CLIP_CODE_NORTH = 0x0008;
	const uint32_t CLIP_CODE_SOUTH = 0x0004;
	const uint32_t CLIP_CODE_EAST =  0x0002;
	const uint32_t CLIP_CODE_WEST =  0x0001;

	const uint32_t CLIP_CODE_NORTH_EAST = 0x000A;
	const uint32_t CLIP_CODE_SOUTH_EAST = 0x0006;
	const uint32_t CLIP_CODE_NOUTH_WEST = 0x0009;
	const uint32_t CLIP_CODE_SOUTH_WEST = 0x0005;

	int32_t xc1 = x1;
	int32_t yc1 = y1;
	int32_t xc2 = x2;
	int32_t yc2 = y2;

	int32_t p1Code = 0;
	int32_t p2Code = 0;

	int32_t minClipX = clipRect.left;
	int32_t maxClipX = clipRect.right;
	int32_t minClipY = clipRect.top;
	int32_t maxClipY = clipRect.bottom;

	// Determine codes for p1 and p2
	if (y1 < minClipY)
	{
		p1Code |= CLIP_CODE_NORTH;
	}
	else if (y1 > maxClipY)
	{
		p1Code |= CLIP_CODE_SOUTH;
	}

	if (x1 < minClipX)
	{
		p1Code |= CLIP_CODE_WEST;
	}
	else if (x1 > maxClipX)
	{
		p1Code |= CLIP_CODE_EAST;
	}

	if (y2 < minClipY)
	{
		p2Code |= CLIP_CODE_NORTH;
	}
	else if (y2 > maxClipY)
	{
		p2Code |= CLIP_CODE_SOUTH;
	}

	if (x2 < minClipX)
	{
		p2Code |= CLIP_CODE_WEST;
	}
	else if (x2 > maxClipX)
	{
		p2Code |= CLIP_CODE_EAST;
	}

	// Try and trivially reject
	if ((p1Code & p2Code))
	{
		return false;
	}

	// Test for totally visible, if so leave points untouched
	if (p1Code == 0 && p2Code == 0)
	{
		return true;
	}

	// Determine end clip point for p1
	switch (p1Code)
	{
	case CLIP_CODE_C:
		break;

	case CLIP_CODE_NORTH:
	{
		yc1 = minClipY;
		xc1 = x1 + 0.5 + (minClipY - y1) * (x2 - x1) / (y2 - y1);
	}
	
	break;

	case CLIP_CODE_SOUTH:
	{
		yc1 = maxClipY;
		xc1 = x1 + 0.5 + (maxClipY - y1) * (x2 - x1) / (y2 - y1);
	}
	
	break;

	case CLIP_CODE_WEST:
	{
		xc1 = minClipX;
		yc1 = y1 + 0.5 + (minClipX - x1) * (y2 - y1) / (x2 - x1);
	}
	
	break;

	case CLIP_CODE_EAST:
	{
		xc1 = maxClipX;
		yc1 = y1 + 0.5 + (maxClipX - x1) * (y2 - y1) / (x2 - x1);
	}
	
	break;

	// These cases are more complex, must compute 2 intersections
	case CLIP_CODE_NORTH_EAST:
	{
		// North hline intersection
		yc1 = minClipY;
		xc1 = x1 + 0.5 + (minClipY - y1) * (x2 - x1) / (y2 - y1);

		// Test if intersection is valid
		// if so then done, else compute next
		if (xc1 < minClipX || xc1 > maxClipX)
		{
			// East vline intersection
			xc1 = maxClipX;
			yc1 = y1 + 0.5 + (maxClipX - x1) * (y2 - y1) / (x2 - x1);
		}
	}
		
	break;

	case CLIP_CODE_SOUTH_EAST:
	{
		// South hline intersection
		yc1 = maxClipY;
		xc1 = x1 + 0.5 + (maxClipY - y1) * (x2 - x1) / (y2 - y1);

		// Test if intersection is valid
		// if so then done, else compute next
		if (xc1 < minClipX || xc1 > maxClipX)
		{
			// East vline intersection
			xc1 = maxClipX;
			yc1 = y1 + 0.5 + (maxClipX - x1) * (y2 - y1) / (x2 - x1);
		}
	}
		
	break;

	case CLIP_CODE_NOUTH_WEST:
	{
		// North hline intersection
		yc1 = minClipY;
		xc1 = x1 + 0.5 + (minClipY - y1) * (x2 - x1) / (y2 - y1);

		// Test if intersection is valid
		// if so then done, else compute next
		if (xc1 < minClipX || xc1 > maxClipX)
		{
			xc1 = minClipX;
			yc1 = y1 + 0.5 + (minClipX - x1) * (y2 - y1) / (x2 - x1);
		}
	}

	break;

	case CLIP_CODE_SOUTH_WEST:
	{
		// South hline intersection
		yc1 = maxClipY;
		xc1 = x1 + 0.5 + (maxClipY - y1) * (x1 - x1) / (y1 - y1);

		// Test if intersection is valid
		// if so then done, else compute next
		if (xc1 < minClipX || xc1 > maxClipX)
		{
			xc1 = minClipX;
			yc1 = y1 + 0.5 + (minClipX - x1) * (y2 - y1) / (x2, -x1);
		}
	}

	break;

	default:
		break;
	}

	// Do bounds check
	if ((xc1 < minClipX) || (xc1 > maxClipX) ||
		(yc1 < minClipY) || (yc1 > maxClipY) ||
		(xc2 < minClipX) || (xc2 > maxClipX) ||
		(yc2 < minClipY) || (yc2 > maxClipY))
	{
		return false;
	}

	// Store vars back
	x1 = xc1;
	y1 = yc1;
	x2 = xc2;
	y2 = yc2;

	return true;
}

void DrawPolygon2D(Polygon2D* polygon, uint32_t* videoBuffer, int32_t pitch)
{
	// Test if the polygon is visible
	if (polygon->state)
	{
		// Loop through and draw a line from vertices 1 to n
		for (int index = 0; index < polygon->numVertices - 1; index++)
		{
			// Draw line from ith to ith + 1 vertex
			DrawClipLine(polygon->vertices[index].x + polygon->x0,
						 polygon->vertices[index].y + polygon->y0,
						 polygon->vertices[index + 1].x + polygon->x0,
						 polygon->vertices[index + 1].y + polygon->y0, 
						 polygon->color,
						 videoBuffer, pitch, windowRect);
		}

		// Now close up polygon
		// Draw line from last vertex to 0th
		DrawClipLine(polygon->vertices[0].x + polygon->x0,
					 polygon->vertices[0].y + polygon->y0,
					 polygon->vertices[polygon->numVertices - 1].x + polygon->x0,
					 polygon->vertices[polygon->numVertices - 1].y + polygon->y0,
					 polygon->color,
					 videoBuffer, pitch, windowRect);
	}
}

void TranslatePolygon2D(Polygon2D* polygon, int32_t dx, int32_t dy)
{
	if (polygon == nullptr)
	{
		return;
	}

	// Translate
	polygon->x0 = windowRect.left + polygon->startX + dx;
	polygon->y0 = windowRect.top + polygon->startY + dy;
}

void RotatePolygon2D(Polygon2D* polygon, float angle)
{
	if (polygon == nullptr)
	{
		return;
	}

	for (int currentVertex = 0; currentVertex < polygon->numVertices; currentVertex++)
	{
		float& x = polygon->vertices[currentVertex].x;
		float& y = polygon->vertices[currentVertex].y;

		x = x * cosf(angle) - y * sinf(angle);
		y = x * sinf(angle) + y * cosf(angle);
	}
}

void ScalePolygon2D(Polygon2D* polygon, float scaleX, float scaleY)
{
	if (polygon == nullptr)
	{
		return;
	}

	for (int currentVertex = 0; currentVertex < polygon->numVertices; currentVertex++)
	{
		polygon->vertices[currentVertex].x *= scaleX;
		polygon->vertices[currentVertex].y *= scaleY;
	}
}

void InitializeTriangles()
{
	for (int i = 0; i < triangleCount; i++)
	{
		triangles[i].startX = rand() % WINDOW_WIDTH;
		triangles[i].startY = rand() % WINDOW_HEIGHT;

		if (triangles[i].startX == windowRect.left)
		{
			triangles[i].startX += 50;
		}

		if (triangles[i].startX == windowRect.right)
		{
			triangles[i].startX -= 50;
		}

		if (triangles[i].startY == windowRect.top)
		{
			triangles[i].startY += 50;
		}

		if (triangles[i].startY == windowRect.bottom)
		{
			triangles[i].startY -= 50;
		}

		triangles[i].x0 = triangles[i].startX;
		triangles[i].y0 = triangles[i].startY;

		triangles[i].xv = 0; // rand() % 10 + 5;
		triangles[i].yv = 0; // rand() % 10 + 5;
		triangles[i].state = 1;
		triangles[i].numVertices = 3;
		triangles[i].color = Color::Yellow;

		triangles[i].vertices = new Vertex2D[triangles[i].numVertices];
		triangles[i].vertices[0] = { 0, -50 };
		triangles[i].vertices[1] = { -50, 50 };
		triangles[i].vertices[2] = { 50, 50 };
	}
}

void MoveTriangles()
{
	for (int i = 0; i < triangleCount; i++)
	{
		TranslatePolygon2D(&triangles[i], triangles[i].xv, triangles[i].yv);

		if (triangles[i].x0 < windowRect.left || triangles[i].x0 > windowRect.right)
		{
			triangles[i].xv = -triangles[i].xv;
			triangles[i].x0 += triangles[i].xv;
		}

		if (triangles[i].y0 < windowRect.top || triangles[i].y0 > windowRect.bottom)
		{
			triangles[i].yv = -triangles[i].yv;
			triangles[i].y0 += triangles[i].yv;
		}
	}
}

void DrawTriangles(const VideoMemory& videoMemory)
{
	for (int i = 0; i < triangleCount; i++)
	{
		DrawPolygon2D(&triangles[i], videoMemory.videoBuffer, videoMemory.memoryPitch);
	}
}

void Clear(uint32_t color)
{
	DDBLTFX ddbltfx;

	DDRAW_INIT_STRUCT(ddbltfx);

	ddbltfx.dwFillColor = color;

	auto surface = fullScreen ? secondarySurface : offScreenSurface;

	DDFailedCheck(surface->Blt(nullptr, nullptr, nullptr, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx), L"Blt failed");

	//DDFailedCheck(offScreenSurface->Blt(nullptr, nullptr, nullptr, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx), L"Blt failed");
}

bool Present()
{
	if (fullScreen)
	{
		DDFailedCheck(primarySurface->Flip(nullptr, DDFLIP_WAIT), L"Flip failed");
	}

	return true;
}

bool GameInit()
{
    InitDirectDraw();

	srand(timeGetTime());

	UpdateClientRect(mainWindow, &windowRect);

	//for (size_t i = 0; i < 100; i++)
	//{
	//	happyFaces[i].x = rand() % SCREEN_WIDTH;
	//	happyFaces[i].y = rand() % SCREEN_HEIGHT;
	//	happyFaces[i].vx = -5 + rand() % 8;
	//	happyFaces[i].vy = -5 + rand() % 10;
	//}

	sprite = std::shared_ptr<Sprite>(CreateSprite(L"kitten.bmp"));

	//std::shared_ptr<Sprite> player(CreateSprite(L"Estelle.bmp"));

	MyLoadBitmap(L"test.bmp", &bitmap);

	//for (int i = 0; i < 100; i++)
	//{
	//	auto actor = std::make_shared<Actor>();

	//	actor->SetSprite(player);

	//	actor->SetAnimator({ 0, rand() % 8, 128, 128, 8 });

	//	renderables.push_back(actor);

	//	actor->SetPosition(rand() % windowRect.right - windowRect.left, rand() % windowRect.bottom - windowRect.top);
	//}

	triangle = std::make_shared<Polygon2D>();

	triangle->state = 1;
	triangle->numVertices = 3;
	triangle->color = Color::Yellow;
	triangle->startX = windowRect.left + 200;
	triangle->startY = windowRect.top + 200;
	triangle->x0 = triangle->startX;
	triangle->y0 = triangle->startY;
	triangle->vertices = new Vertex2D[triangle->numVertices];
	triangle->vertices[0] = { 0, -50 };
	triangle->vertices[1] = { -50, 50 };
	triangle->vertices[2] = { 50, 50 };

	InitializeTriangles();

	return true;
}

bool GameMain(float deltaTime)
{
	if (windowClosed || windowMinimized)
	{
		return true;
	}

	if (!CheckSurfaces())
	{
		return false;
	}

	UpdateClientRect(mainWindow, &windowRect);

	Clear(Color::Black);

	VideoMemory videoMemory = Lock(offScreenSurface);

	//DrawBitmap(bitmap, videoMemory);

	int x0 = windowRect.left;
	int y0 = windowRect.top;
	int x1 = windowRect.right;
	int y1 = windowRect.bottom;

	RECT clipRect = {windowRect.left + 100, windowRect.top + 100, 
					 windowRect.left + 200, windowRect.top + 200};

	//DrawClipLine(x0, y0, x1, y1, Color::Red, videoMemory.videoBuffer, videoMemory.memoryPitch, windowRect);

	triangle->x0 = windowRect.left + triangle->startX;
	triangle->y0 = windowRect.top + triangle->startY;

	DrawPolygon2D(triangle.get(), videoMemory.videoBuffer, videoMemory.memoryPitch);

	RotatePolygon2D(triangle.get(), deltaTime);

	if (GetAsyncKeyState('A') & 0x8000)
	{
		ScalePolygon2D(triangle.get(), 1.1f, 1.1f);
	}
	
	if (GetAsyncKeyState('S') & 0x8000)
	{
		ScalePolygon2D(triangle.get(), 0.9f, 0.9f);
	}

	//MoveTriangles();

	//DrawTriangles(videoMemory);

	Unlock(offScreenSurface);

	// TODO DDERR_INVALIDRECT
	//DrawSprite(sprite, windowRect.left, windowRect.top);

	//DrawRenderables(renderables);

	auto surface = fullScreen ? secondarySurface : primarySurface;

	DDFailedCheck(surface->Blt(nullptr, offScreenSurface, nullptr, DDBLT_WAIT, nullptr), L"Blt failed");

	//positionX += deltaTime * 10;

	//renderables[0]->SetPosition(positionX, 0);

	//renderables[0]->UpdateAnimator(deltaTime);

	// Happy faces
	{
		//for (int32_t i = 0; i < SCREEN_HEIGHT; i++)
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
	}

	Present();

	return true;
}

void GameShutdown()
{
	SAFE_RELEASE(primarySurface);
	SAFE_RELEASE(directDraw);
}

int32_t APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int32_t       nCmdShow)
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

			GameMain(1.0f / 60.0f);
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
BOOL InitInstance(HINSTANCE hInstance, int32_t nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   DWORD style = WS_OVERLAPPEDWINDOW;

   if (fullScreen)
   {
	   style = WS_POPUP;
   }

   mainWindow = CreateWindowW(szWindowClass, szTitle, style,
      CW_USEDEFAULT, 0, 1280, 720, nullptr, nullptr, hInstance, nullptr);

   if (!mainWindow)
   {
      return FALSE;
   }

   ShowWindow(mainWindow, nCmdShow);
   UpdateWindow(mainWindow);

   //AdjustWindowRectEx(&windowRect, GetWindowStyle(mainWindow),
	  // GetMenu(mainWindow) != nullptr,
	  // GetWindowExStyle(mainWindow));

   //MoveWindow(mainWindow, CW_USEDEFAULT, CW_USEDEFAULT,
	  // windowRect.right - windowRect.left,
	  // windowRect.bottom - windowRect.top, FALSE);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAint32_t    - Paint32_t the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, uint32_t message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
		int32_t wmId = LOWORD(wParam);
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
			windowMinimized = false;
		}
		else
		{
			// Application i s being deactivated
			windowMinimized = true;
		}

		break;
	case WM_CLOSE:
	{
		int32_t result = MessageBox(hWnd, L"Are you sure?", L"Notice", MB_YESNO);

		if (result == IDYES)
		{
			windowClosed = true;
			DestroyWindow(hWnd);
		}
	}

	break;
	case WM_SIZE:
	{
		int32_t width = LOWORD(lParam);
		int32_t height = HIWORD(lParam);

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
		int32_t positionX = (int)LOWORD(lParam);
		int32_t positionY = (int)HIWORD(lParam);
	}

	break;

	case WM_CHAR:
		break;

	case WM_KEYDOWN:
	{
		int32_t keyCode = (int)wParam;
	}

	break;

	case WM_MOUSEMOVE:
	{
		int32_t mouseX = (int)LOWORD(lParam);
		int32_t mouseY = (int)HIWORD(lParam);

		int32_t buttons = (int)wParam;

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
		int32_t mouseX = (int)LOWORD(lParam);
		int32_t mouseY = (int)HIWORD(lParam);
	}

	case WM_LBUTTONDOWN:
	{
		int32_t mouseX = (int)LOWORD(lParam);
		int32_t mouseY = (int)HIWORD(lParam);
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
INT_PTR CALLBACK About(HWND hDlg, uint32_t message, WPARAM wParam, LPARAM lParam)
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
