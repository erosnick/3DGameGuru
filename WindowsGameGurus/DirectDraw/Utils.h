#pragma once

class Rect : public RECT
{
public:

	Rect() {}

	Rect operator+(const Rect& rect)
	{
		Rect result;

		result.left = left + rect.left;
		result.top = top + rect.top;
		result.right = right + rect.right;
		result.bottom = bottom + rect.bottom;

		return result;
	}
};

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

#define RGB32(a, r, g, b) ((a & 255) << 24) + ((r & 255) << 16) + ((g & 255) << 8) + (b & 255)

#define DDRAW_INIT_STRUCT(ddstruct) ZeroMemory(&ddstruct, sizeof(ddstruct)); ddstruct.dwSize = sizeof(ddstruct)

namespace Color
{
	const uint32_t White = RGB32(255, 255, 255, 255);
	const uint32_t Black = RGB32(255, 0, 0, 0);
	const uint32_t Red = RGB32(255, 255, 0, 0);
	const uint32_t Green = RGB32(255, 0, 255, 0);
	const uint32_t Blue = RGB32(255, 0, 0, 255);
	const uint32_t Yellow = RGB32(255, 255, 255, 0);
	const uint32_t CornFlower = RGB32(255, 100, 149, 237);
}

#define  RANDOM_COLOR() RGB(rand() % 256, rand() % 256, rand() % 256)

#define SAFE_RELEASE(pointer) pointer->Release(); pointer = nullptr

bool DDFailedCheck(HRESULT hr, const TCHAR* szMessage)
{
	if (FAILED(hr))
	{
		TCHAR buffer[1024];
		wsprintf(buffer, L"%s (%s)", szMessage, DDErrorString(hr));
		MessageBox(nullptr, buffer, L"Error", MB_OK);
		OutputDebugString(buffer);
		return true;
	}

	return false;
}

void UpdateClientRect(HWND hWnd, RECT* windowRect)
{
	GetClientRect(hWnd, windowRect); // get client coords
	ClientToScreen(hWnd, reinterpret_cast<POINT*>(&windowRect->left)); // convert top-left
	ClientToScreen(hWnd, reinterpret_cast<POINT*>(&windowRect->right)); // convert bottom-right
}

