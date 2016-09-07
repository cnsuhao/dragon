#pragma once
#include "UISDK\Control\Inc\Interface\irichtext.h"
#include "model\doc.h"

namespace UI
{

class RichText : public MessageProxy
{
public:
	RichText();
	~RichText();

	UI_DECLARE_OBJECT(RichText, OBJ_CONTROL|CONTROL_LABEL)

	UI_BEGIN_MSG_MAP
		UIMSG_WM_PAINT2(OnPaint)
		UIMSG_WM_MOUSEMOVE(OnMouseMove)
		UIMSG_WM_MOUSELEAVE(OnMouseLeave)
		UIMSG_WM_SETCURSOR(OnSetCursor)
		UIMSG_WM_LBUTTONDOWN(OnLButtonDown)
		UIMSG_WM_LBUTTONUP(OnLButtonUp)
        UIMSG_WM_SIZE(OnSize)
		UIMSG_WM_INITIALIZE(OnInitialize)
	UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(RichText, IControl)

	void  SetIRichText(IRichText* p);

public:
	void  LoadHtml(LPCTSTR szHtml);
	void  AppendText(LPCTSTR szText);
	void  AppendText2(LPCTSTR szText, RichTextCharFormat* pcf);
	void  AppendImage(LPCTSTR szPath);
	void  AppendLink(RichTextLinkInfo* pInfo);
	void  AppendLink2(RichTextLinkInfo* pInfo, RichTextCharFormat* pcf);
    void  GetDefaultCharFormat(RichTextCharFormat* pcf);
	void  GetDefaultLinkCharFormat(RichTextCharFormat* pcf);

	void  SetRangeCharFormat(){}

protected:
	void  OnInitialize();
	void  OnPaint(IRenderTarget* pRT, RenderContext* pContext);
    void  OnSize(UINT nType, int cx, int cy);
	void  OnMouseMove(UINT nFlags, POINT point);
	void  OnMouseLeave();
	BOOL  OnSetCursor(HWND hWnd, UINT nHitTest, UINT message);
	void  OnLButtonDown(UINT nFlags, POINT point);
	void  OnLButtonUp(UINT nFlags, POINT point);

    void  SetLayoutDirty(bool b);

private:
	IRichText*  m_pIRichText;
	RichTextObjectModel::Doc  m_doc;
	RichTextMKMgr*  m_pMKMgr;

    bool  m_bNeedLayout;
};

}