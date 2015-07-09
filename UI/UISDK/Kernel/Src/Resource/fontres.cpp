#include "stdafx.h"
#include "fontres.h"
#include "UISDK\Kernel\Src\Renderlibrary\gdi\gdifont.h"
#include "UISDK\Kernel\Src\Renderlibrary\gdiplus\gdiplusfont.h"
#include "UISDK\Kernel\Src\Resource\skinres.h"
#include "UISDK\Kernel\Inc\Interface\iuires.h"
//#include "UISDK\Project\UI3D\inc\inc.h"

namespace UI
{

#if 0

UIFont::UIFont(LOGFONT* plogfont, UIFont** ppOutRef)
{ 
	m_hFont = NULL; 
	m_dwRef = 0; 
	m_pLogFont = plogfont;

	m_pOutRef = ppOutRef;
	*ppOutRef = this;
}
UIFont::~UIFont() 
{ 
	if (-1 == m_dwRef )
	{
		m_hFont = NULL;
	}
	else
	{
		if (m_hFont)
		{
			::DeleteObject(m_hFont); 
			m_hFont = NULL;
		}
	}
	m_dwRef = 0; 
	m_pLogFont = NULL;

	if(m_pOutRef != NULL)
	{
		*m_pOutRef = NULL;
		m_pOutRef = NULL;
	}
}


/*static*/ void UIFont::CreateInstance(LOGFONT* plogfont, UIFont** ppOutRef) 
{ 
	UIFont* pFont = new UIFont(plogfont, ppOutRef); 
	
	if (plogfont)
	{
		pFont->Load(plogfont);
	}
	pFont->AddRef();
}

bool UIFont::Load( LOGFONT* plogfont )
{
	if (NULL == plogfont)
	{
		UI_LOG_WARN(_T("UIFont::Load plogfont == NULL"));
		return false;
	}

	m_pLogFont = plogfont;
	m_hFont = ::CreateFontIndirect(m_pLogFont);
	if (NULL == m_hFont )
	{
		UI_LOG_WARN(_T("UIFont::SetLogFont CreateFontIndirect failed, facename=%s"), m_pLogFont->lfFaceName );
		return false;
	}

	return true;
}

HFONT UIFont::GetFont()
{ 
	return m_hFont; 
}

void UIFont::ModifyFont(LOGFONT* plogfont)
{
	m_pLogFont = plogfont;
	if (m_hFont)
	{
		::DeleteObject(m_hFont);
		m_hFont = ::CreateFontIndirect(m_pLogFont);
	}
}

//
//	TODO: Attach/Detach怎么与GetFont/Release统一起来使用？
//
//	--> 目前 Detach全由Release来替代，Release自己判断是否需要释放FONT对象
//
void UIFont::Attach(HFONT hFont)
{	
	UIASSERT(hFont != NULL);
	m_hFont = hFont;
	m_pLogFont = NULL;
}
HFONT UIFont::Detach()
{
	UIASSERT(-1 == m_dwRef);
	UIASSERT(m_hFont != NULL);

	HFONT hOldFont = m_hFont;
	m_hFont = NULL;

	return hOldFont;
}
long UIFont::AddRef()
{
	if ( NULL == m_hFont && NULL != m_pLogFont )
	{
		m_hFont = ::CreateFontIndirect( m_pLogFont );
		if (NULL == m_hFont )
		{
			UI_LOG_WARN( _T("UIFont::AddRef, Create font failed, font facename=%s size=%d"), m_pLogFont->lfFaceName, m_pLogFont->lfHeight );
		}
	}

	m_dwRef ++;
	return m_dwRef;
}
long UIFont::Release()
{
	m_dwRef--;
	if (0 >= m_dwRef )
	{
		if (m_hFont && NULL != m_pLogFont)
		{
			::DeleteObject(m_hFont);
		}
		m_hFont = NULL;

		m_dwRef = 0;
		delete this;
		return 0;
	}

	return m_dwRef;
}
#endif

FontResItem::FontResItem() 
{ 
    m_pIFontResItem = NULL;
	m_pGdiFont = NULL;
	m_pGdiplusFont = NULL;
	m_pD2DFont = NULL;
    m_pD3DFont = NULL;
// 	m_wParam = 0;
// 	m_lParam = 0;
}
FontResItem::~FontResItem()
{ 
	SAFE_DELETE(m_pGdiFont);
	SAFE_DELETE(m_pGdiplusFont);
	SAFE_RELEASE(m_pD2DFont);
    SAFE_RELEASE(m_pD3DFont);
    SAFE_DELETE(m_pIFontResItem);
}

IFontResItem*  FontResItem::GetIFontResItem()
{
    if (NULL == m_pIFontResItem)
        m_pIFontResItem = new IFontResItem(this);

    return m_pIFontResItem;
}
//
// 通过结构体来快速赋值
//
void FontResItem::SetLogFont( LOGFONT* pLogFont )
{
	if ( NULL==pLogFont)
		return;

	memcpy(&m_lf, pLogFont, sizeof(LOGFONT));
	_tcscpy(m_lf.lfFaceName, pLogFont->lfFaceName);
	m_lf.lfEscapement = m_lf.lfOrientation;    // TODO: 实现字体角度的话，这两个值好像要相等
}

void FontResItem::ModifyFont(LOGFONT* pLogFont)
{
	this->SetLogFont(pLogFont);

	if (m_pGdiFont)
	{
		m_pGdiFont->ModifyFont(pLogFont);
	}
	if (m_pGdiplusFont)
	{
		m_pGdiplusFont->ModifyFont(pLogFont);
	}
	if (m_pD2DFont)
	{
		m_pD2DFont->ModifyFont(pLogFont);
	}
    if (m_pD3DFont)
    {
        m_pD3DFont->ModifyFont(pLogFont);
    }
}

IRenderFont* FontResItem::GetFont(SkinRes* pSkinRes, GRAPHICS_RENDER_LIBRARY_TYPE eRenderType )
{
	switch(eRenderType)
	{
	case GRAPHICS_RENDER_LIBRARY_TYPE_GDI:
		{
			if (NULL == m_pGdiFont)
			{
				GDIRenderFont::CreateInstance((IRenderFont**)&m_pGdiFont);
				if (m_pGdiFont)
				{
					m_pGdiFont->SetOutRef((IRenderResource**)&m_pGdiFont);
					m_pGdiFont->Load(&m_lf);
					UI_LOG_DEBUG(_T("gdi font create: %s, facename=%s, size=%d, Ptr=0x%08X"), m_strId.c_str(), m_lf.lfFaceName, m_lf.lfHeight, m_pGdiFont );
				}
                return (IRenderFont*)m_pGdiFont;
			}
			if (m_pGdiFont )
			{
				m_pGdiFont->AddRef();
			}
			return (IRenderFont*)m_pGdiFont;
		}
		break;

	case GRAPHICS_RENDER_LIBRARY_TYPE_GDIPLUS:
		{
			if (NULL == m_pGdiplusFont)
			{
				GdiplusRenderFont::CreateInstance((IRenderFont**)&m_pGdiplusFont);
				if (m_pGdiplusFont )
				{
					m_pGdiplusFont->SetOutRef((IRenderResource**)&m_pGdiplusFont);

					m_pGdiplusFont->Load(&m_lf);
					UI_LOG_DEBUG(_T("gdiplus font create: %s \tPtr=0x%08X"), m_strId.c_str(), m_pGdiplusFont );
				}
                return (IRenderFont*)m_pGdiplusFont;
			}
			if (m_pGdiplusFont)
			{
				m_pGdiplusFont->AddRef();
			}
			return (IRenderFont*)m_pGdiplusFont;
		}
		break;

#if 0
	case GRAPHICS_RENDER_LIBRARY_TYPE_DIRECT2D:
		{
			if (NULL == m_pD2DFont)
			{
                IUIApplication*  pUIApp = pSkinRes->GetUIApplication();
                if (!pUIApp)
                    return NULL;

                HMODULE  hModule = pUIApp->GetUID2DModule();
                if (!hModule)
                    return  NULL;

                funcUI3D_CreateD2DRenderFont func = (funcUI3D_CreateD2DRenderFont)GetProcAddress(hModule, NAME_UI3D_CreateD2DRenderFont);
                if (!func)
                {
                    UIASSERT(0);
                    return  NULL;
                }

                func(&m_pD2DFont);
				if (m_pD2DFont )
				{
					m_pD2DFont->SetOutRef((IRenderResource**)&m_pD2DFont);
					m_pD2DFont->Load(&m_lf);
					UI_LOG_DEBUG(_T("%s direct2d font create: %s \tPtr=0x%08X"), FUNC_NAME, m_strId.c_str(), m_pD2DFont );
				}
                return (IRenderFont*)m_pD2DFont;
			}
			if (m_pD2DFont)
			{
				m_pD2DFont->AddRef();
			}
			return (IRenderFont*)m_pD2DFont;
		}
		break;

    case GRAPHICS_RENDER_LIBRARY_TYPE_DIRECT3D:
        {
            if (NULL == m_pD3DFont)
            {
                IUIApplication*  pUIApp = pSkinRes->GetUIApplication();
                if (!pUIApp)
                    return NULL;

                HMODULE  hModule = pUIApp->GetUID3DModule();
                if (!hModule)
                    return  NULL;

                funcUI3D_CreateD3DRenderFont func = (funcUI3D_CreateD3DRenderFont)GetProcAddress(hModule, NAME_UI3D_CreateD3DRenderFont);
                if (!func)
                {
                    UIASSERT(0);
                    return  NULL;
                }

                func(&m_pD3DFont);
                if (m_pD3DFont)
                {
                    m_pD3DFont->SetOutRef((IRenderResource**)&m_pD3DFont);
                    m_pD3DFont->Load(&m_lf);
                    UI_LOG_DEBUG(_T("%s direct3d font create: %s \tPtr=0x%08X"), FUNC_NAME, m_strId.c_str(), m_pD3DFont);
                }
                return (IRenderFont*)m_pD3DFont;
            }
            if (m_pD3DFont)
            {
                m_pD3DFont->AddRef();
            }
            return (IRenderFont*)m_pD3DFont;
        }
        break;;
#endif

	default:
		return NULL;
	}

	return NULL;
}

bool FontResItem::IsMyRenderFont(IRenderFont* pRenderFont)
{ 
    if (m_pGdiFont == pRenderFont || m_pGdiplusFont == pRenderFont ||
		m_pD3DFont == pRenderFont || m_pD2DFont == pRenderFont) 
    {
        return true;
    }  
    return false;
}

FontRes::FontRes(SkinRes* pSkinRes)
{
    m_pSkinRes = pSkinRes;
    m_pIFontRes = NULL;
}
FontRes::~FontRes()
{
	this->Clear();
    SAFE_DELETE(m_pIFontRes);
}
IFontRes*  FontRes::GetIFontRes()
{
    if (NULL == m_pIFontRes)
        m_pIFontRes = new IFontRes(this);

    return m_pIFontRes;
}
long  FontRes::GetFontCount() 
{ 
	return (long) m_vFonts.size(); 
}
bool  FontRes::GetFontResItem(long lIndex, IFontResItem** ppResItem)
{
	if (NULL == ppResItem)
		return false;
	
	FontResItem* pItem = this->GetFontItem(lIndex);
	if (NULL == pItem)
		return false;

    *ppResItem = pItem->GetIFontResItem();
	return true;
}

FontResItem*  FontRes::GetFontItem( int nIndex )
{
	if (nIndex < 0)
		return NULL;
	if (nIndex >= (int)m_vFonts.size() )
		return NULL;

	return m_vFonts[nIndex];
}

FontResItem*  FontRes::GetFontItem(const String& strId)
{
	vector<FontResItem*>::iterator  iter = m_vFonts.begin();
	vector<FontResItem*>::iterator  iterEnd = m_vFonts.end();

	for( ; iter != iterEnd; iter++ )
	{
		FontResItem* p = *iter;
		LPCTSTR szId = p->GetId();
		if (strId == szId)
			return p;
	}
	return NULL;
}

bool FontRes::GetFont(LPCTSTR szFontId, GRAPHICS_RENDER_LIBRARY_TYPE eRenderType, __out IRenderFont** ppOut)
{
	if (NULL == szFontId || NULL == ppOut)
		return false;

	vector<FontResItem*>::iterator iter = m_vFonts.begin();
	vector<FontResItem*>::iterator iterEnd = m_vFonts.end();

	for (; iter != iterEnd; iter++)
	{
		FontResItem* p = *iter;
		if (0 == wcscmp(p->GetId(), szFontId))
		{
			// if (p->GetWParam() == 0 && p->GetLParam() == 0 )
			{
				*ppOut = p->GetFont(m_pSkinRes, eRenderType);
				return true;
			}
		}
	}
	return false;
}

bool  FontRes::GetDefaultFont(GRAPHICS_RENDER_LIBRARY_TYPE eRenderType, __out IRenderFont** ppFont)
{
	if (NULL == ppFont)
		return false;

	if (0 == (int)m_vFonts.size())
		return false;

	if (NULL == m_vFonts[0])
		return false;

	 *ppFont = m_vFonts[0]->GetFont(m_pSkinRes, eRenderType);
	return true;
}

LPCTSTR  FontRes::GetDefaultFontId()
{
	if (0 == (int)m_vFonts.size())
		return NULL;

	if (NULL == m_vFonts[0])
		return NULL;

	return m_vFonts[0]->GetId();
}

LPCTSTR  FontRes::GetRenderFontId(IRenderFont* pFont)
{
	vector<FontResItem*>::iterator iter = m_vFonts.begin();
	vector<FontResItem*>::iterator iterEnd = m_vFonts.end();

	for( ; iter != iterEnd; iter++ )
	{
		FontResItem* p = *iter;
		if (p->IsMyRenderFont(pFont) )
		{
			return p->GetId();
		}
	}
	return NULL;
}

#if 0
bool  FontRes::GetFontEx(IRenderFont* pFont, WPARAM wParam, LPARAM lParam, GRAPHICS_RENDER_LIBRARY_TYPE eRenderType, __out IRenderFont** ppFont)
{
	if (NULL == pFont || NULL == ppFont)
		return false;

	LPCTSTR szFontId = GetRenderFontId(pFont);
	if (!szFontId)
		return false;
	
	vector<FontResItem*>::iterator iter = m_vFonts.begin();
	vector<FontResItem*>::iterator iterEnd = m_vFonts.end();

	FontResItem* pBaseFont = NULL;
	for( ; iter != iterEnd; iter++)
	{
		FontResItem* p = *iter;
		if (0 == _tcscmp(szFontId, p->GetId()))
		{
			if (p->GetWParam() == wParam && p->GetLParam() == lParam)
			{
				*ppFont = p->GetFont(m_pSkinRes, eRenderType);
				return true;
			}
			else
			{
				if (p->GetWParam() == 0 && p->GetLParam() == 0)
				{
					pBaseFont = p;
				}
			}
		}
	}

	if (pBaseFont)  // 没有找到 strFontID+wparam+lparam对应的字体，但找到了strFontID对应的字体
	{
		LOGFONT lf;
		memcpy( &lf, pBaseFont->GetLogFont(), sizeof(LOGFONT) );
		if (wParam & FONTITEM_FLAG_UNDERLINE )
		{
			lf.lfUnderline = 1;
		}
		if (wParam & FONTITEM_FLAG_BOLD )
		{
			lf.lfWeight = FW_BOLD;
		}
		if (wParam & FONTITEM_FLAG_STRIKEOUT )
		{
			lf.lfStrikeOut = 1;
		}
		if ( wParam & FONTITEM_FLAG_ITALIC )
		{
			lf.lfItalic = 1;
		}
		FontResItem* pNewFontItem = this->InsertFont(szFontId, &lf, wParam, lParam );
		if (NULL == pNewFontItem )
		{
			UI_LOG_ERROR( _T("%s InsertFont failed, id=%s wParam=%d lParam=%d"), 
				FUNC_NAME, szFontId, wParam, lParam);
			return false;
		}

		*ppFont = pNewFontItem->GetFont(m_pSkinRes, eRenderType);
		return true;
	}

	return false;
}
#endif
bool FontRes::InsertFont( const String& strID, LOGFONT* pLogFont )
{
	FontResItem* pItem = this->InsertFont(strID, pLogFont, 0,0 );
	if (NULL == pItem )
		return false;
	
	return true;
}

FontResItem* FontRes::InsertFont( 
	const String& strID, 
	LOGFONT* pLogFont,
	WPARAM wParam,
	LPARAM lParam )
{
	if (NULL == pLogFont )
	{
		UI_LOG_ERROR(_T("FontRes::InsertFont failed."));
		return NULL;
	}
	vector<FontResItem*>::iterator  iter = m_vFonts.begin();
	vector<FontResItem*>::iterator  iterEnd = m_vFonts.end();

	for( ; iter != iterEnd; iter++ )
	{
		FontResItem* p = *iter;

		if (strID == p->GetId() /*&& p->GetWParam() == wParam && p->GetLParam() == lParam */)
		{
			UI_LOG_WARN(_T("FontRes::InsertFont failed, insert item=%s"), strID.c_str() );
			return NULL;
		}
	}

	FontResItem*  pFontItem = new FontResItem;
	pFontItem->SetId(strID.c_str());
	pFontItem->SetLogFont( pLogFont );
// 	pFontItem->SetWParam(wParam);
// 	pFontItem->SetLParam(lParam);

	this->m_vFonts.push_back(pFontItem);  
	return pFontItem;
}

bool FontRes::ModifyFont( const String& strID, LOGFONT* pLogFont )
{
	if (NULL == pLogFont )
	{
		UI_LOG_ERROR(_T("FontRes::ModifyFont failed."));
		return NULL;
	}
	vector<FontResItem*>::iterator  iter = m_vFonts.begin();
	vector<FontResItem*>::iterator  iterEnd = m_vFonts.end();

	for (; iter != iterEnd; iter++)
	{
		FontResItem* p = *iter;

		if (strID == p->GetId())
		{
			p->ModifyFont(pLogFont);
			return true;
		}
	}
	UI_LOG_WARN(_T("FontRes::InsertFont failed, insert item=%s"), strID.c_str() );
	return false;
}

bool FontRes::RemoveFont( const String& strID )
{
	vector<FontResItem*>::iterator  iter = m_vFonts.begin();
	vector<FontResItem*>::iterator  iterEnd = m_vFonts.end();

	for( ; iter != iterEnd; iter++ )
	{
		FontResItem* p = *iter;

		if (strID == p->GetId())
		{
			delete p;
			p = NULL;
			m_vFonts.erase(iter);
			return true;
		}
	}
	return false;
}

void FontRes::Clear()
{
	vector<FontResItem*>::iterator  iter = m_vFonts.begin();
	vector<FontResItem*>::iterator  iterEnd = m_vFonts.end();

	for (; iter != iterEnd; iter++)
	{
		FontResItem* p = *iter;
		delete p;
		p = NULL;
	}

	m_vFonts.clear();
}

}