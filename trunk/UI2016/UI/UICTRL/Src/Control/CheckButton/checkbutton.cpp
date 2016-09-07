#include "stdafx.h"
#include "checkbutton.h"
#include "checkbutton_desc.h"

using namespace UI;

CheckButton::CheckButton(ICheckButton* p):Button(p)
{
    m_pICheckButton = p;
	SetButtonType(BUTTON_TYPE_CHECKBUTTON);
	p->SetDescription(CheckButtonDescription::Get());
}

CheckButton::~CheckButton()
{
}

void CheckButton::virtualOnClicked()
{
    // 在外部响应时能够正确调用IsChecked进行判断
    if (m_nCheckState & BST_CHECKED)
        m_nCheckState &= ~BST_CHECKED;
    else
        m_nCheckState |= BST_CHECKED;

	__super::virtualOnClicked();
}

