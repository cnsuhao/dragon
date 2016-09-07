#pragma once
#include "Base\icommand.h"
#include "Business\uieditorfunction.h"

// ɾ��һ���ؼ�
class RemoveObjectCommand : public CommandBase
{
public:
	RemoveObjectCommand()
	{
		m_pRemoveElem = NULL;
		m_pRemoveObject = NULL;
		m_pWindow = NULL;
		m_pParentObj = NULL;
        m_pPrevObj = NULL;

        m_bNeedDeleteObject = false;
	}
	~RemoveObjectCommand()
	{
	    SAFE_RELEASE(m_pRemoveElem);

        if (m_bNeedDeleteObject && m_pRemoveObject)
        {
            SAFE_DELETE_Ixxx(m_pRemoveObject);
        }
	}

	long*  GetKey(void)
	{
		UIASSERT(m_pWindow);
		return (long*)m_pWindow;
	}

	static ICommand*  CreateInstance(IObject* pObj)
	{
		if (NULL == pObj)
			return NULL;

		// ������Ȳ�֧�ִ��ڵ�ɾ��
		if (!pObj->GetParentObject())
			return NULL;

		RemoveObjectCommand* p = new RemoveObjectCommand;
		p->AddRef();
		p->m_pRemoveObject = pObj;
		p->m_pRemoveElem = GetUIEditor()->GetObjectXmlElem(pObj);
		p->m_pRemoveElem->AddRef();  // �����m_uieditor.OnObjectDeleteInd�л��ͷ�elem�����⣬�ౣ��һ������
		p->m_pWindow = pObj->GetWindowObject();
		p->m_pParentObj = pObj->GetParentObject();
        p->m_pPrevObj = pObj->GetPrevObject();

		return p;
	}
	virtual long execute()
	{
		// 1. �ӱ༭����ɾ��
		GetProjectData()->m_uieditor.OnObjectDeleteInd(m_pRemoveObject);

		// 2. �Ӹ��������Ƴ�
		m_pParentObj->RemoveChildInTree(m_pRemoveObject);

		// 3. ��xml���Ƴ�
        IUIElement*  pParentElem = GetUIEditor()->GetObjectXmlElem(m_pParentObj);
		pParentElem->RemoveChild(m_pRemoveElem);

		// ˢ��layoutview
		CChildFrame*  pChildFrame = g_pGlobalData->m_pMainFrame->GetLayoutChildFrameByObject(m_pParentObj);
		if (pChildFrame)
		{
			::InvalidateRect(pChildFrame->m_pClientView->GetHWND(), NULL, TRUE);
		}

        m_bNeedDeleteObject = true;
		on_post_do();
		return 0;
	}
	virtual long undo()
	{
        IUIElement*  pPrevElem = NULL;
        if (m_pPrevObj)
        {
            pPrevElem = GetUIEditor()->GetObjectXmlElem(m_pPrevObj);
        }
        IUIElement*  pParentElem = GetUIEditor()->GetObjectXmlElem(m_pParentObj);


		// ��ӻ�ȥ
		m_pParentObj->InsertChild(m_pRemoveObject, m_pPrevObj);
		pParentElem->AddChildAfter(m_pRemoveElem, pPrevElem);

		// ˢ��
		m_pParentObj->UpdateMyLayout(false);
		GetProjectData()->m_uieditor.OnObjectAttributeLoad(m_pRemoveObject, m_pRemoveElem);

        // ���ص��༭������
        IListItemBase*  pInsertAfter = NULL;
        if (m_pPrevObj)
            pInsertAfter = GetUIEditor()->GetObjectHTREEITEM(m_pPrevObj);
        if (!pInsertAfter)
            pInsertAfter = UITVI_FIRST;

        IListItemBase* pParentItem = GetProjectData()->m_uieditor.GetObjectHTREEITEM(m_pParentObj);
		g_pGlobalData->m_pProjectTreeDialog->InsertLayoutChildObjectNode(
            m_pRemoveObject, 
            pParentItem, 
            pInsertAfter);
        g_pGlobalData->m_pProjectTreeDialog->LoadObjectChildNode(m_pRemoveObject);

		// ˢ��layoutview
		CChildFrame*  pChildFrame = g_pGlobalData->m_pMainFrame->GetLayoutChildFrameByObject(m_pParentObj);
		if (pChildFrame)
		{
			// ѡ���´����Ķ���
			ILayoutMDIClientView* pView = (ILayoutMDIClientView*)pChildFrame->m_pClientView->QueryInterface(IID_ILayoutMDIClientView);
			pView->SelectObject(m_pRemoveObject);
		}

        m_bNeedDeleteObject = false;
		on_post_do();
		return 0;
	}

private:
	void  on_post_do()
	{
        // ����dirty����
        SetDirty(m_pRemoveElem);

        // ǿ��ˢ�´���
        UE_RefreshUIWindow(m_pParentObj);
        UE_RefreshLayoutView(m_pParentObj);
	}
public: 
	IObject*        m_pRemoveObject;
	IUIElement*     m_pRemoveElem;
	IObject*        m_pParentObj;
    IObject*        m_pPrevObj;
	IWindowBase*    m_pWindow;

    bool   m_bNeedDeleteObject;   // ��ִ��ɾ������ʱ����������ɾ������ֻ�ǴӸ��������Ƴ����ѡ������Ҫ�ж�������Ƿ���Ҫ�ͷŸö���
};