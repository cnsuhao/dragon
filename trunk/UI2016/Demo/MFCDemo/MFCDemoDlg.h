
// MFCDemoDlg.h : 头文件
//

#pragma once
#include "UI\UISDK\Inc\Interface\iwindow.h"


// CMFCDemoDlg 对话框
class CMFCDemoDlg : public CDialogEx, public UI::IWindowMessageCallback
{
// 构造
public:
	CMFCDemoDlg(CWnd* pParent = NULL);	// 标准构造函数


// 对话框数据
	enum { IDD = IDD_MFCDEMO_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

    virtual BOOL  OnWindowMessage(UINT msg, WPARAM, LPARAM, LRESULT& lResult)
    {
        if (msg == WM_DESTROY)
        {
            m_pUIWindow->Detach();
        }
        return FALSE;
    };
    virtual BOOL  OnWindowUIMessage(UI::UIMSG* pMsg) {
        return FALSE;
    }

    void OnNcDestroy()
    {
        __super::OnNcDestroy();
        if (m_pUIWindow)
        {
            m_pUIWindow->Release();
            m_pUIWindow = nullptr;
        }
    }

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

    UI::IWindow* m_pUIWindow = nullptr;
};
