#ifndef ISPLITTERBAR_H_D1385BD4_A556_4219_8ED2_90EA506F41E1
#define ISPLITTERBAR_H_D1385BD4_A556_4219_8ED2_90EA506F41E1
#include "UISDK\Kernel\Inc\Interface\ipanel.h"


namespace UI
{

enum SPLITTERBAR_POSCHANGING_RESULT
{
    SPLITTERBAR_NOTHANDLED = 0,
    SPLITTERBAR_HANDLED = 1,
    SPLITTERBAR_HANDLED_AND_STOP_DRAG = 2
};

// 通知外部正在拖拽
// wParam: POINT*  鼠标在parent上的位置
// lParam: POINT*  本次拖拽目标位置
// return: SPLITTERBAR_POSCHANGING_RESULT
#define SPLITTERBAR_NM_POSCHANGING  1404191908

// 通知外部拖拽位置改变
#define SPLITTERBAR_NM_POSCHANGED  1404191909

class SplitterBar;
interface UICTRLAPI ISplitterBar : public IPanel
{
    UI_DECLARE_Ixxx_INTERFACE(ISplitterBar, SplitterBar)

    void  UpdateControlPosBySplitterBarCurPos(bool bRedraw);

    int   GetLeftMin();
    int   GetLeftMax();
    int   GetRightMin();
    int   GetRightMax();
    int   GetUpMin();
    int   GetUpMax();
    int   GetDownMin();
    int   GetDownMax();
};

}

#endif // ISPLITTERBAR_H_D1385BD4_A556_4219_8ED2_90EA506F41E1