#pragma once
#include "Resource.h"

class CMainDlg : public CDialogImpl<CMainDlg>
{
public:
	enum {IDD=IDD_MAINDLG};

	CMainDlg(void);
	~CMainDlg(void);

	BEGIN_MSG_MAP(CMainDlg)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_CLOSE(OnClose)
		COMMAND_HANDLER_EX(IDOK, BN_CLICKED, OnOk)
        MSG_WM_LBUTTONDOWN(OnLButtonDown)
        MSG_WM_LBUTTONUP(OnLButtonUp)
        MSG_WM_MOUSEMOVE(OnMouseMove)
	END_MSG_MAP()
public:
	
	BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
	void OnClose();
	void OnOk(UINT uNotifyCode, int nID, CWindow wndCtl);
    void OnMouseMove(UINT nFlags, CPoint point);
    void OnLButtonUp(UINT nFlags, CPoint point);
    void OnLButtonDown(UINT nFlags, CPoint point);

protected:
	CEdit    m_EditPosX;
	CEdit    m_EditPosY;
    CComboBox  m_comboType;
	CButton  m_btnCheckMouseMove;
	CButton  m_btnCheckLButtonDown;
	CButton  m_btnCheckLButtonUp;
    CButton  m_btnCheckMouseLeave;
    CButton  m_btnCheckLButtonDBClick;
    CButton  m_btnCheckRButtonDown;
    CButton  m_btnCheckRButtonUp;
};

