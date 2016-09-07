#include "stdafx.h"
#include "checkbutton.h"

using namespace UI;

CheckButton::CheckButton()
{
    m_pICheckButton = NULL;
	SetButtonType(BUTTON_STYLE_CHECKBUTTON);
}

CheckButton::~CheckButton()
{
}

void CheckButton::OnClicked()
{
    // ���ⲿ��Ӧʱ�ܹ���ȷ����IsChecked�����ж�
    if (m_nCheckState & BST_CHECKED)
        m_nCheckState &= ~BST_CHECKED;
    else
        m_nCheckState |= BST_CHECKED;

    UIMSG   msg;
    msg.message = UI_WM_NOTIFY;
    msg.nCode   = UI_BN_CLICKED;
    msg.wParam  = NULL; //(WPARAM)&pt;
    msg.lParam  = NULL;
    msg.pMsgFrom = m_pICheckButton;
    LRESULT lRet = m_pICheckButton->DoNotify(&msg);
    if (lRet) // �ⲿ�Ѿ�������
        return;
}

// void CheckButton::SetAttribute(IMapAttribute* pMapAttrib, bool bReload)
// {
//     DO_PARENT_PROCESS(ICheckButton, IButton);
// 
//     if (NULL == m_pICheckButton->GetBkRender())
//     {
// 		ObjStyle s = {0};
// 		s.transparent = 1;
//         m_pICheckButton->ModifyObjectStyle(&s, 0);  // checkĬ����͸����
// 
//         if (NULL == m_pICheckButton->GetForeRender())
//         {
//             IRenderBase* p = NULL;
//             m_pICheckButton->GetUIApplication()->CreateRenderBase(RENDER_TYPE_THEME_CHECK_BUTTON, m_pICheckButton, &p);
// 
//             if (p)
//             {
//                 m_pICheckButton->SetForegndRender(p);
//                 p->Release();
//                 m_lIconMarginText = 4;
//             }
//         }
//     }
// }
