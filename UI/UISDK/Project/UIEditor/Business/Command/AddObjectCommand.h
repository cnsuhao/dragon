#pragma once
#include "Base\icommand.h"
#include "UISDK\Kernel\Inc\Interface\ixmlwrap.h"
#include "Dialog\Framework\statusbar.h"
#include "Dialog\Framework\MainFrame.h"
#include "Business\uieditorfunction.h"
#include "UISDK\Kernel\Inc\Interface\ilayout.h"


// ���һ���ؼ�
class AddObjectCommand : public CommandBase
{
public:
    AddObjectCommand()
    {
        m_pCreateObj = NULL;
        m_pParent = NULL;
        m_pCreateElem = NULL;
        m_bNeedDeleteObject = false;
        m_nConfigLeft = NDEF;
        m_nConfigTop = NDEF;
    }
    ~AddObjectCommand()
    {
        SAFE_RELEASE(m_pCreateElem);

        if (m_bNeedDeleteObject && m_pCreateObj)
        {
            SAFE_DELETE_Ixxx(m_pCreateObj);
        }
    }

    // ����ÿؼ�λ��canvas�����еĻ�������ָ��nLeft,nTopλ�ã�-1��ʾ������
    static ICommand*  CreateInstance(ISkinRes* pSkinRes, IObject* pParent, const TCHAR* szObjName, int nLeft=NDEF, int nTop=NDEF)
    {
        if (NULL == pParent  ||
            NULL == szObjName)
            return NULL;

        AddObjectCommand* p = new AddObjectCommand;
        p->AddRef();
        p->m_pParent = pParent;
        p->m_strObjName = szObjName;
        p->m_nConfigLeft = nLeft;
        p->m_nConfigTop = nTop;

        return p;
    }
    virtual long execute()
    {
		IListItemBase*  pItem = GetUIEditor()->GetObjectHTREEITEM(m_pParent);
		IUIElement*     pParentElem = GetUIEditor()->GetObjectXmlElem(m_pParent);
		
		if (!pItem || !pParentElem)
			return -1;

        if (NULL == m_pCreateObj)
		{
            m_pCreateObj = GetProjectData()->m_pEditUIApp->CreateInstanceByName(m_strObjName.c_str());
		}
        m_pParent->AddChild(m_pCreateObj);

		pParentElem->AddChild(m_strObjName.c_str(), &m_pCreateElem);


		// ֪ͨ���󴴽�����ʼ��һЩ���������С���Ӷ����
		CREATEBYEDITORDATA  data = {0};
		data.pUIApp = GetEditUIApplication();
		data.pXml = m_pCreateElem;
		data.pEditor = GetUIEditor();
		SetRect(&data.rcInitPos, m_nConfigLeft, m_nConfigTop, m_nConfigLeft, m_nConfigTop);
		UISendMessage(m_pCreateObj, UI_WM_CREATEBYEDITOR, (WPARAM)&data, 0,0,0, UIALT_CALLLESS);

		UIMSG msg1, msg2;
		msg1.message = UI_WM_INITIALIZE;
		msg2.message = UI_WM_INITIALIZE2;
		UISendMessage(m_pCreateObj, UI_WM_INITIALIZE);
		m_pCreateObj->ForwardMessageToChildObject2(&msg1, &msg2);
		UISendMessage(m_pCreateObj, UI_WM_INITIALIZE2);

		// �����ʼ��(Ŀǰֻ֧��canvas����)
		m_pCreateObj->CreateLayoutParam();
		ILayoutParam*  pLayoutParam = m_pCreateObj->GetLayoutParam();
		if (pLayoutParam && pLayoutParam->GetLayoutType() == LAYOUT_TYPE_CANVAS)
		{
			ICanvasLayoutParam*  pCanvasLayoutParam = static_cast<ICanvasLayoutParam*>(pLayoutParam);
			pCanvasLayoutParam->SetConfigLeft(data.rcInitPos.left);
			pCanvasLayoutParam->SetConfigTop(data.rcInitPos.top);
			pCanvasLayoutParam->SetConfigWidth(RECTW(data.rcInitPos));
			pCanvasLayoutParam->SetConfigHeight(RECTH(data.rcInitPos));
		}

		GetProjectData()->m_uieditor.OnObjectAttributeLoad(m_pCreateObj, m_pCreateElem);
		GetUIEditor()->SaveAttribute2Xml(m_pCreateObj);

		// TODO: ����UI_WM_INITIALIZE��Ϣ
		// TODO: �����ӿؼ�

		g_pGlobalData->m_pProjectTreeDialog->InsertLayoutChildObjectNode(m_pCreateObj, pItem);
		g_pGlobalData->m_pProjectTreeDialog->LoadObjectChildNode(m_pCreateObj);

		// ǿ��ˢ�´���
		UE_RefreshUIWindow(m_pParent);
		UE_RefreshLayoutView(m_pParent);

		// ѡ���´����Ķ���
		CChildFrame*  pChildFrame = g_pGlobalData->m_pMainFrame->GetLayoutChildFrameByObject(m_pParent);
		if (pChildFrame)
		{
			ILayoutMDIClientView* pView = (ILayoutMDIClientView*)pChildFrame->m_pClientView->QueryInterface(IID_ILayoutMDIClientView);
			pView->SelectObject(m_pCreateObj);
		}

        m_bNeedDeleteObject = false;
        on_post_do();
        return 0;
    }
    virtual long undo()
    {
		GetUIEditor()->OnObjectDeleteInd(m_pCreateObj);

        IUIElement*  pParentElem = GetUIEditor()->GetObjectXmlElem(m_pParent);

        m_pParent->RemoveChildInTree(m_pCreateObj);
        pParentElem->RemoveChild(m_pCreateElem);
        SAFE_RELEASE(m_pCreateElem);

        //SAFE_DELETE_Ixxx(m_pCreateObj);  // ����ɾ�����������������������������iobjectָ��

        UE_RefreshUIWindow(m_pParent);
        UE_RefreshLayoutView(m_pParent);

        m_bNeedDeleteObject = true;
        on_post_do();
        return 0;
    }

	long*  GetKey(void)
	{
		UIASSERT(m_pParent);
		IWindowBase*  pWindow = m_pParent->GetWindowObject();
		if (pWindow)
			return (long*)pWindow;
		else
			return (long*)m_pParent;
	}

private:
    void  on_post_do()
    {
        // ����dirty����
        if (m_pCreateElem)
        {
            SetDirty(m_pCreateElem);
        }
    }

public:
    IObject*      m_pParent;
    String        m_strObjName;
    IObject*      m_pCreateObj;
    IUIElement*   m_pCreateElem;
    int           m_nConfigLeft;
    int           m_nConfigTop;

    bool          m_bNeedDeleteObject;
};

