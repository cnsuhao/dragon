#pragma once
#include "UISDK/Kernel/Src/Base/Message/message.h"

//////////////////////////////////////////////////////////////////////////
//              惯性滚动
//
//  旧的滚动逻辑，已废弃：
//   1. 以m_nScroll_V0作为初速度做减速运动，最终速度到达0停止
//   2. 每一次MOUSEWHEEL时，将速度重置为m_nScroll_V0
//   3. 将滚动能量作为路程
//   4. 每一次MOUSEWHEEL时，累加所需要的路程
//   5. 时间不需要，2as=v^2，没有时间参数
//
//  新的滚动逻辑：
//   1. 采用减速的缓动算法
//   2. 每次滚动时间固定
//   3. 滚动过程中如果再次触发mousewheel，则更新动画的from/to/duration参数（不是销毁再创建）
//
//////////////////////////////////////////////////////////////////////////

// TODO: 仿ios边界弹性滚动动画 m_bAnimateOverflow
//  2014/8/27 
//  . 在动画过程中点出鼠标左键，会停止当前滚动。如何处理
//  . 如何判断弹回来结束动画
//  . 其它细节

// bouncy edge

namespace UI
{

interface  IStoryboard;

enum SmoothScrollResult
{
    INERTIA_SCROLL_STOP = 0,      // 立即结束
    INERTIA_SCROLL_CONTINUE = 1,  // 继续滚动
    INERTIA_SCROLL_OVERFLOW = 2,  // 溢出了
};

interface ISmoothScrollCallback
{
    virtual  void SmoothScroll_Start() = 0;
    virtual  void SmoothScroll_Stop() = 0;
    virtual  SmoothScrollResult SmoothScroll_Scroll(
				int nDeltaPos, bool bAllowOverflow) = 0;
};

class SmoothScroll : public Message
{
public:
    SmoothScroll();
    ~SmoothScroll();

    UI_BEGIN_MSG_MAP
        UIMSG_WM_ANIMATE_TICK(OnAnimateTick)
    UI_END_MSG_MAP

public:
    void  SetUIApplication(IUIApplication* p);
    void  SetCallback(ISmoothScrollCallback* pCallback);
    bool  IsEnable();
    void  SetEnable(bool b);
    bool  IsScrolling();
    void  AddPower(int zDelta, int nViewPage);
    void  StopScroll();

protected:
    void  OnAnimateTick(int nCount, IStoryboard** ppArray);
	void  OnScrollStop();
    void  CreateAnimate();
    void  DestroyAnimate();

private:
    IUIApplication*  m_pUIApplication;
    ISmoothScrollCallback*  m_pCallback;

	// 从下至下滚动，还是从下至上
	enum MOUSEWHEEL_DIR { DOWN, UP};
	MOUSEWHEEL_DIR  m_eMouseWheelDir;

    bool  m_bEnable;
    IStoryboard*  m_pAnimateStoryboard;

	int  m_nStep;         // 每个mousewheel滚动距离
	int  m_nDuration;     // 每次滚动时长
    int  m_nScrollDistance;     // 本次滚动距离
	int  m_nPrevScrollDistance; // 记录上一次ontick通知的s进度，与这一次的s进度相减，得出本次需要移动的距离
};

}