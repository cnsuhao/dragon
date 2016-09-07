#include "stdafx.h"
#include "LayoutPropertyHandler.h"
#include "Business\Command\AddObjectAttributeCommand.h"
#include "Business\Command\ModifyObjectAttributeCommand.h"
#include "Business\Command\RemoveObjectAttributeCommand.h"
#include "Business\Command\Base\commandhistorymgr.h"
#include "Business\Project\ProjectData\projectdata.h"
#include "UISDK\Control\Inc\Interface\iradiobutton.h"
#include "UISDK\Kernel\Inc\Interface\iscrollbarmanager.h"
#include "Dialog\Framework\propertydialog.h"

LayoutPropertyHandler::LayoutPropertyHandler()
{
    CreateImpl();

    m_pPropertyDialog = NULL;
    m_pPropertyCtrl = NULL;
    m_pRichEditDescript = NULL;
    m_pCommandEdit = NULL;
    m_pRadioBtnXmlAttr = NULL;
    m_pRadioBtnObjAttr = NULL;
    m_pRadioBtnStyleAttr = NULL;
    m_eCurAttrType = ObjAttr;
    m_nSearchStringsIndex = 0;

    m_pSkinRes = NULL;
    m_pObject = NULL;
    m_pXmlElement = NULL;
}

LayoutPropertyHandler::~LayoutPropertyHandler()
{
	DetachHandler();
}
void  LayoutPropertyHandler::OnLoad(CPropertyDialog* pWnd)
{
    m_pPropertyDialog = pWnd;
    m_pPropertyCtrl = pWnd->GetPropertyCtrl();
    m_pCommandEdit = pWnd->GetCommandEdit();
	m_pRichEditDescript = pWnd->GetDescriptRichEdit();
    m_pRadioBtnObjAttr = (IRadioButton*)pWnd->FindChildObject(_T("radiobtn_attr1"));
    m_pRadioBtnXmlAttr = (IRadioButton*)pWnd->FindChildObject(_T("radiobtn_attr2"));
    m_pRadioBtnStyleAttr = (IRadioButton*)pWnd->FindChildObject(_T("radiobtn_attr3"));

	if (m_pRadioBtnObjAttr)
	{
		m_pRadioBtnObjAttr->SetText(TEXT("Object"), false);
		m_pRadioBtnObjAttr->SetToolTipText(TEXT("��ʾ�������������"));
		m_pRadioBtnObjAttr->SetChecked();
	}
	if (m_pRadioBtnXmlAttr)
	{
		m_pRadioBtnXmlAttr->SetText(TEXT("Xml"), false);
		m_pRadioBtnXmlAttr->SetToolTipText(TEXT("��ʾ������xml�����õ�����"));
	}
	if (m_pRadioBtnStyleAttr)
	{
		m_pRadioBtnStyleAttr->SetText(TEXT("Style"), false);
		m_pRadioBtnStyleAttr->SetToolTipText(TEXT("��ʾ��������ʽ�̳еõ�������"));
	}
}

void  LayoutPropertyHandler::OnUnload()
{
    ClearData();
    m_pPropertyDialog = NULL;

    m_pPropertyCtrl = NULL;
    m_pCommandEdit = NULL;
    m_pRadioBtnXmlAttr = NULL;
    m_pRadioBtnObjAttr = NULL;
    m_pRadioBtnStyleAttr = NULL;
    m_pRichEditDescript = NULL; 
}

void  LayoutPropertyHandler::ClearData()
{
    // ���浱ǰ�༭״̬�������л���ʱ�ָ�
    if (m_pObject)
        SaveObjectPropertyEditStatus(m_pObject);

    if (m_pPropertyCtrl)
        m_pPropertyCtrl->RemoveAllItem(0);

    m_pSkinRes = NULL;
    m_pObject = NULL;
    m_pXmlElement = NULL;
}

void  LayoutPropertyHandler::OnRadioButtonAttr()
{
    if (m_pRadioBtnObjAttr->IsChecked())
        m_eCurAttrType = ObjAttr;
    else if (m_pRadioBtnXmlAttr->IsChecked())
        m_eCurAttrType = XmlAttr;
    else if (m_pRadioBtnStyleAttr->IsChecked())
        m_eCurAttrType = StyleAttr;
    else
        m_eCurAttrType = ObjAttr;

	AttachObject(m_pSkinRes, m_pObject);
}

bool  LayoutPropertyHandler::IsShowXmlAttr()
{
    if (!m_pRadioBtnXmlAttr)
        return false;

    return m_pRadioBtnXmlAttr->IsChecked();
}
bool  LayoutPropertyHandler::IsShowObjAttr()
{
    if (!m_pRadioBtnObjAttr)
        return false;

    return m_pRadioBtnObjAttr->IsChecked();
}

void  LayoutPropertyHandler::AttachHandler()
{
    g_pGlobalData->m_pPropertyDialog->AttachHandler(static_cast<IPropertyHandler*>(this));
}

void  LayoutPropertyHandler::DetachHandler()
{
	g_pGlobalData->m_pPropertyDialog->DetachHandler(static_cast<IPropertyHandler*>(this));
}

// ���������б����������
void  LayoutPropertyHandler::AttachObject(ISkinRes* hSkin, IObject* pObj)
{
    if (!m_pPropertyCtrl)
	{
        AttachHandler();
		if (!m_pPropertyCtrl)
			return;
	}
	
    this->ClearData();

    if (NULL == hSkin || NULL == pObj)
    {
        m_pPropertyCtrl->LayoutAllItemsAndRedraw();
        return;
    }

    IUIElement* pXmlElem = g_pGlobalData->m_pProjectData->m_uieditor.GetObjectXmlElem(pObj);
    if (NULL == pXmlElem)
        return ;

    m_pSkinRes = hSkin;
    m_pObject = pObj;
    m_pXmlElement = pXmlElem;

    // �������
    FillObjectAttribute();

    // ˢ���б�
    m_pPropertyCtrl->LayoutAllItemsAndRedraw();

    if (!::IsWindowVisible(GetPropertyDialog()->GetHWND()))
    {
        ::ShowWindow(GetPropertyDialog()->GetHWND(), SW_SHOWNOACTIVATE);
    }
}

// ���¼�������
void   LayoutPropertyHandler::ReloadObjectAttribute(IObject* pObj)
{
    if (m_pObject != pObj)
        return;

    if (m_eCurAttrType == ObjAttr)
        SaveObjectPropertyEditStatus(m_pObject);

    // ɾ���ϵ���������
    GetUIEditor()->DestroyObjectAttributeProxy(pObj);

    // ���
    m_pPropertyCtrl->RemoveAllItem(0);

    // ���¼���
    FillObjectAttribute();

    // ˢ���б�
    m_pPropertyCtrl->LayoutAllItemsAndRedraw();
}

// ��ReloadObjectAttribute����AttachObject����
// ֻ�������б����������
void  LayoutPropertyHandler::FillObjectAttribute()
{
#pragma region	// ֱ����ʾxml����
    if (m_pRadioBtnXmlAttr->IsChecked())
    {
        IPropertyCtrlGroupItem* pParentNode = m_pPropertyCtrl->InsertGroupItem(_T("xml"), _T("��������"), UITVI_ROOT, UITVI_LAST, 0);

        IListAttribute*  pListAttrib = NULL;
        m_pXmlElement->GetAttribList2(&pListAttrib);

        LPCTSTR  szKey = NULL;
        LPCTSTR  szValue = NULL;
        pListAttrib->BeginEnum();
        while (pListAttrib->EnumNext(&szKey, &szValue))
        {
			PropertyCtrlEditItemInfo info = {0};
			info.szText = info.szKey = szKey;
			info.szValue = szValue;

            m_pPropertyCtrl->InsertEditProperty(&info, pParentNode, UITVI_LAST, 0);
        }
        pListAttrib->EndEnum();
        SAFE_RELEASE(pListAttrib);
    }
#pragma endregion
#pragma region	// ��ʾ���������б�
    else if (m_pRadioBtnObjAttr->IsChecked())
    {
        // 2015.3.6 ���ԵĲ��룬���¶���uiedtiorimpl���
#if 0
        // 		����styleclass���
        // 		IPropertyCtrlEditItem*  pStyleclassItem = m_pPropertyCtrl->InsertEditProperty(XML_STYLECLASS, L"", L"", XML_STYLECLASS, NULL, UITVI_LAST, LISTITEM_OPFLAG_NOALL);
        IPropertyCtrlEditItem*  pStyleclassItem = (IPropertyCtrlEditItem*)m_pPropertyCtrl->GetFirstItem();

        IMapAttribute*  pMapAttr = NULL;  // �������б�����ʽ����������
        IMapAttribute*  pMapAttrSelf = NULL;  // ��������һ������ֵ���Լ��ģ�����������ʽ��
        m_pXmlElement->GetAttribList(&pMapAttr);
        m_pXmlElement->GetAttribList(&pMapAttrSelf);

        LPCTSTR szStyleclass = pMapAttr->GetAttr(XML_STYLECLASS, false);
        if (szStyleclass && pStyleclassItem)
            pStyleclassItem->SetValueText(szStyleclass);

        // ������ԣ����û�еľ�ֱ���������
        IStyleManager*  pStyleMgr = g_pGlobalData->m_pProjectData->m_pEditUIApp->GetActiveSkinStyleMgr();
        pStyleMgr->ParseStyle(pMapAttr);

        LPCTSTR  szKey = NULL;
        LPCTSTR  szValue = NULL;
        IPropertyCtrlGroupItem* pParentOtherNode = NULL;

        pMapAttr->BeginEnum();
        while (pMapAttr->EnumNext(&szKey, &szValue))
        {
            IListItemBase* pItem = FindPropertyItemByKey(szKey);
            if (pItem)
            {
                UISendMessage(pItem, UI_PROPERTYCTRLITEM_MSG_SETVALUESTRING, (WPARAM)szValue);
            }
            else
            {
                if (!pParentOtherNode)
                {
                    pParentOtherNode = m_pPropertyCtrl->InsertGroupItem(_T("��������"), NULL, UITVI_ROOT, UITVI_LAST, 0);
                }
                pItem = m_pPropertyCtrl->InsertEditProperty(szKey, szValue, NULL, szKey, pParentOtherNode, UITVI_LAST, 0);
            }

            // ���Լ��ģ��Ͳ�Ҫ����Ĭ��ֵ�ˡ� ����ʽ�ľ�����Ĭ��ֵ������ֵ���û��޸�Ϊ����ʽ��ֵһ���ģ��ͽ�������ɾ��
            if (!pMapAttrSelf->GetAttr(szKey, false))  
            {
                UISendMessage(pItem, UI_PROPERTYCTRLITEM_MSG_SETDEFAULTVALUESTRING, (WPARAM)szValue);
            }
        }

        pMapAttr->EndEnum();
        SAFE_RELEASE(pMapAttr);
        SAFE_RELEASE(pMapAttrSelf);
#endif
        g_pGlobalData->m_pProjectData->m_uieditor.LoadAttribute2PropertyCtrl(m_pObject);

        // ������ɺ󣬼������Ե���һ�α༭״̬
        LoadObjectPropertyEditStatus(m_pObject);
    }
#pragma endregion
#pragma region // �������ʽ
    else if (m_pRadioBtnStyleAttr->IsChecked())
    {
        CComBSTR bstrId;
        CComBSTR bstrStyleClass;
        m_pXmlElement->GetAttrib(XML_ID, &bstrId);
        m_pXmlElement->GetAttrib(XML_STYLECLASS, &bstrStyleClass);

        String strId, strStyleClass;
        if (bstrId.Length() > 0)
            strId = (BSTR)bstrId;
        if (bstrStyleClass.Length() > 0)
            strStyleClass = (BSTR)bstrStyleClass;

        //////////////////////////////////////////////////////////////////////////
        // ID ����

        AttachObject_IDAttrib(m_pSkinRes, m_pObject, strId);

        //////////////////////////////////////////////////////////////////////////
        // Class ����

        AttachObject_ClassAttrib(m_pSkinRes, m_pObject, strStyleClass);

        //////////////////////////////////////////////////////////////////////////
        // Tag ����

        AttachObject_TagAttrib(m_pSkinRes, m_pObject, m_pObject->GetObjectName());
    }
#pragma endregion
}

void LayoutPropertyHandler::AttachObject_IDAttrib(ISkinRes* hSkin, IObject* pObj, const String& strID)
{
    if (strID.empty())
        return;

    IStyleRes* pStyleRes = g_pGlobalData->m_pProjectData->m_pEditUIApp->GetActiveSkinStyleRes();
    IStyleResItem* pInfo = pStyleRes->FindItem(STYLE_SELECTOR_TYPE_ID, strID.c_str());
    if (NULL == pInfo)
        return;

    IMapAttribute* pMapAttr = NULL;
    pInfo->GetAttributeMap(&pMapAttr);
    if (NULL == pMapAttr)
        return;

    String strGroupName(_T("Id��ʽ - "));
    strGroupName.append(strID);
    IPropertyCtrlGroupItem* pGroup = NULL;

    LPCTSTR  szKey = NULL;
    LPCTSTR  szValue = NULL;
    pMapAttr->BeginEnum();
    while (pMapAttr->EnumNext(&szKey, &szValue))
    {
        if (!pGroup)
            pGroup = m_pPropertyCtrl->InsertGroupItem(strGroupName.c_str(), _T("Id��ʽ"), UITVI_ROOT, UITVI_LAST, 0);

		PropertyCtrlEditItemInfo info = {0};
		info.szText = info.szKey = szKey;
		info.szValue = szValue;

        m_pPropertyCtrl->InsertEditProperty(&info, pGroup, UITVI_LAST, 0);
    }

    SAFE_RELEASE(pMapAttr);
}

void LayoutPropertyHandler::AttachObject_ClassAttrib(ISkinRes* hSkin, IObject* pObj, const String& strClassStyle)
{
    if (strClassStyle.empty())
        return;

    IStyleRes* pStyleRes = g_pGlobalData->m_pProjectData->m_pEditUIApp->GetActiveSkinStyleRes();
    if (NULL == pStyleRes)
        return;

    Util::ISplitStringEnum*  pEnum = NULL;
    int nCount = Util::SplitString(strClassStyle.c_str(), XML_SEPARATOR, &pEnum);
    if (0 == nCount)
        return;

    for (int i = 0; i < nCount; i++)
    {
        LPCTSTR szText = pEnum->GetText(i);
        if (NULL == szText || _tcslen(szText) == 0)
            continue;

        String strGroupName(_T("Class��ʽ - "));
        strGroupName.append(szText);

        IStyleResItem* pInfo = pStyleRes->FindItem(STYLE_SELECTOR_TYPE_CLASS, szText);
        if (NULL == pInfo)
            continue;

        IMapAttribute* pMapAttr = NULL;
        pInfo->GetAttributeMap(&pMapAttr);
        if (!pMapAttr)
            continue;

        IPropertyCtrlGroupItem* pGroup = m_pPropertyCtrl->InsertGroupItem(strGroupName.c_str(), _T("Class��ʽ"), UITVI_ROOT, UITVI_LAST, 0);

        LPCTSTR  szKey = NULL;
        LPCTSTR  szValue = NULL;
        pMapAttr->BeginEnum();
        while (pMapAttr->EnumNext(&szKey, &szValue))
        {
			PropertyCtrlEditItemInfo info = {0};
			info.szText = info.szKey = szKey;
			info.szValue = szValue;

            m_pPropertyCtrl->InsertEditProperty(&info, pGroup, UITVI_LAST, 0);
        }

        SAFE_RELEASE(pMapAttr);
    }

    SAFE_RELEASE(pEnum);
}
void LayoutPropertyHandler::AttachObject_TagAttrib(ISkinRes* hSkin, IObject* pObj, const String& strTagName)
{
    //    m_pPropertyCtrl->InsertGroupItem(_T("Tag��ʽ"), _T("Tag��ʽ"), UITVI_ROOT, UITVI_LAST, LISTITEM_OPFLAG_NOALL);

    if (strTagName.empty())
        return;

    IStyleRes* pStyleRes = g_pGlobalData->m_pProjectData->m_pEditUIApp->GetActiveSkinStyleRes();
    IStyleResItem* pInfo = pStyleRes->FindItem(STYLE_SELECTOR_TYPE_TAG, strTagName.c_str());
    if (!pInfo)
        return;

    IMapAttribute* pMapAttr = NULL;
    pInfo->GetAttributeMap(&pMapAttr);
    if (!pMapAttr)
        return;

    String strName(_T("Tag��ʽ"));
    strName.append(strTagName);
    IPropertyCtrlGroupItem* pGroup = m_pPropertyCtrl->InsertGroupItem(strName.c_str(), NULL, UITVI_ROOT, UITVI_LAST, 0);

    LPCTSTR  szKey = NULL;
    LPCTSTR  szValue = NULL;
    pMapAttr->BeginEnum();
    while (pMapAttr->EnumNext(&szKey, &szValue))
    {
		PropertyCtrlEditItemInfo info = {0};
		info.szText = info.szKey = szKey;
		info.szValue = szValue;

        m_pPropertyCtrl->InsertEditProperty(&info, pGroup, UITVI_LAST, 0);
    }

    SAFE_RELEASE(pMapAttr);
}


void  LayoutPropertyHandler::OnAcceptContent(LPCTSTR szKey, LPCTSTR szNewValue/*, bool bDefault, bool& bNeedSetItemValue*/)
{
#if 1
    if (0 == UE_ExecuteCommand(
        ModifyObjectAttributeCommand2::CreateInstance(
        m_pObject, szKey, szNewValue)))
    {
        String str = _T("Modify ");
        str.append(szKey);
        str.append(_T(" =\""));
        str.append(szNewValue);
        str.append(_T("\" �ɹ�"));
        ShowStatusText(str.c_str());
    }
#else

    bNeedSetItemValue = false;
    if (IsShowObjAttr())  // ���������б��ڻ�ȡ�����б�ʱ��������Ĭ�����ԣ�ͨ��Ĭ�����Կ��������ж��Ƿ�ɾ��������
    {
        if (bDefault)
        {
            String strKey = szKey;
            String strValue = szNewValue;

            if (0 == UE_ExecuteCommand(RemoveObjectAttributeCommand::CreateInstance(
                m_pSkinRes, m_pObject, m_pXmlElement, szKey)
                ))
            {
                String str = _T("Remove ");
                str.append(strKey.c_str());
                str.append(_T(" =\""));
                str.append(strValue.c_str());
                str.append(_T("\" �ɹ�"));
                ShowStatusText(str.c_str());
            }
        }
        else
        {
            if (0 == UE_ExecuteCommand(
                ModifyObjectAttributeCommand::CreateInstance(
                m_pSkinRes, m_pObject, m_pXmlElement, szKey, szNewValue)))
            {
                bNeedSetItemValue = true;

                String str = _T("Modify ");
                str.append(szKey);
                str.append(_T(" =\""));
                str.append(szNewValue);
                str.append(_T("\" �ɹ�"));
                ShowStatusText(str.c_str());
            }
            else if (0 == UE_ExecuteCommand(  // ������Բ����ڣ���Ϊ��������
                AddObjectAttributeCommand::CreateInstance(
                m_pSkinRes, m_pObject, m_pXmlElement, szKey, szNewValue)))
            {
                String str = _T("Add ");
                str.append(szKey);
                str.append(_T(" =\""));
                str.append(szNewValue);
                str.append(_T("\" �ɹ�"));
                ShowStatusText(str.c_str());
            }
        }
    }
    else
    {
        if (0 == UE_ExecuteCommand(
            ModifyObjectAttributeCommand::CreateInstance(
            m_pSkinRes, m_pObject, m_pXmlElement, szKey, szNewValue)))
        {
            bNeedSetItemValue = true;

            String str = _T("Modify ");
            str.append(szKey);
            str.append(_T(" =\""));
            str.append(szNewValue);
            str.append(_T("\" �ɹ�"));
            ShowStatusText(str.c_str());
        }
    }
#endif
}

LRESULT  LayoutPropertyHandler::OnEditItemAcceptContent(WPARAM wParam, LPARAM lParam)
{
    PROPERTYCTRL_EDIT_ACCEPTCONTENT* pParam = (PROPERTYCTRL_EDIT_ACCEPTCONTENT*)wParam;
    IPropertyCtrlEditItem*  pItem = (IPropertyCtrlEditItem*)pParam->pItem;

    OnAcceptContent(pParam->szKey, pParam->szNewValue);
    return 0;
}
LRESULT  LayoutPropertyHandler::OnComboBoxItemAcceptContent(WPARAM wParam, LPARAM lParam)
{
    PROPERTYCTRL_COMBOBOX_ITEM_ACCEPTCONTENT* pParam = (PROPERTYCTRL_COMBOBOX_ITEM_ACCEPTCONTENT*)wParam;
    IPropertyCtrlComboBoxItem*  pItem = (IPropertyCtrlComboBoxItem*)pParam->pItem;

    OnAcceptContent(pParam->szKey, pParam->szNewValue);
    return 0;
}
LRESULT  LayoutPropertyHandler::OnDelayReloadObjectAttr(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    ReloadObjectAttribute(m_pObject);
    return 0;
}

LRESULT  LayoutPropertyHandler::OnCommandEditKeyDown(WPARAM w, LPARAM l)
{
    if (w == VK_ESCAPE)
    {
        m_pCommandEdit->SetText(NULL, true);
        return UI::HANDLED;
    }
    else if (w == VK_RETURN)
    {
        LPCTSTR szText = m_pCommandEdit->GetText();

        if (0 == DoCommand(szText))
        {
            // �ɹ�����ȫѡ����֪ͨ
            m_pCommandEdit->SetSel(0, -1);
        }
        return UI::HANDLED;
    }
    else if (w == VK_TAB)
    {
        OnCommandEditTab();
        return UI::HANDLED;
    }
    return UI::NOT_HANDLED;
}

LRESULT  LayoutPropertyHandler::OnCommandEnChange(WPARAM w, LPARAM l)
{
    BOOL bSetText = (BOOL)w;
    if (bSetText)
        return 0;

    if (m_vSearchStrings.size() > 0)
    {
        m_vSearchStrings.clear();
        m_nSearchStringsIndex = -1;
    }
    return 0;
}   

void LayoutPropertyHandler::OnCommandEditTab()
{
    LPCTSTR  szText = m_pCommandEdit->GetText();

    // �жϵ�ǰ��дʲô���ݣ�����ǵ�һ������ 
    String  strText(szText);
    TrimStringLeft(strText);

    if (strText.empty())
        return;

    // ����, ����д��ô���ӣ���ʱֻ֧��һ����ĸ������չ
    if (String::npos == strText.find_first_of(_T(' '))) 
    {
        if (strText.length() != 1)
            return;

        switch (strText[0])
        {
        case _T('a'): 
            m_pCommandEdit->SetText(_T("add "), true);
            break;
        case _T('c'):
            m_pCommandEdit->SetText(_T("clear "), true);
            break;
        case _T('d'):
            m_pCommandEdit->SetText(_T("delete "), true);
            break;
        case _T('e'):
            m_pCommandEdit->SetText(_T("erase "), true);
            break;
        case _T('i'):
            m_pCommandEdit->SetText(_T("insert "), true);
            break;
        case _T('m'):
            m_pCommandEdit->SetText(_T("modify "), true);
            break;
        case _T('r'):
            m_pCommandEdit->SetText(_T("remove "), true);
            break;
        }
        return;
    }

    // �Ե�һ������������չ
    String  strCommand, strArgs;
    ParsePropertyCommand(szText, strCommand, strArgs);
    if (strArgs.empty())
        return;
    if (String::npos != strArgs.find_first_of(_T(' ')))
        return;

    IListItemBase*  pItem = m_pPropertyCtrl->GetFirstItem();
    if (NULL == pItem)
        return;

    if (0 == m_vSearchStrings.size())
    {
        m_strCurSearchPrefix = strArgs;

        pItem = pItem->GetChildItem();
        while (pItem)
        {
            LPCTSTR szText = pItem->GetText();
            if (szText == _tcsstr(szText, strArgs.c_str()))
            {
                m_vSearchStrings.push_back(szText);
            }   
            pItem = pItem->GetNextItem();
        }
    }

    if (m_vSearchStrings.size() > 0)
    {
        m_nSearchStringsIndex++;
        if (m_nSearchStringsIndex >= (int)m_vSearchStrings.size())
            m_nSearchStringsIndex = 0;

        String str = strCommand;
        str.append(_T(" "));
        str.append(m_vSearchStrings[m_nSearchStringsIndex]);
        m_pCommandEdit->SetText(str.c_str(), true);
    }
}

// ���������propertyctrl�еı༭״̬
void  LayoutPropertyHandler::SaveObjectPropertyEditStatus(IObject* pObj)
{   
    if (m_eCurAttrType != ObjAttr)
        return;
    if (!pObj || m_pObject != pObj)
        return;
    ObjectEditorData* pData = GetUIEditor()->GetObjectEditorData(m_pObject);
    if (!pData)
        return;

    pData->m_editStatus.Clear();

    IListItemBase* pItem = m_pPropertyCtrl->GetFirstItem();
    while (pItem)
    {
        if (pItem->GetItemFlag() == IPropertyCtrlGroupItem::FLAG)
        {
            if (pItem->IsCollapsed())
            {
                if (pItem->GetParentItem())
                    pData->m_editStatus.AddCollapsedGroup(pItem->GetParentItem()->GetText(), pItem->GetText());
                else
                    pData->m_editStatus.AddCollapsedGroup(NULL, pItem->GetText());
            }
        }
        pItem = pItem->GetNextTreeItem();
    }

    pData->m_editStatus.SetVScrollPos(m_pPropertyCtrl->GetIScrollBarMgr()->GetVScrollPos());
    if (m_pPropertyCtrl->GetFirstSelectItem())
    {
        pData->m_editStatus.SetSelectedItem(m_pPropertyCtrl->GetFirstSelectItem()->GetText());
    }
}

// ��ԭ������propertyctrl�еı༭״̬
void  LayoutPropertyHandler::LoadObjectPropertyEditStatus(IObject* pObj)
{
    if (!pObj || m_pObject != pObj)
        return;
    ObjectEditorData* pData = GetUIEditor()->GetObjectEditorData(m_pObject);
    if (!pData)
        return;

    IListItemBase* pItem = m_pPropertyCtrl->GetFirstItem();
    while (pItem)
    {
        if (pItem->GetItemFlag() == IPropertyCtrlGroupItem::FLAG)
        {
            LPCTSTR szParentKey = NULL;
            if (pItem->GetParentItem())
                szParentKey = pItem->GetParentItem()->GetText();

            if (pData->m_editStatus.IsGroupCollapsed(szParentKey, pItem->GetText()))
            {
                m_pPropertyCtrl->CollapseItem(pItem, false);
            }
        }
        pItem = pItem->GetNextTreeItem();
    }
    pItem = m_pPropertyCtrl->FindItemByKey(pData->m_editStatus.GetSelectedItem());
    if (pItem)
        m_pPropertyCtrl->SelectItem(pItem, false);

    m_pPropertyCtrl->LayoutItem(NULL, false);
    m_pPropertyCtrl->GetIScrollBarMgr()->SetVScrollPos(pData->m_editStatus.GetVScrollPos());
    m_pPropertyCtrl->UpdateObject();
}


long  LayoutPropertyHandler::AddAttributeInCtrl(LPCTSTR szKey, LPCTSTR szValue)
{
    if (!m_pPropertyCtrl)
        return -1;

    if (IsShowXmlAttr())
    {
        IListItemBase* pParent = m_pPropertyCtrl->GetFirstItem();
        if (pParent)
        {
			PropertyCtrlEditItemInfo info = {0};
			info.szText = info.szKey = szKey;
			info.szValue = szValue;

            m_pPropertyCtrl->InsertEditProperty(&info, pParent);
        }
    }
    else
    {
        IListItemBase* pItem = m_pPropertyDialog->FindPropertyItemByKey(szKey);
        if (pItem)
        {
            UISendMessage(pItem, UI_PROPERTYCTRLITEM_MSG_SETVALUESTRING, (WPARAM)szValue);
            m_pPropertyCtrl->InvalidateItem(pItem);
            m_pPropertyCtrl->Refresh();
        }
        else
        {
			PropertyCtrlEditItemInfo info = {0};
			info.szText = info.szKey = szKey;
			info.szValue = szValue;

            m_pPropertyCtrl->InsertEditProperty(&info, NULL);
        }
    }

    return 0;
}

// �øö����ʵ��ֵ�������Կؼ��ж�Ӧ���б���ֵ��������ctrl+z��ԭ����
void   LayoutPropertyHandler::UpdateObjectAttributeInCtrl(IObject* pObj, LPCTSTR szKey)
{
    if (m_pObject != pObj)
        return;

    ObjectEditorData* pData = GetUIEditor()->GetObjectEditorData(pObj);
    if (!pData)
        return;

	if (!pData->pAttributeProxy)
	{
		// ������ModifyObjectAttributeCommand2.on_post_do���޸���styleclass���ԣ�
		//       ���Ƚ�pAttributeProxy���ٵ��ˣ����ֱ��reload
		::PostMessage(GetPropertyDialog()->GetHWND(), MESSAGE_DELAY_RELOAD_OBJECT_ATTR, 0, 0);
		return;
	}

    // ����bkg.render.type�����޸ģ���Ҫ���¼��������б�
	UpdateAttribute2EditorResult eResult = pData->pAttributeProxy->UpdateAttribute2Editor(szKey);
    if (UpdateAttribute2EditorNeedReload == eResult)
    {
        // ���¼���.�ӳ٣��Է���ǰ��item change notify��ջ����
		::PostMessage(GetPropertyDialog()->GetHWND(), MESSAGE_DELAY_RELOAD_OBJECT_ATTR, 0, 0);
    }
}

long   LayoutPropertyHandler::ModifyAttributeInCtrl(LPCTSTR szKey, LPCTSTR szValue)
{
    if (!m_pPropertyCtrl)
        return -1;

    IListItemBase* pItem = m_pPropertyDialog->FindPropertyItemByKey(szKey);
    if (!pItem)
        return -1;

    UISendMessage(pItem, UI_PROPERTYCTRLITEM_MSG_SETVALUESTRING, (WPARAM)szValue);
    m_pPropertyCtrl->InvalidateItem(pItem);
    m_pPropertyCtrl->Refresh();

    return 0;
}

long  LayoutPropertyHandler::RemoveAttribInCtrl(LPCTSTR szKey)
{
    if (!m_pPropertyCtrl)
        return -1;

    IListItemBase* pItem = m_pPropertyDialog->FindPropertyItemByKey(szKey);
    if (!pItem)
        return -1;

    if (IsShowXmlAttr())
    {
        m_pPropertyCtrl->RemoveItem(pItem, UI::LF_REDRAW|UI::LF_LAYOUT);
    }
    else if (IsShowObjAttr())
    {
        UISendMessage(pItem, UI_PROPERTYCTRLITEM_MSG_SETVALUESTRING, NULL);
        m_pPropertyCtrl->InvalidateItem(pItem);
        m_pPropertyCtrl->Refresh();
    }
    return 0;
}
long  LayoutPropertyHandler::ClearAttribInCtrl()
{
    if (!m_pPropertyCtrl)
        return -1;

    if (!IsShowXmlAttr())
        return -1;

    IListItemBase* pFirstItem = m_pPropertyCtrl->GetFirstItem();
    m_pPropertyCtrl->RemoveAllChildItems(pFirstItem, 0);
    return 0;
}

void  LayoutPropertyHandler::ShowStatusText(LPCTSTR  szText)
{
    g_pGlobalData->m_pStatusBar->SetStatusText2(szText);
}

long LayoutPropertyHandler::OnAddGroupProperty(IObject* pObj, LPCTSTR szGroupName)
{
#if 0
    LVGROUP group = {0};
    group.cbSize = sizeof(LVGROUP);
    group.mask = LVGF_HEADER|LVGF_GROUPID|LVGF_ALIGN|LVGF_STATE;
    group.uAlign = LVGA_HEADER_LEFT;
    group.iGroupId = m_listctrlObjectAttrib.GetGroupCount();
    group.pszHeader = (LPWSTR) szGroupName;
    group.state = LVGS_COLLAPSIBLE;
    m_listctrlObjectAttrib.AddGroup(&group);

    return group.iGroupId;
#endif

    IPropertyCtrlGroupItem* pParentNode = m_pPropertyCtrl->InsertGroupItem(szGroupName, NULL, UITVI_ROOT, UITVI_LAST, 0);
    return (long)pParentNode;
}
void LayoutPropertyHandler::OnAddProperty(IObject* pObj, long nGroupID, LPCTSTR szName)
{
#if 0
    int iItemCount = m_listctrlObjectAttrib.GetItemCount();

    LVITEM item = {0};
    item.mask = LVIF_GROUPID|LVIF_TEXT;
    item.iItem = iItemCount;
    item.iGroupId = nGroupID;
    item.pszText = (LPWSTR)szName;

    int nIndex = m_listctrlObjectAttrib.InsertItem(&item);
    //m_listctrlObjectAttrib.SetItemText(nIndex, 1, iter2->second.c_str());
#endif

	PropertyCtrlEditItemInfo info = {0};
	info.szText = info.szKey = szName;

    m_pPropertyCtrl->InsertEditProperty(&info, (IPropertyCtrlGroupItem*)nGroupID, UITVI_LAST, LF_NONE);
}