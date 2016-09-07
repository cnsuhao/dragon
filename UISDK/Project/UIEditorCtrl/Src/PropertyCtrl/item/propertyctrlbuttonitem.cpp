#include "stdafx.h"
#include "propertyctrlbuttonitem.h"
#include "UISDK\Kernel\Inc\Interface\ipanel.h"
#include "UISDK\Kernel\Inc\Interface\ilayout.h"

namespace UI
{

PropertyCtrlButtonItem::PropertyCtrlButtonItem()
{
    m_pIPropertyCtrlButtonItem = NULL;
    m_pBaseShareData = NULL;
}
PropertyCtrlButtonItem::~PropertyCtrlButtonItem()
{
    // ��ɾ��ʱ���ܿؼ���ʧ��״̬���Լ���ѡ��״̬Ҳ�������
    // if (m_pIPropertyCtrlButtonItem->IsSelected())

    IObject* pObj = m_pBaseShareData->pButtonCtrl->GetParentObject();
    if (pObj && pObj == m_pIPropertyCtrlButtonItem->GetRootPanel())
    {
        m_pBaseShareData->pButtonCtrl->SetVisible(false, false, false);
        m_pBaseShareData->pButtonCtrl->SetNotify(NULL, 0);
        m_pBaseShareData->pButtonCtrl->RemoveMeInTheTree();
    }

	pObj = m_pBaseShareData->pEditCtrl->GetParentObject();
	if (pObj && pObj == m_pIPropertyCtrlButtonItem->GetRootPanel())
	{
		m_pBaseShareData->pEditCtrl->SetVisible(false, false, false);
		m_pBaseShareData->pEditCtrl->SetNotify(NULL, 0);
		m_pBaseShareData->pEditCtrl->RemoveMeInTheTree();
	}
}

void  PropertyCtrlButtonItem::OnInitialize()
{
    DO_PARENT_PROCESS(IPropertyCtrlButtonItem, IPropertyCtrlItemBase);
	m_pIPropertyCtrlButtonItem->SetItemType(IPropertyCtrlButtonItem::FLAG);
}


void  PropertyCtrlButtonItem::OnPaint(IRenderTarget* pRenderTarget)
{
    CRect rcValue;
    GetValueColumnRectInCtrl(&rcValue);

    bool bSelectd = m_pIPropertyCtrlButtonItem->IsSelected();
    if (!bSelectd)
    {
        DRAWTEXTPARAM param;
        param.color = 0;
        param.nFormatFlag = DT_LEFT|DT_VCENTER|DT_END_ELLIPSIS|DT_SINGLELINE;
        rcValue.left += TEXT_INDENT;
        param.prc = &rcValue;
        param.szText = m_strValue.c_str();
        pRenderTarget->DrawString(m_pBaseShareData->m_pRenderFont, &param);
    }
}

void  PropertyCtrlButtonItem::OnSize(UINT nType, int cx, int cy)
{
    if (m_pIPropertyCtrlButtonItem->IsSelected() && m_pBaseShareData->pEditCtrl->IsSelfVisible())
    {
        m_pBaseShareData->pEditCtrl->SetConfigLeft(m_pBaseShareData->GetSplitterLinePos() + 1);
    }
    SetMsgHandled(FALSE);
}

void  PropertyCtrlButtonItem::SetValueText(const TCHAR* szText)
{
    if (szText)
        m_strValue = szText;
    else
        m_strValue.clear();

//     if (m_pIPropertyCtrlButtonItem->IsSelected() && m_pBaseShareData->pEditCtrl->IsSelfVisible())
//     {
//         m_pBaseShareData->pEditCtrl->SetText(m_strValue.c_str());
//     }
}

void  PropertyCtrlButtonItem::SetDefaultValueText(const TCHAR* szText)
{
	if (szText)
		m_strDefautValue = szText;
	else
		m_strDefautValue.clear();
}


LRESULT  PropertyCtrlButtonItem::OnStateChanged(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (wParam & OSB_SELECTED)
    {
        bool bSelected = m_pIPropertyCtrlButtonItem->IsSelected();

        if (bSelected)
        {
            BeginEdit();
        }
        else
        {
            if (0 != _tcscmp(m_pBaseShareData->pEditCtrl->GetText(), m_strValue.c_str()))
            {
            //    OnEditReturn(0,0);
            }
            CancelEdit();
        }
    }
    return 0;
}
LRESULT  PropertyCtrlButtonItem::OnSetValue(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	const TCHAR* szText = (const TCHAR* )wParam;
	this->SetValueText(szText);

	return 0;
}
LRESULT  PropertyCtrlButtonItem::OnSetDefaultValue(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	const TCHAR* szText = (const TCHAR* )wParam;
	this->SetDefaultValueText(szText);

	return 0;
}

void  PropertyCtrlButtonItem::CancelEdit()
{
    if (m_pBaseShareData->pButtonCtrl)
	{
		m_pBaseShareData->pButtonCtrl->SetVisible(false, false, false);
		m_pBaseShareData->pButtonCtrl->SetNotify(NULL, 0);
		m_pBaseShareData->pButtonCtrl->RemoveMeInTheTree();
	}
	if (m_pBaseShareData->pEditCtrl)
	{
		m_pBaseShareData->pEditCtrl->SetVisible(false, false, false);
		m_pBaseShareData->pEditCtrl->SetNotify(NULL, 0);
		m_pBaseShareData->pEditCtrl->RemoveMeInTheTree();
		m_pBaseShareData->pEditCtrl->SetText(NULL, false);
	}
}
void  PropertyCtrlButtonItem::BeginEdit()
{
    CRect rc;
    GetValueColumnRectInItem(&rc);
    rc.DeflateRect(2,1,1,1);

	IPanel* pPanel = m_pIPropertyCtrlButtonItem->GetRootPanel();

	if (m_pBaseShareData->pButtonCtrl)
	{
		pPanel->AddChild(m_pBaseShareData->pButtonCtrl);

		m_pBaseShareData->pButtonCtrl->SetConfigRight(1);
		m_pBaseShareData->pButtonCtrl->SetConfigTop(1);
		m_pBaseShareData->pButtonCtrl->SetConfigBottom(1);
		m_pBaseShareData->pButtonCtrl->SetConfigWidth(20);

		m_pBaseShareData->pButtonCtrl->SetVisible(true, false, false);
		m_pBaseShareData->pButtonCtrl->SetNotify(m_pIPropertyCtrlButtonItem, 0);
	}
	
	if (m_pBaseShareData->pEditCtrl)
	{
		m_pBaseShareData->pEditCtrl->SetText(m_strValue.c_str(), false);
		pPanel->AddChild(m_pBaseShareData->pEditCtrl);

		m_pBaseShareData->pEditCtrl->SetConfigLeft(rc.left);
		m_pBaseShareData->pEditCtrl->SetConfigRight(1);
		m_pBaseShareData->pEditCtrl->SetConfigTop(1);
		m_pBaseShareData->pEditCtrl->SetConfigBottom(1);

		m_pBaseShareData->pEditCtrl->SetVisible(true, false, false);
		m_pBaseShareData->pEditCtrl->SetNotify(m_pIPropertyCtrlButtonItem, 0);
	}

    pPanel->GetLayout()->Arrange(NULL, true);

    // ����Edit���㣬���������༭ģʽ
    IListCtrlBase* pCtrl = m_pIPropertyCtrlButtonItem->GetIListCtrlBase();
    pCtrl->SetFocusObject(m_pBaseShareData->pButtonCtrl);

    // ���������ʽ���п����������λ��edit��
    // Ȼ�����EDIT�еĹ��λ��Ϊ����µ�λ��
    HWND  hWnd = pCtrl->GetHWND(); 
    POINT pt;
    GetCursorPos(&pt);
    MapWindowPoints(NULL, hWnd, &pt, 1);
    PostMessage(hWnd, WM_MOUSEMOVE, 0, MAKELPARAM(pt.x, pt.y));
    PostMessage(hWnd, WM_LBUTTONDOWN, 0, MAKELPARAM(pt.x, pt.y));
    PostMessage(hWnd, WM_LBUTTONUP, 0, MAKELPARAM(pt.x, pt.y));
}

}