#include "stdafx.h"
#include "renderbase.h"
#include "UISDK\Kernel\Src\Base\Object\object.h"
#include "UISDK\Kernel\Src\Base\Attribute\attribute.h"
#include "UISDK\Kernel\Inc\Interface\iuires.h"
#include "UISDK\Kernel\Src\Resource\skinres.h"
#pragma comment(lib, "uxtheme.lib")

using namespace UI;


RenderBase::RenderBase()
{
    m_pIRenderBase = NULL;
	m_pObject = NULL;
	m_nRenderType = RENDER_TYPE_NULL;
    m_pUIApplication = NULL;
}

IColorRes*  RenderBase::GetSkinColorRes()
{
	if (m_pObject)
	{
		SkinRes* pSkinRes = m_pObject->GetSkinRes();
		if (pSkinRes)
			return pSkinRes->GetColorRes().GetIColorRes();
	}
	else
	{
		if (m_pUIApplication)
			return m_pUIApplication->GetActiveSkinColorRes();
	}

	return NULL;
}

IImageRes*  RenderBase::GetSkinImageRes()
{
	if (m_pObject)
	{
		 SkinRes* pSkinRes = m_pObject->GetSkinRes();
		 if (pSkinRes)
			 return pSkinRes->GetImageRes().GetIImageRes();
	}
	else
	{
		if (m_pUIApplication)
			return m_pUIApplication->GetActiveSkinImageRes();
	}

	return NULL;
}

void  RenderBase::CheckThemeChanged()
{
    if (this->IsThemeRender())
    {
        UISendMessage(static_cast<IMessage*>(m_pIRenderBase), WM_THEMECHANGED);
    }
}

void  RenderBase::_LoadColor(LPCTSTR szColorId, Color*& pColorRef)
{
	SAFE_RELEASE(pColorRef);
	if (!szColorId)
		return;

	IColorRes* pColorRes = GetSkinColorRes();
	if (!pColorRes)
		return;

	pColorRes->GetColor(szColorId, &pColorRef);
}
LPCTSTR  RenderBase::_GetColorId(Color*& pColorRef)
{
	if (!pColorRef)
		return NULL;

	IColorRes* pColorRes = GetSkinColorRes();
	if (pColorRes)
	{
		LPCTSTR szId = pColorRes->GetColorId(pColorRef);
		if (szId)
			return szId;
	}

	TCHAR* szBuffer = GetTempBuffer();
	pColorRef->ToHexString(szBuffer);
	return szBuffer;
}

void  RenderBase::_LoadBitmap(LPCTSTR szBitmapId, IRenderBitmap*& pBitmapRef)
{
	SAFE_RELEASE(pBitmapRef);
	if (!szBitmapId)
		return;


	if (m_pObject)
	{
		SkinRes* pSkinRes = m_pObject->GetSkinRes();
		if (!pSkinRes)
			return;

		pSkinRes->GetImageRes().GetBitmap(
			szBitmapId, 
			m_pObject->GetIObject()->GetGraphicsRenderLibraryType(),
			&pBitmapRef);
	}
	else
	{
		IImageRes* pImageRes = GetSkinImageRes();
		if (!pImageRes)
			return;

		pImageRes->GetBitmap(szBitmapId, UI::GRAPHICS_RENDER_LIBRARY_TYPE_GDI, &pBitmapRef); 
	}
}

LPCTSTR  RenderBase::_GetBitmapId(IRenderBitmap*& pBitmapRef)
{
	if (!pBitmapRef)
		return NULL;

	IImageRes* pImageRes = GetSkinImageRes();
	if (!pImageRes)
		return NULL;

	return pImageRes->GetRenderBitmapId(pBitmapRef);
}

//////////////////////////////////////////////////////////////////////////

ThemeRenderBase::ThemeRenderBase()
{
    m_bNoTheme = false;
    m_hTheme = NULL;
    m_pIThemeRenderBase = NULL;
}
ThemeRenderBase::~ThemeRenderBase()
{
    if (m_hTheme)
    {
        ::CloseThemeData( m_hTheme );
        m_hTheme = NULL;
    }
}
void ThemeRenderBase::OnInit()
{
    __super::nvProcessMessage(m_pIThemeRenderBase->GetCurMsg(), 0, false);
    this->CreateTheme(); // 这个时候可能还没拿到m_bNoTheme属性
}
void ThemeRenderBase::OnThemeChanged()
{
    this->CreateTheme();
}
void ThemeRenderBase::CreateTheme()
{
    if (m_hTheme)
    {
        ::CloseThemeData(m_hTheme);
        m_hTheme = NULL;
    }
    if (!m_bNoTheme)
    {
        const TCHAR* pThemeName = m_pIThemeRenderBase->GetThemeName();
        if (pThemeName)
        {
            // ps: 正常时，第一个参数用NULL即可，但是在绘制listview item/Treeview item时，
            //     必须先调用SetWindowTheme(m_hWnd, L"explorer", NULL);
            ///    同时用OpenThemeData(m_hWnd)才行。
            m_hTheme = ::OpenThemeData(m_pObject->GetHWND(), pThemeName);
        }
    }
}
void  ThemeRenderBase::OnSerialize(SERIALIZEDATA* pData)
{
    {
        AttributeSerializer s(pData, TEXT("ThemeRenderBase"));
        s.AddBool(XML_RENDER_THEME_DISABLE, m_bNoTheme);
    }

    if (pData->IsLoad() && m_bNoTheme && m_hTheme)
    {
        ::CloseThemeData(m_hTheme);
        m_hTheme = NULL;
    }
}
