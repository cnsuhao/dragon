#include "stdafx.h"
#include "timeout.h"

CTimeout::CTimeout()
{
    m_nTimerId = 0;
    m_thunk.Init(CTimeout::TimerProc, this);

    m_pCallback = NULL;
    m_lId = 0;
    m_wParam = 0;
    m_lParam = 0;
}

CTimeout::~CTimeout()
{
    Cancel();
}


void CALLBACK CTimeout::TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
    CTimeout* pThis = (CTimeout*)hwnd;  // 被thunk修改过了

    KillTimer(NULL, idEvent);
    pThis->m_nTimerId = 0;

    if (pThis)
    {
        pThis->OnTimer(/*NULL, uMsg, idEvent, dwTime*/);
    }
}

void CTimeout::OnTimer()
{
    if (m_pCallback)
    {
        m_pCallback->OnTimeout(m_lId, m_wParam, m_lParam);
    }
}

void  CTimeout::Start(int nElapse)
{
    if (m_nTimerId)
        Cancel();

    m_nTimerId = ::SetTimer(NULL, 0, nElapse, m_thunk.GetTIMERPROC());
}

void  CTimeout::Cancel()
{
    if (m_nTimerId)
    {
        KillTimer(NULL, m_nTimerId);
        m_nTimerId = 0;
    }
}

void  CTimeout::Init(ITimeoutCallback* pCallback)
{
    m_pCallback = pCallback;
}
void  CTimeout::SetParam(long lId, WPARAM wParam, LPARAM lParam)
{
    m_lId = lId;
    m_wParam = wParam;
    m_lParam = lParam;
}