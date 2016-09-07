#include "stdafx.h"
#include "hscrollbar_creator.h"
#include "UISDK\Control\Inc\Interface\iscrollbar.h"
#include "UISDK\Kernel\Inc\Interface\iscrollbarmanager.h"

#define HSCROLLBAR_ID  TEXT("__hscrollbar")

HScrollbarCreator::HScrollbarCreator()
{
	m_pHScrollbar = NULL;
}
HScrollbarCreator::~HScrollbarCreator()
{
	
}

void  HScrollbarCreator::CreateByEditor(CREATEBYEDITORDATA* pData, IObject* pParent)
{
	EditorAddObjectResData data = {0};
	data.pUIApp = pData->pUIApp;
	data.pParentXml = pData->pXml;
	data.pParentObj = pParent;
	data.bNcChild = true;
	
	data.objiid = uiiidof(ISystemHScrollBar);
	data.ppCreateObj = (void**)&m_pHScrollbar;
	data.szId = HSCROLLBAR_ID;
	pData->pEditor->AddObjectRes(&data);
}

void  HScrollbarCreator::Initialize(IObject* pParent, IScrollBarManager* pMgr)
{
	IObject* pObj = pParent->FindNcChildObject(HSCROLLBAR_ID);
	if (pObj)
	{
		m_pHScrollbar = (ISystemHScrollBar*)pObj->
			QueryInterface(uiiidof(ISystemHScrollBar));
	}

	if (!m_pHScrollbar)
	{
		return;
		// 不自动创建了，全由xml控件吧
// 		IUIApplication* pUIApp = pParent->GetUIApplication();
// 		ISystemVScrollBar::CreateInstance(pUIApp, &m_pHScrollbar);
// 		pParent->AddNcChild(m_pHScrollbar);

	}
	if (m_pHScrollbar)
	{
		m_pHScrollbar->SetVisible(false, false, false); 
		m_pHScrollbar->SetIScrollBarMgr(pMgr);
	}
}