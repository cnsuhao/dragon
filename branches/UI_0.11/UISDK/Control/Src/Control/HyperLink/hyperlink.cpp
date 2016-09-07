#include "stdafx.h"
#include "UISDK\Control\Src\Control\HyperLink\hyperlink.h"
#include "UISDK\Kernel\Inc\Interface\iattribute.h"

namespace UI
{

HyperLink::HyperLink()
{
    m_pIHyperLink = NULL;
	SetButtonType(BUTTON_STYLE_HYPERLINK);
}

HRESULT  HyperLink::FinalConstruct(IUIApplication* p)
{
    HRESULT hr = DO_PARENT_PROCESS3(IHyperLink, IButton);
    if (FAILED(hr))
        return hr;

    // Hyperlink默认是透明的
	ObjStyle s = {0};
	s.default_transparent = 1;
    m_pIHyperLink->ModifyObjectStyle(&s, 0);

    return S_OK;
}

#if 0 // 不再提供默认字体。全由外部使用style来实现
void  HyperLink::InitDefaultFont()
{
    // 初始化默认字体
    if (m_pIHyperLink->GetTextRender())
        return;

    IUIApplication*  pUIApplication = m_pIHyperLink->GetUIApplication();
    IFontRes* pFontRes = pUIApplication->GetActiveSkinFontRes();
    if (NULL == pFontRes)
        return;

    bool bNeedReleaseFont = false;
    IRenderFont* pFont = m_pIHyperLink->GetRenderFont();
    if (!pFont)
    {
        pFontRes->GetDefaultFont(m_pIHyperLink->GetGraphicsRenderLibraryType(), &pFont);
        if (!pFont)
            return;

        bNeedReleaseFont = true;
    }

    ITextRenderBase*  pTextRenderBase = NULL;
    pUIApplication->CreateTextRenderBase(TEXTRENDER_TYPE_FONTCOLORLIST, m_pIHyperLink, &pTextRenderBase);
    if (!pTextRenderBase)
        return;

    IMapAttribute* pMapAttrib = NULL;
    m_pIHyperLink->GetMapAttribute(&pMapAttrib);

    SERIALIZEDATA data = {0};
    data.pUIApplication = pUIApplication;
    data.pMapAttrib = pMapAttrib;
    data.szPrefix = NULL;
    data.nFlags = SERIALIZEFLAG_LOAD|SERIALIZEFLAG_LOAD_ERASEATTR;
    pTextRenderBase->Serialize(&data);

    IFontColorListTextRender* p = (IFontColorListTextRender*)pTextRenderBase->QueryInterface(uiiidof(IFontColorListTextRender));
    if (p)
    {
        p->SetCount(4);
        p->SetColor(0, RGBA(0,0,255, 255));
        p->SetColor(1, RGBA(255,0,0, 255));
        p->SetColor(2, RGBA(255,0,0, 255));
        p->SetColor(3, RGBA(192,192,192, 255));

        IRenderFont* pUnderLineFont = NULL;
        pFontRes->GetFontEx(pFont, FONTITEM_FLAG_UNDERLINE, 0, m_pIHyperLink->GetGraphicsRenderLibraryType(), &pUnderLineFont);

        p->SetFont(0, pFont);
        p->SetFont(1, pUnderLineFont);
        p->SetFont(2, pUnderLineFont);
        p->SetFont(3, pFont);
        SAFE_RELEASE(pUnderLineFont);
    }

    m_pIHyperLink->SetTextRender(pTextRenderBase);
    SAFE_RELEASE(pMapAttrib);
    SAFE_RELEASE(pTextRenderBase);

    if (bNeedReleaseFont)
        SAFE_RELEASE(pFont);
}

#endif

LPCTSTR  HyperLink::GetHref()
{
    return m_strHref.c_str();
}
void  HyperLink::SetHref(LPCTSTR sz)
{
    SETSTRING(m_strHref, sz);
}

void HyperLink::OnSerialize(SERIALIZEDATA* pData)
{
	DO_PARENT_PROCESS(IHyperLink, IButton);

	{
		AttributeSerializerWrap s(pData, TEXT("HyperLink"));
		s.AddString(XML_HYPERLINK_HREF, this, 
			memfun_cast<pfnStringSetter>(&HyperLink::SetHref),
			memfun_cast<pfnStringGetter>(&HyperLink::GetHref))
			->AsData();
	}
}


BOOL HyperLink::OnSetCursor(HWND hWnd, UINT nHitTest, UINT message)
{
	::SetCursor(::LoadCursor(NULL, IDC_HAND));
	return TRUE;
}
void HyperLink::OnClicked()
{
	UIMSG   msg;
	msg.message = UI_WM_NOTIFY;
	msg.nCode    = UI_BN_CLICKED;
	msg.wParam  = 0;
	msg.lParam  = NULL;
	msg.pMsgFrom = m_pIHyperLink;
	LRESULT lRet = m_pIHyperLink->DoNotify(&msg);
	if (0 != lRet) // 外部已经处理了
		return;

	if (m_strHref.length() > 0)
	{
		ShellExecute(NULL,_T("open"), m_strHref.c_str(), NULL, NULL, SW_SHOW );
	}
}

}