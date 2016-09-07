#include "stdafx.h"
#include "StylePropertyHandler.h"
#include "Dialog\Framework\propertydialog.h"
#include "UISDK\Kernel\Inc\Interface\ixmlwrap.h"
#include "UISDK\Control\Inc\Interface\iedit.h"
#include "Business\Command\style\StyleCommand.h"
#include "Business\uieditorfunction.h"

StylePropertyHandler::StylePropertyHandler()
{
	CreateImpl();

	m_pPropertyDialog = NULL;
	m_pPropertyCtrl = NULL;

	m_pSkinRes = NULL;
	m_pStyleResItem = NULL;
	m_eCurAttrType = XmlAttr;
}
StylePropertyHandler::~StylePropertyHandler()
{
	DetachHandler();
}

void  StylePropertyHandler::OnLoad(CPropertyDialog* pWnd)
{
	m_pPropertyDialog = pWnd;
	m_pPropertyCtrl = m_pPropertyDialog->GetPropertyCtrl();

	m_pRadioBtnXmlAttr = (IRadioButton*)pWnd->FindChildObject(_T("radiobtn_attr1"));
	m_pRadioBtnParsedAttr = (IRadioButton*)pWnd->FindChildObject(_T("radiobtn_attr2"));
	m_pRadioBtnInheritAttr = (IRadioButton*)pWnd->FindChildObject(_T("radiobtn_attr3"));

	if (m_pRadioBtnXmlAttr)
	{
		m_pRadioBtnXmlAttr->SetText(TEXT("Xml"), false);
		m_pRadioBtnXmlAttr->SetToolTipText(TEXT("��ʾ��ʽ��xml�����õ�����"));
		m_pRadioBtnXmlAttr->SetChecked();
	}
	if (m_pRadioBtnParsedAttr)
	{
		m_pRadioBtnParsedAttr->SetText(TEXT("Parse"), false);
		m_pRadioBtnParsedAttr->SetToolTipText(TEXT("����inherit֮�������"));
	}
	if (m_pRadioBtnInheritAttr)
	{
		m_pRadioBtnInheritAttr->SetText(TEXT("Inherit"), false);
		m_pRadioBtnInheritAttr->SetToolTipText(TEXT("��ʾ�Լ������Ժ�inherit������"));
	}
}
void  StylePropertyHandler::OnUnload()
{
	m_pPropertyDialog = NULL;
	m_pPropertyCtrl = NULL;
}

void  StylePropertyHandler::AttachHandler()
{
	g_pGlobalData->m_pPropertyDialog->AttachHandler(static_cast<IPropertyHandler*>(this));
}
void  StylePropertyHandler::DetachHandler()
{
	g_pGlobalData->m_pPropertyDialog->DetachHandler(static_cast<IPropertyHandler*>(this));
}

void  StylePropertyHandler::AttachStyle(ISkinRes* pSkinRes, IStyleResItem* pStyleResItem)
{
	if (!m_pPropertyCtrl)
	{
		AttachHandler();
		if (!m_pPropertyCtrl)
			return;
	}

	m_pPropertyCtrl->RemoveAllItem(0);
	if (!pStyleResItem || !pSkinRes)
		return;

	m_pSkinRes = pSkinRes;
	m_pStyleResItem = pStyleResItem;

	if (m_eCurAttrType == XmlAttr)
	{
		AttachStyle_Xml(pStyleResItem);
	}
	else if (m_eCurAttrType == ParseAttr)
	{
		AttachStyle_Parsed(pStyleResItem);
	}
	else if (m_eCurAttrType == InheritAttr)
	{
		AttachStyle_Inherit(pStyleResItem);
	}
	else
	{
		AttachStyle_Xml(pStyleResItem);
	}

	m_pPropertyCtrl->LayoutAllItemsAndRedraw();
}

// ��xml��ʽ
void  StylePropertyHandler::AttachStyle_Xml(IStyleResItem* pStyleResItem)
{
	IUIElement* pXml = pStyleResItem->GetXmlElement();
	if (!pXml)
		return;

	IListAttribute* pList = NULL;
	pXml->GetAttribList2(&pList);

	pList->BeginEnum();
	LPCTSTR  szKey = NULL;
	LPCTSTR  szValue = NULL;
	while (pList->EnumNext(&szKey, &szValue))
	{
		PropertyCtrlEditItemInfo info = {0};
		info.szDesc = info.szKey = info.szText = szKey;
		info.szValue = szValue;

		m_pPropertyCtrl->InsertEditProperty(&info, NULL);
	}
	pList->EndEnum();
	SAFE_RELEASE(pList);

	m_pPropertyCtrl->AddAdditionItem(0);
}

// �����̳���֮�����ʽֵ
void  StylePropertyHandler::AttachStyle_Parsed(IStyleResItem* pStyleResItem)
{
	IMapAttribute* pMap = NULL;
	pStyleResItem->GetAttributeMap(&pMap);

	LPCTSTR  szKey = NULL;
	LPCTSTR  szValue = NULL;
	pMap->BeginEnum();

	while (pMap->EnumNext(&szKey, &szValue))
	{
		PropertyCtrlEditItemInfo info = {0};
		info.szDesc = info.szKey = info.szText = szKey;
		info.szValue = szValue;

		m_pPropertyCtrl->InsertEditProperty(&info, NULL);
	}
	pMap->EndEnum();
	pMap->Release();
}

// ��ʾ�̳���
void  StylePropertyHandler::AttachStyle_Inherit(IStyleResItem* pStyleResItem)
{

}

void  StylePropertyHandler::OnRadioButtonAttr()
{
	if (m_pRadioBtnXmlAttr->IsChecked())
		m_eCurAttrType = XmlAttr;
	else if (m_pRadioBtnParsedAttr->IsChecked())
		m_eCurAttrType = ParseAttr;
	else if (m_pRadioBtnInheritAttr->IsChecked())
		m_eCurAttrType = InheritAttr;
	else
		m_eCurAttrType = XmlAttr;

	AttachStyle(m_pSkinRes, m_pStyleResItem);
}

LRESULT  StylePropertyHandler::OnAdditionItem(WPARAM wParam, LPARAM lParam)
{
	if (!m_pStyleResItem)
		return 0;

	LPCTSTR szKey = (LPCTSTR)lParam;
	if (!szKey || !szKey[0])
		return ACCEPT_EDIT_REJECT_AND_CONTINUE_EDIT;

	if (m_eCurAttrType == XmlAttr)
	{
		// �ж��Ƿ��ظ�
		IUIElement* pElement = m_pStyleResItem->GetXmlElement();
		if (pElement->HasAttrib(szKey))
			return ACCEPT_EDIT_REJECT_AND_CONTINUE_EDIT;

		UE_ExecuteCommand(AddStyleItemAttributeCommand::CreateInstance(m_pSkinRes, m_pStyleResItem, szKey, NULL));
	}
	return 0;
}

LRESULT  StylePropertyHandler::OnEditItemAcceptContent(WPARAM wParam, LPARAM)
{
	PROPERTYCTRL_EDIT_ACCEPTCONTENT* pParam = (PROPERTYCTRL_EDIT_ACCEPTCONTENT*)wParam;
	IPropertyCtrlEditItem*  pItem = (IPropertyCtrlEditItem*)pParam->pItem;

	OnAcceptContent(pParam->szKey, pParam->szNewValue);
	return 0;
}

void  StylePropertyHandler::OnAcceptContent(LPCTSTR szKey, LPCTSTR szValue)
{
	UE_ExecuteCommand(ModifyStyleItemAttributeCommand::CreateInstance(m_pSkinRes, m_pStyleResItem, szKey, szValue));
}
LRESULT  StylePropertyHandler::OnPropertyKeyDown(WPARAM w, LPARAM)
{
	if (w == VK_DELETE)
	{
		IListItemBase* pItem = m_pPropertyCtrl->GetFirstSelectItem();
		if (!pItem)
			return UI::NOT_HANDLED;

		if (pItem->GetItemFlag() == IPropertyCtrlEditItem::FLAG)
		{
			UE_ExecuteCommand(
				DeleteStyleItemAttributeCommand::CreateInstance(
				m_pSkinRes, m_pStyleResItem, static_cast<IPropertyCtrlEditItem*>(pItem)->GetKey()));
		}
		return UI::HANDLED;
	}
	return UI::NOT_HANDLED;
}

void  StylePropertyHandler::OnStyleAttributeAdd(IStyleResItem* p, LPCTSTR szKey)
{
	if (m_pStyleResItem != p)
		return;

	if (m_eCurAttrType != XmlAttr)
		return;

	PropertyCtrlEditItemInfo info = {0};
	info.szKey = info.szDesc = info.szText = szKey;
	info.szValue = p->GetAttribute(szKey);
	m_pPropertyCtrl->LayoutItem(
		m_pPropertyCtrl->InsertEditProperty(&info, NULL), 
		true);
}

void  StylePropertyHandler::OnStyleAttributeRemove(IStyleResItem* p, LPCTSTR szKey)
{
	if (m_pStyleResItem != p)
		return;

	if (m_eCurAttrType != XmlAttr)
		return;

	IPropertyCtrlEditItem*  pItem = m_pPropertyCtrl->FindEditItem(szKey);
	if (!pItem)
		return;

	m_pPropertyCtrl->RemoveItem(pItem, LF_LAYOUT|LF_REDRAW);

}
void  StylePropertyHandler::OnStyleAttributeModify(IStyleResItem* p, LPCTSTR szKey)
{
	if (m_pStyleResItem != p)
		return;

	if (m_eCurAttrType != XmlAttr)
		return;

	IPropertyCtrlEditItem*  pItem = m_pPropertyCtrl->FindEditItem(szKey);
	if (!pItem)
		return;
	pItem->SetValueText(p->GetAttribute(szKey));

	m_pPropertyCtrl->InvalidateItem(pItem);
	m_pPropertyCtrl->Refresh();
}