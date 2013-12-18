#include "StdAfx.h"
#include "REShadow.h"
#include "../resource.h"
#include <time.h>
#include "UISDK\Control\Inc\Interface\ilabel.h"
#include "UISDK\Kernel\Inc\Interface\iobject3dwrap.h"

REShadowWindow::REShadowWindow()
{
    m_pHour = NULL;
    m_pMin = NULL;
    m_pSec = NULL;
    m_nMinRotate = m_nSecRotate = m_nHourRotate = 0;
}

REShadowWindow::~REShadowWindow(void)
{
}

void REShadowWindow::OnInitWindow()
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

    SetTimer(GetHWND(), 1, 200, NULL);
    //CheckTime();
}

void  REShadowWindow::CheckTime()
{
    time_t	t;
    tm      s_tm;

    time( &t );
    localtime_s( &s_tm, &t );

}
void  REShadowWindow::OnTimer(UINT_PTR nIDEvent, LPARAM lParam)
{
    m_nHourRotate += 1;
    m_nMinRotate += 2;
    m_nSecRotate += 5;

    if (m_pHour)
    {
        m_pHour->Get3DWrap()->Rotate(0, 0, m_nHourRotate);
    }
    if (m_pMin)
    {
        m_pMin->Get3DWrap()->Rotate(0, 0, m_nMinRotate);
    }
    if (m_pSec)
    {
        m_pSec->Get3DWrap()->Rotate(0, 0, m_nSecRotate);
    }

    this->UpdateObject();
    return;
}
