#include "stdafx.h"
#include "propertyctrlbuttonitem.h"
#include "UISDK\Kernel\Inc\Interface\ipanel.h"
#include "UISDK\Kernel\Inc\Interface\ilayout.h"

namespace UI
{
#pragma region // share data
PropertyCtrlButtonItemShareData::PropertyCtrlButtonItemShareData()
{
    m_pIPropertyCtrlButtonItemShareData = NULL;
    pSlitterLinePos = NULL;
    pRenderFont = NULL;
    pButtonCtrl = NULL;
	pEditCtrl = NULL;
}
PropertyCtrlButtonItemShareData::~PropertyCtrlButtonItemShareData()
{
}
#pragma endregion 

//////////////////////////////////////////////////////////////////////////

PropertyCtrlButtonItem::PropertyCtrlButtonItem()
{
    m_pIPropertyCtrlButtonItem = NULL;
    m_pShareData = NULL;
}
PropertyCtrlButtonItem::~PropertyCtrlButtonItem()
{
    // 被删除时可能控件是失焦状态，自己的选中状态也被清除了
    // if (m_pIPropertyCtrlButtonItem->IsSelected())

    IObject* pObj = m_pShareData->pButtonCtrl->GetParentObject();
    if (pObj && pObj == m_pIPropertyCtrlButtonItem->GetRootPanel())
    {
        m_pShareData->pButtonCtrl->SetVisible(false, false, false);
        m_pShareData->pButtonCtrl->SetNotify(NULL, 0);
        m_pShareData->pButtonCtrl->RemoveMeInTheTree();
    }

	pObj = m_pShareData->pEditCtrl->GetParentObject();
	if (pObj && pObj == m_pIPropertyCtrlButtonItem->GetRootPanel())
	{
		m_pShareData->pEditCtrl->SetVisible(false, false, false);
		m_pShareData->pEditCtrl->SetNotify(NULL, 0);
		m_pShareData->pEditCtrl->RemoveMeInTheTree();
	}
}

void  PropertyCtrlButtonItem::OnInitialize()
{
    DO_PARENT_PROCESS(IPropertyCtrlButtonItem, IListItemBase);

    m_pIPropertyCtrlButtonItem->SetItemType(LISTITEM_TYPE_PropertyCtrlEdit);

    IListCtrlBase*  pListCtrl = m_pIPropertyCtrlButtonItem->GetIListCtrlBase();
    IPropertyCtrlButtonItemShareData*  pData = static_cast<IPropertyCtrlButtonItemShareData*>(
        pListCtrl->GetItemTypeShareData(LISTITEM_TYPE_PropertyCtrlEdit));

    if (!pData)
    {
        IPropertyCtrlButtonItemShareData::CreateInstance(pListCtrl->GetUIApplication(), &pData);
        m_pShareData = pData->GetImpl();
        pListCtrl->SetItemTypeShareData(LISTITEM_TYPE_PropertyCtrlEdit, pData);
        m_pShareData->pSlitterLinePos = (UINT*)UISendMessage(pListCtrl, UI_PROPERTYCTRL_MSG_GETSPLITTERLINEPOS_PTR);
        m_pShareData->pButtonCtrl = (IButton*)UISendMessage(pListCtrl, UI_PROPERTYCTRL_MSG_GETBUTTONCTRL);
		m_pShareData->pEditCtrl = (IEdit*)UISendMessage(pListCtrl, UI_PROPERTYCTRL_MSG_GETEDITCTRL);

        ITextRenderBase* pTextRender = pListCtrl->GetTextRender();
        if (pTextRender)
            m_pShareData->pRenderFont = pTextRender->GetRenderFont();
    }
    else
    {
        m_pShareData = pData->GetImpl();
    }

    m_pIPropertyCtrlButtonItem->CreateRootPanel();
}


void  PropertyCtrlButtonItem::OnPaint(IRenderTarget* pRenderTarget)
{
    IListCtrlBase*  pListCtrl = m_pIPropertyCtrlButtonItem->GetIListCtrlBase();

    CRect rcParent;
    m_pIPropertyCtrlButtonItem->GetParentRect(&rcParent);

    CRect rcBlank(rcParent);
    rcBlank.right = pListCtrl->GetChildNodeIndent()* m_pIPropertyCtrlButtonItem->GetDepth();

    CRect rcLeft(rcParent);
    rcLeft.left = rcBlank.right;
    rcLeft.right = *(m_pShareData->pSlitterLinePos);

    CRect rcRight(rcParent);
    rcRight.left = rcLeft.right;

    // 左侧背景
    Color c(PROPERTYCTRL_LINE_COLOR);
    pRenderTarget->FillRect(&rcBlank, &c);

    // 中间的分隔线
    IRenderPen* pPen = pRenderTarget->CreateSolidPen(1, &c);
    pRenderTarget->DrawLine(rcLeft.right, rcLeft.top, rcLeft.right, rcLeft.bottom, pPen);

    // 一行的横线
//     IListItemBase* pNextItem = m_pIPropertyCtrlButtonItem->GetNextVisibleItem();
//     if (NULL == pNextItem || 
//         (pNextItem && pNextItem->GetDepth() == m_pIPropertyCtrlButtonItem->GetDepth()))
    {
        pRenderTarget->DrawLine(rcParent.left, rcParent.bottom-1, rcParent.right, rcParent.bottom-1, pPen);
    }
    SAFE_RELEASE(pPen);

    // 选中状态
    bool bSelectd = m_pIPropertyCtrlButtonItem->IsSelected();
    bool bFocus = pListCtrl->IsFocus();

    if (bSelectd)
    {
        if (bFocus)
        {
            Color c(10,36,106,255);
            pRenderTarget->FillRect(&rcLeft, &c);
        }
        else
        {
            pRenderTarget->FillRect(&rcLeft, &c);
        }
    }

    // 绘制文本
    if (m_pShareData->pRenderFont)
    {
        DRAWTEXTPARAM param;
        if (bSelectd && bFocus)
            param.color = 0xFFFFFFFF;
        else
            param.color = 0;

        param.nFormatFlag = DT_LEFT|DT_VCENTER|DT_END_ELLIPSIS|DT_SINGLELINE;
        rcLeft.left += 2;
        param.prc = &rcLeft;
        param.szText = m_pIPropertyCtrlButtonItem->GetText();
        pRenderTarget->DrawString(m_pShareData->pRenderFont, &param);

        if (!bSelectd)
        {
            DRAWTEXTPARAM param;
            param.color = 0;
            param.nFormatFlag = DT_LEFT|DT_VCENTER|DT_END_ELLIPSIS|DT_SINGLELINE;
            rcRight.left += 2;
            param.prc = &rcRight;
            param.szText = m_strValue.c_str();
            pRenderTarget->DrawString(m_pShareData->pRenderFont, &param);
        }
    }
}

void  PropertyCtrlButtonItem::OnSize(UINT nType, int cx, int cy)
{
    if (m_pIPropertyCtrlButtonItem->IsSelected() && m_pShareData->pEditCtrl->IsMySelfVisible())
    {
        m_pShareData->pEditCtrl->SetConfigLeft((*m_pShareData->pSlitterLinePos) + 1);

    }
    SetMsgHandled(FALSE);
}

void  PropertyCtrlButtonItem::SetValueText(const TCHAR* szText)
{
    if (szText)
        m_strValue = szText;
    else
        m_strValue.clear();

//     if (m_pIPropertyCtrlButtonItem->IsSelected() && m_pShareData->pEditCtrl->IsMySelfVisible())
//     {
//         m_pShareData->pEditCtrl->SetText(m_strValue.c_str());
//     }
}

void  PropertyCtrlButtonItem::SetDefaultValueText(const TCHAR* szText)
{
	if (szText)
		m_strDefautValue = szText;
	else
		m_strDefautValue.clear();
}

void  PropertyCtrlButtonItem::SetKeyText(const TCHAR* szText)
{
	if (szText)
		m_strKey = szText;
	else
		m_strKey.clear();
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
            if (0 != _tcscmp(m_pShareData->pEditCtrl->GetText(), m_strValue.c_str()))
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

LRESULT  PropertyCtrlButtonItem::OnGetKey(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return (LRESULT)m_strKey.c_str();
}

// LRESULT  PropertyCtrlButtonItem::OnEditEsc(WPARAM w, LPARAM l)
// {
//     m_pShareData->pEditCtrl->SetText(m_strValue.c_str());
//     m_pShareData->pEditCtrl->SetSel(0, -1);
//     return 0;
// }
// LRESULT  PropertyCtrlButtonItem::OnEditReturn(WPARAM w, LPARAM l)
// {   
//     UIMSG  msg;
//     msg.message = UI_PROPERTYCTRL_MSG_ButtonItem_ACCEPTCONTENT;
// 
// 	PROPERTYCTRL_EDIT_ITEM_ACCEPTCONTENT  param = {0};
// 	param.pItem = m_pIPropertyCtrlButtonItem;
// 	param.szKey = m_strKey.c_str();
// 	param.szNewValue = m_pShareData->pEditCtrl->GetText();
// 	if (0 == _tcslen(param.szNewValue) || m_strDefautValue == param.szNewValue)
// 		param.bDefault = true;
// 	msg.wParam = (WPARAM)&param;
// 
//     m_pIPropertyCtrlButtonItem->GetIListCtrlBase()->DoNotify(&msg);
// 
//     return 0;
// }
// 
void  PropertyCtrlButtonItem::GetValueColumnRect(CRect* prc)
{
    if (NULL == prc)
        return;

    CRect rcParent;
    m_pIPropertyCtrlButtonItem->GetParentRect(&rcParent);

    prc->SetRectEmpty();
    prc->right = rcParent.Width();
    prc->bottom = rcParent.Height();
    prc->left = *(m_pShareData->pSlitterLinePos);
}

// 
// void PropertyCtrlButtonItem::AcceptEditContent()
// {
//     if (NULL == m_pEditingItem)
//         return;
// 
//     ListViewSubItemBase* pSubItem = this->GetSubItem((ListTreeViewItem*)m_pEditingItem, PROP_VALUE_COL);
//     if (NULL != pSubItem)
//     {
//         int nLength = m_pEdit->GetTextLength();
//         TCHAR* pszText = new TCHAR[nLength+1];
//         m_pEdit->GetText(pszText, nLength+1);
//         pSubItem->SetText(pszText);
//         SAFE_ARRAY_DELETE(pszText);
//     }
// }
// void PropertyCtrlButtonItem::RejestEditContent()
// {
//     if (NULL == m_pEditingItem)
//         return;
// 
//     ListViewSubItemBase* pSubItem = this->GetSubItem((ListTreeViewItem*)m_pEditingItem, PROP_VALUE_COL);
//     if (NULL != pSubItem)
//     {
//         m_pEdit->SetText(pSubItem->GetText());
//     }
// }

void  PropertyCtrlButtonItem::CancelEdit()
{
    if (m_pShareData->pButtonCtrl)
	{
		m_pShareData->pButtonCtrl->SetVisible(false, false, false);
		m_pShareData->pButtonCtrl->SetNotify(NULL, 0);
		m_pShareData->pButtonCtrl->RemoveMeInTheTree();
	}
	if (m_pShareData->pEditCtrl)
	{
		m_pShareData->pEditCtrl->SetVisible(false, false, false);
		m_pShareData->pEditCtrl->SetNotify(NULL, 0);
		m_pShareData->pEditCtrl->RemoveMeInTheTree();
		m_pShareData->pEditCtrl->SetText(NULL);
	}
}
void  PropertyCtrlButtonItem::BeginEdit()
{
    CRect rc;
    GetValueColumnRect(&rc);
    rc.DeflateRect(2,1,1,1);

	IPanel* pPanel = m_pIPropertyCtrlButtonItem->GetRootPanel();

	if (m_pShareData->pButtonCtrl)
	{
		pPanel->AddChild(m_pShareData->pButtonCtrl);

		m_pShareData->pButtonCtrl->SetConfigRight(1);
		m_pShareData->pButtonCtrl->SetConfigTop(1);
		m_pShareData->pButtonCtrl->SetConfigBottom(1);
		m_pShareData->pButtonCtrl->SetConfigWidth(20);

		m_pShareData->pButtonCtrl->SetVisible(true, false, false);
		m_pShareData->pButtonCtrl->SetNotify(m_pIPropertyCtrlButtonItem, 0);
	}
	
	if (m_pShareData->pEditCtrl)
	{
		m_pShareData->pEditCtrl->SetText(m_strValue.c_str());
		pPanel->AddChild(m_pShareData->pEditCtrl);

		m_pShareData->pEditCtrl->SetConfigLeft(rc.left);
		m_pShareData->pEditCtrl->SetConfigRight(1);
		m_pShareData->pEditCtrl->SetConfigTop(1);
		m_pShareData->pEditCtrl->SetConfigBottom(1);

		m_pShareData->pEditCtrl->SetVisible(true, false, false);
		m_pShareData->pEditCtrl->SetNotify(m_pIPropertyCtrlButtonItem, 0);
	}

    pPanel->GetLayout()->Arrange(NULL, true);

    // 设置Edit焦点，立即启动编辑模式
    IListCtrlBase* pCtrl = m_pIPropertyCtrlButtonItem->GetIListCtrlBase();
    pCtrl->SetFocusObject(m_pShareData->pButtonCtrl);

    // 更新鼠标样式，有可能鼠标现在位于edit上
    // 然后更新EDIT中的光标位置为鼠标下的位置
    HWND  hWnd = pCtrl->GetHWND(); 
    POINT pt;
    GetCursorPos(&pt);
    MapWindowPoints(NULL, hWnd, &pt, 1);
    PostMessage(hWnd, WM_MOUSEMOVE, 0, MAKELPARAM(pt.x, pt.y));
    PostMessage(hWnd, WM_LBUTTONDOWN, 0, MAKELPARAM(pt.x, pt.y));
    PostMessage(hWnd, WM_LBUTTONUP, 0, MAKELPARAM(pt.x, pt.y));
}

}