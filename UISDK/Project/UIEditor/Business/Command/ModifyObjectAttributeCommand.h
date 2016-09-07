#pragma once
#include "Base/icommand.h"
#include "UISDK\Kernel\Inc\Interface\ixmlwrap.h"
#include "Dialog\Framework\statusbar.h"
#include "Dialog\Framework\MainFrame.h"
#include "Business\Project\UIEditor\IUIEditorImpl.h"

class ModifyObjectAttributeCommand2 : public CommandBase
{
public:
    static ICommand*  CreateInstance(IObject* pObject, LPCTSTR szKey, LPCTSTR szNewValue)
    {
        if (!pObject || !szKey)
            return NULL;

        ObjectEditorData*  pData = GetUIEditor()->GetObjectEditorData(pObject);
        if (!pData || !pData->pAttributeProxy)
            return NULL;

        ModifyObjectAttributeCommand2* p = new ModifyObjectAttributeCommand2;
        p->AddRef();
        p->m_pObject = pObject;
        p->m_strKey = szKey;

        if (szNewValue)
        {
            p->m_bNewNull = false;
            p->m_strNewValue = szNewValue;
        }
        else
        {
            p->m_bNewNull = true;
        }

        LPCTSTR  szOldValue = pData->pAttributeProxy->GetAttribute(szKey);
        if (szOldValue)
        {
            p->m_bOldNull = false;
            p->m_strOldValue = szOldValue;
        }
        else
        {
            p->m_bOldNull = true;
        }
        return p;
    }

    ModifyObjectAttributeCommand2()
    {
        m_pObject = NULL;
        m_bNewNull = true;
        m_bOldNull = true;
    }

    virtual long execute()
    {
        ObjectEditorData*  pData = GetUIEditor()->GetObjectEditorData(m_pObject);
        UIASSERT (pData && pData->pAttributeProxy);

		LPCTSTR szValue = m_bNewNull ? NULL:m_strNewValue.c_str();
        pData->pAttributeProxy->SetAttribute(m_strKey.c_str(), szValue);
        on_post_do(pData->pXmlElement, szValue);
        return 0;
    }

    virtual long undo()
    {
        ObjectEditorData*  pData = GetUIEditor()->GetObjectEditorData(m_pObject);
        UIASSERT (pData && pData->pAttributeProxy);

		LPCTSTR szValue = m_bOldNull ? NULL:m_strOldValue.c_str();
        pData->pAttributeProxy->SetAttribute(m_strKey.c_str(), szValue);
        on_post_do(pData->pXmlElement, szValue);
        return 0;
    }

    long*  GetKey(void)
    {
        UIASSERT(m_pObject);
        IWindowBase*  pWindow = m_pObject->GetWindowObject();
        if (pWindow)
            return (long*)pWindow;
        else
            return (long*)m_pObject;
    }
    void  on_post_do(IUIElement* pElement, LPCTSTR szValue)
    {
        // ����dirty����
        SetDirty(m_pObject);

		// ����key: ���¼������ԣ����µ�ǰ�������ԡ�Ȼ���ٱ��浽xml��
		if (m_strKey == XML_STYLECLASS)
		{
			// ���ٵ�ǰ��attributeProxy�������Ե�һЩ_this��Ա�����ͷ�
			GetUIEditor()->DestroyObjectAttributeProxy(m_pObject);

			IMapAttribute* pMapAttr = NULL;
			pElement->GetAttribList(&pMapAttr);
			pMapAttr->AddAttr(XML_STYLECLASS, szValue);

			m_pObject->ParseStyleAndSetAttribute(pMapAttr, true);
			SAFE_RELEASE(pMapAttr);
		}
        // ����idʱ����Ҫͬ�������Ĺ����б�
        else if (m_strKey == XML_ID)
        {
            g_pGlobalData->m_pProjectTreeDialog->UpdatLayoutObjectTreeItemText(m_pObject);
        }

		// ����xml����
		GetUIEditor()->SaveAttribute2Xml(m_pObject);

		// ˢ�½���
		UE_UpdateAttributeInPropertyCtrl(m_pObject, XML_STYLECLASS);

        UE_RefreshUIWindow(m_pObject);
        UE_RefreshLayoutView(m_pObject);

		{
			String  str = _T("�����޸�: ");
			str.append(m_strKey.c_str());
			str.append(_T(" = \""));
			str.append(szValue);
			str.append(_T("\""));
			g_pGlobalData->m_pStatusBar->SetStatusText2(str.c_str());
		}
    }

private:
    IObject*  m_pObject;

    String  m_strKey;
    String  m_strNewValue;
    String  m_strOldValue;
    bool  m_bNewNull;
    bool  m_bOldNull;
};
