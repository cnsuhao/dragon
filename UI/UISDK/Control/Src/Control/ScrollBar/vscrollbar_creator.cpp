#include "stdafx.h"
#include "vscrollbar_creator.h"
#include "UISDK\Control\Inc\Interface\iscrollbar.h"
#include "UISDK\Kernel\Inc\Interface\iscrollbarmanager.h"

#define VSCROLLBAR_ID  TEXT("__vscrollbar")

VScrollbarCreator::VScrollbarCreator()
{
	m_pVScrollbar = NULL;
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
	data.ppCreateObj = (void**)&m_pVScrollbar;
	data.szId = VSCROLLBAR_ID;
	pData->pEditor->AddObjectRes(&data);
}

void  VScrollbarCreator::Initialize(IObject* pParent, IScrollBarManager* pMgr)
{
	IObject* pObj = pParent->FindNcChildObject(VSCROLLBAR_ID);
	if (pObj)
	{
		m_pVScrollbar = (ISystemVScrollBar*)pObj->
			QueryInterface(uiiidof(ISystemVScrollBar));
	}

	if (!m_pVScrollbar)
	{
		return;
		// 不自动创建了，全由xml控件吧
// 		IUIApplication* pUIApp = pParent->GetUIApplication();
// 		ISystemVScrollBar::CreateInstance(pUIApp, &m_pVScrollbar);
// 		pParent->AddNcChild(m_pVScrollbar);

	}
	if (m_pVScrollbar)
	{
		m_pVScrollbar->SetVisible(false, false, false); 
		m_pVScrollbar->SetIScrollBarMgr(pMgr);
	}
}