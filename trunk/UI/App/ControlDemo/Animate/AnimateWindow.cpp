#include "StdAfx.h"
#include "AnimateWindow.h"
#include "UISDK\Kernel\Inc\Interface\ianimate.h"
#include "UISDK\Control\Inc\Interface\ibutton.h"

CAnimateWindow::CAnimateWindow()
{
    m_pButton = NULL;
}

CAnimateWindow::~CAnimateWindow(void)
{
    UI::IUIApplication*  pUIApp = GetUIApplication();
    UI::IAnimateManager* pAniMgr = pUIApp->GetAnimateMgr();
    pAniMgr->ClearStoryboardOfNotify(this);
}

void CAnimateWindow::OnInitWindow()
{
    __super::nvProcessMessage(GetCurMsg(), 0, 0);
    m_pButton = (UI::IButton*)FindChildObject(_T("button1"));

}


void  CAnimateWindow::OnBnClick()
{
    UI::IUIApplication*  pUIApp = GetUIApplication();
    UI::IAnimateManager* pAniMgr = pUIApp->GetAnimateMgr();
    
    UI::IStoryboard*  pStoryboard = pAniMgr->CreateStoryboard();
    pStoryboard->SetNotifyObj(this);
    pStoryboard->SetId(1);

    // x
    {
        UI::IIntLinearMove* pMoveAlgo = NULL;
        UI::IIntTimeline* pTimeline = (UI::IIntTimeline*)pStoryboard->CreateTimeline(UI::TV_INT, 1, UI::TMA_Linear, (UI::IMoveAlgorithm**)&pMoveAlgo);
        pMoveAlgo->SetParam1(10, 1000, 3000); 
        pTimeline->SetTimeType(UI::TT_BY_MS);
        pTimeline->SetRepeateCount(5);
        //pTimeline->SetAutoReverse(true);
    }
    // y
    {
        UI::IIntEasingMove* pMoveAlgo = NULL;
        UI::IIntTimeline* pTimeline = (UI::IIntTimeline*)pStoryboard->CreateTimeline(UI::TV_INT, 2, UI::TMA_Easing, (UI::IMoveAlgorithm**)&pMoveAlgo);
        pMoveAlgo->SetParam(10, 400, 3000, UI::ease_out_bounce); 
        pTimeline->SetTimeType(UI::TT_BY_MS);
        pTimeline->SetRepeateCount(5);
        //pTimeline->SetAutoReverse(true);
    } 

    pStoryboard->Begin();
    m_pButton->SetEnable(false);
}

void  CAnimateWindow::OnAnimateOver()
{
    m_pButton->SetEnable(true);
}
void  CAnimateWindow::OnAnimateTick(int nCount, UI::IStoryboard** ppArray)
{
    for (int i = 0; i < nCount; i++)
    {
        UI::IStoryboard* pStoryboard = ppArray[i];
        switch (pStoryboard->GetId())
        {
        case 1:
            {
                UI::IIntTimeline*  pTimelinex = (UI::IIntTimeline*)pStoryboard->FindTimeline(1);
                UI::IIntTimeline*  pTimeliney = (UI::IIntTimeline*)pStoryboard->FindTimeline(2);

                int xPos = 10;
                if (pTimelinex)
                    pTimelinex->GetCurrentValue(&xPos);

                int yPos = 10;
                if (pTimeliney)
                    pTimeliney->GetCurrentValue(&yPos);

                m_pButton->SetObjectPos(xPos, yPos, 0, 0, SWP_NOSIZE);
            }
            break;
        }
    }
}