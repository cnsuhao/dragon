#include "stdafx.h"
#include "privatechatbiz.h"
#include "App\IM\include\task.h"
#include "../UI/privatechatui.h"

CPrivateChatBiz::CPrivateChatBiz()
{
    m_pFrameWork = IM::GetFramework();
}
CPrivateChatBiz::~CPrivateChatBiz()
{
}

long  CPrivateChatBiz::ExecuteCommand( long nCommand, WPARAM wParam, LPARAM lParam )
{
    if (nCommand == IM::BASE_CMD_ID_EXIT)
    {
//         IM::ITask* pTask = (IM::ITask*)wParam;
//         switch (pTask->GetCommandId())
//         {
//        
//         }
    }
	else if (nCommand == IM::BASE_CMD_ID_INIT)
	{
#ifdef _DEBUG // Test, 直接加载CHAT模块
		HMODULE hModule = GetModuleHandle(_T("Main.dll"));
		if (!hModule)
		{
			IM::OpenPrivateChatDlgReqTask*  pTask = new IM::OpenPrivateChatDlgReqTask(NULL, L"000");
			pTask->m_lOpenMode = 0;
			pTask->m_wParam = 0;
			pTask->m_lParam = 0;
			OpenPrivateChatDlg(pTask);
		}
#endif
	}

    return 0;
}


long  CPrivateChatBiz::OpenPrivateChatDlg(IM::OpenPrivateChatDlgReqTask* p)
{
    p->AddRef();
    IM::GetFramework()->PostFunction(IM::Ui, IM::create_mem_fun1_pkg(&CPrivateChatUI::OnOpenPrivateChatDlg, g_pPrivateChatUI, p), false);

    SAFE_RELEASE(p);
    return 0;
}
