#include <ddraw.h>
#include <fstream>

using namespace std;

class DirectDrawRender
{
public:
	DirectDrawRender();
	~DirectDrawRender();
	bool  Initialize(HWND hWnd, int width, int height, int pixelBit);
	void  SetPixel8(int x, int y, UCHAR color);
	void  SetPixel16(int x, int y, int r, int g, int b);
	void  SetPixel32(int x, int y, int a, int r, int g, int b);
	void  Rectangle(RECT& rect,  DWORD r, DWORD g, DWORD b, DWORD a = 0);
	bool  SetUpPalette();
	void  BeginRender();
	void  EndRender();
	DWORD GetPixelBit();

private:
	int mWidth;
	int mHeight;
	int mPixelBit;

	HWND mHWND;
	LPDIRECTDRAW7 mDirectDraw;
	LPDIRECTDRAWSURFACE7 mPrimarySurface;
	LPDIRECTDRAWSURFACE7 mBackSurface;
	PALETTEENTRY mPaletteEntries[256];
	LPDIRECTDRAWPALETTE	mPalette;
	DDSURFACEDESC2 mSurfaceDesc;

	ofstream mLog;

	typedef void (*SetPixelFun)(int width, int height, UCHAR color);
	SetPixelFun SetPixel;
};

template<typename T>
void SafeRelase(T type)
{
	if (NULL != type)
	{
		type->Release();
		type = NULL;
	}
}

#define _RGB16BIT565(r, g, b) (b) + ((g) << 5) + ((r) << 11)
#define  RGB32BIT(a, r, g, b) ((b) + (g << 8) + (r << 16) + (a << 24))

inline USHORT RGB16BIT565(int r, int g, int b)
{
	r >>= 3;
	g >>= 2;
	b >>= 3;

	return _RGB16BIT565(r, g, b);
}