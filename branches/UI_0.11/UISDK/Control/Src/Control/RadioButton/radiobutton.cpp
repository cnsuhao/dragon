#include "stdafx.h"
#include "radiobutton.h"

using namespace UI;


RadioButton::RadioButton()
{
    m_pIRadioButton = NULL;
	SetButtonType(BUTTON_STYLE_RADIOBUTTON);
}

//
//	TODO:
//		������ע�͵���tab stop�ı仯����Ϊ��ǰ��û��ʵ��ʹ�÷�������л�ͬһ�����ڵ�
//		��������ͬʱҲ����ʹ�÷�������л�ͬһ�����ڵĶ���û��ʹ��TAB���л����ж���
//		���㡣
//
void RadioButton::OnClicked()
{
    UIMSG   msg;
    msg.message = UI_WM_NOTIFY;
    msg.nCode    = UI_BN_CLICKED;
    msg.wParam  = NULL; // (WPARAM)pt;
    msg.lParam  = NULL;
    msg.pMsgFrom = m_pIRadioButton;

    if (m_nCheckState & BST_CHECKED)
    {
    }
    else
    {
        this->SetCheck(BST_CHECKED);
        //		this->SetTabstop(true);

        // ȡ������radio��ť��ѡ��״̬

        bool bFind = false;
        IObject* pPrevRadioButton = m_pIRadioButton->GetPrevObject();  // ��ǰ����
        while (1)
        {
            if (NULL == pPrevRadioButton || (IObject*)m_pIRadioButton == pPrevRadioButton)
                break;

            if (pPrevRadioButton->IsSelfVisible() &&
                pPrevRadioButton->GetObjectExtentType() == CONTROL_RADIOBUTTON)
            {
                if ((static_cast<IButtonBase*>(pPrevRadioButton))->IsChecked())
                {
                    bFind = true;
                    (static_cast<IButtonBase*>(pPrevRadioButton))->SetCheck(false, true);
                    break;
                }
            }
            if (static_cast<IButtonBase*>(pPrevRadioButton)->IsGroup())
                break;

            pPrevRadioButton = pPrevRadioButton->GetPrevObject();
        }

        if (!bFind)
        {
            IObject* pNextRadioButton = m_pIRadioButton->GetNextObject();      // �������
            while (1)
            {
                if (NULL == pNextRadioButton || (IObject*)m_pIRadioButton == pNextRadioButton)
                    break;

                if (static_cast<IButtonBase*>(pNextRadioButton)->IsGroup() )
                    break;

                if (pNextRadioButton->IsSelfVisible() &&
                    pNextRadioButton->GetObjectExtentType() == CONTROL_RADIOBUTTON)
                {
                    if ((static_cast<IButtonBase*>(pNextRadioButton))->IsChecked())
                    {
                        bFind = true;
                        (static_cast<IButtonBase*>(pNextRadioButton))->SetCheck(false, true);
                        break;
                    }
                }
                pNextRadioButton = pNextRadioButton->GetNextObject();
            }
        }
    }

	LRESULT lRet = m_pIRadioButton->DoNotify(&msg);
	if (0 != lRet) // �ⲿ�Ѿ�������
		return;
}


// void RadioButton::SetAttribute(IMapAttribute* pMapAttrib, bool bReload)
// {
//     DO_PARENT_PROCESS(IRadioButton, IButton);
// 
//     if (NULL == m_pIRadioButton->GetBkRender())
//     {
//         m_pIRadioButton->SetTransparent(true); // RadioĬ����͸����
// 
//         if (NULL == m_pIRadioButton->GetForeRender())
//         {
//             IRenderBase* p = NULL;
//             m_pIRadioButton->GetUIApplication()->CreateRenderBase(RENDER_TYPE_THEME_RADIO_BUTTON, m_pIRadioButton, &p);
// 
//             if (p)
//             {
//                 m_pIRadioButton->SetForegndRender(p);
//                 p->Release();
//                 m_lIconMarginText = 3;
//             }
//         }
//     }
// }
