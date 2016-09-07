#include "stdafx.h"
#include "PropertyCtrlComboBoxItem.h"
#include "UISDK\Kernel\Inc\Interface\ipanel.h"
#include "UISDK\Kernel\Inc\Interface\ilayout.h"
#include "UISDK\Control\Inc\Interface\ilistbox.h"
#include "UISDK\Control\Inc\Interface\iedit.h"

using namespace UI;

PropertyCtrlComboBoxItem::PropertyCtrlComboBoxItem()
{
    m_pIPropertyCtrlComboBoxItem = NULL;
    m_pBaseShareData = NULL;
    m_eItemValueTpye = ComboBoxItemValueString;

    m_lValue = m_lDefualtValue = 0;
	m_bReadOnly = false;
    m_bMultiSel = false;
}

PropertyCtrlComboBoxItem::~PropertyCtrlComboBoxItem()
{
    // 被删除时可能控件是失焦状态，自己的选中状态也被清除了
    // if (m_pIPropertyCtrlComboBoxItem->IsSelected())

    if (IsComboBoxActive())
    {
        m_pBaseShareData->pComboBoxCtrl->SetVisible(false, false, false);
        m_pBaseShareData->pComboBoxCtrl->SetNotify(NULL, 0);
        m_pBaseShareData->pComboBoxCtrl->RemoveMeInTheTree();
		m_pBaseShareData->pComboBoxCtrl->ResetContent();
    }

	m_mapItems.clear();
}

void  PropertyCtrlComboBoxItem::OnInitialize()
{
    DO_PARENT_PROCESS(IPropertyCtrlComboBoxItem, IPropertyCtrlItemBase);
	m_pIPropertyCtrlComboBoxItem->SetItemType(IPropertyCtrlComboBoxItem::FLAG);
}

void  PropertyCtrlComboBoxItem::OnPaint(IRenderTarget* pRenderTarget)
{
    DO_PARENT_PROCESS(IPropertyCtrlComboBoxItem, IPropertyCtrlItemBase);

    CRect rcValue;
    GetValueColumnRectInCtrl(&rcValue);

    // 选中状态
    bool bSelectd = m_pIPropertyCtrlComboBoxItem->IsSelected();
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

void  PropertyCtrlComboBoxItem::OnSize(UINT nType, int cx, int cy)
{
    SetMsgHandled(FALSE);

    if (IsComboBoxActive() && m_pBaseShareData->pComboBoxCtrl->IsSelfVisible())
    {
        m_pBaseShareData->pComboBoxCtrl->SetConfigLeft(m_pBaseShareData->GetSplitterLinePos() + 1);
    }
}

void  PropertyCtrlComboBoxItem::SetValueString(LPCTSTR szText)
{
    UIASSERT(m_eItemValueTpye == ComboBoxItemValueString);

    if (szText)
        m_strValue = szText;
    else
        m_strValue.clear();

    UpdateValue2Combo();
}

void  PropertyCtrlComboBoxItem::SetValueLong(long lValue)
{
    UIASSERT(m_eItemValueTpye == ComboBoxItemValueLong);
    m_lValue = lValue;

    if (m_bMultiSel)
    {
        m_strValue.clear();

        _ListIter iter = m_mapItems.begin();
        for (; iter != m_mapItems.end(); ++iter)
        {
            if (iter->lValue & lValue)
            {
                if (!m_strValue.empty())
                    m_strValue.push_back(XML_FLAG_SEPARATOR);

                m_strValue.append(iter->strText);
            }        
        }
    }
    else
    {
        OptionItem* pItem = FindOption(lValue);
        if (pItem)
        {
            m_strValue = pItem->strText;
        }
        else
        {
            TCHAR szText[16] = {0};
            _stprintf(szText, TEXT("%d"), lValue);
            m_strValue = szText;
        }
    }

    UpdateValue2Combo();
}

void  PropertyCtrlComboBoxItem::UpdateValue2Combo()
{
    if (!IsComboBoxActive())
        return;
    
    m_pBaseShareData->pComboBoxCtrl->SetText(m_strValue.c_str(), true);
}

LRESULT  PropertyCtrlComboBoxItem::OnStateChanged(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (wParam & OSB_SELECTED)
    {
        bool bSelected = m_pIPropertyCtrlComboBoxItem->IsSelected();

        if (bSelected)
        {
            BeginEdit();
        }
        else
        {
            if (0 != _tcscmp(
                    m_pBaseShareData->pComboBoxCtrl->GetText(),
                    m_strValue.c_str()))
            {
				AcceptEdit();
            }
            CancelEdit();
        }
    }
    return 0;
}
LRESULT  PropertyCtrlComboBoxItem::OnSetValue(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LPCTSTR szText = (LPCTSTR )wParam;
	this->SetValueString(szText);

	return 0;
}

LRESULT  PropertyCtrlComboBoxItem::OnComboBoxSelChanged(WPARAM wParam, LPARAM lParam)
{
    if (m_bMultiSel)
    {
    }
    else
    {
	    AcceptEdit();
    }
	return 0;
}

void  PropertyCtrlComboBoxItem::OnMultiSelValueChanged()
{
    IListBox* pListBox = m_pBaseShareData->pComboBoxCtrl->GetListBox();
    if (!pListBox)
        return;
    
    m_strValue.clear();
    m_lValue = 0;

    IListItemBase* pItem = pListBox->GetFirstItem();
    while (pItem)
    {
        if (pItem->IsChecked())
        {
            if (!m_strValue.empty())
                m_strValue.push_back(XML_FLAG_SEPARATOR);

            m_strValue.append(pItem->GetText());

            m_lValue |= pItem->GetData();
        }
        pItem = pItem->GetNextItem();
    }

    FireValueChanged();
    m_pBaseShareData->pComboBoxCtrl->SetText(m_strValue.c_str(), true);
}

void  PropertyCtrlComboBoxItem::AcceptEdit()
{
    SETSTRING (m_strValue, m_pBaseShareData->pComboBoxCtrl->GetText());
	m_lValue = m_lDefualtValue;

	IListBox* pListBox = m_pBaseShareData->pComboBoxCtrl->GetListBox();
	if (pListBox)
	{
		IListItemBase* pSelItem = pListBox->GetFirstSelectItem();
		if (pSelItem)
			m_lValue = (long)pSelItem->GetData();
	}

    FireValueChanged();
}

LRESULT  PropertyCtrlComboBoxItem::OnEditKeyDown(WPARAM w, LPARAM l)
{
	if (w == VK_RETURN)
	{
		AcceptEdit();
		return UI::HANDLED;
	}
	else if (w == VK_ESCAPE)
	{
		m_pBaseShareData->pComboBoxCtrl->SetText(m_strValue.c_str(), true);
		return UI::HANDLED;
	}

	return UI::NOT_HANDLED;
}

void  PropertyCtrlComboBoxItem::FireValueChanged()
{
    UIMSG  msg;
    msg.message = UI_WM_NOTIFY;
	msg.nCode = UI_PROPERTYCTRL_NM_COMBOBOXITEM_ACCEPTCONTENT;

    PROPERTYCTRL_COMBOBOX_ITEM_ACCEPTCONTENT  param = {0};
    param.pItem = m_pIPropertyCtrlComboBoxItem;
    param.szKey = m_strKey.c_str();
    param.lNewValue = m_lValue;
    param.szNewValue = m_strValue.c_str();

    msg.wParam = (WPARAM)&param;
    m_pIPropertyCtrlComboBoxItem->GetIListCtrlBase()->DoNotify(&msg);
}

void  PropertyCtrlComboBoxItem::CancelEdit()
{
    if (NULL == m_pBaseShareData->pComboBoxCtrl)
        return;

    m_pBaseShareData->pComboBoxCtrl->SetVisible(false, false, false);
    m_pBaseShareData->pComboBoxCtrl->SetNotify(NULL, 0);
    m_pBaseShareData->pComboBoxCtrl->RemoveMeInTheTree();
    m_pBaseShareData->pComboBoxCtrl->SetText(NULL, false);
	m_pBaseShareData->pComboBoxCtrl->ResetContent();
}

void  PropertyCtrlComboBoxItem::BeginEdit()
{
    if (NULL == m_pBaseShareData->pComboBoxCtrl)
        return;

    IListBox* pListBox = m_pBaseShareData->pComboBoxCtrl->GetListBox();
	IEdit* pEdit = m_pBaseShareData->pComboBoxCtrl->GetEdit();
    if (!pListBox || !pEdit)
        return;

    CRect rc;
    GetValueColumnRectInItem(&rc);

	// combo padding，将绘制内容/edit位置缩进一些
	CRegion4 rPadding(TEXT_INDENT,1,0,1);
	m_pBaseShareData->pComboBoxCtrl->SetPaddingRegion(&rPadding);

	// 将listitem内容缩进一些
	CRegion4 rListItemIndent(TEXT_INDENT, 0, 0, 0);
	pListBox->SetItemContentPadding(&rListItemIndent);

    IPanel* pPanel = m_pIPropertyCtrlComboBoxItem->GetRootPanel();
    pPanel->AddChild(m_pBaseShareData->pComboBoxCtrl);

	pListBox->RemoveAllItem(0);
    m_pBaseShareData->pComboBoxCtrl->SetText(m_strValue.c_str(), false);

	_ListIter iter = m_mapItems.begin();
	for (; iter != m_mapItems.end(); iter++)
	{
		OptionItem& item = *iter;

        IListBoxItem* pListItem = NULL;
        if (m_bMultiSel)
        {
            IMultiSelComboItem* pMultiSelComboItem = NULL;
            IMultiSelComboItem::CreateInstance(pListBox->GetSkinRes(), &pMultiSelComboItem);
            pListBox->AddItem(pMultiSelComboItem, 0);
            pMultiSelComboItem->SetText(item.strText.c_str());
            pMultiSelComboItem->GetImpl()->SetComboBoxItem(this);
            if (m_lValue & item.lValue)
            {
                pMultiSelComboItem->SetChecked(true, false);
            }

            pListItem = pMultiSelComboItem;
        }
        else
        {
            pListItem = pListBox->AddString(item.strText.c_str(), LF_NONE);
            if (item.strText == m_strValue)
            {
                pListBox->SelectItem(pListItem, false);
            }
        }
        pListItem->SetData(item.lValue);
	}

	if (!pListBox->GetFirstSelectItem())
	{
		m_pBaseShareData->pComboBoxCtrl->SetText(m_strValue.c_str(), false);
	}

    m_pBaseShareData->pComboBoxCtrl->SetConfigLeft(rc.left);
    m_pBaseShareData->pComboBoxCtrl->SetConfigRight(0);
    m_pBaseShareData->pComboBoxCtrl->SetConfigTop(1);
    m_pBaseShareData->pComboBoxCtrl->SetConfigBottom(1);

	m_pBaseShareData->pComboBoxCtrl->SetReadOnly(m_bReadOnly);
	if (m_eItemValueTpye==ComboBoxItemValueLong)
		pEdit->SetTextFilterDigit();
	else
		pEdit->ClearTextFilter();

    m_pBaseShareData->pComboBoxCtrl->SetVisible(true, false, false);
    m_pBaseShareData->pComboBoxCtrl->SetNotify(m_pIPropertyCtrlComboBoxItem, 0);
    pPanel->GetLayout()->Arrange(m_pBaseShareData->pComboBoxCtrl, true);

    // 设置Edit焦点，立即启动编辑模式
    IListCtrlBase* pCtrl = m_pIPropertyCtrlComboBoxItem->GetIListCtrlBase();
    pCtrl->SetFocusObject(m_pBaseShareData->pComboBoxCtrl);

    if (!m_bReadOnly)
    {
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
	    else
	    {
		    SendMessage(hWnd, WM_MOUSEMOVE, 0, MAKELPARAM(pt.x, pt.y));
		    SendMessage(hWnd, WM_LBUTTONDOWN, 0, MAKELPARAM(pt.x, pt.y));
		    SendMessage(hWnd, WM_LBUTTONUP, 0, MAKELPARAM(pt.x, pt.y));
	    }

	    if (m_pBaseShareData->pEditCtrl->IsHover())
		    SetCursor(LoadCursor(NULL, IDC_IBEAM));
    }
}

void  PropertyCtrlComboBoxItem::AddOption(LPCTSTR szText, long lValue)
{
	OptionItem item;
	if (szText)
		item.strText = szText;
	item.lValue = lValue;

	if (m_eItemValueTpye == ComboBoxItemValueLong)
	{
		if (lValue == m_lValue && m_strValue.empty() && szText)
		{
			m_strValue = szText;
		}
	}
	m_mapItems.push_back(item);
}

OptionItem*  PropertyCtrlComboBoxItem::FindOption(long lValue)
{
    UIASSERT(m_eItemValueTpye == ComboBoxItemValueLong);

    _ListIter iter = m_mapItems.begin();
    for (; iter != m_mapItems.end(); ++iter)
    {
        if (iter->lValue == lValue)
            return &(*iter);
    }
    return NULL;
}

//////////////////////////////////////////////////////////////////////////

UI_IMPLEMENT_Ixxx_INTERFACE_ACROSSMODULE(IMultiSelComboItem, MultiSelComboItem, IListBoxItem);

MultiSelComboItem::MultiSelComboItem()
{
    m_pIMultiSelComboItem = NULL;
    m_pBindComboItem = NULL;
}
MultiSelComboItem::~MultiSelComboItem()
{

}
void  MultiSelComboItem::OnPaint(IRenderTarget* pRenderTarget)
{
    IListCtrlBase* pListCtrl = m_pIMultiSelComboItem->GetIListCtrlBase();

    // 缩进文本，用于绘制check图标
    REGION4 rOldPadding = {0};
    pListCtrl->GetItemContentPadding(&rOldPadding);
    REGION4 rNewPadding = rOldPadding;
    rNewPadding.left += 20;
    pListCtrl->SetItemContentPadding(&rNewPadding);

    DO_PARENT_PROCESS(IMultiSelComboItem, IListBoxItem);

    pListCtrl->SetItemContentPadding(&rOldPadding);

    if (m_pIMultiSelComboItem->IsChecked())
    {
        CRect rc;
        m_pIMultiSelComboItem->GetParentRect(&rc);
        rc.left += 5;
        rc.right = rc.left + 10;
        rc.top += 5;
        rc.bottom = rc.top + 10;

        if (m_pIMultiSelComboItem->IsHover())
        {
            Color c(0xFFFFFFFF);
            pRenderTarget->DrawRect(&rc, &c);
        }
        else
        {
            Color c(0xFF000000);
            pRenderTarget->DrawRect(&rc, &c);
        }
    }
}

LRESULT  MultiSelComboItem::OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    m_pIMultiSelComboItem->SetChecked(!m_pIMultiSelComboItem->IsChecked(), false);

    if (m_pBindComboItem)
    {
        m_pBindComboItem->OnMultiSelValueChanged();
    }
    return IMKER_HANDLED_BY_ITEM|IMKER_DONT_CHANGE_SELECTION;
}