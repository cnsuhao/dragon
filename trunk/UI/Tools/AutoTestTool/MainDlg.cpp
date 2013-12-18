#include "StdAfx.h"
#include "MainDlg.h"

CMainDlg::CMainDlg(void)
{
}

CMainDlg::~CMainDlg(void)
{
}

void CMainDlg::OnClose()
{
	EndDialog(0);
}

BOOL CMainDlg::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
	m_EditPosX.Attach(GetDlgItem(IDC_EDIT_POS_X));
	m_EditPosY.Attach(GetDlgItem(IDC_EDIT_POS_Y));
    m_comboType.Attach(GetDlgItem(IDC_COMBO_TYPE));

	m_btnCheckMouseMove.Attach(GetDlgItem(IDC_CHECK_MOUSEMOVE));
	m_btnCheckLButtonDown.Attach(GetDlgItem(IDC_CHECK_LBUTTONDOWN));
	m_btnCheckLButtonUp.Attach(GetDlgItem(IDC_CHECK_LBUTTONUP));
    m_btnCheckMouseLeave.Attach(GetDlgItem(IDC_CHECK_MOUSELEAVE));;
    m_btnCheckLButtonDBClick.Attach(GetDlgItem(IDC_CHECK_LBUTTONDBCLICK));;
    m_btnCheckRButtonDown.Attach(GetDlgItem(IDC_CHECK_RBUTTONDOWN));;
    m_btnCheckRButtonUp.Attach(GetDlgItem(IDC_CHECK_RBUTTONUP));;
	
    m_EditPosX.SetWindowText(_T("0"));
    m_EditPosY.SetWindowText(_T("0"));
    m_comboType.AddString(_T("mouse_event"));
    m_comboType.AddString(_T("postmessage"));
    m_comboType.SetCurSel(0);

    m_btnCheckLButtonDown.SetCheck(BST_CHECKED);
    m_btnCheckLButtonUp.SetCheck(BST_CHECKED);

	this->CenterWindow();
	return TRUE;
}
void CMainDlg::OnOk(UINT uNotifyCode, int nID, CWindow wndCtl)
{
    POINT ptOld ;
    GetCursorPos(&ptOld);

    POINT ptx = 
    {
        GetDlgItemInt(IDC_EDIT_POS_X, NULL, TRUE),
        GetDlgItemInt(IDC_EDIT_POS_Y, NULL, TRUE),
    };

    if (m_comboType.GetCurSel() == 1)
    {
        HWND hDstWnd = ::WindowFromPoint(ptx);

        POINT ptDstClient = ptx;
        ::ScreenToClient(hDstWnd, &ptDstClient);
        LPARAM lArg = MAKELONG(ptDstClient.x, ptDstClient.y);

        if (BST_CHECKED == m_btnCheckMouseMove.GetCheck())
            ::PostMessage(hDstWnd, WM_MOUSEMOVE, 0, lArg);

        if (BST_CHECKED == m_btnCheckLButtonDown.GetCheck())
            ::PostMessage(hDstWnd, WM_LBUTTONDOWN, 0, lArg);

        if (BST_CHECKED == m_btnCheckLButtonUp.GetCheck())   
            ::PostMessage(hDstWnd, WM_LBUTTONUP, 0, lArg);

        if (BST_CHECKED == m_btnCheckLButtonDBClick.GetCheck())   
            ::PostMessage(hDstWnd, WM_LBUTTONDBLCLK, 0, lArg);

        if (BST_CHECKED == m_btnCheckRButtonDown.GetCheck())
            ::PostMessage(hDstWnd, WM_RBUTTONDOWN, 0, lArg);

        if (BST_CHECKED == m_btnCheckRButtonUp.GetCheck())   
            ::PostMessage(hDstWnd, WM_RBUTTONUP, 0, lArg);

        if (BST_CHECKED == m_btnCheckMouseLeave.GetCheck())
            ::PostMessage(hDstWnd, WM_MOUSELEAVE, 0, 0);
    }
    else
    {
        SetCursorPos(ptx.x, ptx.y);

        // 这个有问题??
    //     if (BST_CHECKED == m_btnCheckMouseMove.GetCheck())
    //         mouse_event(MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_MOVE, 0, 0, 0, 0);

        if (BST_CHECKED == m_btnCheckLButtonDown.GetCheck())
            mouse_event(MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);

        if (BST_CHECKED == m_btnCheckLButtonUp.GetCheck())   
            mouse_event(MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
        
        if (BST_CHECKED == m_btnCheckLButtonDBClick.GetCheck())   
        {
            mouse_event(MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
            mouse_event(MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
            mouse_event(MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
        }

        if (BST_CHECKED == m_btnCheckRButtonDown.GetCheck())
            mouse_event(MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, 0);

        if (BST_CHECKED == m_btnCheckRButtonUp.GetCheck())   
            mouse_event(MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);

//         if (BST_CHECKED == m_btnCheckMouseLeave.GetCheck())
//             mouse_event(MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);

        SetCursorPos(ptOld.x, ptOld.y);
    }
    return;
}


void CMainDlg::OnMouseMove(UINT nFlags, CPoint point)
{
    // 快速探测屏幕坐标位置
    if (nFlags & VK_LBUTTON)
    {
        ::ClientToScreen(m_hWnd, &point);
        if (WindowFromPoint(point) == m_hWnd)
            return;

        SetDlgItemInt(IDC_EDIT_POS_X, point.x, TRUE);
        SetDlgItemInt(IDC_EDIT_POS_Y, point.y, TRUE);
    }
}
void CMainDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
    ReleaseCapture();
}
void CMainDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
    SetCapture();
}