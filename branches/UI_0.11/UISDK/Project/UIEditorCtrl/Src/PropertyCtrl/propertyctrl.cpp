#include "stdafx.h"
#include "UISDK\Project\UIEditorCtrl\Src\PropertyCtrl\propertyctrl.h"
#include "UISDK\Kernel\Inc\Interface\iscrollbarmanager.h"
#include "UISDK\Control\Inc\Interface\iedit.h"
#include "UISDK\Control\Inc\Interface\ibutton.h"
#include "UISDK\Control\Inc\Interface\icombobox.h"
#include "UISDK\Kernel\Inc\Interface\icolorrender.h"
#include "item\propertyctrllongitem.h"
#include "item\propertyctrlboolitem.h"
#include "item\propertyctrledititem.h"
#include "item\propertyctrlbuttonitem.h"

#define PROPERTY_EDIT_PREFIX       _T("edit.")
#define PROPERTY_COMBOBOX_PREFIX   _T("combobox.")
#define PROPERTY_BUTTON_PREFIX     _T("button.")

#define PROPERTY_EDIT_ID   TEXT("__edit")
#define PROPERTY_BUTTON_ID   TEXT("__button")
#define PROPERTY_COMBOBOX_ID   TEXT("__combobox")

using namespace UI;

PropertyCtrl::PropertyCtrl()
{
    m_nSplitterLinePercent = 512;
    m_nSplitterLinePos = 100;

    m_pEdit = NULL;
    m_pButton = NULL;
    m_pComboBox = NULL;
	m_pAdditionItem = NULL;
}
PropertyCtrl::~PropertyCtrl()
{
    
}

HRESULT  PropertyCtrl::FinalConstruct(IUIApplication* p)
{
    DO_PARENT_PROCESS(IPropertyCtrl, ITreeView);

    ObjStyle s = {0};
    s.enable_ime = 1;
    m_pIPropertyCtrl->ModifyObjectStyle(&s, 0);

#if 0
    if (!m_pEdit)
    {
        IEdit::CreateInstance(p, &m_pEdit);
        m_pEdit->SetVisible(false, false, false);
        m_pEdit->SetOutRef((void**)&m_pEdit);
        // m_pIPropertyCtrl->AddChild(m_pEdit); 不做为控件的子对象，而是item的子对象，要不然刷新不好处理
        
        IRenderBase* pBkRender = NULL;
        p->CreateRenderBase(RENDER_TYPE_COLOR, m_pEdit, &pBkRender);
        if (pBkRender)
        {
            IColorRender* pColorRender = (IColorRender*)pBkRender->QueryInterface(uiiidof(IColorRender));
            if (pColorRender)
                pColorRender->SetBkColor(0xFFFFFFFF);

            m_pEdit->SetBkgndRender(pBkRender);
            SAFE_RELEASE(pBkRender);
        }
    }
	if (!m_pComboBox)
	{
		IComboBox::CreateInstance(p, &m_pComboBox);
		m_pComboBox->SetVisible(false, false, false);
		m_pComboBox->SetOutRef((void**)&m_pComboBox);
        if (m_pComboBox->GetEdit())
        {
            m_pComboBox->GetEdit()->SetReadOnly(true, false); // 仅Edit只读，仅借用可写的combobox背景样式，否则连按钮也不显示了
        }

		IRenderBase* pBkRender = NULL;
		p->CreateRenderBase(RENDER_TYPE_COLOR, m_pComboBox, &pBkRender);
		if (pBkRender)
		{
			IColorRender* pColorRender = (IColorRender*)pBkRender->QueryInterface(uiiidof(IColorRender));
			if (pColorRender)
				pColorRender->SetBkColor(0xFFFFFFFF);

			m_pComboBox->SetBkgndRender(pBkRender);
			SAFE_RELEASE(pBkRender);
		}
	}
	if (!m_pButton)
	{
		IButton::CreateInstance(p, &m_pButton);
		m_pButton->SetVisible(false, false, false);
		m_pButton->SetOutRef((void**)&m_pButton);
	}
#endif

    return S_OK;
}

void  PropertyCtrl::OnInitialize()
{
	DO_PARENT_PROCESS(IPropertyCtrl, ITreeView);

	IUIApplication* pUIApp = m_pIPropertyCtrl->GetUIApplication();
	IObject* pObj = NULL;
	
	pObj = m_pIPropertyCtrl->FindChildObject(PROPERTY_EDIT_ID);
	m_pEdit = (IEdit*)pObj->QueryInterface(uiiidof(IEdit));

	pObj = m_pIPropertyCtrl->FindChildObject(PROPERTY_BUTTON_ID);
	m_pButton = (IButton*)pObj->QueryInterface(uiiidof(IButton));

	pObj = m_pIPropertyCtrl->FindChildObject(PROPERTY_COMBOBOX_ID);
	m_pComboBox = (IComboBox*)pObj->QueryInterface(uiiidof(IComboBox));

	if (m_pEdit)
	{
		m_pEdit->SetVisible(false, false, false);
		m_pEdit->SetOutRef((void**)&m_pEdit);
	}

	if (m_pComboBox)
	{
		m_pComboBox->SetVisible(false, false, false);
		m_pComboBox->SetOutRef((void**)&m_pComboBox);
		if (m_pComboBox->GetEdit())
		{
			m_pComboBox->GetEdit()->SetReadOnly(true, false); // 仅Edit只读，仅借用可写的combobox背景样式，否则连按钮也不显示了
		}
	}
	if (m_pButton)
	{
		m_pButton->SetVisible(false, false, false);
		m_pButton->SetOutRef((void**)&m_pButton);
	}
}

void  PropertyCtrl::OnInitialize2()
{
    DO_PARENT_PROCESS(IPropertyCtrl, ITreeView);

    IUIApplication* pUIApp = m_pIPropertyCtrl->GetUIApplication();

    // 不做为控件的子对象，而是item的子对象，要不然刷新不好处理
    // 注：不能直接在OnInitialize 1阶段移除子控件，要不然子控件就
    // 得不到响应WM_INIALIZE的机会了。
    if (!pUIApp->IsDesignMode())
    {
        if (m_pEdit)
            m_pEdit->RemoveMeInTheTree();
        if (m_pButton)
            m_pButton->RemoveMeInTheTree();
        if (m_pComboBox)
            m_pComboBox->RemoveMeInTheTree();
    }
}

void  PropertyCtrl::FinalRelease()
{
	// design模式下，edit为propertyctrl的子对象，会直接销毁
	// 在正常模式下，edit为item的子对象，由控件主动销毁
    DO_PARENT_PROCESS(IPropertyCtrl, ITreeView);
	
	if (!m_pIPropertyCtrl->GetUIApplication()->IsDesignMode())
	{
		SAFE_DELETE_Ixxx(m_pEdit);
		SAFE_DELETE_Ixxx(m_pComboBox);
		SAFE_DELETE_Ixxx(m_pButton);
	}
}

void  PropertyCtrl::OnCreateByEditor(CREATEBYEDITORDATA* pData)
{
	static_cast<ITreeView*>(m_pIPropertyCtrl)->nvProcessMessage(
		GetCurMsg(), UIALT_CALLLESS, false);

	EditorAddObjectResData addResData = {0};
	addResData.pUIApp = pData->pUIApp;
	addResData.pParentObj = m_pIPropertyCtrl;
	addResData.pParentXml = pData->pXml;

	addResData.objiid = uiiidof(IInstantEdit);
	addResData.ppCreateObj = (void**)&m_pEdit;
	pData->pEditor->AddObjectRes(&addResData);

	addResData.objiid = uiiidof(IButton);
	addResData.ppCreateObj = (void**)&m_pButton;
	pData->pEditor->AddObjectRes(&addResData);

	addResData.objiid = uiiidof(IComboBox);
	addResData.ppCreateObj = (void**)&m_pComboBox;
	pData->pEditor->AddObjectRes(&addResData);
}

// void  PropertyCtrl::SetAttribute(IMapAttribute* pMapAttr, bool bReload)
// {
//     DO_PARENT_PROCESS(IPropertyCtrl, ITreeView);
// 
// 	// 为了能够让edit也拿到window的默认字体，这里先将其加入子结点，否则edit将拿到系统字体，
// 	// 而PropertyCtrl则拿到窗口字体，导致显示不一致
//     if (m_pEdit)
//     {
//         m_pEdit->RemoveMeInTheTree();
//         m_pIPropertyCtrl->AddChild(m_pEdit);  
//         m_pEdit->SetAttributeByPrefix(PROPERTY_EDIT_PREFIX, pMapAttr, bReload, true);
//         m_pEdit->RemoveMeInTheTree();
//     }
// 	if (m_pComboBox)
// 	{
// 		m_pComboBox->RemoveMeInTheTree();
// 		m_pIPropertyCtrl->AddChild(m_pComboBox);  
// 		m_pComboBox->SetAttributeByPrefix(PROPERTY_EDIT_PREFIX, pMapAttr, bReload, true);
// 		m_pComboBox->RemoveMeInTheTree();
// 	}
// 	if (m_pButton)
// 	{
// 		m_pButton->RemoveMeInTheTree();
// 		m_pIPropertyCtrl->AddChild(m_pButton);  
// 		m_pButton->SetAttributeByPrefix(PROPERTY_EDIT_PREFIX, pMapAttr, bReload, true);
// 		m_pButton->RemoveMeInTheTree();
// 	}
// }

IPropertyCtrlGroupItem*  PropertyCtrl::FindGroupItem(IListItemBase* pParentItem, LPCTSTR szName)
{
    IListItemBase* pListItem = NULL;
    if (pParentItem)
        pListItem = pParentItem->GetChildItem();
    else
        pListItem = m_pIPropertyCtrl->GetFirstItem();

    while (pListItem)
    {
        LPCTSTR szItemText = pListItem->GetText();
        if (szItemText && 0 == _tcscmp(szItemText, szName))
        {
            IPropertyCtrlGroupItem* pGroupItem = 
                (IPropertyCtrlGroupItem*)pListItem->QueryInterface(
                uiiidof(IPropertyCtrlGroupItem));
            if (pGroupItem)
                return pGroupItem;
        }
        pListItem = pListItem->GetNextItem();
    }

    return NULL;
}

IListItemBase*  PropertyCtrl::FindItemByKey(LPCTSTR szKey)
{
    if (!szKey || !szKey[0])
        return NULL;

    IListItemBase* pListItem = m_pIPropertyCtrl->GetFirstItem();
    while (pListItem)
    {
        LPCTSTR szItemKey = (LPCTSTR)UISendMessage(pListItem,UI_PROPERTYCTRLITEM_MSG_GETKEYSTRING);
        if (szItemKey && 0 == _tcscmp(szItemKey, szKey))
            return pListItem;

        pListItem = pListItem->GetNextTreeItem();
    }
    return NULL;
}

IPropertyCtrlGroupItem*  PropertyCtrl::InsertGroupItem(
        LPCTSTR szName, 
        LPCTSTR szDesc, 
        IListItemBase* pParent, 
        IListItemBase* pInsertAfter, 
        LISTITEM_OPFLAGS nInsertFlags)
{
    IPropertyCtrlGroupItem*  pItem = NULL;
    IPropertyCtrlGroupItem::CreateInstance(m_pIPropertyCtrl->GetSkinRes(), &pItem);

    if (false == m_pIPropertyCtrl->InsertItem(pItem, pParent, pInsertAfter, nInsertFlags))
    {
        SAFE_DELETE_Ixxx(pItem);
        return NULL;
    }

    pItem->SetText(szName);
    pItem->SetToolTip(szDesc);
    return pItem;
}

IPropertyCtrlEditItem*  PropertyCtrl::InsertEditProperty(
        PropertyCtrlEditItemInfo* pInfo,
        IListItemBase* pParentItem, 
        IListItemBase* pInsertAfter, 
        LISTITEM_OPFLAGS nInsertFlags)
{
    if (pParentItem)
    {

    }
    else
    {

    }


    IPropertyCtrlEditItem*  pItem = NULL;
    IPropertyCtrlEditItem::CreateInstance(m_pIPropertyCtrl->GetSkinRes(), &pItem);
    PropertyCtrlEditItem*  pImpl = pItem->GetImpl();

    pItem->SetText(pInfo->szText);
    pItem->SetToolTip(pInfo->szDesc);
    pImpl->SetValueText(pInfo->szValue);
	pImpl->SetKey(pInfo->szKey);

    if (false == m_pIPropertyCtrl->InsertItem(pItem, pParentItem, pInsertAfter, nInsertFlags&~(LF_LAYOUT|LF_REDRAW)))
    {
        SAFE_DELETE_Ixxx(pItem);
        return NULL;
    }

	// 保持addition item一直在最后面
	if (m_pAdditionItem && m_pAdditionItem->GetNextItem())
	{
		m_pIPropertyCtrl->MoveItem(m_pAdditionItem, NULL, UITVI_LAST, 0);
	}

	if (nInsertFlags & LF_LAYOUT)
	{
		m_pIPropertyCtrl->LayoutItem(pItem, false);
	}
	if (nInsertFlags & LF_REDRAW)
	{
		m_pIPropertyCtrl->UpdateObject();
	}
    return pItem;
}

IListItemBase*  PropertyCtrl::FindItem(LPCTSTR szKey)
{
	if (!szKey)
		return NULL;
	IListItemBase* pListItem = m_pIPropertyCtrl->GetFirstItem();
	while (pListItem)
	{
		LPCTSTR szItemKey = (LPCTSTR)UISendMessage(pListItem, UI_PROPERTYCTRLITEM_MSG_GETKEYSTRING);
		if (szItemKey && 0 == _tcscmp(szItemKey, szKey))
		{
			return pListItem;
		}
		pListItem = pListItem->GetNextTreeItem();
	}
	return NULL;
}

IPropertyCtrlEditItem*  PropertyCtrl::FindEditItem(LPCTSTR szKey)
{
	IListItemBase* pListItem = FindItem(szKey);
	if (!pListItem)
		return NULL;

	return (IPropertyCtrlEditItem*)pListItem->QueryInterface(uiiidof(IPropertyCtrlEditItem));
}
IPropertyCtrlLongItem*  PropertyCtrl::FindLongItem(LPCTSTR szKey)
{
    IListItemBase* pListItem = FindItem(szKey);
    if (!pListItem)
        return NULL;

    return (IPropertyCtrlLongItem*)pListItem->QueryInterface(uiiidof(IPropertyCtrlLongItem));
}
IPropertyCtrlComboBoxItem*  PropertyCtrl::FindComboBoxItem(LPCTSTR szKey)
{
    IListItemBase* pListItem = FindItem(szKey);
    if (!pListItem)
        return NULL;

    return (IPropertyCtrlComboBoxItem*)pListItem->QueryInterface(uiiidof(IPropertyCtrlComboBoxItem));
}
IPropertyCtrlBoolItem*  PropertyCtrl::FindBoolItem(LPCTSTR szKey)
{
	IListItemBase* pListItem = FindItem(szKey);
	if (!pListItem)
		return NULL;

	return (IPropertyCtrlBoolItem*)pListItem->QueryInterface(uiiidof(IPropertyCtrlBoolItem));
}

IPropertyCtrlLongItem*  PropertyCtrl::InsertLongProperty(
        PropertyCtrlLongItemInfo* pInfo, 
        IListItemBase* pParentItem, 
        IListItemBase* pInsertAfter, 
        LISTITEM_OPFLAGS nInsertFlags)
{
    IPropertyCtrlLongItem*  pItem = NULL;
    IPropertyCtrlLongItem::CreateInstance(m_pIPropertyCtrl->GetSkinRes(), &pItem);

    if (false == m_pIPropertyCtrl->InsertItem(pItem, pParentItem, pInsertAfter, nInsertFlags))
    {
        SAFE_DELETE_Ixxx(pItem);
        return NULL;
    }

	PropertyCtrlLongItem* pImpl = pItem->GetImpl();

	pItem->SetText(pInfo->szText);
	pItem->SetToolTip(pInfo->szDesc);
	pImpl->SetKey(pInfo->szKey);
	pImpl->SetValue(pInfo->lValue);

    return pItem;
}

IPropertyCtrlBoolItem*  PropertyCtrl::InsertBoolProperty(
        PropertyCtrlBoolItemInfo* pInfo,
        IListItemBase* pParentItem, 
        IListItemBase* pInsertAfter, 
        LISTITEM_OPFLAGS nInsertFlags)
{
    IPropertyCtrlBoolItem*  pItem = NULL;
    IPropertyCtrlBoolItem::CreateInstance(m_pIPropertyCtrl->GetSkinRes(), &pItem);

    if (false == m_pIPropertyCtrl->InsertItem(pItem, pParentItem, pInsertAfter, nInsertFlags))
    {
        SAFE_DELETE_Ixxx(pItem);
        return NULL;
    }

    PropertyCtrlBoolItem* pImpl = pItem->GetImpl();

    pItem->SetText(pInfo->szText);
    pItem->SetToolTip(pInfo->szDesc);
    pImpl->SetKey(pInfo->szKey);
    pImpl->SetBool(pInfo->bValue);

	return pItem;
}

IPropertyCtrlComboBoxItem*  PropertyCtrl::InsertComboBoxProperty(
        PropertyCtrlComboBoxItemInfo* pInfo,
        IListItemBase* pParentItem, 
        IListItemBase* pInsertAfter, 
        LISTITEM_OPFLAGS nInsertFlags)
{
	// TODO: 先查找该key是否已经存在
	if (pParentItem)
	{

	}
	else
	{

	}


	IPropertyCtrlComboBoxItem*  pItem = NULL;
	IPropertyCtrlComboBoxItem::CreateInstance(m_pIPropertyCtrl->GetSkinRes(), &pItem);
   
	if (false == m_pIPropertyCtrl->InsertItem(pItem, pParentItem, pInsertAfter, nInsertFlags))
	{
		SAFE_DELETE_Ixxx(pItem);
		return NULL;
	}

	PropertyCtrlComboBoxItem*  pImpl = pItem->GetImpl();

	pItem->SetText(pInfo->szText);
	pItem->SetToolTip(pInfo->szDesc);
	pImpl->SetKey(pInfo->szKey);
	pImpl->SetValueType(pInfo->eType);
	pImpl->SetReadOnly(pInfo->bReadOnly);
    pImpl->SetMultiSel(pInfo->bMultiSel);

	if (pInfo->eType == ComboBoxItemValueLong)
		pImpl->SetValueLong(pInfo->lValue);
	else if (pInfo->eType == ComboBoxItemValueString)
		pImpl->SetValueString(pInfo->szValue);

	return pItem;
}

IPropertyCtrlButtonItem*   PropertyCtrl::InsertButtonProperty(
        LPCTSTR szText, 
        LPCTSTR szValue, 
        LPCTSTR szDesc, 
        LPCTSTR szKey,
        IListItemBase* pParentItem, 
        IListItemBase* pInsertAfter, 
        LISTITEM_OPFLAGS nInsertFlags)
{
	// TODO: 先查找该key是否已经存在
	if (pParentItem)
	{

	}
	else
	{

	}


	IPropertyCtrlButtonItem*  pItem = NULL;
	IPropertyCtrlButtonItem::CreateInstance(m_pIPropertyCtrl->GetSkinRes(), &pItem);
    PropertyCtrlButtonItem*  pImpl = pItem->GetImpl();

	pItem->SetText(szText);
	pItem->SetToolTip(szDesc);
	pImpl->SetValueText(szValue);
	pImpl->SetKey(szKey);

	if (false == m_pIPropertyCtrl->InsertItem(pItem, pParentItem, pInsertAfter, nInsertFlags))
	{
		SAFE_DELETE_Ixxx(pItem);
		return NULL;
	}

	return pItem;
}

IPropertyCtrlAdditionItem*  PropertyCtrl::AddAdditionItem(LISTITEM_OPFLAGS nFlags)
{
	IPropertyCtrlAdditionItem*  pItem = NULL;
	IPropertyCtrlAdditionItem::CreateInstance(m_pIPropertyCtrl->GetSkinRes(), &pItem);

	if (false == m_pIPropertyCtrl->AddItem(pItem, nFlags))
	{
		SAFE_DELETE_Ixxx(pItem);
		return NULL;
	}

	m_pAdditionItem = pItem;
	m_pAdditionItem->AddDelayRef((void**)&m_pAdditionItem);
	return pItem;
}

void  PropertyCtrl::OnSize(UINT nType, int cx, int cy)
{
    m_nSplitterLinePos = cx * m_nSplitterLinePercent >> 10;
    SetMsgHandled(FALSE);
}

LRESULT  PropertyCtrl::OnGetSplitterLinePosPtr(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return (LRESULT)&m_nSplitterLinePos;
}
LRESULT  PropertyCtrl::OnGetEditCtrl(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return (LRESULT)m_pEdit;
}
LRESULT  PropertyCtrl::OnGetComboBoxCtrl(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return (LRESULT)m_pComboBox;
}
LRESULT  PropertyCtrl::OnGetButtonCtrl(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return (LRESULT)m_pButton;
}
LRESULT  PropertyCtrl::OnPreShowTooltip(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return 1;
}

