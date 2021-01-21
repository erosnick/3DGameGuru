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

const WCHAR* DDErrorString(HRESULT hr)
{
	switch (hr)
	{
		case DDERR_ALREADYINITIALIZED:           return L"DDERR_ALREADYINITIALIZED";
		case DDERR_CANNOTATTACHSURFACE:          return L"DDERR_CANNOTATTACHSURFACE";
		case DDERR_CANNOTDETACHSURFACE:          return L"DDERR_CANNOTDETACHSURFACE";
		case DDERR_CURRENTLYNOTAVAIL:            return L"DDERR_CURRENTLYNOTAVAIL";
		case DDERR_EXCEPTION:                    return L"DDERR_EXCEPTION";
		case DDERR_GENERIC:                      return L"DDERR_GENERIC";
		case DDERR_HEIGHTALIGN:                  return L"DDERR_HEIGHTALIGN";
		case DDERR_INCOMPATIBLEPRIMARY:          return L"DDERR_INCOMPATIBLEPRIMARY";
		case DDERR_INVALIDCAPS:                  return L"DDERR_INVALIDCAPS";
		case DDERR_INVALIDCLIPLIST:              return L"DDERR_INVALIDCLIPLIST";
		case DDERR_INVALIDMODE:                  return L"DDERR_INVALIDMODE";
		case DDERR_INVALIDOBJECT:                return L"DDERR_INVALIDOBJECT";
		case DDERR_INVALIDPARAMS:                return L"DDERR_INVALIDPARAMS";
		case DDERR_INVALIDPIXELFORMAT:           return L"DDERR_INVALIDPIXELFORMAT";
		case DDERR_INVALIDRECT:                  return L"DDERR_INVALIDRECT";
		case DDERR_LOCKEDSURFACES:               return L"DDERR_LOCKEDSURFACES";
		case DDERR_NO3D:                         return L"DDERR_NO3D";
		case DDERR_NOALPHAHW:                    return L"DDERR_NOALPHAHW";
		case DDERR_NOCLIPLIST:                   return L"DDERR_NOCLIPLIST";
		case DDERR_NOCOLORCONVHW:                return L"DDERR_NOCOLORCONVHW";
		case DDERR_NOCOOPERATIVELEVELSET:        return L"DDERR_NOCOOPERATIVELEVELSET";
		case DDERR_NOCOLORKEY:                   return L"DDERR_NOCOLORKEY";
		case DDERR_NOCOLORKEYHW:                 return L"DDERR_NOCOLORKEYHW";
		case DDERR_NODIRECTDRAWSUPPORT:          return L"DDERR_NODIRECTDRAWSUPPORT";
		case DDERR_NOEXCLUSIVEMODE:              return L"DDERR_NOEXCLUSIVEMODE";
		case DDERR_NOFLIPHW:                     return L"DDERR_NOFLIPHW";
		case DDERR_NOGDI:                        return L"DDERR_NOGDI";
		case DDERR_NOMIRRORHW:                   return L"DDERR_NOMIRRORHW";
		case DDERR_NOTFOUND:                     return L"DDERR_NOTFOUND";
		case DDERR_NOOVERLAYHW:                  return L"DDERR_NOOVERLAYHW";
		case DDERR_NORASTEROPHW:                 return L"DDERR_NORASTEROPHW";
		case DDERR_NOROTATIONHW:                 return L"DDERR_NOROTATIONHW";
		case DDERR_NOSTRETCHHW:                  return L"DDERR_NOSTRETCHHW";
		case DDERR_NOT4BITCOLOR:                 return L"DDERR_NOT4BITCOLOR";
		case DDERR_NOT4BITCOLORINDEX:            return L"DDERR_NOT4BITCOLORINDEX";
		case DDERR_NOT8BITCOLOR:                 return L"DDERR_NOT8BITCOLOR";
		case DDERR_NOTEXTUREHW:                  return L"DDERR_NOTEXTUREHW";
		case DDERR_NOVSYNCHW:                    return L"DDERR_NOVSYNCHW";
		case DDERR_NOZBUFFERHW:                  return L"DDERR_NOZBUFFERHW";
		case DDERR_NOZOVERLAYHW:                 return L"DDERR_NOZOVERLAYHW";
		case DDERR_OUTOFCAPS:                    return L"DDERR_OUTOFCAPS";
		case DDERR_OUTOFMEMORY:                  return L"DDERR_OUTOFMEMORY";
		case DDERR_OUTOFVIDEOMEMORY:             return L"DDERR_OUTOFVIDEOMEMORY";
		case DDERR_OVERLAYCANTCLIP:              return L"DDERR_OVERLAYCANTCLIP";
		case DDERR_OVERLAYCOLORKEYONLYONEACTIVE: return L"DDERR_OVERLAYCOLORKEYONLYONEACTIVE";
		case DDERR_PALETTEBUSY:                  return L"DDERR_PALETTEBUSY";
		case DDERR_COLORKEYNOTSET:               return L"DDERR_COLORKEYNOTSET";
		case DDERR_SURFACEALREADYATTACHED:       return L"DDERR_SURFACEALREADYATTACHED";
		case DDERR_SURFACEALREADYDEPENDENT:      return L"DDERR_SURFACEALREADYDEPENDENT";
		case DDERR_SURFACEBUSY:                  return L"DDERR_SURFACEBUSY";
		case DDERR_CANTLOCKSURFACE:              return L"DDERR_CANTLOCKSURFACE";
		case DDERR_SURFACEISOBSCURED:            return L"DDERR_SURFACEISOBSCURED";
		case DDERR_SURFACELOST:                  return L"DDERR_SURFACELOST";
		case DDERR_SURFACENOTATTACHED:           return L"DDERR_SURFACENOTATTACHED";
		case DDERR_TOOBIGHEIGHT:                 return L"DDERR_TOOBIGHEIGHT";
		case DDERR_TOOBIGSIZE:                   return L"DDERR_TOOBIGSIZE";
		case DDERR_TOOBIGWIDTH:                  return L"DDERR_TOOBIGWIDTH";
		case DDERR_UNSUPPORTED:                  return L"DDERR_UNSUPPORTED";
		case DDERR_UNSUPPORTEDFORMAT:            return L"DDERR_UNSUPPORTEDFORMAT";
		case DDERR_UNSUPPORTEDMASK:              return L"DDERR_UNSUPPORTEDMASK";
		case DDERR_VERTICALBLANKINPROGRESS:      return L"DDERR_VERTICALBLANKINPROGRESS";
		case DDERR_WASSTILLDRAWING:              return L"DDERR_WASSTILLDRAWING";
		case DDERR_XALIGN:                       return L"DDERR_XALIGN";
		case DDERR_INVALIDDIRECTDRAWGUID:        return L"DDERR_INVALIDDIRECTDRAWGUID";
		case DDERR_DIRECTDRAWALREADYCREATED:     return L"DDERR_DIRECTDRAWALREADYCREATED";
		case DDERR_NODIRECTDRAWHW:               return L"DDERR_NODIRECTDRAWHW";
		case DDERR_PRIMARYSURFACEALREADYEXISTS:  return L"DDERR_PRIMARYSURFACEALREADYEXISTS";
		case DDERR_NOEMULATION:                  return L"DDERR_NOEMULATION";
		case DDERR_REGIONTOOSMALL:               return L"DDERR_REGIONTOOSMALL";
		case DDERR_CLIPPERISUSINGHWND:           return L"DDERR_CLIPPERISUSINGHWND";
		case DDERR_NOCLIPPERATTACHED:            return L"DDERR_NOCLIPPERATTACHED";
		case DDERR_NOHWND:                       return L"DDERR_NOHWND";
		case DDERR_HWNDSUBCLASSED:               return L"DDERR_HWNDSUBCLASSED";
		case DDERR_HWNDALREADYSET:               return L"DDERR_HWNDALREADYSET";
		case DDERR_NOPALETTEATTACHED:            return L"DDERR_NOPALETTEATTACHED";
		case DDERR_NOPALETTEHW:                  return L"DDERR_NOPALETTEHW";
		case DDERR_BLTFASTCANTCLIP:              return L"DDERR_BLTFASTCANTCLIP";
		case DDERR_NOBLTHW:                      return L"DDERR_NOBLTHW";
		case DDERR_NODDROPSHW:                   return L"DDERR_NODDROPSHW";
		case DDERR_OVERLAYNOTVISIBLE:            return L"DDERR_OVERLAYNOTVISIBLE";
		case DDERR_NOOVERLAYDEST:                return L"DDERR_NOOVERLAYDEST";
		case DDERR_INVALIDPOSITION:              return L"DDERR_INVALIDPOSITION";
		case DDERR_NOTAOVERLAYSURFACE:           return L"DDERR_NOTAOVERLAYSURFACE";
		case DDERR_EXCLUSIVEMODEALREADYSET:      return L"DDERR_EXCLUSIVEMODEALREADYSET";
		case DDERR_NOTFLIPPABLE:                 return L"DDERR_NOTFLIPPABLE";
		case DDERR_CANTDUPLICATE:                return L"DDERR_CANTDUPLICATE";
		case DDERR_NOTLOCKED:                    return L"DDERR_NOTLOCKED";
		case DDERR_CANTCREATEDC:                 return L"DDERR_CANTCREATEDC";
		case DDERR_NODC:                         return L"DDERR_NODC";
		case DDERR_WRONGMODE:                    return L"DDERR_WRONGMODE";
		case DDERR_IMPLICITLYCREATED:            return L"DDERR_IMPLICITLYCREATED";
		case DDERR_NOTPALETTIZED:                return L"DDERR_NOTPALETTIZED";
		case DDERR_UNSUPPORTEDMODE:              return L"DDERR_UNSUPPORTEDMODE";
		case DDERR_NOMIPMAPHW:                   return L"DDERR_NOMIPMAPHW";
		case DDERR_INVALIDSURFACETYPE:           return L"DDERR_INVALIDSURFACETYPE";
		case DDERR_DCALREADYCREATED:             return L"DDERR_DCALREADYCREATED";
		case DDERR_CANTPAGELOCK:                 return L"DDERR_CANTPAGELOCK";
		case DDERR_CANTPAGEUNLOCK:               return L"DDERR_CANTPAGEUNLOCK";
		case DDERR_NOTPAGELOCKED:                return L"DDERR_NOTPAGELOCKED";
		case DDERR_NOTINITIALIZED:               return L"DDERR_NOTINITIALIZED";
	}
	return L"Unknown Error";
}

#define MAX_LOADSTRING 100

const int32_t SCREEN_WIDTH = 1920;
const int32_t SCREEN_HEIGHT = 1080;
const int32_t SCREEN_BPP = 32;

int32_t tileSize = 128;

int32_t tileX = 0;

float animTime = 0.0f;

int32_t animFPS = 15;

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
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

LPDIRECTDRAW7 directDraw = nullptr;
LPDIRECTDRAWSURFACE7 primarySurface = nullptr;
LPDIRECTDRAWSURFACE7 secondarySurface = nullptr;
LPDIRECTDRAWCLIPPER clipper = nullptr;
LPDIRECTDRAWSURFACE7 offScreenSurface = nullptr;

#define RGB32(a, r, g, b) ((a & 255) << 24) + ((r & 255) << 16) + ((g & 255) << 8) + (b & 255)

#define DDRAW_INIT_STRUCT(ddstruct) ZeroMemory(&ddstruct, sizeof(ddstruct)); ddstruct.dwSize = sizeof(ddstruct)

#define  RANDOM_COLOR() RGB(rand() % 256, rand() % 256, rand() % 256)

#define SAFE_RELEASE(pointer) pointer->Release(); pointer = nullptr

bool DDFailedCheck(HRESULT hr, const TCHAR* szMessage)
{
	if (FAILED(hr))
	{
		TCHAR buffer[1024];
		wsprintf(buffer, L"%s (%s)n", szMessage, DDErrorString(hr));
		MessageBox(nullptr, buffer, L"Error", MB_OK);
		OutputDebugString(buffer);
		return true;
	}

	return false;
}

LPDIRECTDRAWSURFACE7 CreateSurface(int32_t width, int32_t height, int32_t memoryFlags = DDSCAPS_VIDEOMEMORY)
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

EXTERN_C ULONG64 myAdd(ULONG64 u1, ULONG64 u2);
EXTERN_C ULONG64 mySub(ULONG64 u1, ULONG64 u2);

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
		width = bitmap.width;
		height = bitmap.height;
		bpp = bitmap.bpp;
		imageSize = bitmap.imageSize;
		bytesPerLine = bitmap.bytesPerLine;

		imageData = new UCHAR[imageSize];

		memcpy_s(imageData, imageSize, bitmap.imageData, bitmap.imageSize);
	}

	~Bitmap() { delete[] imageData; }

	int32_t width;
	int32_t height;
	int32_t bpp;
	int32_t imageSize;
	int32_t bytesPerLine;
	UCHAR* imageData;
};

struct Sprite
{
	Sprite() 
	: x(0),
	  y(0),
	  surface(nullptr)
	{}

	Sprite(const Sprite& sprite)
	{
		x = sprite.x;
		y = sprite.y;
		bitmap = sprite.bitmap;

		surface = CreateSurface(bitmap.width, bitmap.height);
	}
	
	~Sprite() { SAFE_RELEASE(surface); }

	int32_t x;
	int32_t y;

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

std::vector<std::shared_ptr<Actor>> renderables;

std::shared_ptr<Sprite> sprite;

struct VideoMemory
{
	int32_t memoryPitch = 0;
	uint32_t* videoBuffer = nullptr;
};

Bitmap atlas;

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
				videoBuffer[indexX] = RGB32(255, 255, 255, 0);
			}
		}

		videoBuffer += memoryPitch;
		bitmapBuffer += bitmapWidth;
	}
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

	if (DDFailedCheck(directDraw->SetCooperativeLevel(mainWindow, DDSCL_FULLSCREEN | DDSCL_EXCLUSIVE | DDSCL_ALLOWREBOOT), L"SetCooperativeLevel failed"))
	{
		return false;
	}

	if (DDFailedCheck(directDraw->SetDisplayMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, 0, 0), L"SetDisplayMode failed"))
	{
		return false;
	}

	DDSURFACEDESC2 surfaceDesc;

	ZeroMemory(&surfaceDesc, sizeof(DDSURFACEDESC2));

	surfaceDesc.dwSize = sizeof(DDSURFACEDESC2);
	surfaceDesc.dwBackBufferCount = 1;
	surfaceDesc.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
	surfaceDesc.dwBackBufferCount = 1;
	surfaceDesc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_COMPLEX | DDSCAPS_FLIP;

	if (DDFailedCheck(directDraw->CreateSurface(&surfaceDesc, &primarySurface, nullptr), L"CreateSurface failed"))
	{
		return false;
	}

	surfaceDesc.ddsCaps.dwCaps = DDSCAPS_BACKBUFFER;

	if (DDFailedCheck(primarySurface->GetAttachedSurface(&surfaceDesc.ddsCaps, &secondarySurface), L"GetAttachedSurface failed"))
	{
		return false;
	}

	if (DDFailedCheck(directDraw->CreateClipper(0, &clipper, nullptr), L"CreateClipper failed"))
	{
		return false;
	}

	RECT clipList[] = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};

	clipper = AttachClipper(secondarySurface, 1, clipList);

	offScreenSurface = CreateSurface(1024, 1024);

    return true;
}

VideoMemory Lock(LPDIRECTDRAWSURFACE7 surface)
{
	DDSURFACEDESC2 surfaceDesc;

	ZeroMemory(&surfaceDesc, sizeof(DDSURFACEDESC2));

	surfaceDesc.dwSize = sizeof(DDSURFACEDESC2);

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

void PlotPixel(int32_t x, int32_t y, uint32_t color, const VideoMemory& videoMemory)
{
	videoMemory.videoBuffer[y * videoMemory.memoryPitch + x] = color;
}

void DrawBitmap(const Bitmap& bitmap, const VideoMemory& videoMemory)
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

			PlotPixel(x, y, color, videoMemory);
		}
	}
}

Sprite* CreateSprite(const TCHAR* fileName)
{
	Sprite* sprite = new Sprite();

	MyLoadBitmap(fileName, &sprite->bitmap);

	FlipBitmap(sprite->bitmap.imageData, sprite->bitmap.bytesPerLine, sprite->bitmap.height);

	sprite->surface = CreateSurface(sprite->bitmap.width, sprite->bitmap.height);

	VideoMemory videoMemory = Lock(sprite->surface);

	DrawBitmap(sprite->bitmap, videoMemory);

	Unlock(sprite->surface);

	return sprite;
}

void DrawSprite(const std::shared_ptr<Sprite>& sprite, int x, int y)
{
	RECT destRect = { x, y, destRect.left + sprite->bitmap.width, destRect.top + sprite->bitmap.height };

	RECT sourceRect = { 0, 0, sourceRect.left + sprite->bitmap.width, sourceRect.top + sprite->bitmap.height };

	DDFailedCheck(secondarySurface->Blt(&destRect, sprite->surface, &sourceRect, DDBLT_WAIT | DDBLT_KEYSRC, nullptr), L"Blt failed");
}

void DrawSprite(const std::shared_ptr<Sprite>& spriteSheet, int32_t x, int32_t y, int32_t tileX, int32_t tileY, RECT destRect, RECT sourceRect)
{
	DDFailedCheck(secondarySurface->Blt(&destRect, spriteSheet->surface, &sourceRect, DDBLT_WAIT | DDBLT_KEYSRC, nullptr), L"Blt failed");
}

void Clear(uint32_t color)
{
	DDBLTFX ddbltfx;

	DDRAW_INIT_STRUCT(ddbltfx);

	ddbltfx.dwFillColor = color;
	
	DDFailedCheck(secondarySurface->Blt(nullptr, nullptr, nullptr, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx), L"Blt failed");
}

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

	sprite = std::shared_ptr<Sprite>(CreateSprite(L"kitten.bmp"));

	std::shared_ptr<Sprite> player(CreateSprite(L"Estelle.bmp"));

	for (int i = 0; i < 100; i++)
	{
		auto actor = std::make_shared<Actor>();

		actor->SetSprite(player);

		actor->SetAnimator({ 0, rand() % 8, 128, 128, 8 });

		renderables.push_back(actor);

		actor->SetPosition(rand() % SCREEN_WIDTH, rand() % SCREEN_HEIGHT);
	}

	return true;
}

void DrawRenderables(const std::vector<std::shared_ptr<Actor>>& renderables)
{
	for (const auto renderable : renderables)
	{
		Animator animator = renderable->GetAnimator();

		RECT destRect = { renderable->x, renderable->y, destRect.left + 
						  animator.tileSizeX, destRect.top + animator.tileSizeY };

		RECT sourceRect = { animator.tileX * animator.tileSizeX, animator.tileY * animator.tileSizeY, 
							sourceRect.left + animator.tileSizeX, sourceRect.top + animator.tileSizeY };

		DrawSprite(renderable->GetSprite(), renderable->x, renderable->y, animator.tileX, animator.tileY, destRect, sourceRect);
	}
}

bool Present()
{
	DDFailedCheck(primarySurface->Flip(nullptr, DDFLIP_WAIT), L"Flip failed");

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

	Clear(RGB32(255, 100, 149, 237));

	DrawSprite(sprite, 100, 100);

	DrawRenderables(renderables);

	positionX += deltaTime * 10;

	renderables[0]->SetPosition(positionX, 0);

	renderables[0]->UpdateAnimator(deltaTime);

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
