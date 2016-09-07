#include "stdafx.h"
#include "vscrollbar_creator.h"
#include "UISDK\Control\Inc\Interface\iscrollbar.h"
#include "UISDK\Kernel\Inc\Interface\iscrollbarmanager.h"

#define VSCROLLBAR_ID  TEXT("__vscrollbar")

VScrollbarCreator::VScrollbarCreator()
{
	m_pVScrollBar = NULL;
}
VScrollbarCreator::~VScrollbarCreator()
{
	
}

void  VScrollbarCreator::CreateByEditor(CREATEBYEDITORDATA* pData, IObject* pParent)
{
	EditorAddObjectResData data = {0};
	data.pUIApp = pData->pUIApp;
	data.pParentXml = pData->pXml;
	data.pParentObj = pParent;
	data.bNcChild = true;
	
	data.objiid = uiiidof(ISystemVScrollBar);
	data.ppCreateObj = (void**)&m_pVScrollBar;
	data.szId = VSCROLLBAR_ID;
	pData->pEditor->AddObjectRes(&data);
}

void  VScrollbarCreator::Initialize(IObject* pParent, IScrollBarManager* pMgr)
{
	IObject* pObj = pParent->FindNcChildObject(VSCROLLBAR_ID);
	if (pObj)
	{
		m_pVScrollBar = (ISystemVScrollBar*)pObj->
			QueryInterface(uiiidof(ISystemVScrollBar));
	}

	if (!m_pVScrollBar)
	{
		return;
		// ���Զ������ˣ�ȫ��xml�ؼ���
// 		IUIApplication* pUIApp = pParent->GetUIApplication();
// 		ISystemVScrollBar::CreateInstance(pUIApp, &m_pVScrollBar);
// 		pParent->AddNcChild(m_pVScrollBar);

	}
	if (m_pVScrollBar)
	{
		m_pVScrollBar->SetVisible(false, false, false); 
		m_pVScrollBar->SetIScrollBarMgr(pMgr);
	}
}