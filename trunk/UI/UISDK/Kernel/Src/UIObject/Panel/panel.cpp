#include "stdafx.h"
#include "panel.h"
#include "UISDK\Kernel\Inc\Interface\ilayout.h"
#include "UISDK\Kernel\Inc\Interface\iwindow.h"
#include "UISDK\Kernel\Src\Helper\layout\layout.h"
#include "UISDK\Kernel\Inc\Interface\imapattr.h"
#include "UISDK\Kernel\Src\UIObject\Window\windowbase.h"

namespace UI
{

Panel::Panel()
{
    m_pIPanel = NULL;
	m_pLayout = NULL;

    m_pTextureRender = NULL;
    m_rcTextureRenderRegion.SetRectEmpty();

//     m_pMaskRender = NULL;
//     m_rcMaskRenderRegion.SetRectEmpty();

	SetRectEmpty(&m_rcBkgndRenderRegion);
	SetRectEmpty(&m_rcForegndRenderRegion);
}
Panel::~Panel()
{
	SAFE_RELEASE(m_pLayout);
    SAFE_RELEASE(m_pTextureRender);
}

ILayout* Panel::GetLayout()
{
	return this->m_pLayout; 
}

void  Panel::SetLayoutType(LAYOUTTYPE eLayoutType)
{
    SAFE_RELEASE(m_pLayout);
 
    IUIApplication*  pUIApp = GetUIApplication();
    if (pUIApp)
        pUIApp->CreateLayout(eLayoutType, m_pIPanel, &m_pLayout);
}

LRESULT Panel::OnGetLayoutPtr(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return (LRESULT)m_pLayout;
}

void  Panel::virtualOnSize(UINT nType, UINT cx, UINT cy)
{
	Object::virtualOnSize(nType, cx, cy);
    if (m_pLayout)
        m_pLayout->Arrange(NULL);
}

void  Panel::OnGetDesiredSize(SIZE* pSize)
{
    pSize->cx = pSize->cy = 0;

    if (this->m_pLayout)
        *pSize= this->m_pLayout->Measure();

    pSize->cx += GetNonClientW();
    pSize->cy += GetNonClientH();
}

void  Panel::ResetAttribute()
{
	__super::ResetAttribute();
	SAFE_RELEASE(m_pLayout);

	this->ModifyStyle(OBJECT_STYLE_TRANSPARENT|OBJECT_STYLE_REJECT_MOUSE_MSG_SELF, 0, 0);
	SetRectEmpty(&m_rcBkgndRenderRegion);
	SetRectEmpty(&m_rcForegndRenderRegion);

    SAFE_RELEASE(m_pTextureRender);
    m_rcTextureRenderRegion.SetRectEmpty();
//     SAFE_RELEASE(m_pMaskRender);
//     m_rcMaskRenderRegion.SetRectEmpty();
}
void  Panel::SetAttribute(IMapAttribute* pMapAttrib, bool bReload)
{
	__super::SetAttribute(pMapAttrib, bReload);

	SAFE_RELEASE(m_pLayout);
    const TCHAR* szText = pMapAttrib->GetAttr(XML_LAYOUT, true);
    GetUIApplication()->CreateLayoutByName((BSTR)szText, m_pIPanel, true, &m_pLayout);

    if (m_pLayout)
        m_pLayout->SetAttribute(pMapAttrib);

    // 加载皮肤属性
    SAFE_RELEASE(m_pTextureRender);
    pMapAttrib->GetAttr_RenderBase(XML_TEXTURE_RENDER_PREFIX, XML_RENDER_TYPE, true, m_pUIApplication, m_pIObject, &m_pTextureRender);
    pMapAttrib->GetAttr_CRegion4(XML_TEXTURE_RENDER_PREFIX XML_PANEL_RENDER_REGION, true, &m_rcTextureRenderRegion);

//     SAFE_RELEASE(m_pMaskRender);
//     pMapAttrib->GetAttr_RenderBase(XML_MASK_RENDER_PREFIX, XML_RENDER_TYPE, true, m_pUIApplication, m_pIObject, &m_pMaskRender);
//     pMapAttrib->GetAttr_CRegion4(XML_MASK_RENDER_PREFIX XML_PANEL_RENDER_REGION, true, &m_rcMaskRenderRegion);

    pMapAttrib->GetAttr_CRegion4(XML_BACKGND_RENDER_PREFIX XML_PANEL_RENDER_REGION, true, &m_rcBkgndRenderRegion);
    pMapAttrib->GetAttr_CRegion4(XML_FOREGND_RENDER_PREFIX XML_PANEL_RENDER_REGION, true, &m_rcForegndRenderRegion);
}

void  Panel::OnEditorGetAttrList(EDITORGETOBJECTATTRLISTDATA* pData)
{
	__super::OnEditorGetAttrList(pData);
    IUIEditor* pEditor = pData->pEditor;

	IUIEditorGroupAttribute*  pPanelGroup = pEditor->CreateGroupAttribute(pData->pGroupAttr, Panel::GetXmlName(), NULL);

    IUIEditorGroupAttribute*  pLayoutGroup = pEditor->CreateGroupAttribute(pPanelGroup, XML_LAYOUT, NULL);
	pEditor->CreateTextAttribute(pLayoutGroup, XML_LAYOUT, pData->szPrefix);
    if (m_pLayout)
    {
        EDITORGETOBJECTATTRLISTDATA data = {0};
        data.pEditor = pEditor;
        data.pGroupAttr = pLayoutGroup;
        data.szPrefix = pData->szPrefix;
        m_pLayout->OnEditorGetAttrList(&data);
    }

	pEditor->CreateTextAttribute(pPanelGroup, XML_TEXTURE_RENDER_PREFIX XML_RENDER_TYPE, pData->szPrefix);
	pEditor->CreateTextAttribute(pPanelGroup, XML_TEXTURE_RENDER_PREFIX XML_PANEL_RENDER_REGION, pData->szPrefix);
	pEditor->CreateTextAttribute(pPanelGroup, XML_BACKGND_RENDER_PREFIX XML_PANEL_RENDER_REGION, pData->szPrefix);
	pEditor->CreateTextAttribute(pPanelGroup, XML_FOREGND_RENDER_PREFIX XML_PANEL_RENDER_REGION, pData->szPrefix);

    IUIEditorGroupAttribute* pTextureRenderGroup = pEditor->CreateGroupAttribute(pPanelGroup, XML_TEXTURE_RENDER_PREFIX, NULL);
    pEditor->CreateTextAttribute(pTextureRenderGroup, XML_RENDER_TYPE, pData->szPrefix, XML_TEXTURE_RENDER_PREFIX);
    if (m_pTextureRender)
    {
        String strPrefix;
        if (pData->szPrefix)
            strPrefix.append(pData->szPrefix);
        strPrefix.append(XML_TEXTURE_RENDER_PREFIX);

        EDITORGETOBJECTATTRLISTDATA data;
        data.pEditor = pData->pEditor;
        data.pGroupAttr = pTextureRenderGroup;
        data.szPrefix = strPrefix.c_str();
        UISendMessage(m_pTextureRender, UI_EDITOR_GETOBJECTATTRLIST, (WPARAM)&data);
    }
}

// Panel作为一个容器，不应该有自己的前景，否则上面的子对象无常获取到正确的背景了
// 但为了适应两个Render的绘制，将foregnd render也在onerasebkgnd中绘制
//
// TODO: 为了将用于换肤的图片能够绘制在Frame图片的阴影之内，这里加了m_rcBkgndRenderRegion的配置
//       
void Panel::OnEraseBkgnd(IRenderTarget* pRenderTarget)
{
	CRect rc(0,0, this->GetWidth(), this->GetHeight());
	
	if (m_pBkgndRender)
	{
		CRect rcBkgnd(&rc);
		rcBkgnd.DeflateRect(&m_rcBkgndRenderRegion);
		m_pBkgndRender->DrawState(pRenderTarget, &rcBkgnd, 0);
	}
	
    if (m_pTextureRender)
    {
        CRect rcTextureRegion(&rc);
        rcTextureRegion.DeflateRect(&m_rcTextureRenderRegion);
        m_pTextureRender->DrawState(pRenderTarget, &rcTextureRegion, 0);
    }

	if (m_pForegndRender)
	{
		CRect rcForegnd(&rc);
		rcForegnd.DeflateRect(&m_rcForegndRenderRegion);
		m_pForegndRender->DrawState(pRenderTarget, &rcForegnd, 0);
	}
}


void Panel::SetTextureRender(IRenderBase* p)
{
    SAFE_RELEASE(m_pTextureRender);
    m_pTextureRender = p;

    if (m_pTextureRender)
        m_pTextureRender->AddRef();
}
IRenderBase*  Panel::GetTextureRender()
{
    return m_pTextureRender;
}

}