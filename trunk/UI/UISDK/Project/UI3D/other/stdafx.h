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

#include <d3d9.h>
#include <d3dx9math.h>
#include <d3dx9tex.h>
#include <d3dx9mesh.h>
#include <d3dx9core.h>

#pragma comment(lib, "Msimg32.lib")   // AlphaBlend

#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "d3d9.lib")

#include "UISDK\Project\UIUtil\h\Util.h"
#pragma comment(lib, "UIUtil.lib")

#include "UISDK\Kernel\Inc\Base\inc.h"
#pragma comment(lib, "UISDK.lib")

#include "UISDK\Project\UI3D\inc\inc.h"
#include "UISDK\Kernel\Inc\Base\leakdetect.h"
using namespace UI;

#include "UISDK\Project\UI3D\src\D3D\d3dapp.h"
extern D3DApp*  g_pD3DApp;
IDirect3DDevice9*  GetDevice();

extern UI::IUIApplication*  g_pUIApplication;