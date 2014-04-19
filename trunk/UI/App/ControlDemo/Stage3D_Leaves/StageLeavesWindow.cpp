#include "StdAfx.h"
#include "StageLeavesWindow.h"
#include "UISDK\Project\UI3D\inc\ielement.h"
#include "UISDK\Kernel\Inc\Interface\ianimate.h"
#include "UISDK\Control\Inc\Interface\ilabel.h"
#include <math.h>
#include "UISDK\Kernel\Inc\Interface\iimagerender.h"
#include "UISDK\Kernel\Inc\Interface\ilayout.h"

CStageLeavesWindow::CStageLeavesWindow()
{
    m_pStage3D = NULL;
    m_pLeavesPanel = NULL;

	m_fyWorldDropTo = -2.0f;
	m_fyWolrdDropToFade = -1.8f;
	m_fyWolrdDropFadeHeight = 0.2f;

    for (int i = 0; i < LEAVES_COUNT; i++)
    {
        m_pImgs[i] = NULL;
    }

    srand(GetTickCount());
//    UI3D_Init(g_pUIApp, true);
}

CStageLeavesWindow::~CStageLeavesWindow(void)
{
    UI::IUIApplication*  pUIApp = GetUIApplication();
    UI::IAnimateManager* pAniMgr = pUIApp->GetAnimateMgr();
    pAniMgr->ClearStoryboardOfNotify(this);

//    UI::UI3D_Release();
}

int randomInteger(int low, int high)
{
    if (high == low)
        return low;

    int fSub = high - low;
    return low + round((abs(sin((float)rand()))*fSub));
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

    m_pLeavesPanel = (UI::IPanel*)this->GetChildObject();
    m_pImgs[0] = (UI::IPictureCtrl*)FindChildObject(_T("img1"));
    if (m_pImgs[0])
    {
        int i = 0; 
        while (m_pImgs[i]->GetNextObject())
        {
            if (i >= LEAVES_COUNT-1)
                break;

            m_pImgs[i+1] = (UI::IPictureCtrl*)m_pImgs[i]->GetNextObject();
            i++;
        }
    }

//     m_pStage3D = (UI::IStage3D*)FindChildObject(_T("stageparticle"));
//     if (!m_pStage3D)
//         return;
// 
// 	float fx = 0, fy = 0, fz = 0;
// 
//     String strPath = g_pUIApp->GetActiveSkinRes()->GetPath();
//     strPath.append(_T("leaves\\backgroundLeaves.jpg"));
// 
//
    // 修改为使用Panel直接显示背景图片
// 	UI::IImageElement* pBkgndImage = m_pStage3D->AddImageElement();
// 	pBkgndImage->LoadFromFile(strPath.c_str());
// 
// 	// 获取3D场景坐标，计算树叶drop区域
// 	float fWidth = 0, fHeight = 0;
// 	m_pStage3D->GetFullWorldCoord(0, &fWidth, &fHeight);
// 
// 	const float fLeaveHeight = 1.0f;  // 假设树叶高度
// 
// 	m_fyWorldDropFrom = fHeight*1.5f;  // 提前掉下来。
// 	m_fyWorldDropTo = -(fHeight-fLeaveHeight);
// 	m_fyWolrdDropToFade = m_fyWorldDropTo*0.5f;
// 	m_fyWolrdDropFadeHeight = abs(m_fyWorldDropTo*0.5f);
// 
// 	// 将背景图片铺满整个3D视图
// #if 0
// 	float fWidth = 0, fHeight = 0;
// 	m_pStage3D->GetFullWorldCoord(0.00001f, &fWidth, &fHeight);
// 	pBkgndImage->SetDrawRect(-fWidth, fHeight, fWidth, -fHeight, 0.00001f); // z++，放在树叶后面
// #else
// 	RECT rc = {0, 0, m_pStage3D->GetWidth(), m_pStage3D->GetHeight()};
// 	pBkgndImage->SetDirectDrawRect(&rc, 1.0f);
// #endif
    
	// 禁用相机旋转
//	m_pStage3D->EnableCamera(false);
// 
//     UI::IRectangleElement*  pRectElem = m_pStage3D->AddRectangleElement();
//     UI::RECTF rcf;
//     rcf.Set(-1.0f, 1.0f, 1.0f, -1.0f);
//     pRectElem->SetRect(&rcf, 0.f);

    m_fyWorldDropFrom = -300;
    m_fyWorldDropTo = 600;
    m_fyWolrdDropToFade = 500;
    m_fyWolrdDropFadeHeight = 100;

	// 添加树叶
    for (int i = 0; i < LEAVES_COUNT; i++)
    {
        ResetLeaf(m_pImgs[i]);
    }

    // 刷新树叶布局
    m_pLeavesPanel->GetLayout()->Arrange(NULL, false);
}

void  CStageLeavesWindow::ResetLeaf(UI::IPictureCtrl* pImg)
{
    if (!pImg)
        return;

    //////////////////////////////////////////////////////////////////////////
    // 1. 设置树叶图片
    {
        const TCHAR* szSkinResPath = g_pUIApp->GetActiveSkinRes()->GetPath();
        UI::IImageRes* pImgRes = g_pUIApp->GetActiveSkinImageRes();
        UI::GRAPHICS_RENDER_LIBRARY_TYPE  eGrphicsType = this->GetIWindowRender()->GetGraphicsRenderType();

        UI::IRenderBase*   pBkRenderBase = pImg->GetBkRender();
        UI::IImageRender*  pBkImgRender = (UI::IImageRender*)pBkRenderBase->QueryInterface(UI::uiiidof(IImageRender));
        if (!pBkImgRender)
            return;

        UI::IRenderBitmap*  pBitmap = NULL;
        TCHAR szImgId[MAX_PATH] = {0};
        _stprintf(szImgId, _T("leaf%d"), randomInteger(1,4));

        pImgRes->GetBitmap(szImgId, eGrphicsType, &pBitmap);
        if (NULL == pBitmap)
            return;

        pBkImgRender->SetRenderBitmap(pBitmap);
        SAFE_RELEASE(pBitmap);
    }
    //////////////////////////////////////////////////////////////////////////
    // 2. 设置树叶位置
    {
        pImg->SetConfigTop(/*-100*/randomInteger(0, 400));
        pImg->SetConfigLeft(randomInteger(0, 500));
    }

    //////////////////////////////////////////////////////////////////////////
    // 3. 设置树叶动画
    {
        UI::IAnimateManager* pAniMgr = g_pUIApp->GetAnimateMgr();
        UI::IStoryboard*  pStoryboard = pAniMgr->CreateStoryboard(this, 0, (WPARAM)pImg);

        // rotate
        {
            // 必须是先加速，然后再减速，这样反复动画才看起来连贯些
            UI::IFloatEasingMove* pMoveAlgo = NULL;
            UI::IFloatTimeline* pTimeline = (UI::IFloatTimeline*)pStoryboard->CreateTimeline(UI::TV_FLOAT, 1, UI::TMA_Easing, (UI::IMoveAlgorithm**)&pMoveAlgo);

            float f50 = 50; 
            float f_50 = -f50;

            if (rand()%2)
            {
                pMoveAlgo->SetParam(f50, f_50, (float)randomInteger(4000, 8000), UI::ease_in_out); 
            }
            else
            {
                // TODO: 将树叶反向

                pMoveAlgo->SetParam(f_50, f50, (float)randomInteger(4000, 8000), UI::ease_in_out); 
            }

            pTimeline->SetAutoReverse(true);
            pTimeline->SetRepeateCount(-1);
            pImg->GetRenderLayer2()->SetRotateCenterPos(UI::ROTATE_CENTER_TOPLEFT, 0, 0);
        }
        // drop and fade
        {
            UI::IFloatLinearMove* pMoveAlgo = NULL;
            UI::IFloatTimeline* pTimeline = (UI::IFloatTimeline*)pStoryboard->CreateTimeline(UI::TV_FLOAT, 2, UI::TMA_Linear, (UI::IMoveAlgorithm**)&pMoveAlgo);

            pMoveAlgo->SetParam1(m_fyWorldDropFrom, m_fyWorldDropTo, (float)randomInteger(5000, 11000));  
            pTimeline->SetRepeateCount(-1);
        }

        pStoryboard->Begin();
    }

#if 0  // 3d旋转，废弃
    // UI::IImageElement* pImageElement

    const TCHAR* szSkinResPath = g_pUIApp->GetActiveSkinRes()->GetPath();
    UI::IAnimateManager* pAniMgr = g_pUIApp->GetAnimateMgr();

    TCHAR szTexturePath[MAX_PATH] = {0};
    _stprintf(szTexturePath, _T("%s%s%d.png"), szSkinResPath, _T("leaves\\realLeaf"), randomInteger(1,4));
    pImageElement->LoadFromFile(szTexturePath);

	float fLeaveSize = randomFloat(0.6f, 0.75f);  // 树叶大小

	if (rand()%2)  // 随即树叶朝向（左、右）
		pImageElement->SetDrawRect(0.0f, 0.0f, fLeaveSize, -fLeaveSize, 0.f); // 绕左上角旋转，将其绘制到第四象限
	else
		pImageElement->SetDrawRect(fLeaveSize, 0.0f, 0.0f, -fLeaveSize, 0.f); // 绕左上角旋转，将其绘制到第四象限

    pImageElement->MoveWorld(randomFloat(-2.0f, 1.0f), 0, 0);
    pImageElement->SetTransparency(1.0f);

    UI::IStoryboard*  pStoryboard = pAniMgr->CreateStoryboard(this, 0, (WPARAM)pImageElement);

    // rotate
    {
        // 必须是先加速，然后再减速，这样反复动画才看起来连贯些
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
#endif
}

void  CStageLeavesWindow::OnAnimateTick(int nCount, UI::IStoryboard** ppArray)
{
    for (int i = 0; i < nCount; i++)
    {
        UI::IStoryboard* pStory = ppArray[i];
        UI::IPictureCtrl*  pImg = (UI::IPictureCtrl*)pStory->GetWParam();
        UI::IRenderLayer2*  pLayer = pImg->GetRenderLayer2();

        float fAngle = 0.f;
        ((UI::IFloatTimeline*)pStory->FindTimeline(1))->GetCurrentValue(&fAngle);
        pLayer->Rotate2D(round(fAngle));

        float fDrop = 0;
        ((UI::IFloatTimeline*)pStory->FindTimeline(2))->GetCurrentValue(&fDrop);
        pImg->SetConfigTop(round(fDrop));

        if (fDrop > m_fyWolrdDropToFade)
        {
            float fAlpha = 1-(fDrop - m_fyWolrdDropToFade)/m_fyWolrdDropFadeHeight;
            pLayer->SetOpacity(round(255*fAlpha));
        }
        else
        {
            pLayer->SetOpacity(255);
        }
    }

    if (m_pLeavesPanel)
    {
        m_pLeavesPanel->GetLayout()->Arrange(NULL);
        m_pLeavesPanel->UpdateObject(true);
    }
    

#if 0
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
#endif
}
