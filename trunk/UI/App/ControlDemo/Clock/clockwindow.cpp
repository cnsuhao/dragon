#include "StdAfx.h"
#include "clockwindow.h"
#include "../Other/resource.h"
#include <time.h>
#include "UISDK\Control\Inc\Interface\ilabel.h"
#include "UISDK\Kernel\Inc\Interface\iobject3dwrap.h"

ClockWindow::ClockWindow()
{
    m_pHour = NULL;
    m_pMin = NULL;
    m_pSec = NULL;
    m_nMinRotate = m_nSecRotate = m_nHourRotate = 0;
}

ClockWindow::~ClockWindow(void)
{
}

void ClockWindow::OnInitWindow()
{
    __super::nvProcessMessage(GetCurMsg(), 0, 0);

    m_pHour = (UI::IPictureCtrl*)FindChildObject(_T("hour"));
    m_pMin  = (UI::IPictureCtrl*)FindChildObject(_T("min"));
    m_pSec  = (UI::IPictureCtrl*)FindChildObject(_T("sec"));

    if (m_pHour)
    {
        m_pHour->Begin3D();
        m_pHour->Get3DWrap()->Rotate(0, 0, 30);
    }
    if (m_pMin)
    {
        m_pMin->Begin3D();
        m_pMin->Get3DWrap()->Rotate(0, 0, 300);
    }
    if (m_pSec)
    {
        m_pSec->Begin3D();
        m_pSec->Get3DWrap()->Rotate(0, 0, 100);
    }

    SetTimer(GetHWND(), 1, 50, NULL);
    CheckTime();
}

void  ClockWindow::CheckTime()
{
    time_t	t;
    tm      s_tm;

    time( &t );
    localtime_s( &s_tm, &t );

    // 由时间计算各各指针的旋转角度，满圈360度
    // 秒针:
    int nSecRotate = s_tm.tm_sec * 6;
    // 分针:
    int nMinRotate = s_tm.tm_min * 6;
    // 时针： 满圈是12*60分钟也就是720分钟1分钟走0.5度
    int nHourRotate = ((s_tm.tm_hour%12)*60 + s_tm.tm_min) >> 1;

    bool bNeedUpdate = false;
    if (nSecRotate != m_nSecRotate && m_pSec)
    {
        m_nSecRotate = nSecRotate;
        m_pSec->Get3DWrap()->Rotate(0, 0, nSecRotate);
        bNeedUpdate = true;
    }

    if (nMinRotate != m_nMinRotate && m_pMin)
    {   
        m_nMinRotate = nMinRotate;
        m_pMin->Get3DWrap()->Rotate(0, 0, m_nMinRotate);        
        bNeedUpdate = true;
    }

    if (nHourRotate != m_nHourRotate && m_pHour)
    {
        m_nHourRotate = nHourRotate;
        m_pHour->Get3DWrap()->Rotate(0, 0, m_nHourRotate);
        bNeedUpdate = true;
    }

    if (bNeedUpdate)
        this->UpdateObject();
}
void  ClockWindow::OnTimer(UINT_PTR nIDEvent, LPARAM lParam)
{
    CheckTime();
    return;
}
