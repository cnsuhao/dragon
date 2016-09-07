#pragma once
#include "Base/icommand.h"
#include "UISDK\Kernel\Inc\Interface\ixmlwrap.h"
#include "Dialog\Framework\statusbar.h"
#include "Dialog\Framework\MainFrame.h"

class ClearObjectAttributeCommand : public CommandBase
{
public:
    ClearObjectAttributeCommand() 
    {
        pAttribList = NULL;
    }
    ~ClearObjectAttributeCommand() 
    {
        SAFE_RELEASE(pAttribList);
    }

    static ICommand*  CreateInstance(ISkinRes* pSkinRes, IObject* pObject, IUIElement* pUIElement)
    {
        if (NULL == pSkinRes || 
            NULL == pObject  ||
            NULL == pUIElement)
            return NULL;

        ClearObjectAttributeCommand* p = new ClearObjectAttributeCommand;
        p->AddRef();

        p->pSkinRes = pSkinRes;
        p->pObject = pObject;
        p->pUIElement = pUIElement;

        // ��ȡ�����б�
        pUIElement->GetAttribList2(&p->pAttribList);
        return p;
    }
    virtual long execute()
    {
        if (false == pUIElement->ClearAttrib())
        {
            String  str = _T("��ն�������ʧ��");
            g_pGlobalData->m_pStatusBar->SetStatusText2(str.c_str());
            return false;
        }

        // ͬ�������Կؼ�
        // TODO:
        UIASSERT(0);
//         if (g_pGlobalData->m_pPropertyDialog->m_pObject == this->pObject)
//         {
//             g_pGlobalData->m_pPropertyDialog->ClearAttribInCtrl();
//         }

        on_post_do();
        return 0;
    }
    virtual long undo()
    {
        if (!pAttribList)
            return -1;

        pAttribList->BeginEnum();

        const TCHAR* szKey = NULL;
        const TCHAR* szValue = NULL;
        while (pAttribList->EnumNext(&szKey, &szValue))
        {
            if (false == pUIElement->AddAttrib(szKey, szValue))
            {
                String  str = _T("�������ʧ��: ");
                str.append(szKey);
                str.append(_T(" = \""));
                str.append(szValue);
                str.append(_T("\""));
                g_pGlobalData->m_pStatusBar->SetStatusText2(str.c_str());
                return -1;
            }

            // ͬ�������Կؼ�
            // TODO:
            UIASSERT(0);
//             if (g_pGlobalData->m_pPropertyDialog->m_pObject == this->pObject)
//             {
//                 g_pGlobalData->m_pPropertyDialog->AddAttributeInCtrl(szKey, szValue);
//             }
        }
        pAttribList->EndEnum();
        
        on_post_do();
        return 0;
    }

	long*  GetKey(void)
	{
		UIASSERT(pObject);
		IWindowBase*  pWindow = pObject->GetWindowObject();
		if (pWindow)
			return (long*)pWindow;
		else
			return (long*)pObject;
	}

private:
    void  on_post_do()
    {
        // ����dirty����
        SetDirty(pUIElement);

        // ˢ�½���
        UE_ReloadObjectAttribute(pSkinRes, pObject, pUIElement);
        UE_RefreshLayoutView(pObject);
    }

public:
    ISkinRes*     pSkinRes;
    IObject*      pObject;
    IUIElement*   pUIElement;
    IListAttribute*  pAttribList;
};