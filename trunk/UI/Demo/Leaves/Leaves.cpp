// Leaves.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "Leaves.h"
#include <shlwapi.h>

#include "UISDK\Kernel\Inc\Base\inc.h"
#include "UISDK\Control\Inc\Base\inc.h"
#include "UISDK\Kernel\Inc\Interface\iwindow.h"
#include "UISDK\Control\Inc\Interface\ilabel.h"
#include "UISDK\Kernel\Inc\Interface\irenderlayer.h"
#include "UISDK\Kernel\Inc\Interface\ianimate.h"
#include "UISDK\Kernel\Inc\Interface\ilayout.h"
#include "UISDK\Kernel\Inc\Interface\iimagerender.h"

#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "uisdk.lib")
#pragma comment(lib, "uictrls.lib")
#pragma warning(disable:4996)

UI::IUIApplication*  g_pUIApplication = NULL;

// 树叶数量
#define LEAVES_COUNT  5
#define TIMELINE_INDEX_DROPFADE  0
#define TIMELINE_INDEX_ROTATE    1

class DemoWindow : public UI::IWindow
{
public:
	DemoWindow()
	{
        m_pLeavesPanel = NULL;

        m_fyWorldDropTo = -2.0f;
        m_fyWolrdDropToFade = -1.8f;
        m_fyWolrdDropFadeHeight = 0.2f;

        for (int i = 0; i < LEAVES_COUNT; i++)
        {
            m_pImgs[i] = NULL;
        }

        srand(GetTickCount());
	}

	UI_BEGIN_MSG_MAP_Ixxx(DemoWindow)
		UIMSG_WM_ANIMATE_TICK(OnAnimateTick)
		UIMSG_WM_DESTROY(OnDestroy)
		UIMSG_WM_INITIALIZE(OnInitialize)
	UI_END_MSG_MAP_CHAIN_PARENT(IWindow);

private:
	void OnInitialize()
	{
        this->SetHardComposite(true);

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

        m_fyWorldDropFrom = -300;  
        //m_fyWorldDropFrom = 0;   // 测试
        m_fyWorldDropTo = 600;
        m_fyWolrdDropToFade = 500;
        m_fyWolrdDropFadeHeight = 100;

        // 添加树叶
        for (int i = 0; i < LEAVES_COUNT; i++)
        {
            reset_leaf(m_pImgs[i]);
        }

        // 刷新树叶布局
        m_pLeavesPanel->GetLayout()->Arrange(NULL, false);
	}

	void OnDestroy()
	{
		g_pUIApplication->GetAnimateMgr()->ClearStoryboardOfNotify(this);
		PostQuitMessage(0); 
	}

	void  OnAnimateTick(int nCount, UI::IStoryboard** ppArray)
	{
        for (int i = 0; i < nCount; i++)
        {
            UI::IStoryboard* pStory = ppArray[i];
            UI::IPictureCtrl*  pImg = (UI::IPictureCtrl*)pStory->GetWParam();
            UI::IRenderLayer*  pLayer = pImg->GetSelfRenderLayer2();

            float fDrop = 0;
            ((UI::IFloatTimeline*)pStory->GetTimeline(TIMELINE_INDEX_DROPFADE))->GetCurrentValue(&fDrop);

            float fAngle = 0.f;
            ((UI::IFloatTimeline*)pStory->GetTimeline(TIMELINE_INDEX_ROTATE))->GetCurrentValue(&fAngle);

            if (pLayer)
            {
                UI::IRenderLayerTransform3D*  pTransform3d = pLayer->GetTransform3D(true);
                pTransform3d->translate3d((float)pImg->GetConfigLeft(), fDrop, 0);
                pTransform3d->rotate3d(0, 0, fAngle);
            }

            //         int  nTop = (int)fDrop;
            //         float fOffset = fDrop - (float)nTop;
            // 
            //         pImg->SetConfigTop(nTop);
            //         if (pLayer)
            //         {
            //             pLayer->SetPosFloatDeviation(0, fOffset);
            // 
            //             if (fDrop > m_fyWolrdDropToFade)
            //             {
            //                 float fAlpha = 1-(fDrop - m_fyWolrdDropToFade)/m_fyWolrdDropFadeHeight;
            //                 pLayer->SetOpacity(round(255*fAlpha));
            //             }
            //             else
            //             {
            //                 pLayer->SetOpacity(255);
            //             }
            //         }
            // 
            //         if (pLayer)
            //         {
            //             float fAngle = 0.f;
            //             ((UI::IFloatTimeline*)pStory->GetTimeline(TIMELINE_INDEX_ROTATE))->GetCurrentValue(&fAngle);
            //             pLayer->Rotate2D(fAngle);
            //         }
        }

        if (IsHardComposite())
        {
            DirectComposite();
        }
        else
        {
            m_pLeavesPanel->UpdateObject();
        }
	}

private:
    void  reset_leaf(UI::IPictureCtrl* pImg)
    {
        if (!pImg)
            return;

        //////////////////////////////////////////////////////////////////////////
        // 1. 设置树叶图片
        {
            const TCHAR* szSkinResPath = g_pUIApplication->GetDefaultSkinRes()->GetPath();
            UI::IImageRes* pImgRes = g_pUIApplication->GetActiveSkinImageRes();
            UI::GRAPHICS_RENDER_LIBRARY_TYPE  eGrphicsType = this->GetIWindowRender()->GetGraphicsRenderType();

            UI::IRenderBase*   pBkRenderBase = pImg->GetBkRender();
            UI::IImageRender*  pBkImgRender = (UI::IImageRender*)pBkRenderBase->QueryInterface(uiiidof(IImageRender));
            if (!pBkImgRender)
                return;

            UI::IRenderBitmap*  pBitmap = NULL;
            TCHAR szImgId[MAX_PATH] = {0};
            _stprintf(szImgId, _T("leaf%d"), random_integer(1,4));

            pImgRes->GetBitmap(szImgId, eGrphicsType, &pBitmap);
            if (NULL == pBitmap)
                return;

            pBkImgRender->SetRenderBitmap(pBitmap);
            SAFE_RELEASE(pBitmap);
        }
        //////////////////////////////////////////////////////////////////////////
        // 2. 设置树叶位置
        {
            //pImg->SetConfigTop(randomInteger(0, 400));  // 测试
            pImg->SetConfigTop(-100);
            pImg->SetConfigLeft(random_integer(0, 400));

            pImg->SetConfigTop(100);
            pImg->SetConfigLeft(100);
        }

        //////////////////////////////////////////////////////////////////////////
        // 3. 设置树叶动画
        {
            UI::IAnimateManager* pAniMgr = g_pUIApplication->GetAnimateMgr();
            UI::IStoryboard*  pStoryboard = pAniMgr->CreateStoryboard(this, 0, (WPARAM)pImg);

            // drop and fade
            {
                UI::IFloatTimeline* pTimeline = pStoryboard->CreateFloatTimeline(TIMELINE_INDEX_DROPFADE);
                //pTimeline->SetTimeType(UI::TT_BY_FRAME);

                pTimeline->SetLinerParam1(m_fyWorldDropFrom, m_fyWorldDropTo, (float)random_integer(5000, 11000));  
                pTimeline->SetRepeateCount(-1);
            }

            // rotate
            {
                // 必须是先加速，然后再减速，这样反复动画才看起来连贯些
                UI::IFloatTimeline* pTimeline = pStoryboard->CreateFloatTimeline(TIMELINE_INDEX_ROTATE);
                //pTimeline->SetTimeType(UI::TT_BY_FRAME);

#define PI ((float)3.141592654f)
                // float f50 = 50;           // 软件使用了角度
                float f50 = 50 /** PI / 180*/;   // 硬件加速使用了弧度
                float f_50 = -f50;

                if (rand()%2)
                {
                    pTimeline->SetEaseParam(f50, f_50, (float)random_integer(4000, 8000), UI::ease_in_out); 
                }
                else
                {
                    // TODO: 将树叶反向

                    pTimeline->SetEaseParam(f_50, f50, (float)random_integer(4000, 8000), UI::ease_in_out); 
                }

                pTimeline->SetAutoReverse(true);
                pTimeline->SetRepeateCount(-1);
                //TODO: pImg->GetRenderLayer2()->SetRotateCenterPos(UI::ROTATE_CENTER_TOPLEFT, 0, 0);

                UI::IRenderLayerTransform3D*  pTransform3d = pImg->GetRenderLayer2()->GetTransform3D(true);
                pTransform3d->set_transform_rotate_origin(
                    UI::TRANSFORM_ROTATE_ORIGIN_LEFT, 0, 
                    UI::TRANSFORM_ROTATE_ORIGIN_TOP, 0, 0);
            }


            pStoryboard->Begin();
        }
    }

    int random_integer(int low, int high)
    {
        if (high == low)
            return low;

        int fSub = high - low;
        return low + round((abs(sin((float)rand()))*fSub));
    }

    float random_float(float low, float high)
    {
        if (high == low)
            return low;

        float fSub = (high - low);
        return low + abs(sin((float)rand())*fSub);
    }


private:
    UI::IPictureCtrl*  m_pImgs[LEAVES_COUNT];
    UI::IPanel*  m_pLeavesPanel;

    float  m_fyWorldDropFrom;        // 树叶开始降落的高度
    float  m_fyWorldDropTo;          // 树叶降落到的价格
    float  m_fyWolrdDropToFade;      // 树叶开始渐隐的坐标
    float  m_fyWolrdDropFadeHeight;  // 树叶渐隐的高度
};

int APIENTRY _tWinMain(
					   HINSTANCE hInstance,
					   HINSTANCE hPrevInstance,
					   LPTSTR    lpCmdLine,
					   int       nCmdShow)
{

	TCHAR szDir[MAX_PATH] = {0};
	GetModuleFileName(GetModuleHandle(NULL), szDir, MAX_PATH);
	TCHAR* p = _tcsrchr(szDir, '\\');
	if(p != NULL)
		*(p+1) = 0;

	{
		TCHAR szUIDllPath[MAX_PATH] = {0};
		TCHAR szUIDllPath2[MAX_PATH] = {0};
		_tcscpy(szUIDllPath, szDir);
		_tcscat(szUIDllPath, TEXT("..\\..\\UI"));

		PathCanonicalize(szUIDllPath2, szUIDllPath);
		SetDllDirectory(szUIDllPath2);
	}

	CreateUIApplicationInstance(&g_pUIApplication);
	UICtrl_RegisterUIObject(g_pUIApplication);

	{
		TCHAR szLogXml[MAX_PATH];
		_tcscpy(szLogXml, szDir);
		_tcscat(szLogXml, TEXT("uilog.xml"));
		g_pUIApplication->LogUI(szLogXml);

		String skinPath(szDir);
		skinPath.append(TEXT("Default"));
		g_pUIApplication->LoadSkinRes(skinPath.c_str());
	}

#if 1  // 启用硬件合成 
	g_pUIApplication->EnableGpuComposite();
#endif

	DemoWindow* pWnd = NULL;
	DemoWindow::CreateInstance(g_pUIApplication->GetDefaultSkinRes(), &pWnd);
	if (pWnd->Create(TEXT("DemoWnd")))
	{
		pWnd->ShowWindow();
		g_pUIApplication->MsgHandleLoop();
	}
	SAFE_DELETE_Ixxx(pWnd);

	g_pUIApplication->Release();
	return 0;
}