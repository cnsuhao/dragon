#pragma once
#include "Other\resource.h"
class CMainFrame;


//
//	����һ��color�Ի���
//
class CNewColorDlg : public CDialogImpl<CNewColorDlg>
{
public:
	enum {IDD = IDD_NEW_COLOR };
	CNewColorDlg( ISkinRes* hSkin, bool bModifyMode=false );
	~CNewColorDlg();

	BEGIN_MSG_MAP_EX(CNewColorDlg)
		MSG_WM_INITDIALOG( OnInitDialog )
		COMMAND_ID_HANDLER_EX( IDOK, OnOk )
		COMMAND_ID_HANDLER_EX( IDCANCEL, OnCancel )
		COMMAND_ID_HANDLER_EX( IDC_BTN_CHOOSECOL, OnChooseColor )
	END_MSG_MAP()
	
	void   OnOk( UINT,int,HWND	);
	void   OnCancel( UINT,int,HWND	);
	void   OnChooseColor( UINT,int,HWND	);
	
	BOOL   OnInitDialog(CWindow wndFocus, LPARAM lInitParam);

	void   SetColorInfo( TCHAR* szColorName, TCHAR* szColorValue );

public:
	String    m_strId;
	String    m_strColor;

	ISkinRes* m_pSkin;

private:
	bool      m_bModifyMode;  //  ��ǰ���������ڴ��������Ǳ������޸�
};


//
//	����һ��font�Ի���
//
class CNewFontDlg : public CDialogImpl<CNewFontDlg>
{
public:
	enum {IDD = IDD_NEW_FONT };
	CNewFontDlg( ISkinRes* hSkin, bool bModifyMode=false);
	~CNewFontDlg();

	BEGIN_MSG_MAP_EX(CNewFontDlg)
		MSG_WM_INITDIALOG( OnInitDialog )
		COMMAND_ID_HANDLER_EX( IDOK, OnOk )
		COMMAND_ID_HANDLER_EX( IDCANCEL, OnCancel )
	END_MSG_MAP()

	void   OnOk( UINT,int,HWND	);
	void   OnCancel( UINT,int,HWND	);
	BOOL   OnInitDialog(CWindow wndFocus, LPARAM lInitParam);

	void   SetFontInfo( const String& strID, LOGFONT* plf);
public:
	String    m_strId;
	String    m_strFaceName;
	int       m_nSize;
	int       m_nOrientation;
	bool      m_bBold;
	bool      m_bItalic;
	bool      m_bUnderline;
	bool      m_bStrikeout;

	CButton   m_btnBold;
	CButton   m_btnItalic;
	CButton   m_btnUnderline;
	CButton   m_btnStrikeout;

	LOGFONT   m_lf;
	ISkinRes* m_pSkin;
	
private:
	bool      m_bModifyMode;  //  ��ǰ���������ڴ��������Ǳ������޸�
};

//
//	������Դ���ڵ�һЩ����������������������
//
template <class T>
class CResourceCodeDlg
{
public:
	CResourceCodeDlg(UI_RESOURCE_TYPE eResType){ m_eResType = eResType; m_pSkin=NULL; }
	virtual ~CResourceCodeDlg() { m_pSkin = NULL; }

	BEGIN_MSG_MAP_EX(CImageCodeDlg)
		COMMAND_CODE_HANDLER_EX(EN_CHANGE, OnEnChange)
		MSG_WM_INITDIALOG( OnInitDialog )
		MSG_WM_SIZE( OnSize )
		MSG_WM_DESTROY(OnDestroy)
		MSG_WM_SHOWWINDOW( OnShowWindow )
		COMMAND_HANDLER_EX(IDC_BTN_SAVE,BN_CLICKED,OnBtnSave);
	END_MSG_MAP()

public:

	// ��tmp�ļ�����ȡ��edit��
	bool  Update_From_TempFile()
	{
		if (m_strDataSource.empty())
			return false;

		String strTempFile = m_strDataSource + TEMP_FILE_EXT;
		if( !::PathFileExists(strTempFile.c_str()) )
			return false;

		ifstream f;
		f.open(strTempFile.c_str(), ios_base::in );
		if( f.fail() )
			return false;

		string strContent;
		string strLine;
		while( f.good() )
		{
			::getline(f, strLine);
			strContent.append(strLine);
			strContent.append("\r\n");
		}
		f.close();

		Util::CBufferT<wchar_t> buffer;

		Util::UTF82U(strContent.c_str(), buffer);  // ����������
		::SetWindowTextW(m_edit.m_hWnd, buffer );
		return true;
	}

	// ��edit�����ύ��tmp�ļ���
	bool  Commit_2_TempFile()
	{
		if (m_strDataSource.empty())
			return false;

		String strTempFile = m_strDataSource + TEMP_FILE_EXT;
		if( !::PathFileExists(strTempFile.c_str()) )
			return false;

		ofstream f;
		f.open(strTempFile.c_str(), ios_base::out|ios_base::binary );
		if( f.fail() )
			return false;

		string strContent;
		string strLine;
		int nLength = m_edit.GetWindowTextLength();
		wchar_t* szContent = new wchar_t[nLength+1];
		wcscpy( szContent, L"" );
		::GetWindowTextW(m_edit.m_hWnd, szContent,nLength);
		szContent[nLength] = L'';

		Util::CBufferT<char> utf8;
		Util::U2UTF8(szContent, utf8);     // xml��utf8�ļ�
		//f << '\xef' << '\xbb' << '\xbf' ;  // utf8 �ļ���־<-- ����Ҫ����ΪszContent���Ѿ������ñ�־
		//f << utf8;
		f.write(utf8, utf8.GetBufSize());

		delete[] szContent;
		f.close();

		return true;
	}

	void  AttachSkin(ISkinRes* hSkin )
	{
		if( m_pSkin == hSkin )
			return;

		m_pSkin = hSkin;

		// ��ȡxxx.xml�����ļ�·��
		TCHAR pszDataSource[MAX_PATH] = _T("");
		::UI_GetResourceDataSourcePath(m_pSkin, m_eResType, pszDataSource, MAX_PATH);
		m_strDataSource = pszDataSource ;

		Update_From_TempFile();
	}

	ISkinRes* GetSkin() { return m_pSkin; }

protected:
	BOOL   OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
	{
		m_edit.Attach( static_cast<T*>(this)->GetDlgItem(IDC_EDIT) );
		m_btnSave.Attach(static_cast<T*>(this)->GetDlgItem(IDC_BTN_SAVE) );
		m_btnSave.EnableWindow(FALSE);
		return TRUE;
	}
	void   OnSize(UINT nType, CSize size)
	{
		m_btnSave.SetWindowPos(NULL, WINDOW_PADDING, WINDOW_PADDING, WIDTH_TOOLBAR_BTN, HEIGHT_TOOLBAR_BTN, SWP_NOZORDER );
		m_edit.SetWindowPos(0,WINDOW_PADDING,2*WINDOW_PADDING+HEIGHT_TOOLBAR_BTN,size.cx-2*WINDOW_PADDING,size.cy-3*WINDOW_PADDING-HEIGHT_TOOLBAR_BTN,SWP_NOZORDER);

		SetMsgHandled(FALSE);
	}
	//
	//	Ϊ��ʵ�ֹر�childframe���ٴ�ʱ���ᶪʧ�༭������
	//	�ڴ��ڹر�ʱ���������ύ��tmp�ļ���
	//  ֱ���û������������ʱ���ٱ��浽xml�ļ���
	//
	void   OnDestroy()
	{
		this->Commit_2_TempFile();

		SetMsgHandled(FALSE);

	}
	void   OnEnChange(UINT,int,HWND)
	{
		T* pThis = static_cast<T*>(this);
		pThis->SET_DIRTY3(true);
		m_btnSave.EnableWindow(TRUE);

		SetMsgHandled(FALSE);
	}
	void   OnBtnSave(UINT,int,HWND)
	{
		//1. �ύ��tmp�ļ���
		if ( false == this->Commit_2_TempFile() )
			return;

		//2. ��tmp�ļ�copyΪ��ʽ�ļ�
		::DeleteFile(m_strDataSource.c_str());
		::CopyFile((m_strDataSource+TEMP_FILE_EXT).c_str(), m_strDataSource.c_str(), FALSE );

		T* pThis = static_cast<T*>(this);
		pThis->SET_DIRTY3(false);
		m_btnSave.EnableWindow(FALSE);

		SetMsgHandled(FALSE);
	}

	void   OnShowWindow(BOOL bShow, UINT nStatus)
	{
		T* pThis = static_cast<T*>(this);
		if(pThis->IS_DIRTY3())
		{
			m_btnSave.EnableWindow(TRUE);
		}
		else
		{
			m_btnSave.EnableWindow(FALSE);
		}
		SetMsgHandled(FALSE);
	}

private:
	ISkinRes* m_pSkin;
	CEdit    m_edit;
	CButton  m_btnSave;
	String   m_strDataSource;
	UI_RESOURCE_TYPE m_eResType;
};

