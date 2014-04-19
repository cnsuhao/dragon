#ifndef ISPLITTERBAR_H_D1385BD4_A556_4219_8ED2_90EA506F41E1
#define ISPLITTERBAR_H_D1385BD4_A556_4219_8ED2_90EA506F41E1
#include "UISDK\Kernel\Inc\Interface\ipanel.h"


namespace UI
{
// 通知外部正在拖拽
#define UI_WM_SPLITTERBAR_POSCHANGED  1404191909

class SplitterBar;
interface UICTRLAPI ISplitterBar : public IPanel
{
    UI_DECLARE_Ixxx_INTERFACE(ISplitterBar, SplitterBar)
};

}

#endif // ISPLITTERBAR_H_D1385BD4_A556_4219_8ED2_90EA506F41E1