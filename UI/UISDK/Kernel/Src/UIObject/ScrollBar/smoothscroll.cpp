#include "stdafx.h"
#include "smoothscroll.h"
#include "UISDK\Kernel\Inc\Interface\ianimate.h"

SmoothScroll::SmoothScroll()
{
    m_pCallback = NULL;
    m_pUIApplication = NULL;

    m_bEnable = true;
    m_eMouseWheelDir = DOWN;
    m_pAnimateStoryboard = NULL;

	m_nScrollDistance = 0;
    m_nPrevScrollDistance = 0;
	m_nStep = 200;
	m_nDuration = 600;
}
SmoothScroll::~SmoothScroll()
{
    StopScroll();
}

void  SmoothScroll::SetUIApplication(IUIApplication* p)
{
    m_pUIApplication = p;
}
void  SmoothScroll::SetCallback(ISmoothScrollCallback* pCallback)
{
    m_pCallback = pCallback;
}

bool  SmoothScroll::IsEnable()
{
    return m_bEnable;
}
void  SmoothScroll::SetEnable(bool b)
{
    m_bEnable = b;
}

bool  SmoothScroll::IsScrolling()
{
    return m_pAnimateStoryboard == NULL ? false:true;
}

// nViewPage: 当前页面的范围。一次mousewheel滚动范围最好不好起过一屏，否则会导致几个列表项总是滚不到
void  SmoothScroll::AddPower(int zDelta, int nViewPage)
{
    if (0 == zDelta) 
        return;

	MOUSEWHEEL_DIR eDir = zDelta < 0 ? DOWN:UP;
    if (m_eMouseWheelDir != eDir && IsScrolling())
        StopScroll();

    m_eMouseWheelDir = eDir;
    zDelta = abs(zDelta);

    int nMouseWheelDelta = zDelta/WHEEL_DELTA;
    int nStep = m_nStep;
    if (nViewPage > 0)
        nStep = min(m_nStep, nViewPage);

	// 上次剩余的加上本次新加的
    m_nScrollDistance = (m_nScrollDistance - m_nPrevScrollDistance) + nMouseWheelDelta * nStep;
	m_nPrevScrollDistance = 0;

	bool bIsScrolling = IsScrolling();
    CreateAnimate();

	// 是第一次开始滚动
    if (!bIsScrolling && m_pCallback)
    {
        m_pCallback->SmoothScroll_Start();
    }
}

void  SmoothScroll::CreateAnimate()
{
	if (!m_pUIApplication)
		return;

	// 使用in out类型，会导致动画过程中再触发mousewheel后，前后动画不连贯
	// ease_out，感觉在最后减速阶段还是减的太快了些
	// ease_out_sine ease_out_quad还行

#define EASETYPE  ease_out_sine

    if (m_pAnimateStoryboard)
	{ 
		// 当前正在滚动中，直接重置滚动参数
		IIntTimeline* pTimeline = static_cast<IIntTimeline*>(m_pAnimateStoryboard->FindTimeline(0));
		pTimeline->SetEaseParam(0, m_nScrollDistance, m_nDuration, EASETYPE);  
	}
	else
	{
		IAnimateManager*  pAnimateMgr = m_pUIApplication->GetAnimateMgr();
		m_pAnimateStoryboard = pAnimateMgr->CreateStoryboard(this->GetIMessage());
		IIntTimeline*  pTimeline = m_pAnimateStoryboard->CreateIntTimeline(0);
		pTimeline->SetEaseParam(0, m_nScrollDistance, m_nDuration, EASETYPE);
		m_pAnimateStoryboard->Begin();
	}

}

void  SmoothScroll::DestroyAnimate()
{
    if (m_pUIApplication && m_pAnimateStoryboard)
    {
        IAnimateManager* pAnimateMgr = m_pUIApplication->GetAnimateMgr();

        pAnimateMgr->RemoveStoryboard(m_pAnimateStoryboard);
        m_pAnimateStoryboard = NULL;
    }
}


void  SmoothScroll::StopScroll()
{
    if (!IsScrolling())
        return;
   
    DestroyAnimate();
    OnScrollStop();
}

// 1. 主动调用StopScroll触发
// 2. 动画结束触发
void  SmoothScroll::OnScrollStop()
{
	m_nScrollDistance = 0;
	m_nPrevScrollDistance = 0;

	if (m_pCallback)
	{
		m_pCallback->SmoothScroll_Stop();
	}
}

void  SmoothScroll::OnAnimateTick(int nCount, IStoryboard** ppArray)
{
    UIASSERT(nCount == 1 && ppArray[0] == m_pAnimateStoryboard);
	bool bFinish = m_pAnimateStoryboard->IsFinish();

	do 
	{
		int nCurValue = 0;
		m_pAnimateStoryboard->GetTimeline(0)->GetCurrentValue(&nCurValue);

		// 本次需要滚动的距离
		int nScrollNow = nCurValue - m_nPrevScrollDistance;
		m_nPrevScrollDistance = nCurValue;

		if (0 == nScrollNow)
			break;

		if (m_eMouseWheelDir == UP)
			nScrollNow = -nScrollNow;

		SmoothScrollResult lResult = INERTIA_SCROLL_STOP;
		if (m_pCallback)
		{
			lResult = m_pCallback->SmoothScroll_Scroll(nScrollNow, false);
		}

		if (INERTIA_SCROLL_STOP == lResult || INERTIA_SCROLL_OVERFLOW == lResult)
		{
			StopScroll();
			break;
		}
	}
	while (0);

	if (bFinish)
	{
		m_pAnimateStoryboard = NULL;

		OnScrollStop();
	}

}
