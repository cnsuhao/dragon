#pragma once
#include "model\doc.h"
#include "..\Caret\caret.h"

namespace UI
{

class RichText : public MessageProxy
{
public:
	RichText(IRichText* p);
	~RichText();

    UI_BEGIN_MSG_MAP()
		UIMSG_PAINT(OnPaint)
		MSG_WM_MOUSEMOVE(OnMouseMove)
		MSG_WM_MOUSELEAVE(OnMouseLeave)
		MSG_WM_SETCURSOR(OnSetCursor)
		MSG_WM_LBUTTONDOWN(OnLButtonDown)
		MSG_WM_LBUTTONUP(OnLButtonUp)
        MSG_WM_KEYDOWN(OnKeyDown)
        UIMSG_SIZE(OnSize)
        UIMSG_SETFOCUS(OnSetFocus)
        UIMSG_KILLFOCUS(OnKillFocus)

        UICHAIN_MSG_MAP_MEMBER(m_doc.GetMouseKey())

        UIMSG_GETDESIREDSIZE(GetDesiredSize)
        UIMSG_QUERYINTERFACE(RichText)
        UIMSG_SERIALIZE(OnSerialize)
		UIMSG_INITIALIZE(OnInitialize)
	UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(RichText, IControl)

    IRichText* GetIRichText() { return m_pIRichText; }

public:
	void  LoadHtml(LPCTSTR szHtml);
    void  Clear();
    void  SetText(LPCTSTR szText);
	void  AppendText(LPCTSTR szText);
	void  AppendText2(LPCTSTR szText, CharFormat* pcf);
	void  AppendImage(LPCTSTR szPath);
    RT::ILink*  AppendLink(LPCTSTR);
    RT::ILink*  AppendLink2(LPCTSTR, CharFormat* pcf);
    void  GetDefaultCharFormat(CharFormat* pcf);
	void  GetDefaultLinkCharFormat(CharFormat* pcf);

    PageMode  GetPageMode();
    void  SetPageMode(PageMode e);

	void  SetRangeCharFormat(){}

    RT::Doc&  GetDoc() { return m_doc; }
    
protected:
	void  OnInitialize();
	void  OnPaint(IRenderTarget* pRT);
    void  OnSize(UINT nType, int cx, int cy);
	void  OnMouseMove(UINT nFlags, POINT point);
	void  OnMouseLeave();
    void  OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	BOOL  OnSetCursor(HWND hWnd, UINT nHitTest, UINT message);
	void  OnLButtonDown(UINT nFlags, POINT point);
	void  OnLButtonUp(UINT nFlags, POINT point);
    void  GetDesiredSize(SIZE* pSize);
    void  SetLayoutDirty(bool b);
    void  OnSetFocus(IObject* pOldFocusObj);
    void  OnKillFocus(IObject* pNewFocusObj);
    void  OnSerialize(SERIALIZEDATA* pData);

private:
    void  try_layout();

private:
	IRichText*  m_pIRichText;
    RT::Doc  m_doc;

    // CCaret  m_caret;

    bool  m_bNeedLayout;

public:
    signal<IRichText*, RT::ILink*>  link_clicked;
};

}