#include "StdAfx.h"
#include "Direct2DWindow.h"
#include "UISDK\Project\UI3D\inc\ielement.h"

CDirect2DWindow::CDirect2DWindow()
{
    UI3D_Init(g_pUIApp, false);
}

CDirect2DWindow::~CDirect2DWindow(void)
{
    UI::UI3D_Release();
}

void CDirect2DWindow::OnInitWindow()
{
    __super::nvProcessMessage(GetCurMsg(), 0, 0);

#if 0
// 	const TCHAR* szSkinResPath = g_pUIApp->GetActiveSkinRes()->GetPath();
// 	if (m_pStage3D)
// 	{
// 		// 移动chair，使得老虎刚好能够站在凳子上面
// 		TCHAR szXPath[MAX_PATH] = {0};
// 		_tcscpy(szXPath, szSkinResPath);
// 		_tcscat(szXPath, _T("x\\chair.x"));
// 		UI::IMeshElement*  pMeshElementChair = m_pStage3D->AddMeshElement();
// 		pMeshElementChair->LoadFromFile(szXPath);
// 		pMeshElementChair->MoveWorld(0.2f, -3.2f, 0.0f);
// 
// 		_tcscpy(szXPath, szSkinResPath);
// 		_tcscat(szXPath, _T("x\\tiger.x"));
// 		UI::IMeshElement*  pMeshElementTiger = m_pStage3D->AddMeshElement();
// 		pMeshElementTiger->LoadFromFile(szXPath);
// 	}
#endif
}
