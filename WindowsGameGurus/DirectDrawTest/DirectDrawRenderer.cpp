#include "stdafx.h"
#include "DirectDrawRenderer.h"

DirectDrawRender::DirectDrawRender()
	: mDirectDraw(NULL),
	  mPrimarySurface(NULL),
	  mBackSurface(NULL),
	  mPalette(NULL)
{
	mLog.open(L"log.txt");
}

DirectDrawRender::~DirectDrawRender()
{
	SafeRelase(mBackSurface);
	SafeRelase(mPrimarySurface);
	SafeRelase(mDirectDraw);
	mLog.close();
}

bool DirectDrawRender::Initialize(HWND hWnd, int width, int height, int pixelBit)
{	
	if (FAILED(DirectDrawCreateEx(NULL, (LPVOID*)&mDirectDraw, IID_IDirectDraw7, NULL)))
	{
		MessageBox(hWnd, L"Create DirectDraw interface failed!", L"Error", MB_OK);
		return DD_FALSE;
	}

	mLog << "Create DirectDraw interface successed." << endl;
	
	mDirectDraw->SetCooperativeLevel(hWnd, DDSCL_FULLSCREEN | DDSCL_EXCLUSIVE | DDSCL_ALLOWREBOOT | DDSCL_ALLOWMODEX);
	
	//ULONG pixBit = GetPixelBit();

	//mLog << pixBit << endl;
	
	mDirectDraw->SetDisplayMode(800, 600, pixelBit, 0, 0);

	ZeroMemory(&mSurfaceDesc, sizeof (DDSURFACEDESC2));
	mSurfaceDesc.dwSize = sizeof (DDSURFACEDESC2);
	mSurfaceDesc.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
	mSurfaceDesc.dwBackBufferCount = 2;
	mSurfaceDesc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_COMPLEX | DDSCAPS_FLIP;

	if (FAILED(mDirectDraw->CreateSurface(&mSurfaceDesc, &mPrimarySurface, NULL)))
	{
		MessageBox(hWnd, L"Create primary surface failed!", L"Error", MB_OK);
		return DD_FALSE;
	}

	mLog << "Create primary surface successed." << endl;

	mSurfaceDesc.ddsCaps.dwCaps = DDSCAPS_BACKBUFFER;

	if (FAILED(mPrimarySurface->GetAttachedSurface(&mSurfaceDesc.ddsCaps, &mBackSurface)))
	{
		MessageBox(hWnd, L"Get back surface failed!", L"Error", MB_OK);
	}

	mLog << "Get back surface successed." << endl;

	return true;
}

void DirectDrawRender::BeginRender()
{
	ZeroMemory(&mSurfaceDesc, sizeof (DDSURFACEDESC2));
	mSurfaceDesc.dwSize = sizeof (DDSURFACEDESC2);

	mBackSurface->Lock(NULL, &mSurfaceDesc, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL);
}

void DirectDrawRender::EndRender()
{
	mBackSurface->Unlock(NULL);

	mPrimarySurface->Flip(NULL, DDFLIP_WAIT);

	Sleep(500);
}

// 8位模式，每个像素1字节，因为用UCHAR转换lpSurface
// memPitch默认为8位(1字节)步进
void DirectDrawRender::SetPixel8(int x, int y, UCHAR color)
{
	UCHAR* buffer = (UCHAR*)mSurfaceDesc.lpSurface;
	int memPitch = (int)mSurfaceDesc.lPitch;
	buffer[x + y * memPitch] = color;
}

// 16位模式，每个像素2字节，因此用USHORT转换lpSurface
// memPitch默认为8位(1字节)步进，因此需要除以2(memPitch >> 1)来正确定位像素
void DirectDrawRender::SetPixel16(int x, int y, int r, int g, int b)
{
	USHORT* buffer = (USHORT*)mSurfaceDesc.lpSurface;
	int memPitch = (int)mSurfaceDesc.lPitch;

	USHORT color = RGB16BIT565(r, g, b);

	buffer[x + y * (memPitch >> 1)] = color;
}

// 32位模式，每个像素4字节，因此用DWORD转换lpSurface
// memPitch默认为8位(1字节)步进，因此需要除以4(memPitch >> 2)来正确定位像素
void DirectDrawRender::SetPixel32(int x, int y, int a, int r, int g, int b)
{
	DWORD* buffer = (DWORD*)mSurfaceDesc.lpSurface;
	int memPitch = (int)mSurfaceDesc.lPitch;

	DWORD color = RGB32BIT(a, r, g, b);

	buffer[x + y * (memPitch >> 2)] = color;
}

void DirectDrawRender::Rectangle(RECT& rect, DWORD r, DWORD g, DWORD b, DWORD a)
{
	DDBLTFX ddbltfx;
	ZeroMemory(&ddbltfx, sizeof (DDBLTFX));

	ddbltfx.dwSize = sizeof (DDBLTFX);

	ddbltfx.dwFillColor = RGB32BIT(a, r, g, b);

	HRESULT ret = mBackSurface->Blt(&rect, NULL, NULL, DDBLT_COLORFILL, &ddbltfx);

	switch (ret)
	{
	case DDERR_GENERIC:
		mLog << "DDERR_GENERIC" << endl;
		break;

	case DDERR_INVALIDCLIPLIST:
		mLog << "DDERR_INVALIDCLIPLIST" << endl;
		break;

	case DDERR_INVALIDOBJECT:
		mLog << "DDERR_INVALIDOBJECT" << endl;
		break;

	case DDERR_INVALIDPARAMS:
		mLog << "DDERR_INVALIDPARAMS" << endl;
		break;

	case DDERR_INVALIDRECT:
		mLog << "DDERR_INVALIDRECT" << endl;
		break;

	case DDERR_NOALPHAHW:
		mLog << "DDERR_NOALPHAHW" << endl;
		break;

	case DDERR_NOBLTHW:
		mLog << "DDERR_NOBLTHW" << endl;
		break;

	case DDERR_NOCLIPLIST:
		mLog << "DDERR_NOCLIPLIST" << endl;
		break;

	case DDERR_NORASTEROPHW:
		mLog << "DDERR_NORASTEROPHW" << endl;
		break;

	case DDERR_NOSTRETCHHW:
		mLog << "DDERR_NOSTRETCHHW" << endl;
		break;

	case DDERR_SURFACEBUSY:
		mLog << "DDERR_SURFACEBUSY" << endl;
		break;

	case DDERR_SURFACELOST:
		mLog << "DDERR_SURFACELOST" << endl;
		break;

	case DDERR_UNSUPPORTED:
		mLog << "DDERR_UNSUPPORTED" << endl;
		break;

	case DDERR_WASSTILLDRAWING:
		mLog << "DDERR_WASSTILLDRAWING" << endl;
		break;
	}

	//mLog << "Color=" << ddbltfx.dwFillColor << endl;

	mLog << "(" << rect.left << ", " << rect.top << ") (" << rect.right << ", " << rect.bottom << ")" << endl;

	//mLog << "Filling rectangle..." << endl;
}

DWORD DirectDrawRender::GetPixelBit()
{
	DDPIXELFORMAT pixel;

	ZeroMemory(&pixel, sizeof (DDPIXELFORMAT));

	pixel.dwSize = sizeof (DDPIXELFORMAT);

	mPrimarySurface->GetPixelFormat(&pixel);

	mLog << pixel.dwRGBBitCount << endl;

	if (pixel.dwFlags & DDPF_RGB)
	{
		switch (pixel.dwRGBBitCount)
		{
		case  15:
			mLog << "15bit" << endl;
			break;

		case 16:
			mLog << "16bit" << endl;
			break;

		case 24:
			mLog << "24bit" << endl;
			break;

		case 32:
			mLog << "32bit" << endl;
			break;
		}
	}

	return pixel.dwRGBBitCount;
}

bool DirectDrawRender::SetUpPalette()
{
	for (int color = 1; color < 255; color++)
	{
		mPaletteEntries[color].peRed = rand() % 256;
		mPaletteEntries[color].peGreen = rand() % 256;
		mPaletteEntries[color].peBlue = rand() % 256;

		mPaletteEntries[color].peFlags = PC_NOCOLLAPSE;
	}

	mPaletteEntries[0].peRed = 0;
	mPaletteEntries[0].peGreen = 0;
	mPaletteEntries[0].peBlue = 0;
	mPaletteEntries[0].peFlags = PC_NOCOLLAPSE;

	mPaletteEntries[255].peRed = 255;
	mPaletteEntries[255].peGreen = 255;
	mPaletteEntries[255].peBlue = 255;
	mPaletteEntries[255].peFlags = PC_NOCOLLAPSE;

	if (FAILED(mDirectDraw->CreatePalette(DDPCAPS_8BIT | DDPCAPS_ALLOW256 | DDPCAPS_INITIALIZE,
		mPaletteEntries, &mPalette, NULL)))
	{
		MessageBox(NULL, L"Create palette failed!", L"Error", MB_OK);
		return false;
	}

	mLog << "Create palette successed." << endl;

	mPrimarySurface->SetPalette(mPalette);

	mLog << "Initialize() successed." << endl;

	return true;
}