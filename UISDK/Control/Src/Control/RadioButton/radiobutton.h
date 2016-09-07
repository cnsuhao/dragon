#ifndef RADIOBUTTON_H_3913A8BF_C812_4c9a_A29B_9237BFFC5319
#define RADIOBUTTON_H_3913A8BF_C812_4c9a_A29B_9237BFFC5319

#include "UISDK\Control\Src\Control\Button\button.h"
#include "UISDK\Control\Inc\Interface\iradiobutton.h"


namespace UI
{

class RadioButton : public Button
{
public:
    RadioButton();
    
    UI_DECLARE_OBJECT(RadioButton, OBJ_CONTROL|CONTROL_RADIOBUTTON)

    UI_BEGIN_MSG_MAP
        UIMSG_BN_CLICKED2(m_pIButtonBase, OnClicked)
        UIMSG_WM_GETDLGCODE(OnGetDlgCode)
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
        UIMSG_WM_GETOBJECTINFO(OnGetObjectInfo)
    UI_END_MSG_MAP_CHAIN_PARENT(Button)

    void  SetIRadioButton(IRadioButton* p) { m_pIRadioButton = p; }

public:
    void  OnClicked();
    UINT  OnGetDlgCode(LPMSG lpMsg) { return 0; }

protected:
    IRadioButton*  m_pIRadioButton;
};

}

#endif // RADIOBUTTON_H_3913A8BF_C812_4c9a_A29B_9237BFFC5319