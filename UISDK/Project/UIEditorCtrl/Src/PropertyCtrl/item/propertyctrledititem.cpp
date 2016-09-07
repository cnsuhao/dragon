#include "stdafx.h"
#include "propertyctrledititem.h"
#include "UISDK\Kernel\Inc\Interface\ipanel.h"
#include "UISDK\Kernel\Inc\Interface\ilayout.h"
#include "UISDK\Kernel\Inc\Interface\iwindow.h"

namespace UI
{
PropertyCtrlEditItem::PropertyCtrlEditItem()
{
    m_pIPropertyCtrlEditItem = NULL;
    m_pBaseShareData = NULL;
}
PropertyCtrlEditItem::~PropertyCtrlEditItem()
{
    // 被删除时可能控件是失焦状态，自己的选中状态也被清除了
    if (IsEditActive())
    {
        m_pBaseShareData->pEditCtrl->SetVisible(false, false, false);
        m_pBaseShareData->pEditCtrl->SetNotify(NULL, 0);
        m_pBaseShareData->pEditCtrl->RemoveMeInTheTree();
    }
}

void  PropertyCtrlEditItem::OnInitialize()
{
    DO_PARENT_PROCESS(IPropertyCtrlEditItem, IPropertyCtrlItemBase);
	m_pIPropertyCtrlEditItem->SetItemFlag(IPropertyCtrlEditItem::FLAG);
}


void  PropertyCtrlEditItem::OnPaint(IRenderTarget* pRenderTarget)
{
    DO_PARENT_PROCESS(IPropertyCtrlEditItem, IPropertyCtrlItemBase);

    CRect rcValue;
    GetValueColumnRectInCtrl(&rcValue);

    // 绘制文本
    bool bSelectd = m_pIPropertyCtrlEditItem->IsSelected();
    if (!bSelectd || !IsEditActive())
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

void  PropertyCtrlEditItem::OnSize(UINT nType, int cx, int cy)
{
    if (m_pIPropertyCtrlEditItem->IsSelected() && m_pBaseShareData->pEditCtrl->IsSelfVisible())
    {
        m_pBaseShareData->pEditCtrl->SetConfigLeft(m_pBaseShareData->GetSplitterLinePos() + 1);
    }
    SetMsgHandled(FALSE);
}

void  PropertyCtrlEditItem::SetValueText(LPCTSTR szText)
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

void  PropertyCtrlEditItem::SetDefaultValueText(LPCTSTR szText)
{
	if (szText)
		m_strDefautValue = szText;
	else
		m_strDefautValue.clear();
}

void  PropertyCtrlEditItem::OnValueColumnClick(POINT ptWnd, POINT ptItem)
{
	BeginEdit();
}

LRESULT  PropertyCtrlEditItem::OnSetValue(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LPCTSTR szText = (LPCTSTR )wParam;
	this->SetValueText(szText);

	return 0;
}

LRESULT  PropertyCtrlEditItem::OnSetDefaultValue(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LPCTSTR szText = (LPCTSTR )wParam;
	this->SetDefaultValueText(szText);

	return 0;
}

void  PropertyCtrlEditItem::EndEdit()
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

void  PropertyCtrlEditItem::AcceptEdit()
{
    UIMSG  msg;
	msg.message = UI_WM_NOTIFY;
    msg.nCode = UI_PROPERTYCTRL_NM_EDITITEM_ACCEPTCONTENT;

    PROPERTYCTRL_EDIT_ACCEPTCONTENT  param = {0};
    param.pItem = m_pIPropertyCtrlEditItem;
    param.szKey = m_strKey.c_str();
    param.szNewValue = m_pBaseShareData->pEditCtrl->GetText();
    msg.wParam = (WPARAM)&param;

    m_pIPropertyCtrlEditItem->GetIListCtrlBase()->DoNotify(&msg);
}

LRESULT  PropertyCtrlEditItem::OnAcceptEdit(WPARAM wParam, LPARAM lParam)
{
	AcceptEdit();
	return ACCEPT_EDIT_ACCEPT;
}

void  PropertyCtrlEditItem::BeginEdit()
{
    if (NULL == m_pBaseShareData->pEditCtrl)
        return;
	if (IsEditActive())
		return;

    CRect rc;
    GetValueColumnRectInItem(&rc);
    rc.DeflateRect(TEXT_INDENT,1,1,1);

    IPanel* pPanel = m_pIPropertyCtrlEditItem->GetRootPanel();
    pPanel->AddChild(m_pBaseShareData->pEditCtrl);

    m_pBaseShareData->pEditCtrl->SetText(m_strValue.c_str(), false);

    m_pBaseShareData->pEditCtrl->SetConfigLeft(rc.left);
    m_pBaseShareData->pEditCtrl->SetConfigRight(1);
    m_pBaseShareData->pEditCtrl->SetConfigTop(1);
    m_pBaseShareData->pEditCtrl->SetConfigBottom(1);

    m_pBaseShareData->pEditCtrl->SetVisible(true, false, false);
    m_pBaseShareData->pEditCtrl->SetNotify(m_pIPropertyCtrlEditItem, 0);
    pPanel->GetLayout()->Arrange(m_pBaseShareData->pEditCtrl, true);

    // 设置Edit焦点，立即启动编辑模式
    IListCtrlBase* pCtrl = m_pIPropertyCtrlEditItem->GetIListCtrlBase();
    pCtrl->SetFocusObject(m_pBaseShareData->pEditCtrl);

    // 更新鼠标样式，有可能鼠标现在位于edit上
    // 然后更新EDIT中的光标位置为鼠标下的位置
    HWND  hWnd = pCtrl->GetHWND(); 
    POINT pt;
    GetCursorPos(&pt);
    MapWindowPoints(NULL, hWnd, &pt, 1);

    // 此时鼠标还没弹起来，最后不发送up消息。否则不能直接拖选文字了
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