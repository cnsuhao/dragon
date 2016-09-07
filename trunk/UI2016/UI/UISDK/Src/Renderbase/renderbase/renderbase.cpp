#include "stdafx.h"
#include "renderbase.h"
#include "Src\Base\Object\object.h"
#include "Src\Base\Attribute\attribute.h"
#include "Inc\Interface\iuires.h"
#include "Src\Resource\skinres.h"
#include "Src\Base\Application\uiapplication.h"
#include "Inc\Interface\renderlibrary.h"
#include "Src\UIObject\Window\windowbase.h"
#include "Src\Layer\windowrender.h"
#pragma comment(lib, "uxtheme.lib")

using namespace UI;


RenderBase::RenderBase(IRenderBase* p) : Message(p)
{
    m_pIRenderBase = p;
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
			return m_pUIApplication->GetActiveSkinColorRes()->GetIColorRes();
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
			return m_pUIApplication->GetActiveSkinImageRes()->GetIImageRes();
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

		GRAPHICS_RENDER_LIBRARY_TYPE eType = GRAPHICS_RENDER_LIBRARY_TYPE_GDI;
		WindowBase* pWnd = m_pObject->GetWindowObject();
		if (pWnd)
		{
			WindowRender* pRender = pWnd->GetWindowRender();
			if (pRender)
				eType = pRender->GetGraphicsRenderType();
		}

		pSkinRes->GetImageRes().GetBitmap(
			szBitmapId, 
			eType,
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

