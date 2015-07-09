#pragma once
#include "UISDK\Control\Inc\Interface\ihyperlink.h"
#include "UISDK\Control\Src\Control\Button\button.h"
namespace UI
{

class HyperLink : public Button
{
public:
	HyperLink();

	UI_DECLARE_OBJECT(HyperLink, OBJ_CONTROL|CONTROL_HYPERLINK)

	UI_BEGIN_MSG_MAP
		UIMSG_WM_SETCURSOR(OnSetCursor)
        UIMSG_BN_CLICKED2(m_pIHyperLink, OnClicked)
        UIMSG_WM_GETDLGCODE(OnGetDlgCode)
        UIMSG_WM_GETOBJECTINFO(OnGetObjectInfo)
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
        UIMSG_WM_SERIALIZE(OnSerialize)
        UIMSG_WM_FINALCONSTRUCT(FinalConstruct)
	UI_END_MSG_MAP_CHAIN_PARENT(Button)

    void  SetIHyperLink(IHyperLink* p) { m_pIHyperLink = p;}

public:
    LPCTSTR  GetHref();
    void  SetHref(LPCTSTR);

protected:
    HRESULT  FinalConstruct(IUIApplication* p);
    void  OnSerialize(SERIALIZEDATA*);
	BOOL  OnSetCursor( HWND hWnd, UINT nHitTest, UINT message );
    void  OnClicked();
    UINT  OnGetDlgCode(LPMSG lpMsg) { return 0; }

private:
    IHyperLink*  m_pIHyperLink;
    String  m_strHref;
};


}
