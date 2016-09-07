#include "stdafx.h"
#include "PropertyCtrlAdditionItem.h"
#include "UISDK\Kernel\Inc\Interface\ipanel.h"
#include "UISDK\Kernel\Inc\Interface\ilayout.h"
#include "UISDK\Kernel\Inc\Interface\iwindow.h"

namespace UI
{
PropertyCtrlAdditionItem::PropertyCtrlAdditionItem()
{
    m_pIPropertyCtrlAdditionItem = NULL;
    m_pBaseShareData = NULL;
}
PropertyCtrlAdditionItem::~PropertyCtrlAdditionItem()
{
    // ��ɾ��ʱ���ܿؼ���ʧ��״̬���Լ���ѡ��״̬Ҳ�������
    if (IsEditActive())
    {
        m_pBaseShareData->pEditCtrl->SetVisible(false, false, false);
        m_pBaseShareData->pEditCtrl->SetNotify(NULL, 0);
        m_pBaseShareData->pEditCtrl->RemoveMeInTheTree();
    }
}

void  PropertyCtrlAdditionItem::OnInitialize()
{
    DO_PARENT_PROCESS(IPropertyCtrlAdditionItem, IPropertyCtrlItemBase);

	m_pIPropertyCtrlAdditionItem->SetItemType(IPropertyCtrlAdditionItem::FLAG);
}


void  PropertyCtrlAdditionItem::OnPaint(IRenderTarget* pRenderTarget)
{
	DO_PARENT_PROCESS(IPropertyCtrlAdditionItem, IListItemBase);
	IListCtrlBase*  pListCtrl = m_pIPropertyCtrlAdditionItem->GetIListCtrlBase();

	CRect rcParent;
	m_pIPropertyCtrlAdditionItem->GetParentRect(&rcParent);

	CRect rcBlank(rcParent);
	rcBlank.right = 0;//pListCtrl->GetChildNodeIndent();

	CRect rcLeft(rcParent);
	rcLeft.left = rcBlank.right;
	rcLeft.right = m_pBaseShareData->GetSplitterLinePos();

	// ��౳��
	Color c(PROPERTYCTRL_LINE_COLOR);
	pRenderTarget->DrawRect(&rcBlank, &c);

	// ��ʾ�ı�
	DRAWTEXTPARAM param;
	param.color = 0xFF808080;
	param.nFormatFlag = DT_LEFT|DT_VCENTER|DT_END_ELLIPSIS|DT_SINGLELINE;
	rcLeft.left += TEXT_INDENT;
	param.prc = &rcLeft;
	param.szText = TEXT("Add new item ...");
	pRenderTarget->DrawString(m_pBaseShareData->m_pRenderFont, &param);

	// �м�ķָ���
	IRenderPen* pPen = pRenderTarget->CreateSolidPen(1, &c);
	pRenderTarget->DrawLine(rcLeft.right, rcLeft.top, rcLeft.right, rcLeft.bottom, pPen);

	// һ�еĺ���
	pRenderTarget->DrawLine(rcParent.left, rcParent.bottom-1, rcParent.right, rcParent.bottom-1, pPen);
	SAFE_RELEASE(pPen);
}

void  PropertyCtrlAdditionItem::OnSize(UINT nType, int cx, int cy)
{
    if (m_pIPropertyCtrlAdditionItem->IsSelected() && m_pBaseShareData->pEditCtrl->IsSelfVisible())
    {
        m_pBaseShareData->pEditCtrl->SetConfigLeft(m_pBaseShareData->GetSplitterLinePos() + 1);
    }
    SetMsgHandled(FALSE);
}

void  PropertyCtrlAdditionItem::SetValueText(LPCTSTR szText)
{
    if (szText)
        m_strValue = szText;
    else
        m_strValue.clear();

	if (IsEditActive())
	{
		m_pBaseShareData->pEditCtrl->SetText(m_strValue.c_str(), false);
	}
}

void  PropertyCtrlAdditionItem::SetDefaultValueText(LPCTSTR szText)
{
	if (szText)
		m_strDefautValue = szText;
	else
		m_strDefautValue.clear();
}

LRESULT  PropertyCtrlAdditionItem::OnStateChanged(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (wParam & OSB_SELECTED)
    {
        bool bSelected = m_pIPropertyCtrlAdditionItem->IsSelected();

        if (bSelected)
        {
            BeginEdit();
        }
        else
        {
            if (0 != _tcscmp(m_pBaseShareData->pEditCtrl->GetText(), m_strValue.c_str()))
            {
                AcceptEdit();
            }
            EndEdit();
        }
    }
    return 0;
}
LRESULT  PropertyCtrlAdditionItem::OnSetValue(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LPCTSTR szText = (LPCTSTR )wParam;
	this->SetValueText(szText);

	return 0;
}
LRESULT  PropertyCtrlAdditionItem::OnSetDefaultValue(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LPCTSTR szText = (LPCTSTR )wParam;
	this->SetDefaultValueText(szText);

	return 0;
}

LRESULT  PropertyCtrlAdditionItem::OnEditKeyDown(WPARAM w, LPARAM l)
{
	if (w == VK_ESCAPE)
	{
		m_pBaseShareData->pEditCtrl->SetText((TCHAR*)NULL, true);
		return UI::HANDLED;
	}
	else if (w == VK_RETURN)
	{
		AcceptEdit();
		m_pIPropertyCtrlAdditionItem->GetIListCtrlBase()->Refresh();
		return UI::HANDLED;
	}

	return UI::NOT_HANDLED;
}

void  PropertyCtrlAdditionItem::EndEdit()
{
    if (NULL == m_pBaseShareData->pEditCtrl)
        return;

	if (IsEditActive())
	{
		m_pBaseShareData->pEditCtrl->SetVisible(false, false, false);
		m_pBaseShareData->pEditCtrl->SetNotify(NULL, 0);
		m_pBaseShareData->pEditCtrl->RemoveMeInTheTree();
		m_pBaseShareData->pEditCtrl->SetText(NULL, false);
	}
}

void  PropertyCtrlAdditionItem::AcceptEdit()
{
    UIMSG  msg;
	msg.message = UI_WM_NOTIFY;
    msg.nCode = UI_PROPERTYCTRL_NM_ADDITIONITEM_ACCEPTCONTENT;
    msg.wParam = (WPARAM)m_pIPropertyCtrlAdditionItem;
    msg.lParam= (LPARAM)m_pBaseShareData->pEditCtrl->GetText();

    ACCEPT_EDIT_RESULT eRes = (ACCEPT_EDIT_RESULT)
			m_pIPropertyCtrlAdditionItem->GetIListCtrlBase()->DoNotify(&msg);
	
	switch (eRes)
	{
	case ACCEPT_EDIT_ACCEPT:
	case ACCEPT_EDIT_REJECT:
		if (IsEditActive())
		{
			EndEdit();
		}
		break;

	case ACCEPT_EDIT_REJECT_AND_CONTINUE_EDIT:
		{
			if (IsEditActive())
			{
				m_pBaseShareData->pEditCtrl->SetFocusInWindow();
			}
		}
		break;
	}

	m_pIPropertyCtrlAdditionItem->GetIListCtrlBase()
		->InvalidateItem(m_pIPropertyCtrlAdditionItem);
}

void  PropertyCtrlAdditionItem::BeginEdit()
{
    if (NULL == m_pBaseShareData->pEditCtrl)
        return;

    CRect rc;
    GetKeyColoumnRectInItem(&rc);
    rc.DeflateRect(TEXT_INDENT,1,1,1);

    IPanel* pPanel = m_pIPropertyCtrlAdditionItem->GetRootPanel();
    pPanel->AddChild(m_pBaseShareData->pEditCtrl);

    m_pBaseShareData->pEditCtrl->SetConfigLeft(rc.left);
    m_pBaseShareData->pEditCtrl->SetConfigRight(NDEF);
    m_pBaseShareData->pEditCtrl->SetConfigWidth(rc.Width());
    m_pBaseShareData->pEditCtrl->SetConfigTop(1);
    m_pBaseShareData->pEditCtrl->SetConfigBottom(1);

	m_pBaseShareData->pEditCtrl->SetText(NULL, false);
    m_pBaseShareData->pEditCtrl->SetVisible(true, false, false);
    m_pBaseShareData->pEditCtrl->SetNotify(m_pIPropertyCtrlAdditionItem, 0);
    pPanel->GetLayout()->Arrange(m_pBaseShareData->pEditCtrl, true);

    // ����Edit���㣬���������༭ģʽ
    IListCtrlBase* pCtrl = m_pIPropertyCtrlAdditionItem->GetIListCtrlBase();
    pCtrl->SetFocusObject(m_pBaseShareData->pEditCtrl);

    // ���������ʽ���п����������λ��edit��
    // Ȼ�����EDIT�еĹ��λ��Ϊ����µ�λ��
    HWND  hWnd = pCtrl->GetHWND(); 
    POINT pt;
    GetCursorPos(&pt);
    MapWindowPoints(NULL, hWnd, &pt, 1);

    // ��ʱ��껹û����������󲻷���up��Ϣ��������ֱ����ѡ������
    if (Util::IsKeyDown(VK_LBUTTON))
    {
        SendMessage(hWnd, WM_LBUTTONUP, 0, MAKELPARAM(pt.x, pt.y));
        SendMessage(hWnd, WM_MOUSEMOVE, 0, MAKELPARAM(pt.x, pt.y));
        SendMessage(hWnd, WM_LBUTTONDOWN, 0, MAKELPARAM(pt.x, pt.y));
    }

    if (m_pBaseShareData->pEditCtrl->IsHover())
        SetCursor(LoadCursor(NULL, IDC_IBEAM));
}

}