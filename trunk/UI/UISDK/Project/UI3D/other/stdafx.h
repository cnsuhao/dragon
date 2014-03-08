// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <atlbase.h>
#include <algorithm>

// D2D
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "windowscodecs.lib")

// D3D
#include <D3D10.h>
#include <d3dx10.h>
#include <D3D10_1.h> 

#pragma comment(lib, "D3D10_1.lib")
#pragma comment(lib, "d3d10.lib")
#ifdef _DEBUG
#pragma comment(lib, "d3dx10d.lib")
#else
#pragma comment(lib, "d3dx10.lib")
#endif

#pragma comment(lib, "Msimg32.lib")   // AlphaBlend

#include "UISDK\Project\UIUtil\h\Util.h"
#pragma comment(lib, "UIUtil.lib")

#include "UISDK\Kernel\Inc\Base\inc.h"
#pragma comment(lib, "UISDK.lib")

#include "UISDK\Project\UI3D\inc\inc.h"
#include "UISDK\Kernel\Inc\Base\leakdetect.h"
using namespace UI;

#include "UISDK\Project\UI3D\inc\inc.h"
#include "UISDK\Project\UI3D\src\D3D\d3dapp.h"
#include "UISDK\Project\UI3D\src\D2D\d2dapp.h"
ID3D10Device*  GetDevice();

extern D3D10App*  g_pD3DApp;
extern D2DApp*    g_pD2DApp;
extern UI::IUIApplication*  g_pUIApplication;
extern HMODULE  g_hInstance;