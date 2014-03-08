#include "StdAfx.h"
#include "StageLeavesWindow.h"
#include "UISDK\Project\UI3D\inc\ielement.h"
#include "UISDK\Kernel\Inc\Interface\ianimate.h"
#include <math.h>

CStageLeavesWindow::CStageLeavesWindow()
{
 //   m_pStage3D = NULL;

	m_fyWorldDropTo = -2.0f;
	m_fyWolrdDropToFade = -1.8f;
	m_fyWolrdDropFadeHeight = 0.2f;

    srand(GetTickCount());
    UI3D_Init(g_pUIApp);
}

CStageLeavesWindow::~CStageLeavesWindow(void)
{
    UI::IUIApplication*  pUIApp = GetUIApplication();
    UI::IAnimateManager* pAniMgr = pUIApp->GetAnimateMgr();
    pAniMgr->ClearStoryboardOfNotify(this);

    UI::UI3D_Release();
}

int randomInteger(int low, int high)
{
    if (high == low)
        return low;

    int fSub = high - low;
    return low + (int)((abs(sin((float)rand()))*fSub) + 0.5f);
}

float randomFloat(float low, float high)
{
    if (high == low)
        return low;

    float fSub = (high - low);
    return low + abs(sin((float)rand())*fSub);
}



void CStageLeavesWindow::OnInitWindow()
{
    __super::nvProcessMessage(GetCurMsg(), 0, 0);
#if 0
    m_pStage3D = (UI::IStage3D*)FindChildObject(_T("stageparticle"));
    if (!m_pStage3D)
        return;

	float fx = 0, fy = 0, fz = 0;

    String strPath = g_pUIApp->GetActiveSkinRes()->GetPath();
    strPath.append(_T("leaves\\backgroundLeaves.jpg"));

	UI::IImageElement* pBkgndImage = m_pStage3D->AddImageElement();
	pBkgndImage->LoadFromFile(strPath.c_str());

	// ��ȡ3D�������꣬������Ҷdrop����
	float fWidth = 0, fHeight = 0;
	m_pStage3D->GetFullWorldCoord(0, &fWidth, &fHeight);

	const float fLeaveHeight = 1.0f;  // ������Ҷ�߶�

	m_fyWorldDropFrom = fHeight*1.5f;  // ��ǰ��������
	m_fyWorldDropTo = -(fHeight-fLeaveHeight);
	m_fyWolrdDropToFade = m_fyWorldDropTo*0.5f;
	m_fyWolrdDropFadeHeight = abs(m_fyWorldDropTo*0.5f);

	// ������ͼƬ��������3D��ͼ
#if 0
	float fWidth = 0, fHeight = 0;
	m_pStage3D->GetFullWorldCoord(0.00001f, &fWidth, &fHeight);
	pBkgndImage->SetDrawRect(-fWidth, fHeight, fWidth, -fHeight, 0.00001f); // z++��������Ҷ����
#else
	RECT rc = {0, 0, m_pStage3D->GetWidth(), m_pStage3D->GetHeight()};
	pBkgndImage->SetDirectDrawRect(&rc, 1.0f);
#endif
    
	// ���������ת
	m_pStage3D->EnableCamera(false);

	// �����Ҷ
    for (int i = 0; i < 25; i++)
    {
        UI::IImageElement* pImageElement = m_pStage3D->AddImageElement();
        ResetLeaf(pImageElement);
    }
#endif
}

#if 0
void  CStageLeavesWindow::ResetLeaf(UI::IImageElement* pImageElement)
{
    const TCHAR* szSkinResPath = g_pUIApp->GetActiveSkinRes()->GetPath();
    UI::IAnimateManager* pAniMgr = g_pUIApp->GetAnimateMgr();

    TCHAR szTexturePath[MAX_PATH] = {0};
    _stprintf(szTexturePath, _T("%s%s%d.png"), szSkinResPath, _T("leaves\\realLeaf"), randomInteger(1,4));
    pImageElement->LoadFromFile(szTexturePath);

	float fLeaveSize = randomFloat(0.6f, 0.75f);  // ��Ҷ��С

	if (rand()%2)  // �漴��Ҷ�������ң�
		pImageElement->SetDrawRect(0.0f, 0.0f, fLeaveSize, -fLeaveSize, 0.f); // �����Ͻ���ת��������Ƶ���������
	else
		pImageElement->SetDrawRect(fLeaveSize, 0.0f, 0.0f, -fLeaveSize, 0.f); // �����Ͻ���ת��������Ƶ���������

    pImageElement->MoveWorld(randomFloat(-2.0f, 1.0f), 0, 0);
    pImageElement->SetTransparency(1.0f);

    UI::IStoryboard*  pStoryboard = pAniMgr->CreateStoryboard(this, 0, (WPARAM)pImageElement);

    // rotate
    {
        // �������ȼ��٣�Ȼ���ټ��٣��������������ſ���������Щ
        UI::IFloatEasingMove* pMoveAlgo = NULL;
        UI::IFloatTimeline* pTimeline = (UI::IFloatTimeline*)pStoryboard->CreateTimeline(UI::TV_FLOAT, 1, UI::TMA_Easing, (UI::IMoveAlgorithm**)&pMoveAlgo);

        float f1 = 3.14f/180;
        float f50 = f1*50; 
        float f_50 = -f50;

        if (rand()%2)
            pMoveAlgo->SetParam(f50, f_50, (float)randomInteger(4000, 8000), UI::ease_in_out); 
        else
            pMoveAlgo->SetParam(f_50, f50, (float)randomInteger(4000, 8000), UI::ease_in_out); 

        pTimeline->SetAutoReverse(true);
        pTimeline->SetRepeateCount(-1);
    }
    // drop 
    {
        UI::IFloatLinearMove* pMoveAlgo = NULL;
        UI::IFloatTimeline* pTimeline = (UI::IFloatTimeline*)pStoryboard->CreateTimeline(UI::TV_FLOAT, 2, UI::TMA_Linear, (UI::IMoveAlgorithm**)&pMoveAlgo);

        pMoveAlgo->SetParam1(m_fyWorldDropFrom, m_fyWorldDropTo, (float)randomInteger(4000, 8000));  
        pTimeline->SetRepeateCount(-1);
    }

    pStoryboard->Begin();
}

void  CStageLeavesWindow::OnAnimateTick(int nCount, UI::IStoryboard** ppArray)
{
    for (int i = 0; i < nCount; i++)
    {
        UI::IStoryboard* pStory = ppArray[i];
        UI::IImageElement*  pImageElement = (UI::IImageElement*)pStory->GetWParam();

        float fAngle = 0.f;
        ((UI::IFloatTimeline*)pStory->FindTimeline(1))->GetCurrentValue(&fAngle);

        float fDrop = 0;
        ((UI::IFloatTimeline*)pStory->FindTimeline(2))->GetCurrentValue(&fDrop);
        if (fDrop < m_fyWolrdDropToFade)
        {
            float fAlpha = 1-(m_fyWolrdDropToFade - fDrop)/m_fyWolrdDropFadeHeight;
            pImageElement->SetTransparency(fAlpha*fAlpha*fAlpha);
        }
		else
		{
			pImageElement->SetTransparency(1.0f);
		}

        float fOldX = 0.f;
        pImageElement->GetPos(&fOldX, 0, 0);
        pImageElement->MoveWorld(fOldX, fDrop, 0);
        pImageElement->RotateWorld(0, 0, fAngle); 
    }   

    m_pStage3D->UpdateObject();
}
#endif