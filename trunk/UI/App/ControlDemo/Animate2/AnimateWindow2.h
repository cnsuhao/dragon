#pragma once
#include "UISDK\Kernel\Inc\Interface\iwindow.h"
#include "UISDK\Control\Inc\Interface\ilistbox.h"

#define PAGE_COUNT  6     // 页面数
#define ANIMATE_COUNT  2  // 动画数

#define TIMELINE_ID_CUR   0
#define TIMELINE_ID_NEXT  1

//
// 仿http://tympanus.net/Development/PageTransitions/
// 实现各种动画效果
//
namespace UI
{
    interface IButton;
    interface IStoryboard;
    interface IListBox;
}

class CAnimateWindow2 : public UI::IWindow
{
public:
    CAnimateWindow2();
	~CAnimateWindow2(void);

 	UI_BEGIN_MSG_MAP_Ixxx(CAnimateWindow2)
        UIMSG_WM_INITIALIZE(OnInitWindow)
        UIMSG_BN_CLICKED2(m_pBtnShowNextTransition, OnClickShowNextTransition)
        UIMSG_WM_NOTIFY(UI_LCN_CLICK, OnListBoxClick)
        UIMSG_WM_SIZE(OnSize)
        UIMSG_WM_ANIMATE_TICK(OnAnimateTick)
        UIMSG_WM_ANIMATE_OVER(OnAnimateOver)
 	UI_END_MSG_MAP_CHAIN_PARENT(IWindow)

 	void  OnInitWindow();
    void  OnAnimateTick(int nCount, UI::IStoryboard** ppArray);
    void  OnAnimateOver();
    void  OnSize(UINT nType, int cx, int cy);


    void  OnClickShowNextTransition();
    LRESULT  OnListBoxClick(WPARAM w, LPARAM l);

    void  nextPage(long lType);

private:
    SIZE  m_sizeWindow;

    UI::IListBox*  m_pMenu1;
    UI::IButton*   m_pBtnShowNextTransition;

    long      m_nCurrentPage;
    IPanel*   m_pPages[PAGE_COUNT];
    IPanel*   m_pCurPage;
    IPanel*   m_pNextPage;

    bool      m_isAnimating;
    long      m_nAnimateCursor;  // 当前使用的动画索引，用于next时遍历
};


