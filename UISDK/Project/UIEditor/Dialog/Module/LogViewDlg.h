#pragma once
#include "Other\resource.h"
#include "Dialog\Framework\MainFrame.h"
#include "Dialog\Framework\childframe.h"

class CLogViewDlg : public CDialogImpl<CLogViewDlg>, public ILogMDIClientView
{
public:
	CLogViewDlg(HWND);
	~CLogViewDlg();

	enum{ IDD=IDD_DIALOG_LOG };
	BEGIN_MSG_MAP_EX( CImagePreView )
		MSG_WM_INITDIALOG( OnInitDialog )
		COMMAND_HANDLER_EX(IDOK,BN_CLICKED,OnOk)
		COMMAND_HANDLER_EX(IDC_BTN_BROWNSE,BN_CLICKED,OnBtnPath)
		COMMAND_HANDLER_EX(IDC_BTN_LOG_SWITCH,BN_CLICKED,OnBtnLogSwitch )
		COMMAND_HANDLER_EX(IDC_CHECK_LOGPATH_RELATIVE,BN_CLICKED, OnChangeLogPathRelative )
		COMMAND_RANGE_CODE_HANDLER_EX(IDC_RADIO_FILEFALG_DONOTWRITEFILE,
		        IDC_RADIO_FILEFLAG_LOOP, BN_CLICKED, OnRadioBtnFileLog )
	END_MSG_MAP()

protected:
	BOOL     OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
	void     OnOk(UINT,int,HWND);
	void     OnBtnPath(UINT,int,HWND);
	void     OnBtnLogSwitch(UINT,int,HWND);
	void     OnChangeLogPathRelative(UINT,int,HWND);
	void     OnRadioBtnFileLog(UINT,int,HWND);
	void     clear();

public:
	void     SetMode(bool bModify);

#pragma  region
	virtual void**  QueryInterface(REFIID riid);
	virtual ISkinRes*  GetSkinRes(){ return NULL; }
	virtual void   SetSkinRes(ISkinRes* hSkin) { }
	virtual long*   GetKey()  { return NULL; }

	virtual UI_RESOURCE_TYPE GetResourceType(){ return UI_RESOURCE_LOGMODULE; }
	virtual HWND   GetHWND() { return m_hWnd; }

	virtual bool AttachLogItem(__in const TCHAR* szLogItem);
#pragma  endregion

private:
	CButton     m_btnLogGlobalSwitch;

	CEdit       m_editModuleName;
	CButton     m_btnLogItemSwitch;
	CComboBox   m_comboLogLevel;
	CEdit       m_editLogPath;
	CButton     m_btnLogPathRelative;
	CButton     m_btnLF_Write2Console;
	CButton     m_btnLF_OutputDebugString;
	CButton     m_btnLF_PrintThreadId;
	CButton     m_btnFF_NoFileOutput;
	CButton     m_btnFF_OverWrite;
	CButton     m_btnFF_AddTimeExt;
	CButton     m_btnFF_Loop;
	CEdit       m_editLoopCount;
	CStatic     m_staticGroupBoxLogItem;
	
	bool        m_bModifyMode;   //  ��ǰ���������ڴ��������Ǳ������޸�
	HWND        m_hWndTreePanel; //  ��־���ؼ����ھ�������ڷ�����Ϣ
	String      m_strModuleName; //  ����ģ�����ƣ��������޸�ģ������ʱ����ԭģ����
	long        m_lLogCookie;
};