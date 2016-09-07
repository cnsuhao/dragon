// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#pragma warning(disable:4996)
// warning C4201: 使用了非标准扩展 : 无名称的结构/联合
#pragma warning(disable:4201)
// warning C4100: “lpReserved”: 未引用的形参
#pragma warning(disable:4100)
// warning C4481: 使用了非标准扩展: 请重写说明符“override”
#pragma warning(disable:4481)

// #define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <tchar.h>
#include <vector>
using namespace std;


#include <D3D10_1.h> 
#include <d3dx10.h>
#include "UISDK/Kernel/Inc/Base/leakdetect.h"
#include "UISDK/Kernel/Inc/Base/commondefine.h"

#ifdef _DEBUG
    #pragma comment(lib, "d3dx10d.lib") 
#else
    #pragma comment(lib, "d3dx10.lib")
#endif
#pragma comment(lib, "D3D10_1.lib")

namespace UI
{
    class D3D10App;
    extern D3D10App*  g_pD3D10App;
}
extern HINSTANCE g_hInstance;