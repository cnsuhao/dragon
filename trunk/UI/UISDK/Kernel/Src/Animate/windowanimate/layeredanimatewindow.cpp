#include "stdafx.h"
#include "layeredanimatewindow.h"
#include "UISDK\Kernel\Inc\Interface\ianimate.h"
#include "UISDK\Kernel\Src\UIObject\Window\windowbase.h"
#include "UISDK\Kernel\Src\Animate\windowanimate\windowanimatebase.h"
#include "UISDK\Kernel\Src\RenderLayer2\layer\windowrender.h"

namespace UI
{

UI_IMPLEMENT_Ixxx_INTERFACE(ILayeredAnimateWindow, LayeredAnimateWindow, Message);

LayeredAnimateWindow::LayeredAnimateWindow()
{
    m_pWindowAnimate = NULL;
    m_lOldExStyle = -1;
    m_lOldStyle = -1;
    m_pILayeredAnimateWindow = NULL;
}
LayeredAnimateWindow::~LayeredAnimateWindow()
{
    // ��ԭ�ֲ�����
    if (-1 != m_lOldExStyle)
    {
        SetWindowLong(m_hHostWnd, GWL_EXSTYLE, m_lOldExStyle);
        SetWindowLong(m_hHostWnd, GWL_STYLE, m_lOldStyle);

        // ���ȡ���ֲ㴰�����Ժ󣬴��ڱ�ڵ�����
        HDC hDC = ::GetDC(m_hHostWnd);
        RECT rc = {0,0, m_pWindow->GetWidth(), m_pWindow->GetHeight()};
        m_pWindow->DrawMemBitmap(hDC, &rc, false);
        ReleaseDC(m_hHostWnd, hDC);
    }

    //	m_pWindow->UpdateObject(); // �п����Ƕ����ĵ�һ����������˲���������
}
void LayeredAnimateWindow::Soul_Attach(WindowBase* pWnd)
{
    if (NULL == pWnd)
        return;

    m_pWindow = pWnd;
    m_hHostWnd = pWnd->GetHWND();

    // ��ֹ���������������������Ӱ�춯����ʾ
    m_pWindow->GetWindowRender()->SetCanCommit(false);

    pWnd->AddHook(this->GetIMessage(), 0, 0);

    if (m_pWindowAnimate->GetWndTransparentType() & WINDOW_TRANSPARENT_TYPE_LAYERED)
    {

    }
    else
    {
        m_lOldExStyle = ::GetWindowLong(m_hHostWnd, GWL_EXSTYLE);
        m_lOldStyle = ::GetWindowLong(m_hHostWnd, GWL_STYLE);
        InitLayered();
    }
    ReSize();
}

// �ڶ�������������������Ϣ�������ж��������Ϣ
BOOL LayeredAnimateWindow::nvProcessMessage(UIMSG* pMsg, int nMsgMapID, bool bDoHook)
{
    if (pMsg->message <= WM_MOUSEFIRST && pMsg->message >= WM_MOUSELAST)
        return TRUE;
    if (pMsg->message <= WM_KEYFIRST && pMsg->message >= WM_KEYLAST)
        return TRUE;

    switch (pMsg->message)
    {
    case UI_WM_ANIMATE_TICK:
        {
            int nCount = pMsg->wParam;
            IStoryboard** ppStoryboardArray = (IStoryboard**)pMsg->lParam;

            m_pWindowAnimate->OnTick(nCount, ppStoryboardArray);
            this->Commit2LayeredWindow();

            // ������������ˣ��򾡿��ͷŶԴ�����Ϣ�����ء����ⴰ������ӦTick Finishʱ�����µĶ�����
            // ����ܶ���Ϣ�������ˣ���QueryInterface IID_ICustomWindow��ȡ����͸�����ͣ�
            
            bool bAllFinish = true;
            for (int i = 0; i < nCount; i++)
            {
                if (!ppStoryboardArray[i]->IsFinish())
                {
                    bAllFinish = false;
                    break;
                }
            }
            if (bAllFinish)
            {
                m_pWindow->RemoveHook(this->GetIMessage());
                m_pWindow->GetWindowRender()->SetCanCommit(true);
            }
            return FALSE;
        }
        break;

//     case UI_WM_ANIMATE_OVER:
//         {
//             return FALSE;
//         }
        break;

        // �п��ܶ���ʱ������һЩ������ˢ�£�������������Ϣ�����´��ڻ��治����
//     case WM_ERASEBKGND:
//     case WM_PAINT:
//         return FALSE;
    }
    return FALSE;
}


}