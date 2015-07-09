#include "stdafx.h"
#include "Dialog\Framework\propertydialog.h"
#include "Business\uieditorfunction.h"
#include "Business\Command\Base\commandhistorymgr.h"
#include "Business\Command\AddObjectAttributeCommand.h"
#include "Business\Command\RemoveObjectAttributeCommand.h"
#include "Business\Command\ModifyObjectAttributeCommand.h"
#include "Business\Command\ClearObjectAttributeCommand.h"
#include "LayoutPropertyHandler.h"

long  LayoutPropertyHandler::DoCommand(const TCHAR*  szCommand)
{
    if (NULL == szCommand)
        return -1;

    // �Ȳ��������: command  args
    String  strCommand;
    String  strArgs;

    ParsePropertyCommand(szCommand, strCommand, strArgs);

    // ������������
    PROPERTY_COMMAND_TYPE eCommand = GetCommandType(strCommand);
    if (eCommand == PROPERTY_COMMAND_UNKNOWN)
    {
        String str = _T("���ܽ���������: ");
        str.append(strCommand);
        ShowStatusText(str.c_str());
        return -1;
    }

    return DoCommand(eCommand, strArgs.c_str());
}

long  LayoutPropertyHandler::DoCommand(PROPERTY_COMMAND_TYPE eType, const TCHAR* szArg)
{
    if (NULL == m_pSkinRes || NULL == m_pObject)
    {
        ShowStatusText(_T("��Ŀ�����"));
        return -1;
    }

    switch (eType)
    {
    case PROPERTY_COMMAND_ADD:
        return OnCommand_Add(szArg);
        break;

    case PROPERTY_COMMAND_DELETE:
        return OnCommand_Delete(szArg);
        break;

    case PROPERTY_COMMAND_CLEAR:
        return OnCommand_Clear(szArg);
        break;

    case PROPERTY_COMMAND_MODIFY:
        return OnCommand_Modify(szArg);
        break;
    }

    return -1;
}

long  LayoutPropertyHandler::OnCommand_Add(const TCHAR* szArg)
{
    String strKey;
    String strValue;

    ParsePropertyCommandArg(szArg, strKey, strValue);
    if (strKey.empty())
    {
        String  str = _T("��Ч����");
        ShowStatusText(str.c_str());
        return -1;
    }

    if (-1 == UE_ExecuteCommand(AddObjectAttributeCommand::CreateInstance(
                m_pSkinRes, m_pObject, m_pXmlElement, strKey.c_str(), strValue.c_str())))
    {
        return -1;
    }

    {
        String  str = _T("Command: Add ");
        str.append(strKey.c_str());
        str.append(_T("=\""));
        str.append(strValue.c_str());
        str.append(_T("\" �ɹ�"));
        ShowStatusText(str.c_str());
    }
    return 0;
}

long  LayoutPropertyHandler::OnCommand_Delete(const TCHAR* szArg)
{
    if (NULL == szArg)
        return -1;
    
    String strKey(szArg);
    TrimString(strKey);

    if (strKey.empty())
    {
        // ��ȡѡ����
        IListItemBase* pItem = m_pPropertyCtrl->GetFirstSelectItem();
		if (NULL == pItem || pItem->GetItemFlag() == IPropertyCtrlGroupItem::FLAG)
        {
            String  str = _T("delete ������Ҫһ�������ֶβ���");
            ShowStatusText(str.c_str());
            return -1;
        }

        strKey = pItem->GetText();
        {
            String  str = _T("�ٴλس�ȷ��ɾ����ǰѡ����:");
            str.append(strKey);
            ShowStatusText(str.c_str());

            if (!WaitForEnter2Confirm(m_pPropertyDialog->GetHWND()))
            {
                String  str = _T("delete ������ȡ��");
                ShowStatusText(str.c_str());
                return -1;
            }
        }
    }

    if (-1 == UE_ExecuteCommand(
				RemoveObjectAttributeCommand::CreateInstance(
				m_pSkinRes, m_pObject, m_pXmlElement, strKey.c_str())))
    {
        return -1;
    }

    {
        String  str = _T("ɾ�������ֶ����: ");
        str.append(strKey);
        ShowStatusText(str.c_str());
    }
    return 0;
}


long  LayoutPropertyHandler::OnCommand_Modify(const TCHAR* szArg)
{
    String strKey;
    String strValue;

    ParsePropertyCommandArg(szArg, strKey, strValue);
    if (strKey.empty())
    {
        String  str = _T("��Ч����");
        ShowStatusText(str.c_str());
        return -1;
    }

    if (-1 == UE_ExecuteCommand(
        ModifyObjectAttributeCommand2::CreateInstance(
            m_pObject, strKey.c_str(), strValue.c_str())))
    {
        return -1;
    }

    {
        String  str = _T("Command: Modify ");
        str.append(strKey.c_str());
        str.append(_T("=\""));
        str.append(strValue.c_str());
        str.append(_T("\" �ɹ�"));
        ShowStatusText(str.c_str());
    }

    return 0;
}

long  LayoutPropertyHandler::OnCommand_Clear(const TCHAR* szArg)
{
    String  str = _T("�ٴλس�ȷ����ն�����������");
    ShowStatusText(str.c_str());

    if (!WaitForEnter2Confirm(m_pPropertyDialog->GetHWND()))
    {
        String  str = _T("clear ������ȡ��");
        ShowStatusText(str.c_str());
        return -1;
    }

    if (-1 == UE_ExecuteCommand(
				ClearObjectAttributeCommand::CreateInstance(
					m_pSkinRes, m_pObject, m_pXmlElement)))
    {
        return -1;
    }

    {
        String  str = _T("��ն����������");
        ShowStatusText(str.c_str());
    }

    return 0;
}

long  LayoutPropertyHandler::OnCommand_Insert(const TCHAR* szArg)
{
    return -1;
}
long  LayoutPropertyHandler::OnCommand_InsertBefore(const TCHAR* szArg)
{
    return -1;
}


// TODO: ���������٣�ֱ��if�жϡ��Ժ���������ٿ����Ż�
PROPERTY_COMMAND_TYPE  LayoutPropertyHandler::GetCommandType(String& strCommand)
{
    PROPERTY_COMMAND_TYPE eType = PROPERTY_COMMAND_UNKNOWN;
    if (strCommand.empty())
        return eType;

    std::transform(strCommand.begin(), strCommand.end(), strCommand.begin(), tolower);

    if (strCommand == _T("insert") ||
        strCommand == _T("i")      ||
        strCommand == _T("insertafter"))
    {
        eType = PROPERTY_COMMAND_INSERTAFTER;
    }else
    if (strCommand == _T("insertbefore") ||
        strCommand == _T("ib"))
    {
        eType = PROPERTY_COMMAND_INSERTBEFORE;
    }else
    if (strCommand == _T("add") ||
        strCommand == _T("a"))
    {
        eType = PROPERTY_COMMAND_ADD;
    }else 
    if (strCommand == _T("clear") ||
        strCommand == _T("c"))
    {
        eType = PROPERTY_COMMAND_CLEAR;
    }else
    if (strCommand == _T("delete") ||
        strCommand == _T("d")      ||
        strCommand == _T("remove") ||
        strCommand == _T("r")      ||
        strCommand == _T("erase")  ||
        strCommand == _T("e"))
    {
        eType = PROPERTY_COMMAND_DELETE;
    }else
    if (strCommand == _T("modify") ||
        strCommand == _T("m"))
    {
        eType = PROPERTY_COMMAND_MODIFY;
    }

    return eType;
}
