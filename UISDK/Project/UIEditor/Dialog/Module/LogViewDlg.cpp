#include "stdafx.h"
#include "LogViewDlg.h"
#include "Dialog\Framework\projecttreedialog.h"
#include "Dialog\Framework\childframe.h"

CLogViewDlg::CLogViewDlg(HWND hWnd)
{
	m_bModifyMode = false;
	m_hWndTreePanel = hWnd;
	m_lLogCookie = 0;
}

CLogViewDlg::~CLogViewDlg()
{

}

//
//	清除界面数据
//
void CLogViewDlg::clear()
{
	m_editModuleName.SetWindowText(_T(""));
	m_btnLogItemSwitch.SetCheck(BST_CHECKED);
	m_comboLogLevel.SetCurSel(0);
	m_btnLF_Write2Console.SetCheck(BST_UNCHECKED);
	m_btnLF_OutputDebugString.SetCheck(BST_UNCHECKED);;
	m_btnLF_PrintThreadId.SetCheck(BST_UNCHECKED);;
	m_btnFF_NoFileOutput.SetCheck(BST_UNCHECKED);;
	m_btnFF_OverWrite.SetCheck(BST_UNCHECKED);;
	m_btnFF_AddTimeExt.SetCheck(BST_UNCHECKED);;
	m_btnFF_Loop.SetCheck(BST_UNCHECKED);;
	m_editLoopCount.SetWindowText(_T("") );
	m_staticGroupBoxLogItem.SetWindowText(_T("Log_Item_Operator:"));
	::EnableWindow(GetDlgItem(IDC_EDIT_LOOPCOUNT), FALSE );

#if 0 // TODO:
	TCHAR szProjDir[MAX_PATH] = _T("");

	CComBSTR bstr;
	g_pGlobalData->m_pEditSkinMgr->GetUIDataSourceDir(&bstr);
	bstr.Append(_T("\\log.xml"));
	m_editLogPath.SetWindowText(bstr);
	m_btnLogPathRelative.SetCheck(BST_CHECKED);
	m_lLogCookie = 0;
#endif
}

BOOL CLogViewDlg::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
	m_btnLogGlobalSwitch.Attach(GetDlgItem(IDC_CHECK_LOGSWITCH));

	m_editModuleName.Attach(GetDlgItem(IDC_EDIT_MODULENAME));
	m_btnLogItemSwitch.Attach(GetDlgItem(IDC_CHECK_LOGITEM_SWITCH));
	m_comboLogLevel.Attach(GetDlgItem(IDC_COMBO_LOGLEVEL));
	m_editLogPath.Attach(GetDlgItem(IDC_EDIT_LOGPATH));
	m_btnLogPathRelative.Attach(GetDlgItem(IDC_CHECK_LOGPATH_RELATIVE ));
	m_btnLF_Write2Console.Attach(GetDlgItem(IDC_CHECK_LOGFLAG_CONSOLE));
	m_btnLF_OutputDebugString.Attach(GetDlgItem(IDC_CHECK_LOGFLAG_DEBUG));
	m_btnLF_PrintThreadId.Attach(GetDlgItem(IDC_CHECK_LOGFLAG_THREADID));
	m_btnFF_NoFileOutput.Attach(GetDlgItem(IDC_RADIO_FILEFALG_DONOTWRITEFILE));
	m_btnFF_OverWrite.Attach(GetDlgItem(IDC_RADIO_FILEFLAG_OVERWRITE));
	m_btnFF_AddTimeExt.Attach(GetDlgItem(IDC_RADIO_FILEFLAG_ADDTIMEEXT));
	m_btnFF_Loop.Attach(GetDlgItem(IDC_RADIO_FILEFLAG_LOOP));
	m_editLoopCount.Attach(GetDlgItem(IDC_EDIT_LOOPCOUNT));
	m_staticGroupBoxLogItem.Attach(GetDlgItem(IDC_STATIC_GROPBOX));
	
	m_comboLogLevel.AddString(_T("DEBUG"));
	m_comboLogLevel.AddString(_T("INFO"));
	m_comboLogLevel.AddString(_T("WARN"));
	m_comboLogLevel.AddString(_T("ERROR"));
	m_comboLogLevel.AddString(_T("FATAL"));

	if( NULL != g_pGlobalData->m_pLog )
	{
		VARIANT_BOOL bEnable = VARIANT_FALSE;
		g_pGlobalData->m_pLog->IsEnable(&bEnable);
		if(bEnable == VARIANT_TRUE)
		{
			m_btnLogGlobalSwitch.SetCheck( BST_CHECKED );
		}
	}
	

	if( ! m_bModifyMode )
	{
		this->clear();
	}

	this->SetMode(m_bModifyMode);

	return TRUE;
}


void CLogViewDlg::SetMode(bool bModify)
{
	if(bModify)
	{
		TCHAR szInfo[256] = _T("");
		_stprintf(szInfo, _T("Log_Item_Modify: %s"), m_strModuleName.c_str() );
		::SetWindowText(::GetDlgItem(m_hWnd,IDC_STATIC_GROPBOX), szInfo );
		::SetWindowText(::GetDlgItem(m_hWnd,IDOK),_T("Modify") );

	}
	else
	{
		::SetWindowText(::GetDlgItem(m_hWnd,IDC_STATIC_GROPBOX),_T("Log_Item_Create") );
		::SetWindowText(::GetDlgItem(m_hWnd,IDOK),_T("Create") );
	}

	m_bModifyMode = bModify;
}

void CLogViewDlg::OnOk(UINT,int,HWND)
{
	if (NULL == g_pGlobalData->m_pLog)
		return;

	TCHAR  szTemp[MAX_STRING_LEN] = _T("");
	UINT   nTemp = 0;

	m_editModuleName.GetWindowText(szTemp, MAX_STRING_LEN);
	String strModuleName = szTemp;
	if (strModuleName.empty())
	{
		::MessageBox( NULL, _T("Module name is empty"), _T("Warning"), MB_OK|MB_ICONWARNING );
		return;
	}

	nTemp = m_btnLogItemSwitch.GetCheck();
	VARIANT_BOOL bEnable = nTemp==BST_CHECKED? VARIANT_TRUE:VARIANT_FALSE;

	nTemp = m_comboLogLevel.GetCurSel();
	LONG lLevel = nTemp;

	m_editLogPath.GetWindowText(szTemp, MAX_PATH);
	String strPath = szTemp;
	if( strPath.empty())
	{
		::MessageBox( NULL, _T("Module path is empty"), _T("Warning"), MB_OK|MB_ICONWARNING );
		return;
	}

	long lLogFlag = 0;
	nTemp = m_btnLF_Write2Console.GetCheck();
	if( nTemp == BST_CHECKED )
		lLogFlag |= LOG_FLAG_CONSOLE;
	nTemp = m_btnLF_OutputDebugString.GetCheck();
	if( nTemp == BST_CHECKED )
		lLogFlag |= LOG_FLAG_DBGVIEW;
	nTemp = m_btnLF_PrintThreadId.GetCheck();
	if( nTemp == BST_CHECKED )
		lLogFlag |= LOG_FLAG_THREADID;

	long lFileFlag = 0;
	nTemp = m_btnFF_NoFileOutput.GetCheck();
	if( nTemp == BST_CHECKED )
		lFileFlag = LOG_FLAG_FILE_DISABLE;
	nTemp = m_btnFF_OverWrite.GetCheck();
	if( nTemp == BST_CHECKED )
		lFileFlag = LOG_FLAG_FILE_SIMPLE;
	nTemp = m_btnFF_AddTimeExt.GetCheck();
	if( nTemp == BST_CHECKED )
		lFileFlag = LOG_FLAG_FILE_TIME;
	nTemp = m_btnFF_Loop.GetCheck();
	if( nTemp == BST_CHECKED )
	{
		lFileFlag = LOG_FLAG_FILE_LOOP;
		nTemp = ::GetDlgItemInt( m_hWnd, IDC_EDIT_LOOPCOUNT, NULL, TRUE );
		nTemp = nTemp << 24;
		lFileFlag += nTemp;
	}
	
	if (m_bModifyMode)
	{
		CComBSTR bstrOldModuleName(m_strModuleName.c_str());
		CComBSTR bstrModuleName(strModuleName.c_str());
		CComBSTR bstrPath(strPath.c_str());

		LOGITEM  logItem;
		memset(&logItem, 0, sizeof(LOGITEM));
		logItem.bEnable = bEnable;
		logItem.logName = bstrModuleName;
		logItem.logFile = bstrPath;
		logItem.logLevel = (LOG_LEVEL)lLevel;
		logItem.uLogFlag = lLogFlag;
		logItem.uLogFileFlag = lFileFlag;
		logItem.uLogFileLoopCount = 0;

		if (FAILED(g_pGlobalData->m_pLog->ModifyLogItem(m_lLogCookie, &logItem)))
		{
			::MessageBox( NULL, _T("Modify LogItem Failed"), _T("Error"), MB_OK|MB_ICONERROR );
			return;
		}

		// 更新到左侧结点
		::SendMessage(m_hWndTreePanel, WM_MODIFY_LOG_ITEM, (WPARAM)strModuleName.c_str(), (LPARAM)m_strModuleName.c_str() );

		m_strModuleName = strModuleName;
		this->SetMode(true);

		::MessageBox(NULL, _T("Modify Module Success"), _T("Tip"), MB_OK|MB_ICONINFORMATION);
	}
	else
	{
		CComBSTR bstrModuleName(strModuleName.c_str());
		CComBSTR bstrPath(strPath.c_str());

		LOGITEM  logItem;
		memset(&logItem, 0, sizeof(LOGITEM));
		logItem.bEnable = bEnable;
		logItem.logName = bstrModuleName;
		logItem.logFile = bstrPath;
		logItem.logLevel = (LOG_LEVEL)lLevel;
		logItem.uLogFlag = lLogFlag;
		logItem.uLogFileFlag = lFileFlag;
		logItem.uLogFileLoopCount = 0;

		if (FAILED(g_pGlobalData->m_pLog->AddLogItem(&logItem, &m_lLogCookie)))
		{
			::MessageBox( NULL, _T("Add LogItem Failed"), _T("Error"), MB_OK|MB_ICONERROR );
			return;
		}

		// 添加成功
		// 1. 插入到左侧结点
		::SendMessage(m_hWndTreePanel, WM_INSERT_LOG_ITEM, (WPARAM)(BSTR)bstrModuleName, m_lLogCookie);
		m_strModuleName = strModuleName;

		// 2. 更新数据界面
		this->SetMode(true);

		::MessageBox( NULL, _T("Add Module Success"), _T("Tip"), MB_OK|MB_ICONINFORMATION );
	}
}
void CLogViewDlg::OnBtnPath(UINT,int,HWND)
{
	CFileDialog  dlg(FALSE);

	TCHAR szDir[MAX_PATH] = _T("");
	m_editLogPath.GetWindowText(szDir, MAX_PATH);

	dlg.m_ofn.lpstrInitialDir = szDir;
	if( IDOK == dlg.DoModal() )
	{
		m_editLogPath.SetWindowText(dlg.m_szFileName);

		this->OnChangeLogPathRelative(0,0,0);
	}
}

void CLogViewDlg::OnBtnLogSwitch(UINT,int,HWND)
{
	if( NULL == g_pGlobalData->m_pLog )
		return ;

	UINT nRet = ::IsDlgButtonChecked(m_hWnd, IDC_CHECK_LOGSWITCH );
	if( BST_CHECKED == nRet )
	{
		g_pGlobalData->m_pLog->EnableLog( TRUE );	
	}
	else
	{
		g_pGlobalData->m_pLog->EnableLog( FALSE );
	}
}

void CLogViewDlg::OnRadioBtnFileLog(UINT ,int nID, HWND)
{
	UINT nCheck = ::IsDlgButtonChecked(m_hWnd, IDC_RADIO_FILEFLAG_LOOP );
	if( BST_CHECKED == nCheck )
	{
		::EnableWindow(GetDlgItem(IDC_EDIT_LOOPCOUNT), TRUE );
	}
	else
	{
		::EnableWindow(GetDlgItem(IDC_EDIT_LOOPCOUNT), FALSE );
	}
}

void CLogViewDlg::OnChangeLogPathRelative(UINT,int,HWND)
{
	TCHAR szPath[MAX_PATH] = _T("");
	m_editLogPath.GetWindowText(szPath, MAX_PATH);
	if( 0 == _tcslen(szPath) )
		return;

#if 0 // TODO
	UINT uCheck = ::IsDlgButtonChecked(m_hWnd, IDC_CHECK_LOGPATH_RELATIVE );
	if (uCheck == BST_CHECKED)
	{
		// 切换为相对路径
		if( Util::IsFullPath(szPath) )
		{
			CComBSTR bstr;
			g_pGlobalData->m_pEditSkinMgr->GetUIDataSourceDir(&bstr);

			TCHAR szResult[MAX_PATH] = _T("");
			Util::CalcRelativePathToDir(bstr, szPath, szResult );

			this->m_editLogPath.SetWindowText(szResult);
		}
	}
	else
	{
		// 使用绝对路径
		if (!Util::IsFullPath(szPath))
		{
			CComBSTR bstr;
			g_pGlobalData->m_pEditSkinMgr->GetUIDataSourceDir(&bstr);

			TCHAR szResult[MAX_PATH] = _T("");
			Util::CalcFullPathByRelative(bstr, szPath, szResult);

			this->m_editLogPath.SetWindowText(szResult);
		}
    }
#endif

}

void**  CLogViewDlg::QueryInterface(REFIID riid)
{
	if (::IsEqualIID(IID_ILogMDIClientView, riid))
	{
		return (void**)static_cast<ILogMDIClientView*>(this);
	}
    return __super::QueryInterface(riid);
}


//
//	关联确认显示哪个模块数据
//
bool CLogViewDlg::AttachLogItem(__in const TCHAR* szLogItem)
{
	if (NULL == szLogItem || 0 == _tcslen(szLogItem))
	{
		this->clear();
		SetMode(false);
		return true;
	}

	if (0 == _tcscmp(m_strModuleName.c_str(), szLogItem))
		return true;
	this->clear();

	if (g_pGlobalData->m_pLog)
	{
		m_strModuleName = szLogItem;

		CComBSTR bstr(m_strModuleName.c_str());
		ILogItem* pLogItem = NULL;
		g_pGlobalData->m_pLog->GetLogItemByName(bstr, &pLogItem);
		if (pLogItem)
		{
			BSTR bstr = NULL;
			pLogItem->GetName(&bstr);
			m_editModuleName.SetWindowText((TCHAR*)bstr);
			::SysFreeString(bstr);

			pLogItem->GetCookie(&m_lLogCookie);

			VARIANT_BOOL bEnable = VARIANT_FALSE;
			pLogItem->IsEnable(&bEnable);
			if (VARIANT_TRUE == bEnable)
				m_btnLogItemSwitch.SetCheck(BST_CHECKED);
			else
				m_btnLogItemSwitch.SetCheck(BST_UNCHECKED);


			LONG lVal = 0;
			pLogItem->GetLevel(&lVal);
			m_comboLogLevel.SetCurSel(lVal);

			pLogItem->GetPath(&bstr);
			m_editLogPath.SetWindowText((TCHAR*)bstr);
			if (Util::IsFullPath((TCHAR*)bstr))
				m_btnLogPathRelative.SetCheck(BST_UNCHECKED);				
			else
				m_btnLogPathRelative.SetCheck(BST_CHECKED);
			SysFreeString(bstr);

			pLogItem->GetLogFlag(&lVal);
			if (lVal & LOG_FLAG_CONSOLE)
				m_btnLF_Write2Console.SetCheck(BST_CHECKED);
			if (lVal & LOG_FLAG_DBGVIEW)
				m_btnLF_OutputDebugString.SetCheck(BST_CHECKED);
			if (lVal & LOG_FLAG_THREADID)
				m_btnLF_PrintThreadId.SetCheck(BST_CHECKED);

			pLogItem->GetFileFlag(&lVal);
			if (lVal & LOG_FLAG_FILE_DISABLE)
				m_btnFF_NoFileOutput.SetCheck(BST_CHECKED);
			if (lVal & LOG_FLAG_FILE_SIMPLE)
				m_btnFF_OverWrite.SetCheck(BST_CHECKED);
			if (lVal & LOG_FLAG_FILE_TIME)
				m_btnFF_AddTimeExt.SetCheck(BST_CHECKED);


			if (lVal & LOG_FLAG_FILE_LOOP)
			{
				m_btnFF_Loop.SetCheck(BST_CHECKED);
				m_editLoopCount.EnableWindow(TRUE);
				int nCount = lVal>>24;
				::SetDlgItemInt(m_hWnd, IDC_EDIT_LOOPCOUNT, nCount, TRUE);
			}
			else
				m_editLoopCount.EnableWindow(FALSE);

			pLogItem->Release();
		}
	}

	this->SetMode(true);
	return true;
}