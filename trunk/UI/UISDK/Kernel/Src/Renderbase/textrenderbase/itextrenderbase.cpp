#include "stdafx.h"
#include "UISDK\Kernel\Inc\Interface\itextrenderbase.h"
#include "UISDK\Kernel\Src\Renderbase\textrenderbase\textrender.h"
#include "UISDK\Kernel\Src\Base\Object\object.h"

namespace UI
{

ITextRenderBase::ITextRenderBase()
{
    m_bCreateTextRenderBaseImpl = NULL;
    m_pTextRenderBaseImpl = NULL;
    m_lRef = 0;
}
ITextRenderBase::~ITextRenderBase()                    
{                                              
    if (m_bCreateTextRenderBaseImpl)            
        SAFE_DELETE(m_pTextRenderBaseImpl);     
}      

UI_IMPLEMENT_Ixxx_INTERFACE_ProcessMessage(ITextRenderBase, TextRenderBase)
UI_IMPLEMENT_Ixxx_INTERFACE_CreateImpl(ITextRenderBase, TextRenderBase, Message)
UI_IMPLEMENT_Ixxx_INTERFACE(IThemeTextRenderBase, ThemeTextRenderBase, TextRenderBase)
UI_IMPLEMENT_Ixxx_INTERFACE(ISimpleTextRender, SimpleTextRender, TextRenderBase)
UI_IMPLEMENT_Ixxx_INTERFACE(IContrastColorTextRender, ContrastColorTextRender, TextRenderBase)
UI_IMPLEMENT_Ixxx_INTERFACE(IContrastColorListTextRender, ContrastColorListTextRender, TextRenderBase)
UI_IMPLEMENT_Ixxx_INTERFACE(IColorListTextRender, ColorListTextRender, TextRenderBase)
UI_IMPLEMENT_Ixxx_INTERFACE(IFontColorListTextRender, FontColorListTextRender, TextRenderBase)

long  ITextRenderBase::AddRef() 
{ 
    m_lRef++;  
    return m_lRef; 
}
long  ITextRenderBase::Release()
{ 
    m_lRef--; 
    if (0 >= m_lRef) 
    {
        this->delete_this(); 
        return 0;
    }
    return m_lRef; 
}

void  ITextRenderBase::SetObject(IUIApplication*  pUIApp, IObject* pObject)
{
    Object* pObj = NULL;
    if (pObject)
        pObj = pObject->GetImpl();

    m_pTextRenderBaseImpl->SetObject(pUIApp, pObj);
}
IObject*  ITextRenderBase::GetObject()
{
    Object* pObj = m_pTextRenderBaseImpl->GetObject();
    if (pObj)
        return pObj->GetIObject();
    return NULL;
}
void  ITextRenderBase::SetTextAlignment(int nDrawFlag)
{
    return m_pTextRenderBaseImpl->SetTextAlignment(nDrawFlag);
}
int  ITextRenderBase::GetTextAlignment()
{
    return m_pTextRenderBaseImpl->GetTextAlignment();
}
void  ITextRenderBase::SetTextEffect(TEXT_EFFECT e)
{
    m_pTextRenderBaseImpl->SetTextEffect(e);
}
TEXT_EFFECT  ITextRenderBase::GetTextEffect()
{
    return m_pTextRenderBaseImpl->GetTextEffect();
}
void  ITextRenderBase::SetDrawTextParam(WPARAM w, LPARAM l)
{
    m_pTextRenderBaseImpl->SetDrawTextParam(w, l);
}
void  ITextRenderBase::SetType(const TEXTRENDER_TYPE& nType)
{
    m_pTextRenderBaseImpl->SetTextRenderType(nType);
}
TEXTRENDER_TYPE  ITextRenderBase::GetType()
{
    return m_pTextRenderBaseImpl->GetTextRenderType();
}

IRenderFont*  ITextRenderBase::GetRenderFont()
{
    return (IRenderFont*)UISendMessage(static_cast<IMessage*>(this),
        UI_WM_GETRENDERFONT);
}

void  ITextRenderBase::Serialize(SERIALIZEDATA* pData)
{
    UISendMessage(static_cast<IMessage*>(this), UI_WM_SERIALIZE, (WPARAM)pData);
}

void  ITextRenderBase::_LoadFont(LPCTSTR szFontId, IRenderFont*& pRenderFont)
{
	m_pTextRenderBaseImpl->_LoadFont(szFontId, pRenderFont);
}
LPCTSTR  ITextRenderBase::_SaveFont(IRenderFont*& pRenderFont)
{	
	return m_pTextRenderBaseImpl->_SaveFont(pRenderFont);
}
void  ITextRenderBase::_LoadDefalutFont(IRenderFont*& pRenderFont)
{
	m_pTextRenderBaseImpl->_LoadDefalutFont(pRenderFont);
}
void  ITextRenderBase::_LoadColor(LPCTSTR szColorId, Color*& pColor)
{
	m_pTextRenderBaseImpl->_LoadColor(szColorId, pColor);
}
LPCTSTR  ITextRenderBase::_SaveColor(Color*& pColor)
{
	return m_pTextRenderBaseImpl->_SaveColor(pColor);
}

SIZE  ITextRenderBase::GetDesiredSize(LPCTSTR szText, int nLimitWidth)
{
//     SIZE s = {0,0}; 
// 
//     GETDESIREDSIZEINFO info;
//     info.nLimitWidth = nLimitWidth;
//     info.szText = szText;
//     UISendMessage(static_cast<IMessage*>(this), UI_WM_GETDESIREDSIZE, (WPARAM)&s, (LPARAM)&info);
// 
//     return s; 

    return m_pTextRenderBaseImpl->GetDesiredSize(szText, nLimitWidth);
}
void  ITextRenderBase::DrawState(
        IRenderTarget* pRenderTarget, 
        const CRect* prc, 
        int nState, 
        LPCTSTR szText, 
        int nDrawTextFlag)
{
    TEXTRENDERBASE_DRAWSTATE drawstate = {0};
    drawstate.ds_renderbase.pRenderTarget = pRenderTarget;
    if (prc)
        CopyRect(&drawstate.ds_renderbase.rc, prc);
    drawstate.ds_renderbase.nState = nState;

    drawstate.szText = szText;
    drawstate.nDrawTextFlag = nDrawTextFlag;

    UISendMessage(static_cast<IMessage*>(this), 
        UI_WM_RENDERBASE_DRAWSTATE, (WPARAM)&drawstate, 0);
}
void  ITextRenderBase::Init()
{
     UISendMessage(static_cast<IMessage*>(this), UI_WM_INITIALIZE);
}
void  ITextRenderBase::CheckSkinTextureChanged()
{
    return m_pTextRenderBaseImpl->CheckSkinTextureChanged();
}



LPCTSTR  IThemeTextRenderBase::GetThemeName()
{
    LPCTSTR pszName = NULL;
    UISendMessage(static_cast<IMessage*>(this), 
        UI_WM_RENDERBASE_GETTHEMENAME, (WPARAM)&pszName, 0);
    return pszName;
}

HTHEME  IThemeTextRenderBase::GetTHEME()
{
    return m_pThemeTextRenderBaseImpl->GetTHEME();
}
void  IThemeTextRenderBase::Serialize(AttributeSerializerWrap* ps)
{
	m_pThemeTextRenderBaseImpl->Serialize(ps);
}

//////////////////////////////////////////////////////////////////////////

void  ISimpleTextRender::SetColor(Color* pColText) 
{
    return m_pSimpleTextRenderImpl->SetColor(pColText); 
}
void  ISimpleTextRender::SetRenderFont(IRenderFont* p) 
{
    m_pSimpleTextRenderImpl->SetRenderFont(p);
}

//////////////////////////////////////////////////////////////////////////

void  IColorListTextRender::SetRenderFont(IRenderFont* p) 
{
    m_pColorListTextRenderImpl->SetRenderFont(p);
}
void  IColorListTextRender::SetCount(int nCount)
{
    m_pColorListTextRenderImpl->SetCount(nCount); 
}
void  IColorListTextRender::SetColor(int nIndex, COLORREF col)
{
    m_pColorListTextRenderImpl->SetColor(nIndex, col); 
}

//////////////////////////////////////////////////////////////////////////

void  IFontColorListTextRender::SetCount(int nCount)
{
    return m_pFontColorListTextRenderImpl->SetCount(nCount);
}
void  IFontColorListTextRender::SetColor(int nIndex, DWORD color) 
{ 
    return m_pFontColorListTextRenderImpl->SetColor(nIndex, color);
}
void  IFontColorListTextRender::SetFont(int nIndex, IRenderFont* p) 
{
    return m_pFontColorListTextRenderImpl->SetFont(nIndex, p); 
}


}