#include "stdafx.h"

#include "UISDK\Kernel\Inc\Interface\iwindow.h"
#include "UISDK\Kernel\Src\Renderlibrary\gdi\gdibitmap.h"
#include "UISDK\Kernel\Src\Renderlibrary\gdi\gdirender.h"
#include "UISDK\Kernel\Src\Renderlibrary\gdiplus\gdiplusbitmap.h"
#include "UISDK\Kernel\Src\Renderlibrary\gdiplus\gdiplusrender.h"
#include "UISDK\Kernel\Src\Base\Object\object.h"
#include "UISDK\Project\UI3D\inc\inc.h"

void RenderBitmapFactory::CreateInstance(IUIApplication* pUIApp, GRAPHICS_RENDER_LIBRARY_TYPE eGraphicsRenderType, IMAGE_ITEM_TYPE eType, IRenderBitmap** ppOut)
{
	if (NULL == ppOut)
		return;

	switch (eGraphicsRenderType)
	{	
	case GRAPHICS_RENDER_LIBRARY_TYPE_GDI:
		{
			if (eType == IMAGE_ITEM_TYPE_ICON)
			{
				GDIIconRenderBitmap::CreateInstance(ppOut);
			}
			else if (eType == IMAGE_ITEM_TYPE_IMAGE_LIST)
			{
				GDIImageListRenderBitmap::CreateInstance(ppOut);
			}
// 			else if (eType == IMAGE_ITEM_TYPE_GIF)
// 			{
// 				GDIGifRenderBitmap::CreateInstance(ppOut);
// 			}
// 			else if (eType == IMAGE_ITEM_TYPE_PNGLISTGIF)
// 			{
// 				GDIGifRenderBitmap::CreateInstance(ppOut);
// 			}
			else 
			{
				GDIRenderBitmap::CreateInstance(ppOut);
			}
		}
		break;

	case GRAPHICS_RENDER_LIBRARY_TYPE_GDIPLUS:
		{
			if (eType == IMAGE_ITEM_TYPE_ICON)
			{
				GdiplusIconRenderBitmap::CreateInstance(ppOut);
			}
			else if (eType == IMAGE_ITEM_TYPE_IMAGE_LIST)
			{
				GdiplusImageListRenderBitmap::CreateInstance(ppOut);
			}
			else 
			{
				GdiplusRenderBitmap::CreateInstance(ppOut);
			}
		}
		break;

	case GRAPHICS_RENDER_LIBRARY_TYPE_DIRECT2D:
		{
            if (!pUIApp)
                return;

            HMODULE  hModule = pUIApp->GetUI3DModule();
            if (!hModule)
                return;

            funcUI3D_CreateD2DRenderBitmap func = (funcUI3D_CreateD2DRenderBitmap)GetProcAddress(hModule, "UI3D_CreateD2DRenderBitmap");
            if (!func)
            {
                UIASSERT(0);
                return;
            }

            func(eType, ppOut);
		}
		break;
	}

}
 

//////////////////////////////////////////////////////////////////////////
namespace UI
{

// IRenderTarget* 没有引用计数机制
IRenderTarget*  UICreateRenderTarget(IUIApplication* pUIApp, GRAPHICS_RENDER_LIBRARY_TYPE eType)
{
    IRenderTarget*  pRenderTarget = NULL;

    switch (eType)
    {
    case GRAPHICS_RENDER_LIBRARY_TYPE_GDI:
        {
            pRenderTarget = new GdiRenderTarget();
        }
        break;
    case GRAPHICS_RENDER_LIBRARY_TYPE_GDIPLUS:
        {
            pRenderTarget = new GdiplusRenderTarget();
        }
        break;
    case GRAPHICS_RENDER_LIBRARY_TYPE_DIRECT2D:
        {
            if (!pUIApp)
            {
                UI_LOG_WARN(_T("%s CreateD2D RenderTarget Failed. pUIApp is NULL"), FUNC_NAME);
                return UICreateRenderTarget(pUIApp, GRAPHICS_RENDER_LIBRARY_TYPE_GDIPLUS);
            }
            HMODULE  hModule = pUIApp->GetUI3DModule();
            if (!hModule)
            {
                UI_LOG_WARN(_T("%s CreateD2D RenderTarget Failed. GetUI3DModule Failed."), FUNC_NAME);
                return UICreateRenderTarget(pUIApp, GRAPHICS_RENDER_LIBRARY_TYPE_GDIPLUS);
            }
         
            funcUI3D_CreateD2DRenderTarget func = (funcUI3D_CreateD2DRenderTarget)GetProcAddress(hModule, "UI3D_CreateD2DRenderTarget");
            if (!func)
            {
                UIASSERT(0);
                return UICreateRenderTarget(pUIApp, GRAPHICS_RENDER_LIBRARY_TYPE_GDIPLUS);
            }
            func(&pRenderTarget);
        }
        break;
    default:
        UIASSERT(0);
        break;
    }
    return pRenderTarget;
}

void  UICreateRenderBitmap(IUIApplication*  pUIApp, GRAPHICS_RENDER_LIBRARY_TYPE eGraphicsRenderType, IMAGE_ITEM_TYPE eType, IRenderBitmap** ppOut)
{
    RenderBitmapFactory::CreateInstance(pUIApp, eGraphicsRenderType, eType, ppOut);
}

GRAPHICS_RENDER_LIBRARY_TYPE  UIParseGraphicsRenderLibraryType(const TCHAR* szText)
{
	if (szText)
	{
		if (0 == _tcscmp(szText, XML_WINDOW_GRAPHICS_RENDER_LIBRARY_GDIPLUS))
		{
			return GRAPHICS_RENDER_LIBRARY_TYPE_GDIPLUS;
		}
		else if (0 == _tcscmp(szText, XML_WINDOW_GRAPHICS_RENDER_LIBRARY_D2D) ||
			0 == _tcscmp(szText, XML_WINDOW_GRAPHICS_RENDER_LIBRARY_DIRECT2D))
		{
			return GRAPHICS_RENDER_LIBRARY_TYPE_DIRECT2D;
		}
		else
		{
			return GRAPHICS_RENDER_LIBRARY_TYPE_GDI;
		}
	}

	return GRAPHICS_RENDER_LIBRARY_TYPE_GDI;
}
}