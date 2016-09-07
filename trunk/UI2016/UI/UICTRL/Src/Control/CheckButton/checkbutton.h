#pragma once
#include "..\Button\button.h"
#include "Inc\Interface\icheckbutton.h"

namespace UI
{

class CheckButton : public Button
{
public:
    CheckButton(ICheckButton* p);
    ~CheckButton();

	UI_BEGIN_MSG_MAP()
        MSG_WM_GETDLGCODE(OnGetDlgCode)
        UIMSG_QUERYINTERFACE(CheckButton)
    UI_END_MSG_MAP_CHAIN_PARENT(Button)

	virtual void  virtualOnClicked();
    UINT  OnGetDlgCode(LPMSG lpMsg) { return 0; }

protected:
    ICheckButton*  m_pICheckButton;
};


}
