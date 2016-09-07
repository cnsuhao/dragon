#pragma once
#include "UISDK\Control\Src\Control\Button\button.h"
#include "UISDK\Control\Inc\Interface\icheckbutton.h"

namespace UI
{

class CheckButton : public Button
{
public:
    CheckButton();
    ~CheckButton();

    UI_DECLARE_OBJECT(CheckButton, OBJ_CONTROL|CONTROL_CHECKBUTTON)

    UI_BEGIN_MSG_MAP
        UIMSG_BN_CLICKED2(m_pIButtonBase, OnClicked)
        UIMSG_WM_GETDLGCODE(OnGetDlgCode)
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
        UIMSG_WM_GETOBJECTINFO(OnGetObjectInfo)
    UI_END_MSG_MAP_CHAIN_PARENT(Button)

    void  SetICheckButton(ICheckButton* p) { m_pICheckButton = p; }

	void  OnClicked();
    UINT  OnGetDlgCode(LPMSG lpMsg) { return 0; }

protected:
    ICheckButton*  m_pICheckButton;
};


}
