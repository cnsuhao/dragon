#include "StdAfx.h"
#include "fontmanager.h"
#include "UISDK\Kernel\Src\Resource\skinres.h"
#include "UISDK\Kernel\Inc\Interface\ixmlwrap.h"
#include "UISDK\Kernel\Inc\Interface\iuires.h"

FontManager::FontManager(SkinRes* p):m_resFont(p)
{
    m_pIFontManager = NULL;
	m_pSkinRes = p;
}

FontManager::~FontManager(void)
{
	this->Clear();
    SAFE_DELETE(m_pIFontManager);

	_MyListItem* pItem = m_listUIElement.GetFirst();
	while (pItem)
	{
		(**pItem).pXmlElement->Release();
		pItem = pItem->GetNext();
	}
	m_listUIElement.Clear();
}

IFontManager*  FontManager::GetIFontManager()
{
    if (NULL == m_pIFontManager)
        m_pIFontManager = new IFontManager(this);

    return m_pIFontManager;
}

/*
**	清除所有图片资源
*/
void FontManager::Clear()
{
	m_resFont.Clear();
}

int FontManager::GetFontCount()
{
	return m_resFont.GetFontCount();
}
IFontResItem*  FontManager::GetFontItemInfo(int nIndex)
{
	FontResItem* pItem = m_resFont.GetFontItem(nIndex);
	if (NULL == pItem)
		return NULL;

    return pItem->GetIFontResItem();
}

FontRes&  FontManager::GetFontRes()
{
	return m_resFont;
}


HRESULT  FontManager::UIParseFontTagCallback(IUIElement* pElem, ISkinRes* pSkinRes)
{
    IFontManager*  pFontMgr = pSkinRes->GetFontManager();
    if (NULL == pFontMgr)
        return E_FAIL;

    return pFontMgr->GetImpl()->ParseNewElement(pElem);
}

HRESULT  FontManager::ParseNewElement(IUIElement* pElem)
{
	FontTagElementInfo info;
	info.pXmlElement = pElem;
	pElem->AddRef();

	CComBSTR bstrId;
	if (pElem->GetAttrib(XML_ID, &bstrId))
	{
		info.strId = bstrId;
	}
	else
	{
		// TODO: 取当前xml名称作为id
	}

	m_listUIElement.Add(info);

    // 遍历其子元素
    IUIChildNodeEnum* pEnum = pElem->EnumChild();
    if (pEnum)
    {
        IUIElement* pChildElement = NULL;
        while (pChildElement = pEnum->NextElement())
        {
            this->OnNewChild(pChildElement);
            SAFE_RELEASE(pChildElement);
        }

        SAFE_RELEASE(pEnum);
    }

    return S_OK;
}

int CALLBACK _EnumFontsProc(
                           CONST LOGFONT *lplf,     // logical-font data
                           CONST TEXTMETRIC *lptm,  // physical-font data
                           DWORD dwType,            // font type
                           LPARAM lpData            // application-defined data
                           )
{
    BYTE* pbChatSet = (BYTE*)lpData;
    *pbChatSet = lplf->lfCharSet;
    return 0;
}

void  FontManager::OnNewChild(IUIElement* pElem)
{
    CComBSTR bstrTagName;
    pElem->GetTagName(&bstrTagName);

    //	加载所有属性
    IMapAttribute* pMapAttrib = NULL;
    pElem->GetAttribList(&pMapAttrib);

    String  strID;
    String  strFaceName;
    int   nFontHeight = 9;
    int   lfOrientation = 0;
    bool  bBold = false;
    bool  bItalic = false;
    bool  bUnderline = false;
    bool  bStrikeout = false;
    bool  bClearType = false;
    
    const TCHAR* szText = pMapAttrib->GetAttr(XML_ID, true);
    if (szText)
        strID = szText;

    szText = pMapAttrib->GetAttr(XML_FONT_FACENAME, true);
    if (szText)
        strFaceName = szText;

    if (!Util::IsFontExist(strFaceName.c_str()))
    {
        szText = pMapAttrib->GetAttr(XML_FONT_FACENAME2, true);
        if (szText)
            strFaceName = szText;
    }

    pMapAttrib->GetAttr_int(XML_FONT_HEIGHT, true, &nFontHeight);
    pMapAttrib->GetAttr_int(XML_FONT_ORIENTATION, true, &lfOrientation);
    pMapAttrib->GetAttr_bool(XML_FONT_BOLD, true, &bBold);
    pMapAttrib->GetAttr_bool(XML_FONT_ITALIC, true, &bItalic);
    pMapAttrib->GetAttr_bool(XML_FONT_UNDERLINE, true, &bUnderline);
    pMapAttrib->GetAttr_bool(XML_FONT_STRIKEOUT, true, &bStrikeout);
    pMapAttrib->GetAttr_bool(XML_FONT_CLEARTYPE, true, &bClearType);

    if (strFaceName.empty() )
        strFaceName = _T("SYSTEM");
    
    LOGFONT lf;
    ::ZeroMemory( &lf, sizeof(lf) );
    _tcsncpy(lf.lfFaceName, strFaceName.c_str(), 31);

    lf.lfHeight = Util::FontSize2Height(nFontHeight);
    lf.lfOrientation = lfOrientation;
    lf.lfEscapement = lf.lfOrientation;
    lf.lfWeight = bBold?FW_BOLD:FW_NORMAL;
    lf.lfItalic = bItalic?1:0;
    lf.lfUnderline = bUnderline?1:0;
    lf.lfStrikeOut = bStrikeout?1:0;
    lf.lfQuality = bClearType ? CLEARTYPE_QUALITY : DEFAULT_QUALITY;

    // 获取这个字体的chatset。
    // 场景 ♪ 在xp下面，gdi显示成口，将chatset修改为gb2312之后显示正常
    HDC hDC = GetDC(NULL);
    EnumFonts(hDC, lf.lfFaceName, _EnumFontsProc, (WPARAM)&lf.lfCharSet);
    ReleaseDC(NULL, hDC);

    if (false == m_resFont.InsertFont(strID, &lf))
        UI_LOG_WARN( _T("insert font failed. m_strID=%s, facename=%s"), strID.c_str(), strFaceName.c_str() );

    SAFE_RELEASE(pMapAttrib);
}
