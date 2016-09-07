#pragma once
#include "UISDK\Kernel\Inc\Interface\ixmlwrap.h"
#include "Dialog\Framework\statusbar.h"
#include "Dialog\Framework\MainFrame.h"
#include "Dialog\Framework\childframe.h"
#include "Business\Project\UIEditor\IUIEditorImpl.h"

interface ICommand : public IRootInterface
{
    virtual long   execute() = 0;
    virtual long   undo() = 0;

	virtual long*  GetKey() = 0;
    virtual ULONG  AddRef(void) = 0;
    virtual ULONG  Release(void) = 0;
};

//
//  ��ע��
//    ������У���Ҫ����IUIElement* IListItemBase*ָ�룬��ЩԪ����ʱ����
//    ���ͷŵ���Ȼ�󴴽�һ���µġ��������ŵ�ָ��ֻ��IObject*�����ҲҪע��
//    ��ִ��ɾ���������ʱ����Ҫdelete pObj
//
class CommandBase : public ICommand
{
protected:
    CommandBase()
    {
        m_lRef = 0;
    }

public:
    virtual long execute()
    {
        return 0;
    }
    virtual long undo()
    {
        return 0;
    }

    virtual ULONG  AddRef(void)
    {
        return ++m_lRef;
    }
    virtual ULONG  Release(void)
    {
        --m_lRef; 
        if (0 == m_lRef)
        {
            delete this; 
            return 0;
        } 
        return m_lRef;
    }

    void  SetDirty(IUIElement* pXmlElem)
    {
        // ����dirty����
        if (pXmlElem)
        {
            IUIDocument* pDoc = NULL;
            pXmlElem->GetDocument(&pDoc);
            if (pDoc)
            {
                pDoc->SetDirty(true);
                SAFE_RELEASE(pDoc);
            }
        }

        g_pGlobalData->m_pMainFrame->SetDirty();
    }

    void  SetDirty(IObject* pObject)
    {
        // ����dirty����
        if (!pObject)
            return;

        ObjectEditorData* pData = GetUIEditor()->GetObjectEditorData(pObject);
        if (!pData)
            return;

        SetDirty(pData->pXmlElement);
    }

protected:
    ULONG  m_lRef;
};